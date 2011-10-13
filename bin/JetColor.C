// System includes
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

// ROOT includes
#include "TROOT.h"
#include "TColor.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TVector2.h"

// User Includes

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;

void usage(const char*)
{
  printf("Usage: JetColor [args] [filename]\n");
}
double rMag(double eta,double phi){ return TMath::Sqrt(eta*eta + phi*phi);}

double deltaR(double eta1, double phi1, double eta2, double phi2) 
{ 
  return rMag(eta1-eta2,phi1-phi2);
}
void calcJetPull(double cl_pt, double jet_pt, 
		 double reta, double rphi,
		 double* teta, double* tphi)
{
  // Takes in pt_i, jet_pt, r_i and returns t, the pull vector.  
  // r_i and t are arrays of length three
  //double cl_rmag = rMag(reta, rphi);
  double alpha = ((cl_pt*rMag(reta, rphi))/jet_pt);
  /*
  printf("cl_pt: %'.2f, jet_pt: %'.2f, reta: %'.2f, rphi: %'.2f, teta: %f, tphi: %f, alpha: %'.2f\n",
	 cl_pt,         jet_pt,        reta,        rphi,        *teta,        *tphi, alpha);
  */
  *teta+=alpha*reta;
  *tphi+=alpha*rphi;
  return;
}
/* //not needed since calcTheta automatically normalizes the vector components to [0,1]
pair<double,double> calcNormedR(double x,double y )
{
  pair<double,double> vector;
  double mag = rMag(x, y);
  vector.first  = x/mag;
  vector.second = y/mag;
  return vector;
}
*/
 /* No need to reinvent the wheel, use ROOT built-ins instead
double calcTheta(pair<double,double> vec1, pair<double,double> vec2)
{
  double mag1 = rMag(vec1.first, vec1.second);
  double mag2 = rMag(vec2.first, vec2.second);
  return TMath::ACos((vec1.first*vec2.first + vec1.second*vec2.second)/(mag1*mag2));
}
*/
static void print_2d_histo(TH2* plot, string outname)
{
  TCanvas canvas("canvas",plot->GetTitle(),600,400);
  gStyle->SetPalette(1);
  //create color gradient
  UInt_t Number = 4;
  Double_t Red[4]    = { 0.00, 0.00, 1.00, 1.00};
  Double_t Green[4]  = { 0.00, 1.00, 1.00, 0.00};
  Double_t Blue[4]   = { 1.00, 1.00, 0.00, 0.00};
  Double_t Length[4] = { 0.00, 0.25, 0.75, 1.00};
  Int_t nb=50;
  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
  plot->SetContour(nb);
  plot->Draw("COLZ");
  outname = outname + ".eps";
  canvas.SaveAs(outname.c_str());
  outname.replace(outname.size()-3,3,"gif");
  canvas.SaveAs(outname.c_str());
}

static void print_histo(TH1* plot, string outname)
{
  TCanvas genericCanvas;
  //plot->Sumw2();
  plot->Draw("E1P0");
  outname = outname + ".eps";
  genericCanvas.SaveAs(outname.c_str());
  outname.replace(outname.size()-3,3,"gif");
  genericCanvas.SaveAs(outname.c_str());
}

