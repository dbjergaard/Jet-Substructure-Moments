#include <TFile.h>
#include <TH1F.h>
#include <THStack.h>
#include <TString.h>
#include <string>
#include <vector>

//Recursive iterator for processing root files
void KeyIterator( TDirectory *target, TFile *file, std::string Name, const int mode);
//void KeyIterator( TDirectory *target, TFile *file, string Name);

//Take care of setting up and painting canvases
void painter(std::vector<TH1*> histos, std::vector<THStack*> stack_histos, TString dir_name, const std::string Name, int mode=0);

//Display help and return
void HistoMaker();

// Worker function
void HistoMaker(const std::string FileName, const std::string Name, const int mode);
