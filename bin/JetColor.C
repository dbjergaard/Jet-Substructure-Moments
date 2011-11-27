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
#include "TChain.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TVector2.h"

// User Includes
#include "../include/JetMomentCalculator.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::map;

void usage(const char* name)
{
  printf("Usage: %s [args] [[file:] data1.root ... dataN.root]\n", name);
  printf("\t [args] can be one of the following:\n");
  printf("\t -o outName :  Output file name \n");
  printf("\t -h : Display this help message \n");
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
  TFile* outFile=NULL;
  TChain* physics = new TChain("physics");
  //parse command line args 
  for(int i=0; i < argc; i++)
    {
      string arg(argv[i]);
      if(arg == "-h" || arg == "--help")
	{
	  usage(argv[0]);
	  return -1;
	}
      else if(arg == "-o") //output file name
	{
	  printf("Writing output products to: %s \n", argv[i+1]);
	  outFile = new TFile(argv[i+1],"RECREATE");
	}
      else if( i > 1 && string(argv[i-1]) != "-o" && arg.compare(arg.size()-5,5,".root")==0)
	{
	  printf("Adding %s to data chain\n", argv[i]);
	  physics->Add(argv[i]);
	}
    }
  Int_t nEvents = physics->GetEntries();
  printf("Found %d events... Looping\n",nEvents);
  unsigned int jet_n = 0, cl_n = 0;
  vector<float>* jet_E=NULL; vector<float>* jet_pt=NULL; vector<float>* jet_eta=NULL; vector<float>*  jet_phi=NULL;
  vector<float>* cl_pt=NULL; vector<float>* cl_eta=NULL; vector<float>* cl_phi=NULL;

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
  map<string,TH1F*> Histos1D;
  Histos1D["h_clu"]       = new TH1F("h_clu", "Number of clusters in a jet", 15, -0.5, 15.5);
  Histos1D["h_pull_mag"]	= new TH1F("h_pull_mag", "Pull Magnitude", 20, 0,.4);
  Histos1D["h_theta"]	= new TH1F("h_pull", "Pull Angle (#Theta_r)", 20, -TMath::Pi(), TMath::Pi());

  // 2D 
  map<string,TH2F*> Histos2D;
  Histos2D["h_eta_phi"] = new TH2F("h_eta_phi", "Cluster Density in #eta-#phi", 50, -2.5, 2.5, 50, -4, 4);
  Histos2D["h_pull_eta_phi"] = new TH2F("h_pull_eta_phi", "Pull Density in #eta-#phi", 40, -.25, .25, 40, -.25, .25);
  

  for(int i=0; i < nEvents; i++)
    {
      physics->GetEntry(i);
      JetMomentCalculator jetCalc;

      if(!(i%100))
	printf("Processing event %d\n",i);

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

	      if( jetCalc.deltaR(cl_eta->at(k), cl_phi->at(k), jet_eta->at(j), jet_phi->at(j)) < delR_cut )
		{
		  n++;
		  jetCalc.calcJetPull(cl_pt->at(k),jet_pt->at(j),cl_eta->at(k)-jet_eta->at(j),
				      cl_phi->at(k) - jet_phi->at(j));

		  for(unsigned int l=j; l < jet_n; l++) // getting too many alphabet letters here... need to be more intelligent 
		    {
		      TVector2 delta_jet(jet_eta->at(j)-jet_eta->at(l),
					 jet_phi->at(j)-jet_phi->at(l));

		      theta=delta_jet.DeltaPhi(TVector2(jetCalc.getEtaComponent(),
						    jetCalc.getPhiComponent()));
		    }
		}
	    }
	  Histos1D["h_clu"]->Fill(n);	
	  Histos1D["h_theta"]->Fill(theta);
	  Histos1D["h_pull_mag"]->Fill(jetCalc.rMag(jetCalc.getEtaComponent(),jetCalc.getPhiComponent()));
	  Histos2D["h_eta_phi"]->Fill(jet_eta->at(j), jet_phi->at(j), (n+0.0)/(jet_n+0.0));//normalize to the jet number
	  Histos2D["h_pull_eta_phi"]->Fill(jetCalc.getEtaComponent(),jetCalc.getPhiComponent());
	}
    }
  outFile->Write();
  return 0;
}
