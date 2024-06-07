#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "string"

TTree *_tree;
	// Beam
   vector<int>     *beamPdg;
   vector<float>   *beamPosX_cm;
   vector<float>   *beamPosY_cm;
   vector<float>   *beamPosZ_cm;
   vector<float>   *beamMomX_eV;
   vector<float>   *beamMomY_eV;
   vector<float>   *beamMomZ_eV;
   vector<float>   *beamEnergy_eV;
   vector<float>   *beamTransfMomSqr_GeV2;
	// Taggers
   vector<string>  *sensName;
   vector<int>     *sensPDG;
   vector<float>   *sensPosX_cm;
   vector<float>   *sensPosY_cm;
   vector<float>   *sensPosZ_cm;
   vector<float>   *sensMomX_eV;
   vector<float>   *sensMomY_eV;
   vector<float>   *sensMomZ_eV;
   vector<float>   *sensEnergy_eV;
   vector<float>   *sensTime_ms;

void LoadTree(TString fName, TString tName);
void AnaTree();
void SaveHisto(TString fName);
void CleanMem();

TH1D* _h1;

void ana1()
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	TString fName = "/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm/output_-490.root";
	LoadTree(fName,"tree"); 
	AnaTree();
	SaveHisto("output.root");
	CleanMem();

	return;
}

void AnaTree()
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	Long64_t nEntries = _tree->GetEntries();
	cout<<"--> nEntries = "<<nEntries<<endl;

	_h1 = new TH1D("h1","beamPosX; cm",1000,-1,1);
	_h2 = new TH1D("h2","beamPosY; cm",1000,-1,1);
	_h3 = new TH1D("h3","beamPosZ; cm",1000,-1,1);
	_h4 = new TH1D("h4","beamMomX; GeV",1000,-20,20);
	_h5 = new TH1D("h5","beamMomY; GeV",1000,-20,20);
	_h6 = new TH1D("h6","beamMomZ; GeV",1000,-20,20);
	_h7 = new TH1D("h7","beamEnergy; GeV",1000,-20,20);
	_h8 = new TH1D("h8","beamTransfMomSqr_GeV2; GeV^2",1000,-20,20);

	for(Int_t iEntry = 0; iEntry < nEntries; iEntry++)
	{
		_tree->GetEntry(iEntry);
		for(Int_t i = 0; i < beamPdg->size(); i++)
		{
			_h1->Fill(beamPosX_cm->at(i));
		}
	}

	return;
}

void SaveHisto(TString fName)
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	TFile* f = new TFile(fName.Data(),"recreate");
	f->cd();
	_h1->Write();
	f->Close();
	cout<<"--> Output file name: "<<f->GetName()<<endl;

	return;
}

void LoadTree(TString fName, TString tName)
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fName.Data());
	if (!f || !f->IsOpen())
		f = new TFile(fName.Data());
	f->GetObject(tName.Data(),_tree);

   // Set object pointer
   beamPdg = 0;
   beamPosX_cm = 0;
   beamPosY_cm = 0;
   beamPosZ_cm = 0;
   beamMomX_eV = 0;
   beamMomY_eV = 0;
   beamMomZ_eV = 0;
   beamEnergy_eV = 0;
   beamTransfMomSqr_GeV2 = 0;

   sensName = 0;
   sensPDG = 0;
   sensPosX_cm = 0;
   sensPosY_cm = 0;
   sensPosZ_cm = 0;
   sensMomX_eV = 0;
   sensMomY_eV = 0;
   sensMomZ_eV = 0;
   sensEnergy_eV = 0;
   sensTime_ms = 0;

   // Set branch addresses and branch pointers
   if (!_tree) return;

   _tree->SetBranchAddress("beamPdg", &beamPdg);
   _tree->SetBranchAddress("beamPosX_cm", &beamPosX_cm);
   _tree->SetBranchAddress("beamPosY_cm", &beamPosY_cm);
   _tree->SetBranchAddress("beamPosZ_cm", &beamPosZ_cm);
   _tree->SetBranchAddress("beamMomX_eV", &beamMomX_eV);
   _tree->SetBranchAddress("beamMomY_eV", &beamMomY_eV);
   _tree->SetBranchAddress("beamMomZ_eV", &beamMomZ_eV);
   _tree->SetBranchAddress("beamEnergy_eV", &beamEnergy_eV);
   _tree->SetBranchAddress("beamTransfMomSqr_GeV2", &beamTransfMomSqr_GeV2);

   _tree->SetBranchAddress("sensName", &sensName);
   _tree->SetBranchAddress("sensPDG", &sensPDG);
   _tree->SetBranchAddress("sensPosX_cm", &sensPosX_cm);
   _tree->SetBranchAddress("sensPosY_cm", &sensPosY_cm);
   _tree->SetBranchAddress("sensPosZ_cm", &sensPosZ_cm);
   _tree->SetBranchAddress("sensMomX_eV", &sensMomX_eV);
   _tree->SetBranchAddress("sensMomY_eV", &sensMomY_eV);
   _tree->SetBranchAddress("sensMomZ_eV", &sensMomZ_eV);
   _tree->SetBranchAddress("sensEnergy_eV", &sensEnergy_eV);
   _tree->SetBranchAddress("sensTime_ms", &sensTime_ms);

	return;
}

void CleanMem()
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	if (!_tree) return;
	delete _tree->GetCurrentFile();
	return;
}

