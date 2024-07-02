#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

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
vector<float>   *beamTransfEnergy_GeV;
vector<float>   *beamTargetMass_GeV;
vector<float>   *beamKillPosX_cm;
vector<float>   *beamKillPosY_cm;
vector<float>   *beamKillPosZ_cm;

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
Float_t sensRefPosX_cm;
Float_t sensRefPosY_cm;
Float_t sensRefPosZ_cm;
Float_t sensRefAngl_rad;

void LoadTree(TString fName, TString tName);
void AnaTree();
void SaveHisto(TString fName);
void CleanMem();

TH1F* _h1;
TH1F* _h2;
TH1F* _h3;
TH1F* _h4;
TH1F* _h5;
TH1F* _h6;
TH1F* _h7;
TH1F* _h8;
TH2F* _h9;
TH2F* _h10;
TH2F* _h11;
TH1F* _h12;
TH2F* _h13;
TH2F* _h14;
TH2F* _h15;
TH2F* _h16;
TH2F* _h17;
TH2F* _h18;

const Int_t nTag = 5;

TH1F* _h1_etag[nTag];
TH1F* _h2_etag[nTag];
TH1F* _h3_etag[nTag];
TH1F* _h4_etag[nTag];
TH1F* _h5_etag[nTag];
TH2F* _h6_etag[nTag];
TH2F* _h7_etag[nTag];
TH1F* _h8_etag[nTag];
TH2F* _h9_etag[nTag];

TH1F* _h1_gtag[nTag];
TH1F* _h2_gtag[nTag];
TH1F* _h3_gtag[nTag];
TH1F* _h4_gtag[nTag];
TH1F* _h5_gtag[nTag];
TH2F* _h6_gtag[nTag];
TH2F* _h7_gtag[nTag];
TH1F* _h8_gtag[nTag];
TH2F* _h9_gtag[nTag];
TH2F* _h10_gtag[nTag];

const Double_t protonMass_GeV = 9.3826997280120850e-01; // GeV/c^2

void ana1(TString fNameIn, TString fNameOut)
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	cout<<"[INFO] fNameIn = "<<fNameIn<<" | fNameOut = "<<fNameOut<<endl;
	LoadTree(fNameIn,"tree"); 
	AnaTree();
	SaveHisto(fNameOut);
	CleanMem();

// root -l -b -q ana1.C\(\"/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_ExtendedFarBwd_Tagger1_-500cm_ChamberH_10cm/output_0.root\",\"/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_ExtendedFarBwd_Tagger1_-500cm_ChamberH_10cm/ana1_output_0.root\"\)

	return;
}

