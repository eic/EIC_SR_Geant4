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

TH1F* _h1_etag1;
TH1F* _h2_etag1;
TH1F* _h3_etag1;
TH1F* _h4_etag1;
TH1F* _h5_etag1;
TH2F* _h6_etag1;
TH2F* _h7_etag1;
TH1F* _h8_etag1;

TH1F* _h1_etag2;
TH1F* _h2_etag2;
TH1F* _h3_etag2;
TH1F* _h4_etag2;
TH1F* _h5_etag2;
TH2F* _h6_etag2;
TH2F* _h7_etag2;
TH1F* _h8_etag2;

TH1F* _h1_gtag1;
TH1F* _h2_gtag1;
TH1F* _h3_gtag1;
TH1F* _h4_gtag1;
TH1F* _h5_gtag1;
TH2F* _h6_gtag1;
TH2F* _h7_gtag1;
TH1F* _h8_gtag1;

TH1F* _h1_gtag2;
TH1F* _h2_gtag2;
TH1F* _h3_gtag2;
TH1F* _h4_gtag2;
TH1F* _h5_gtag2;
TH2F* _h6_gtag2;
TH2F* _h7_gtag2;
TH1F* _h8_gtag2;

void ana1(TString fNameIn, TString fNameOut)
{
	cout<<endl<<"######################## Func: "<<__FUNCTION__<<endl;
	cout<<"[INFO] fNameIn = "<<fNameIn<<" | fNameOut = "<<fNameOut<<endl;
	LoadTree(fNameIn,"tree"); 
	AnaTree();
	SaveHisto(fNameOut);
	CleanMem();

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
	_h11 = new TH2F("h11","log(beamTransfMomSqr) vs beamEnergy; E_{f} [GeV]; log(Q^2) [GeV^2]",1000,-20,20,8000,-40,40);

	_h1_etag1 = new TH1F("h1_etag1","Tagger1 electron posX; X [cm]",8000,-40,40);
	_h2_etag1 = new TH1F("h2_etag1","Tagger1 electron posY; Y [cm]",8000,-40,40);
	_h3_etag1 = new TH1F("h3_etag1","Tagger1 electron posZ; Z [cm]",50000,-5000,5000);
	_h4_etag1 = new TH1F("h4_etag1","Tagger1 electron Energy; E_{e} [GeV]",20000,0,20);
	_h5_etag1 = new TH1F("h5_etag1","Tagger1 electron Q^2; Q^2 [GeV^2]",4000,-2,2);
	_h6_etag1 = new TH2F("h6_etag1","Tagger1 electron log(Q^2) vs Energy; E_{e} [GeV]; log(Q^2) [GeV^2]",2000,0,20,8000,-40,40);
	_h7_etag1 = new TH2F("h7_etag1","Tagger1 electron posY vs posX; X [cm]; Y [cm]",8000,-40,40,8000,-40,40);
	_h8_etag1 = new TH1F("h8_etag1","Tagger1 electron Time; t [#mus]",10000,0,1);

	_h1_etag2 = new TH1F("h1_etag2","Tagger2 electron posX; X [cm]",8000,-40,40);
	_h2_etag2 = new TH1F("h2_etag2","Tagger2 electron posY; Y [cm]",8000,-40,40);
	_h3_etag2 = new TH1F("h3_etag2","Tagger2 electron posZ; Z [cm]",50000,-5000,5000);
	_h4_etag2 = new TH1F("h4_etag2","Tagger2 electron Energy; E_{e} [GeV]",20000,0,20);
	_h5_etag2 = new TH1F("h5_etag2","Tagger2 electron Q^2; Q^2 [GeV^2]",4000,-2,2);
	_h6_etag2 = new TH2F("h6_etag2","Tagger2 electron log(Q^2) vs Energy; E_{e} [GeV]; log(Q^2) [GeV^2]",2000,0,20,8000,-40,40);
	_h7_etag2 = new TH2F("h7_etag2","Tagger2 electron posY vs posX; X [cm]; Y [cm]",8000,-40,40,8000,-40,40);
	_h8_etag2 = new TH1F("h8_etag2","Tagger2 electron Time; t [#mus]",10000,0,1);

	_h1_gtag1 = new TH1F("h1_gtag1","Tagger1 photon posX; X [cm]",8000,-40,40);
	_h2_gtag1 = new TH1F("h2_gtag1","Tagger1 photon posY; Y [cm]",8000,-40,40);
	_h3_gtag1 = new TH1F("h3_gtag1","Tagger1 photon posZ; Z [cm]",50000,-5000,5000);
	_h4_gtag1 = new TH1F("h4_gtag1","Tagger1 photon Energy; E_{ph} [keV]",10000,0,1000);
	_h5_gtag1 = new TH1F("h5_gtag1","Tagger1 photon Q^2; Q^2 [GeV^2]",4000,-2,2);
	_h6_gtag1 = new TH2F("h6_gtag1","Tagger1 photon log(Q^2) vs Energy; E_{f} [GeV]; log(Q^2) [GeV^2]",2000,0,20,8000,-40,40);
	_h7_gtag1 = new TH2F("h7_gtag1","Tagger1 photon posY vs posX; X [cm]; Y [cm]",8000,-40,40,8000,-40,40);
	_h8_gtag1 = new TH1F("h8_gtag1","Tagger1 photon Time; t [#mus]",10000,0,1);

	_h1_gtag2 = new TH1F("h1_gtag2","Tagger2 photon posX; X [cm]",8000,-40,40);
	_h2_gtag2 = new TH1F("h2_gtag2","Tagger2 photon posY; Y [cm]",8000,-40,40);
	_h3_gtag2 = new TH1F("h3_gtag2","Tagger2 photon posZ; Z [cm]",50000,-5000,5000);
	_h4_gtag2 = new TH1F("h4_gtag2","Tagger2 photon Energy; E_{ph} [keV]",10000,0,1000);
	_h5_gtag2 = new TH1F("h5_gtag2","Tagger2 photon Q^2; Q^2 [GeV^2]",4000,-2,2);
	_h6_gtag2 = new TH2F("h6_gtag2","Tagger2 photon log(Q^2) vs Energy; E_{f} [GeV]; log(Q^2) [GeV^2]",2000,0,20,8000,-40,40);
	_h7_gtag2 = new TH2F("h7_gtag2","Tagger2 photon posY vs posX; X [cm]; Y [cm]",8000,-40,40,8000,-40,40);
	_h8_gtag2 = new TH1F("h8_gtag2","Tagger2 photon Time; t [#mus]",10000,0,1);

	// loop over entries
	for(Int_t iEntry = 0; iEntry < nEntries; iEntry++)
	{
		// load an entry
		_tree->GetEntry(iEntry);
		TVector3 center(sensRefPosX_cm,sensRefPosY_cm,sensRefPosZ_cm);
		// should be only one primary
		if(beamPdg->size() != 1){cout<<endl<<"[ERROR] More than one primary!"<<endl;exit(-1);}
		// loop over primaries
		for(Int_t i = 0; i < beamPdg->size(); i++)
		{
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
				if(sensName->at(i) == "sens_tag1") // tagger1
				{
					_h1_etag1->Fill(pos.x());
					_h2_etag1->Fill(pos.y());
					_h3_etag1->Fill(pos.z());
					_h4_etag1->Fill(sensEnergy_eV->at(i)/1e9);
					_h5_etag1->Fill(beamTransfMomSqr_GeV2->at(0));
					_h6_etag1->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
					_h7_etag1->Fill(pos.x(),pos.y());
					_h8_etag1->Fill(sensTime_ms->at(i)*1e3);
				}
				else if(sensName->at(i) == "sens_tag2") // tagger2
				{
					_h1_etag2->Fill(pos.x());
					_h2_etag2->Fill(pos.y());
					_h3_etag2->Fill(pos.z());
					_h4_etag2->Fill(sensEnergy_eV->at(i)/1e9);
					_h5_etag2->Fill(beamTransfMomSqr_GeV2->at(0));
					_h6_etag2->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
					_h7_etag2->Fill(pos.x(),pos.y());
					_h8_etag2->Fill(sensTime_ms->at(i)*1e3);
				}
				else
				{
					cout<<endl<<"[ERROR] Wrong sensor name: '"<<sensName->at(i)<<"' !"<<endl;
					exit(-1);
				}
			}		
			else if(sensPDG->at(i) == 22) // photons
			{
				if(sensName->at(i) == "sens_tag1") // tagger1
				{
					_h1_gtag1->Fill(pos.x());
					_h2_gtag1->Fill(pos.y());
					_h3_gtag1->Fill(pos.z());
					_h4_gtag1->Fill(sensEnergy_eV->at(i)/1e3);
					_h5_gtag1->Fill(beamTransfMomSqr_GeV2->at(0));
					_h6_gtag1->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
					_h7_gtag1->Fill(pos.x(),pos.y());
					_h8_gtag1->Fill(sensTime_ms->at(i)*1e3);
				}
				else if(sensName->at(i) == "sens_tag2") // tagger2
				{
					_h1_gtag2->Fill(pos.x());
					_h2_gtag2->Fill(pos.y());
					_h3_gtag2->Fill(pos.z());
					_h4_gtag2->Fill(sensEnergy_eV->at(i)/1e3);
					_h5_gtag2->Fill(beamTransfMomSqr_GeV2->at(0));
					_h6_gtag2->Fill(beamEnergy_eV->at(0)/1e9,TMath::Log10(beamTransfMomSqr_GeV2->at(0)));
					_h7_gtag2->Fill(pos.x(),pos.y());
					_h8_gtag2->Fill(sensTime_ms->at(i)*1e3);
				}
				else
				{
					cout<<endl<<"[ERROR] Wrong sensor name: '"<<sensName->at(i)<<"' !"<<endl;
					exit(-1);
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
	_h1_etag1->Write();
	_h2_etag1->Write();
	_h3_etag1->Write();
	_h4_etag1->Write();
	_h5_etag1->Write();
	_h6_etag1->Write();
	_h7_etag1->Write();
	_h8_etag1->Write();
	_h1_etag2->Write();
	_h2_etag2->Write();
	_h3_etag2->Write();
	_h4_etag2->Write();
	_h5_etag2->Write();
	_h6_etag2->Write();
	_h7_etag2->Write();
	_h8_etag2->Write();
	_h1_gtag1->Write();
	_h2_gtag1->Write();
	_h3_gtag1->Write();
	_h4_gtag1->Write();
	_h5_gtag1->Write();
	_h6_gtag1->Write();
	_h7_gtag1->Write();
	_h8_gtag1->Write();
	_h1_gtag2->Write();
	_h2_gtag2->Write();
	_h3_gtag2->Write();
	_h4_gtag2->Write();
	_h5_gtag2->Write();
	_h6_gtag2->Write();
	_h7_gtag2->Write();
	_h8_gtag2->Write();

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

