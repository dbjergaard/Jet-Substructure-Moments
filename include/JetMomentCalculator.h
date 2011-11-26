// -*-c++-*-
#include <iostream>

class JetMomentCalculator 
{
  double teta,tphi;
  //std::pair<double,double> pull_vector;   // N.B. first -> eta, second -> phi
public:
  JetMomentCalculator();
  ~JetMomentCalculator();

  double rMag(double eta,double phi);
  double deltaR(double eta1, double phi1, double eta2, double phi2);
  void calcJetPull(double cl_pt, double jet_pt, 
		   double reta, double rphi);//double* teta, double* tphi
  double getEtaComponent(){return teta;}
  double getPhiComponent(){return tphi;}
};