void AnaTree()
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	Long64_t nEntries = _tree->GetEntries();
	cout<<"--> nEntries = "<<nEntries<<endl;

	_h1 = new TH1F("h1","beamPosX; cm",1000,-1,1);
	_h2 = new TH1F("h2","beamPosY; cm",1000,-0.1,0.1);
	_h3 = new TH1F("h3","beamPosZ; cm",10000,-100,100);
	_h4 = new TH1F("h4","beamMomX; GeV",1000,-20,20);
	_h5 = new TH1F("h5","beamMomY; GeV",1000,-20,20);
	_h6 = new TH1F("h6","beamMomZ; GeV",1000,-20,20);
	_h7 = new TH1F("h7","beamEnergy; E_{f} [GeV]",1000,-20,20);
	_h8 = new TH1F("h8","beamTransfMomSqr; GeV^2",4000,-2,2);
	_h9 = new TH2F("h9","beamPosX vs beamPosZ;cm;cm",10000,-100,100,1000,-1,1);
	_h10 = new TH2F("h10","beamPosY vs beamPosZ;cm;cm",10000,-100,100,1000,-0.1,0.1);
	_h11 = new TH2F("h11","log(beamTransfMomSqr) vs beamEnergy; E_{f} [GeV]; log_{10}(Q^2)",1000,-20,20,2000,-100,100);
	_h12 = new TH1F("h12","Bjorken scaling variable; x",2000,-100,100);
	_h13 = new TH2F("h13","Beam log_{10}(Q^2) vs log_{10}(x); log_{10}(x); log_{10}(Q^2)",2000,-100,100,2000,-100,100);
	_h14 = new TH2F("h14","Kill PosX vs PosZ; PosZ [cm]; PosX [cm}",7000,-6000,1000,2000,-100,100);
	_h15 = new TH2F("h15","Kill PosY vs PosZ; PosZ [cm]; PosY [cm}",7000,-6000,1000,2000,-100,100);
	_h16 = new TH2F("h16","Kill PosX vs PosZ (Q3eR); PosZ [cm]; PosY [cm}",7000,-6000,1000,2000,-100,100);
	_h17 = new TH2F("h17","Kill PosY vs PosZ (Q3eR); PosZ [cm]; PosY [cm}",7000,-6000,1000,2000,-100,100);
	_h18 = new TH2F("h18","Kill PosY vs PosX (Q3eR); PosX [cm]; PosY [cm}",2000,-100,100,2000,-100,100);

	for(Int_t i = 0; i < nTag; i ++)
	{
		_h1_etag[i] = new TH1F(Form("h1_etag%d",i),Form("Tagger%d electron posX; X [cm]",i),8000,-40,40);
		_h2_etag[i] = new TH1F(Form("h2_etag%d",i),Form("Tagger%d electron posY; Y [cm]",i),8000,-40,40);
		_h3_etag[i] = new TH1F(Form("h3_etag%d",i),Form("Tagger%d electron posZ; Z [cm]",i),50000,-5000,5000);
		_h4_etag[i] = new TH1F(Form("h4_etag%d",i),Form("Tagger%d electron Energy; E_{e} [GeV]",i),20000,0,20);
		_h5_etag[i] = new TH1F(Form("h5_etag%d",i),Form("Tagger%d electron Q^2; Q^2 [GeV^2]",i),4000,-2,2);
		_h6_etag[i] = new TH2F(Form("h6_etag%d",i),Form("Tagger%d electron log_{10}(Q^2) vs Energy; E_{e} [GeV]; log_{10}(Q^2)",i),2000,0,20,2000,-100,100);
		_h7_etag[i] = new TH2F(Form("h7_etag%d",i),Form("Tagger%d electron posY vs posX; X [cm]; Y [cm]",i),8000,-40,40,8000,-40,40);
		_h8_etag[i] = new TH1F(Form("h8_etag%d",i),Form("Tagger%d electron Time; t [#mus]",i),10000,0,1);
		_h9_etag[i] = new TH2F(Form("h9_etag%d",i),Form("Tagger%d photon log_{10}(Q^2) vs log_{10}(x); log_{10}(x); log_{10}(Q^2)",i),2000,-100,100,2000,-100,100);

		_h1_gtag[i] = new TH1F(Form("h1_gtag%d",i),Form("Tagger%d photon posX; X [cm]",i),8000,-40,40);
		_h2_gtag[i] = new TH1F(Form("h2_gtag%d",i),Form("Tagger%d photon posY; Y [cm]",i),8000,-40,40);
		_h3_gtag[i] = new TH1F(Form("h3_gtag%d",i),Form("Tagger%d photon posZ; Z [cm]",i),50000,-5000,5000);
		_h4_gtag[i] = new TH1F(Form("h4_gtag%d",i),Form("Tagger%d photon Energy; E_{ph} [keV]",i),10000,0,1000);
		_h5_gtag[i] = new TH1F(Form("h5_gtag%d",i),Form("Tagger%d photon Q^2; Q^2 [GeV^2]",i),4000,-2,2);
		_h6_gtag[i] = new TH2F(Form("h6_gtag%d",i),Form("Tagger%d photon log_{10}(Q^2) vs Energy; E_{f} [GeV]; log_{10}(Q^2)",i),2000,0,20,2000,-100,100);
		_h7_gtag[i] = new TH2F(Form("h7_gtag%d",i),Form("Tagger%d photon posY vs posX; X [cm]; Y [cm]",i),8000,-40,40,8000,-40,40);
		_h8_gtag[i] = new TH1F(Form("h8_gtag%d",i),Form("Tagger%d photon Time; t [#mus]",i),10000,0,1);
		_h9_gtag[i] = new TH2F(Form("h9_gtag%d",i),Form("Tagger%d photon log_{10}(Q^2) vs log_{10}(x); log_{10}(x); log_{10}(Q^2)",i),2000,-100,100,2000,-100,100);
		_h10_gtag[i] = new TH2F(Form("h10_gtag%d",i),Form("Tagger%d photon posY vs posX; X [cm]; Y [cm]; E_{#gamma} [GeV]",i),8000,-40,40,8000,-40,40);
	}

	// loop over entries
	for(Int_t iEntry = 0; iEntry < nEntries; iEntry++)
	{
		// load an entry
		_tree->GetEntry(iEntry);
		TVector3 center(sensRefPosX_cm,sensRefPosY_cm,sensRefPosZ_cm);
		// should be only one primary
		if(beamPdg->size() != 1 || beamKillPosZ_cm->size() != 1){cout<<endl<<"[ERROR] More than one primary!"<<endl;exit(-1);}
		// loop over primaries
		for(Int_t i = 0; i < beamPdg->size(); i++)
		{
			if(beamTransfEnergy_GeV->at(i) <= 0){continue;}
			// fill histos with primary coordinates
			_h1->Fill(beamPosX_cm->at(i));
			_h2->Fill(beamPosY_cm->at(i));
			_h3->Fill(beamPosZ_cm->at(i));
			_h4->Fill(beamMomX_eV->at(i)/1e9);
			_h5->Fill(beamMomY_eV->at(i)/1e9);
			_h6->Fill(beamMomZ_eV->at(i)/1e9);
			_h7->Fill(beamEnergy_eV->at(i)/1e9);
			_h8->Fill(beamTransfMomSqr_GeV2->at(i));
			_h9->Fill(beamPosZ_cm->at(i),beamPosX_cm->at(i));
			_h10->Fill(beamPosZ_cm->at(i),beamPosY_cm->at(i));
			_h11->Fill(beamEnergy_eV->at(i)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(i)));
			_h12->Fill(beamTransfMomSqr_GeV2->at(i)/(2.*protonMass_GeV*beamTransfEnergy_GeV->at(i)));
			_h13->Fill(	TMath::Log10(beamTransfMomSqr_GeV2->at(i)/(2.*protonMass_GeV*beamTransfEnergy_GeV->at(i))),
					TMath::Log10(beamTransfMomSqr_GeV2->at(i)));
			_h14->Fill(beamKillPosZ_cm->at(i),beamKillPosX_cm->at(i));
			_h15->Fill(beamKillPosZ_cm->at(i),beamKillPosY_cm->at(i));

			if(beamKillPosZ_cm->at(i) < -4474)
			{
				_h16->Fill(beamKillPosZ_cm->at(i),beamKillPosX_cm->at(i));
				_h17->Fill(beamKillPosZ_cm->at(i),beamKillPosY_cm->at(i));
				_h18->Fill(beamKillPosX_cm->at(i),beamKillPosY_cm->at(i));
			}

		}
		// loop over all hits on the taggers
		for(Int_t i = 0; i < sensName->size(); i++)
		{
			// get hit coordinates in the global coord.syst.
			TVector3 pos(sensPosX_cm->at(i),sensPosY_cm->at(i),sensPosZ_cm->at(i));
			// translate to the origin
			pos = pos - center;
			// rotate around Y-axis: 20 mrad - B2eR bend angle
			pos.RotateY(-sensRefAngl_rad);
			// fill histos
			if(sensPDG->at(i) == 11) // electrons
			{
				for(Int_t j = 0; j < nTag; j++)
				{
					if(sensName->at(i) == Form("sens_tag%d",j+1))
					{
						_h1_etag[j]->Fill(pos.x());
						_h2_etag[j]->Fill(pos.y());
						_h3_etag[j]->Fill(pos.z());
						_h4_etag[j]->Fill(sensEnergy_eV->at(i)/1e9);
						_h5_etag[j]->Fill(beamTransfMomSqr_GeV2->at(0));
						_h6_etag[j]->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
						_h7_etag[j]->Fill(pos.x(),pos.y());
						_h8_etag[j]->Fill(sensTime_ms->at(i)*1e3);
						_h9_etag[j]->Fill(
							TMath::Log10(beamTransfMomSqr_GeV2->at(0)/(2.*protonMass_GeV*beamTransfEnergy_GeV->at(0))),
							TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
					}
				}
			}		
			else if(sensPDG->at(i) == 22) // photons
			{

				for(Int_t j = 0; j < nTag; j++)
				{
					if(sensName->at(i) == Form("sens_tag%d",j))
					{
						_h1_gtag[j]->Fill(pos.x());
						_h2_gtag[j]->Fill(pos.y());
						_h3_gtag[j]->Fill(pos.z());
						_h4_gtag[j]->Fill(sensEnergy_eV->at(i)/1e3);
						_h5_gtag[j]->Fill(beamTransfMomSqr_GeV2->at(0));
						_h6_gtag[j]->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
						_h7_gtag[j]->Fill(pos.x(),pos.y());
						_h8_gtag[j]->Fill(sensTime_ms->at(i)*1e3);
						_h9_gtag[j]->Fill(
							TMath::Log10(beamTransfMomSqr_GeV2->at(0)/(2.*protonMass_GeV*beamTransfEnergy_GeV->at(0))),
							TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
						_h10_gtag[j]->Fill(pos.x(),pos.y(),sensEnergy_eV->at(i));
					}
				}
			}		
			else
			{
				cout<<endl<<"[ERROR] Wrong particle PDG code: '"<<sensPDG->at(i)<<"' !"<<endl;
				exit(-1);
			}
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
	_h2->Write();
	_h3->Write();
	_h4->Write();
	_h5->Write();
	_h6->Write();
	_h7->Write();
	_h8->Write();
	_h9->Write();
	_h10->Write();
	_h11->Write();
	_h12->Write();
	_h13->Write();
	_h14->Write();
	_h15->Write();
	_h16->Write();
	_h17->Write();
	_h18->Write();

	for(Int_t i = 0; i < nTag; i++)
	{
		_h1_etag[i]->Write();
		_h2_etag[i]->Write();
		_h3_etag[i]->Write();
		_h4_etag[i]->Write();
		_h5_etag[i]->Write();
		_h6_etag[i]->Write();
		_h7_etag[i]->Write();
		_h8_etag[i]->Write();
		_h9_etag[i]->Write();

		_h1_gtag[i]->Write();
		_h2_gtag[i]->Write();
		_h3_gtag[i]->Write();
		_h4_gtag[i]->Write();
		_h5_gtag[i]->Write();
		_h6_gtag[i]->Write();
		_h7_gtag[i]->Write();
		_h8_gtag[i]->Write();
		_h9_gtag[i]->Write();
		_h10_gtag[i]->Write();
	}

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
	beamTargetMass_GeV = 0;
	beamKillPosX_cm = 0;
	beamKillPosY_cm = 0;
	beamKillPosZ_cm = 0;

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
	_tree->SetBranchAddress("beamTransfEnergy_GeV", &beamTransfEnergy_GeV);
	_tree->SetBranchAddress("beamTargetMass_GeV", &beamTargetMass_GeV);
	_tree->SetBranchAddress("beamKillPosX_cm", &beamKillPosX_cm);
	_tree->SetBranchAddress("beamKillPosY_cm", &beamKillPosY_cm);
	_tree->SetBranchAddress("beamKillPosZ_cm", &beamKillPosZ_cm);

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
	_tree->SetBranchAddress("sensRefPosX_cm",&sensRefPosX_cm);
	_tree->SetBranchAddress("sensRefPosY_cm",&sensRefPosY_cm);
	_tree->SetBranchAddress("sensRefPosZ_cm",&sensRefPosZ_cm);
	_tree->SetBranchAddress("sensRefAngl_rad",&sensRefAngl_rad);

	return;
}

void CleanMem()
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	if (!_tree) return;
	delete _tree->GetCurrentFile();
	return;
}

