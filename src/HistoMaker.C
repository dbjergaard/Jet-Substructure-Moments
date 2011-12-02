/*-------------------------------------------------------------------
 * This is a hack of HistoCompare to simply print all the histograms
 * in a root file for the quick and easy viewing 
 *
 *  Author: David Bjergaard (dbjergaa@fnal.gov)  Date: 02/20/09
 ********************************************************************/

#include <iostream>

#include <TKey.h>
#include <TTree.h>
#include <TChain.h>
#include <Riostream.h>
#include <TObject.h>
#include <vector>
#include <TCanvas.h>
#include <sstream>
#include <string>
#include <TLegend.h>
#include <TPaveLabel.h>
#include <TStyle.h>
#include "TColor.h"


#include "AtlasStyle.h"
// should add preprocessor defines to see if we're in CINT or not
#include "HistoMaker.h"

using std::string;


//number to string
template <class T>
inline std::string stringify(const T& t)
{
  std::ostringstream o;
  if (!(o << t))
    return "err";
  return o.str();
} 

void HistoMaker(const string FileName, const string Name, const int mode)
{
  SetAtlasStyle();
  //SetStyle(mode);

  cout << "I'm processing: "<<Name<<endl;
  TDirectory *outfile = TFile::Open("compared_results.root","RECREATE");
  //Iterate over the list of keys and perform our operations
  //TList *fileList = new TList();
  //fileList->Add(TFile::Open(skFn1.c_str()));
  TFile *file = new TFile(FileName.c_str());

  KeyIterator(outfile,file,Name, mode); 

  //Close and delete dummy file
  outfile->Close();
  if( remove( "compared_results.root" ) != 0 ) 
    perror( "Error deleting compared_results.root" ); 
  else 
    puts( "Successfully deleted compared_results.root" ); 

  return;
}
void HistoMaker()
{
  cout << "Usage:\t HistoMaker(\"path/to/filename\",\"Name of dataset\", mode);"<<endl;
  cout <<"\t Path names are relative to current $PWD."<<endl;
  cout <<"\t Try \".! echo $PWD\" to find out where you are."<<endl;
  cout <<"\t Names of datasets can be formatted as TLatex strings:"<<endl;
  cout <<"\t\t \"H #rightarrow ZZ #rightarrow #mu #mu #mu #mu\"" <<endl;
  cout <<"\t mode specifies how to draw the histograms"<<endl;
  cout <<"\t\t values are 0, 1, 2"<<endl;
  cout <<"\t\t 0 == print histos to file, 6 to a page"<<endl;
  cout <<"\t\t 1 == print each histo to its own file (eps and gif copies)"<<endl;
  cout <<"\t\t 2 == print each histo on screen"<<endl;
  cout <<"\t Histos will be saved as (category)_results.ps for option 1"<<endl;
  cout <<"\t or as (histoname).gif/.eps for option 2"<<endl;
  cout <<"\t You can now copy this locally using a command like scp, or rsync\n"<<endl;
  cout <<"\t\t David Bjergaard (dbjergaa@fnal.gov)"<<endl;

  return;
}
void KeyIterator( TDirectory *target, TFile *file, string Name, const int mode)
{
  //Vectors for holding the histograms as we find them
  vector<TH1*> first_histos (0);  //We start with zero histograms and add when we find one
  vector<THStack*> stack_histos (0);
  TString path((char*)strstr(target->GetPath(),":"));
  path.Remove(0,2);
  cout<<"Current path is: "<<path<<endl;

  file->cd(path);
  TDirectory *c_sourcedir = gDirectory;

  //Loop over all keys in the directory
  TIter nextkey( c_sourcedir->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextkey()))
    {
      //Change to the current path and grab object there
      file->cd(path);
      TObject *obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom("TH1"))
	{
	  //Perform operations on histo
	  TH1 *h1 = (TH1*)obj;
	  if(h1->Integral()!=0)
	    first_histos.push_back(h1);
	}
      else if ( obj->IsA()->InheritsFrom("THStack") )
	{
	  THStack *hStack = (THStack*)obj;
	  stack_histos.push_back(hStack);
	}
      else if ( obj->IsA()->InheritsFrom("TDirectory"))
	{
	  cout << "Found subdirectory: "<<obj->GetName()<<endl;
	  
 	  //create new subdir in output root file
 	  target->cd();
 	  TDirectory *newdir = target->mkdir(obj->GetName(),obj->GetTitle());
 	  KeyIterator(newdir,file,Name, mode);
	}
      else
	{
	  cout <<"Unknown object type, name: "
	       << obj->GetName()<< " title: " <<obj->GetTitle() <<endl;
	}
    }//while

  //Now we have a vector full of histograms 
  //with the same names and different distributions.
  //We can begin to paint them!
  painter(first_histos, stack_histos, path, Name, mode);
  return;
}//KeyIterator();
void painter(vector<TH1*> histos, vector<THStack*> stack_histos, TString dir_name, const string Name, int mode)
{
  //mode==0 print to a file with six plots to a page
  //mode==1 print each plot to its own file
  //mode==2 print the plot on screen


  TString outname;
  cout <<"Beginning to Paint Histos"<<endl;
  //Initialize stuff for the pages
  string name = "page_1";
  string title = "Page 1";
  //Make new canvas and subdivide it for our pads
  TCanvas *page = new TCanvas(name.c_str(),title.c_str(),1);
  if(mode==0)
    page->Divide(2,3);
  Ssiz_t n_str = dir_name.Index("/",1); //counter to snip off first directory from path
  n_str++;

  unsigned int n=1;   //Number of pages in our book
  if(histos.size()==0) return; //We actually have stuff to look at!
  outname = dir_name.Remove(0,n_str);
  if(mode==0)
    {
      outname = outname + "_results.ps";
      page->Print(outname+"["); //Open our "book" to print
    }
  if(mode != 0)
    {
      for(vector<THStack*>::const_iterator it=stack_histos.begin(); it != stack_histos.end(); ++it)
	{
	  (*it)->Draw("nostack");
	  if(mode == 1)
	    {
	      page->Print((string)(*it)->GetName() + "_" + outname +".eps");
	      page->Print((string)(*it)->GetName() + "_" + outname +".gif");
	      TLegend *legend = new TLegend(0.58,0.65,.84,.82);
	      TList* hists = (*it)->GetHists();
	      TListIter it(hists);
	      while(TObject* obj = it.Next() )
		{
		  //cout <<obj->GetName()<<"\n";
		  legend->AddEntry(obj,obj->GetName(),"f");
		}
	      legend->SetBorderSize(0);
	      legend->Draw();
	      legend->SetFillColor(kWhite);
	    }
	}
    }
  for(unsigned int i=0; i < histos.size();i++)
    {
      page->SetName(name.c_str());
      page->SetTitle(title.c_str());
      if(mode==0)
	page->cd((i+1 - 6*(n-1))); //vectors start from 0 pads don't...
      //6*(n-1) keeps us in the range 1-6

      //Normalize both histos for aesthetic reasons
      //histos[i]->Sumw2();
      //histos[i]->Scale(1.0/histos[i]->GetEntries());

      //Color our histos for eye candy
      histos[i]->SetLineColor(kBlue-9);
      histos[i]->SetFillColor(kBlue-10);

      //Tweak axes ticks
      histos[i]->GetYaxis()->SetTickLength(-0.03);
      histos[i]->GetYaxis()->SetLabelOffset(0.026);
      histos[i]->GetYaxis()->SetTitleOffset(1.7);
      //stack histos before we draw them to ensure the scales are setup correctly
      //THStack *hs = new THStack("hs", histos[i]->GetTitle());
      //hs->Add(histos[i],"hist");
      if(histos[i]->IsA()->InheritsFrom("TH2"))
	{
	  //create color gradient
	  UInt_t Number = 4;
	  Double_t Red[4]    = { 0.00, 0.00, 1.00, 1.00};
	  Double_t Green[4]  = { 0.00, 1.00, 1.00, 0.00};
	  Double_t Blue[4]   = { 1.00, 1.00, 0.00, 0.00};
	  Double_t Length[4] = { 0.00, 0.25, 0.75, 1.00};
	  Int_t nb=50;
	  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
	  histos[i]->SetContour(nb);
	  histos[i]->Draw("COLZ");
	  TString title = TString(histos[i]->GetTitle());
	  histos[i]->SetTitle(title);
	  title.ToLower();
	  if(title.Contains("mass"))
	    {
	      histos[i]->GetXaxis()->SetTitle("GeV/c^2");
	      histos[i]->GetYaxis()->SetTitle("GeV/c^2");
	    }
	  else if (title.Contains("pt") || title.Contains("p_t") || title.Contains("p_{t}"))
	    {
	      histos[i]->GetXaxis()->SetTitle("GeV/c");
	      histos[i]->GetYaxis()->SetTitle("GeV/c");
	    }
	}
      else
	{
	  histos[i]->Draw();
	  TString title = TString(histos[i]->GetTitle());
	  title.ToLower();
	  if(title.Contains("mass"))
	    histos[i]->GetXaxis()->SetTitle("GeV/c^2");
	  else if (title.Contains("pt") || title.Contains("p_t") || title.Contains("p_{t}"))
	    histos[i]->GetXaxis()->SetTitle("GeV/c");
	}
      gPad->RedrawAxis();
      if(mode==0)
	{
	  if((i+1 - 6*(n-1))==2)  //We're at the upper right corner of the page, print legend
	    {
	      TLegend *legend = new TLegend(0.58,0.65,.84,.82);
	      legend->AddEntry(histos[i],Name.c_str(),"f");
	      legend->SetBorderSize(0);
	      legend->Draw();
	      legend->SetFillColor(kWhite);
	    }
	  if((i+1)==6*n)  //we're at the end of our page
	    {
	      page->Print(outname);
	      n++;
	      name = "page_"+ stringify(n);
	      title = "Page "+ stringify(n);
	    }
	  else if(i+1==histos.size()) //Takes care of the case when 
	    {                         //we have a partially filled page	     
	      page->Print(outname);
	    }
	  page->Print(outname+"]"); //Close our "book"
	}
      else if(mode==1)
	{
	  page->Print((string)histos[i]->GetName() + "_" + outname +".eps");
	  page->Print((string)histos[i]->GetName() + "_" + outname +".gif");
	}
      else if(mode==2)
	page->Draw();
    }
}


