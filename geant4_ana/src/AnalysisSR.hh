#ifndef AnalysisSR_hh
#define AnalysisSR_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>

#include "vector"
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"

using namespace HepMC3;

class AnalysisSR {
public :
  	AnalysisSR();
	virtual ~AnalysisSR();

   	// Declaration of leaf types
	Int_t           event;
	Int_t           beamPdg;
	Float_t         beamPosX_cm;
	Float_t         beamPosY_cm;
	Float_t         beamPosZ_cm;
	Float_t         beamMomX_eV;
	Float_t         beamMomY_eV;
	Float_t         beamMomZ_eV;
	Float_t         beamEnergy_eV;
	vector<float>   *gammaPosX_cm;
	vector<float>   *gammaPosY_cm;
	vector<float>   *gammaPosZ_cm;
	vector<float>   *gammaMomX_eV;
	vector<float>   *gammaMomY_eV;
	vector<float>   *gammaMomZ_eV;
	vector<float>   *gammaEnergy_eV;
	vector<float>   *gammaTime_ms;

   	// List of branches
	TBranch        *b_event;   //!
	TBranch        *b_beamPdg;   //!
	TBranch        *b_beamPosX_cm;   //!
	TBranch        *b_beamPosY_cm;   //!
	TBranch        *b_beamPosZ_cm;   //!
	TBranch        *b_beamMomX_eV;   //!
	TBranch        *b_beamMomY_eV;   //!
	TBranch        *b_beamMomZ_eV;   //!
	TBranch        *b_beamEnergy_eV;   //!
	TBranch        *b_gammaPosX_cm;   //!
	TBranch        *b_gammaPosY_cm;   //!
	TBranch        *b_gammaPosZ_cm;   //!
	TBranch        *b_gammaMomX_eV;   //!
	TBranch        *b_gammaMomY_eV;   //!
	TBranch        *b_gammaMomZ_eV;   //!
	TBranch        *b_gammaEnergy_eV;   //!
	TBranch        *b_gammaTime_ms;   //!

 	void Init(TString);
	void Loop();
	void SetInputFileName(TString val){_inputFileName = val; return;}
	TString GetInputFileName(){return _inputFileName;}
	void SetOutputFileNameRoot(TString val){_outputFileName_root = val; return;}
	TString GetOutputFileNameRoot(){return _outputFileName_root;}
	void SetOutputFileNameHepmc(TString val){_outputFileName_hepmc = val; return;}
	TString GetOutputFileNameHepmc(){return _outputFileName_hepmc;}

private:
	TString _inputFileName;
	TString _outputFileName_root;
	TString _outputFileName_hepmc;
   	TChain* _fChain;
	WriterAscii* _fHepmc;

	TH1F* _h1;
	TH2F* _h2;
};
#endif
