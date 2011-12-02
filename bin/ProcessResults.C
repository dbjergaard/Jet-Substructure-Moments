// System includes
#include <iostream>

#include "TROOT.h"
#include "TStyle.h"


#include "HistoMaker.h"
#include "AtlasStyle.h"

using std::cout;
using std::endl;
using std::string;

void usage(const char* name)
{
  cout << "Usage:\t " << string(name) << "\"path/to/filename\" \"Name of dataset\"  mode "<<endl;
  cout <<"\t Name of dataset can be formatted as a TLatex string:"<<endl;
  cout <<"\t\t \"H #rightarrow ZZ #rightarrow #mu #mu #mu #mu\"" <<endl;
  cout <<"\t mode specifies how to draw the histograms"<<endl;
  cout <<"\t\t values are 0, 1, 2"<<endl;
  cout <<"\t\t 0 == print histos to file, 6 to a page"<<endl;
  cout <<"\t\t 1 == print each histo to its own file (eps and gif copies)"<<endl;
  cout <<"\t\t 2 == print each histo on screen"<<endl;
  cout <<"\t Histos will be saved as (category)_results.ps for option 1"<<endl;
  cout <<"\t or as (histoname).gif/.eps for option 2"<<endl;
  cout <<"\t\t David Bjergaard (dbjergaa@fnal.gov)"<<endl;
}

int main(int argc, const char* argv[])
{
  if(argc < 4)
    {
      usage(argv[0]);
      return -1;
    }
  int mode = 0;
  string modestr(argv[3]);
  if(modestr=='2')
    mode=2;
  else if(modestr=='1')
    mode=1;
  else
    mode=0;
  SetAtlasStyle();
  TStyle* style = gROOT->GetStyle("ATLAS");


  style->SetOptTitle(1);

  // Margins:
  style->SetPadTopMargin(0.15);
  style->SetPadBottomMargin(0.13);
  style->SetPadLeftMargin(0.13);
  style->SetPadRightMargin(0.35);

  style->SetCanvasDefH(500);
  style->SetCanvasDefW(800);
  
  style->SetTitleFont(42);
  style->SetTitleColor(1);
  style->SetTitleTextColor(1);
  style->SetTitleFillColor(10);
  style->SetTitleFontSize(0.06);
  style->SetTitleBorderSize(0);

  style->SetOptStat("emrou");
  style->SetStatColor(kWhite);
  style->SetStatFont(42);
  style->SetStatFontSize(0.03);
  style->SetStatTextColor(1);
  style->SetStatFormat("6.4g");
  style->SetStatBorderSize(0);
  style->SetStatX(1.0);
  style->SetStatY(0.95);//0.85
  style->SetStatH(0.1);
  style->SetStatW(0.2);
  
  

  HistoMaker(argv[1],argv[2],mode);
  return 0;
}