int main(int argc,const char* argv[])
{
  const double delR_cut = 0.4;
  const double jetEta_cut = 2.5;
  if(argc < 2)
    {
      usage(argv[0]);
      return -1;
    }
  printf("Making Jet Color structure pulls, using %s as input...\n", argv[argc-1]);
  TFile file(argv[argc-1],"READ");
  TTree* physics=NULL; 
  file.GetObject("physics",physics);
  if(physics == NULL)
    {
      printf("Failed to get tree: \"physics\", Aborting.\n");
      return -1;
    }

  Int_t nEvents = physics->GetEntries();
  printf("Found %d events... Looping\n",nEvents);
  unsigned int jet_n = 0, cl_n = 0;
  vector<float>* jet_E=NULL; vector<float>* jet_pt=NULL; vector<float>* jet_eta=NULL; vector<float>*  jet_phi=NULL;
  vector<float>* cl_pt=NULL; vector<float>* cl_eta=NULL; vector<float>* cl_phi=NULL;

  //vector<double> jet_pull;

  pair<double,double> pull_vector;   // N.B. first -> eta, second -> phi

  physics->SetBranchAddress("jet_n", &jet_n);
  physics->SetBranchAddress("jet_E", &jet_E);

  physics->SetBranchAddress("jet_pt", &jet_pt);
  physics->SetBranchAddress("jet_eta", &jet_eta);
  physics->SetBranchAddress("jet_phi", &jet_phi);

  physics->SetBranchAddress("cl_n", &cl_n);
  physics->SetBranchAddress("cl_pt", &cl_pt);
  physics->SetBranchAddress("cl_eta", &cl_eta);
  physics->SetBranchAddress("cl_phi", &cl_phi);

  //Book Histograms
  // 1D
  TH1F* h_clu = new TH1F("h_clu", "Number of clusters in a jet", 15, -0.5, 15.5);
  TH1F* h_pull_mag = new TH1F("h_pull_mag", "Pull Magnitude", 20, 0,.4);
  TH1F* h_theta = new TH1F("h_pull", "Pull Angle (#Theta_r)", 20, -TMath::Pi(), TMath::Pi());

  // 2D 
  TH2F* h_eta_phi = new TH2F("h_eta_phi", "Cluster Density in #eta-#phi", 50, -2.5, 2.5, 50, -4, 4);
  TH2F* h_pull_eta_phi = new TH2F("h_pull_eta_phi", "Pull Density in #eta-#phi", 40, -.25, .25, 40, -.25, .25);
  

  for(int i=0; i < nEvents; i++)
    {
      physics->GetEntry(i);

      if(!(i%100))
	printf("Processing event %d\n",i);

      //printf("Read Values from entry %d:\n", i);
      //printf("Jet N: %d, Jet E size: %d\n", jet_n, jet_E->size());
      if(jet_E->size() != jet_n || jet_pt->size() != jet_n || jet_eta->size() != jet_n || jet_phi->size() != jet_n)
	{
	  printf("Jet size mismatch, skipping event %d\n", i);
	  continue;
	}

      //would like to do something like this: 
      //for(vector<float>::iterator jet=jet_eta->begin(); jet != jet_eta->end(); ++jet)
      // but it gets too confusing when you need jet_blah stuff (have to do kludges like jet_blah->at(jet-jet_eta->begin()))
      for(unsigned int j=0; j < jet_n; j++)
	{
	  int n=0;
	  double theta = 0.0;

	  if(fabs(jet_eta->at(j)) > jetEta_cut)
	    continue;
	  for(unsigned int k=0; k < cl_n; k++)
	    {
	      if(cl_pt->size() != cl_n || cl_eta->size() != cl_n || cl_phi->size() != cl_n)
		{
		  printf("Cluster size mismatch, skipping cluster number %d\n", k);
		  continue;
		}

	      if( deltaR(cl_eta->at(k), cl_phi->at(k), jet_eta->at(j), jet_phi->at(j)) < delR_cut )
		{
		  n++;
		  calcJetPull(cl_pt->at(k),jet_pt->at(j),cl_eta->at(k)-jet_eta->at(j),
			      cl_phi->at(k) - jet_phi->at(j), &pull_vector.first, &pull_vector.second);
		  
		  //printf("Found a cluster within Delta R: %'.2f, cluster count: %d, jet #%d\n",delR_cut,n,j);
		  //printf("Current Jet Pull teta: %'.2f, tphi: %'.2f\n", teta,tphi);//(%d, %d,%d) (i, j,k)

		  for(unsigned int l=j; l < jet_n; l++) // getting too many alphabet letters here... need to be more intelligent 
		    {
		      TVector2 delta_jet(jet_eta->at(j)-jet_eta->at(l),
					 jet_phi->at(j)-jet_phi->at(l));
		      //calcTheta(delta_jet, pull_vector);
		      
		      theta=delta_jet.DeltaPhi(TVector2(pull_vector.first,pull_vector.second));
		    }
		}
	      
	    }
	  
	  h_clu->Fill(n);	
	  h_theta->Fill(theta);
	  h_pull_mag->Fill(rMag(pull_vector.first,pull_vector.second));
	  h_eta_phi->Fill(jet_eta->at(j), jet_phi->at(j), n/jet_n);//normalize to the jet number
	  h_pull_eta_phi->Fill(pull_vector.first, pull_vector.second);

	  //printf("Found %d clusters within delR: %'.2f of jet #%d\n",n,delR_cut,j);
	}
    }
  // Print results
  print_histo(h_clu, "cluster_frequency_plot");  
  print_histo(h_pull_mag, "pull_magnitude_plot");
  print_histo(h_theta, "theta_frequency_plot");
  print_2d_histo(h_eta_phi, "cluster_density_plot");
  print_2d_histo(h_pull_eta_phi, "pull_density_plot");


  return 0;
}
