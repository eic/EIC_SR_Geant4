#include "AnalysisSR.hh"

AnalysisSR::AnalysisSR() : _fChain(0) 
{}

AnalysisSR::~AnalysisSR()
{
   	if (!_fChain) return;
   	delete _fChain->GetCurrentFile();
}

void AnalysisSR::Init(TString treeName = "tree")
{
	_fChain = new TChain(treeName.Data());
   	if(!_fChain){return;};
	_fChain->Add(_inputFileName.Data());

	//_fChain->GetListOfFiles()

   	// Set object pointer
   	gammaPosX_cm = 0;
   	gammaPosY_cm = 0;
   	gammaPosZ_cm = 0;
   	gammaMomX_eV = 0;
   	gammaMomY_eV = 0;
   	gammaMomZ_eV = 0;
   	gammaEnergy_eV = 0;
   	gammaTime_ms = 0;
   
	// Set branch addresses and branch pointers

   	_fChain->SetBranchAddress("event", &event, &b_event);
   	_fChain->SetBranchAddress("beamPdg", &beamPdg, &b_beamPdg);
   	_fChain->SetBranchAddress("beamPosX_cm", &beamPosX_cm, &b_beamPosX_cm);
   	_fChain->SetBranchAddress("beamPosY_cm", &beamPosY_cm, &b_beamPosY_cm);
   	_fChain->SetBranchAddress("beamPosZ_cm", &beamPosZ_cm, &b_beamPosZ_cm);
   	_fChain->SetBranchAddress("beamMomX_eV", &beamMomX_eV, &b_beamMomX_eV);
   	_fChain->SetBranchAddress("beamMomY_eV", &beamMomY_eV, &b_beamMomY_eV);
   	_fChain->SetBranchAddress("beamMomZ_eV", &beamMomZ_eV, &b_beamMomZ_eV);
   	_fChain->SetBranchAddress("beamEnergy_eV", &beamEnergy_eV, &b_beamEnergy_eV);
   	_fChain->SetBranchAddress("gammaPosX_cm", &gammaPosX_cm, &b_gammaPosX_cm);
   	_fChain->SetBranchAddress("gammaPosY_cm", &gammaPosY_cm, &b_gammaPosY_cm);
   	_fChain->SetBranchAddress("gammaPosZ_cm", &gammaPosZ_cm, &b_gammaPosZ_cm);
   	_fChain->SetBranchAddress("gammaMomX_eV", &gammaMomX_eV, &b_gammaMomX_eV);
   	_fChain->SetBranchAddress("gammaMomY_eV", &gammaMomY_eV, &b_gammaMomY_eV);
   	_fChain->SetBranchAddress("gammaMomZ_eV", &gammaMomZ_eV, &b_gammaMomZ_eV);
   	_fChain->SetBranchAddress("gammaEnergy_eV", &gammaEnergy_eV, &b_gammaEnergy_eV);
   	_fChain->SetBranchAddress("gammaTime_ms", &gammaTime_ms, &b_gammaTime_ms);

	return;
}

void AnalysisSR::Loop()
{
  	if(!_fChain){return;}

   	Long64_t nEntries = _fChain->GetEntries();
	cout<<"[INFO] AnalysisSR::Loop ==> nEntries = "<<nEntries<<endl;

	Long64_t jentry = 0;
   	while(jentry < nEntries) 
	{
		_fChain->GetEntry(jentry);
		
		if(jentry%1000 == 0)
        	{
           	 	printf("\r[INFO] Progress: %.3f [%%]",100.0*jentry/nEntries);
            		fflush(stdout);
        	}
		jentry++;
   	}
 	printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jentry/nEntries);

	return;
}
