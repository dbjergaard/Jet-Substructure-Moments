#include "../include/JetMomentCalculator.h"
#include "TMath.h"

double JetMomentCalculator::rMag(double eta,double phi){ return TMath::Sqrt(eta*eta + phi*phi);}

double JetMomentCalculator::deltaR(double eta1, double phi1, double eta2, double phi2)
{return rMag(eta1-eta2,phi1-phi2); }

JetMomentCalculator::JetMomentCalculator()
  :teta(0.0),tphi(0.0)
{ /*do nothing yet*/ }
JetMomentCalculator::~JetMomentCalculator()
{/* Blow up and die here */}
void JetMomentCalculator::calcJetPull(double cl_pt, double jet_pt, 
		 double reta, double rphi)
{
  // Takes in pt_i, jet_pt, r_i and returns t, the pull vector.  
  // r_i and t are arrays of length three
  double alpha = ((cl_pt*rMag(reta, rphi))/jet_pt);
  teta+=alpha*reta;
  tphi+=alpha*rphi;
  return;
}
