void getSimpleHisto(TString dirName = "/gpfs02/eic/anatochii/output_g4ana_v6.3_nosrmask_loglog", Int_t nFiles = 10000)
{
	cout<<"[INFO] dirName = "<<dirName<<" | nFiles = "<<nFiles<<endl;
	Long64_t nEntries = 0;
	TH1F* h1_com = new TH1F("h1_com","SR photon energy;E_{#gamma} [eV];# of photons",101000,-1000,100000);
	TH2F* h2_com = new TH2F("h2_com","SR photon position;Z_{#gamma} [cm];X_{#gamma} [cm]",8000,-4000,4000,2000,-100,100);
	TH2F* h21_com = new TH2F("h21_com","SR photon position vs energy;Z_{#gamma} [cm];X_{#gamma} [cm];E_{#gamma} [eV]",8000,-4000,4000,2000,-100,100);

	for(Int_t i = 0; i < nFiles; i++)
	{
		TFile *_file0 = TFile::Open(Form("%s/output_ana_%d.root",dirName.Data(),i));
		if(!_file0){continue;}
		cout<<"[FILE] "<<_file0->GetName()<<endl;
		TH1F* h1 = (TH1F*)_file0->Get("h1");
		TH2F* h2 = (TH2F*)_file0->Get("h2");
		TH2F* h21 = (TH2F*)_file0->Get("h21");
		nEntries += h1->GetEntries();
		h1_com->Add(h1);
		h2_com->Add(h2);
		h21_com->Add(h21);
		_file0->Close();
	}

	h1_com->SaveAs(Form("%s_all_h1.root",dirName.Data()));
	h2_com->SaveAs(Form("%s_all_h2.root",dirName.Data()));
	h21_com->SaveAs(Form("%s_all_h21.root",dirName.Data()));

	cout<<"[INFO] nEntries = "<<h2_com->GetEntries()<<" (= "<<nEntries<<")"<<endl;

	return;
}

/*
  KEY: TH1F	h0;1	SR photon energy (abs(gammaPosZ_cm) < 65 cm)
  KEY: TH1F	h1;1	SR photon energy
  KEY: TH2F	h2;1	SR photon position
  KEY: TH2F	h3x;1	SR photon vertex position
  KEY: TH2F	h3y;1	SR photon vertex position
  KEY: TH2F	h4;1	Monitor []: Px vs X
  KEY: TH2F	h5;1	Monitor []: Py vs Y
  KEY: TH1F	h6;1	Monitor []: Momentum
  KEY: TH2F	h7;1	Beam [(-8.0e+00,0.0e+00,3.7e+03) cm, -2.6e-02 rad]: Px vs X
  KEY: TH2F	h8;1	Beam [(-8.0e+00,0.0e+00,3.7e+03) cm, -2.6e-02 rad]: Py vs Y
  KEY: TH1F	h9;1	Beam [(-8.0e+00,0.0e+00,3.7e+03) cm, -2.6e-02 rad]: Momentum
  KEY: TH2F	h21;1	SR photon position vs energy
  KEY: TH1F	h10_0;1	Number of generated SR photons per event in mag_D2EF1
  KEY: TH1F	h11_0;1	Energy sum of generated SR photons per event in mag_D2EF1
  KEY: TH1F	h10_1;1	Number of generated SR photons per event in mag_D2EF2
  KEY: TH1F	h11_1;1	Energy sum of generated SR photons per event in mag_D2EF2
  KEY: TH1F	h10_2;1	Number of generated SR photons per event in mag_D1EF
  KEY: TH1F	h11_2;1	Energy sum of generated SR photons per event in mag_D1EF
  KEY: TH1F	h10_3;1	Number of generated SR photons per event in mag_Q1EF
  KEY: TH1F	h11_3;1	Energy sum of generated SR photons per event in mag_Q1EF
  KEY: TH1F	h10_4;1	Number of generated SR photons per event in mag_Q0EF
  KEY: TH1F	h11_4;1	Energy sum of generated SR photons per event in mag_Q0EF
  KEY: TH1F	h10_5;1	Number of generated SR photons per event in mag_Q1ER
  KEY: TH1F	h11_5;1	Energy sum of generated SR photons per event in mag_Q1ER
  KEY: TH1F	h10_6;1	Number of generated SR photons per event in mag_Q2ER
  KEY: TH1F	h11_6;1	Energy sum of generated SR photons per event in mag_Q2ER
  KEY: TH1F	h10_7;1	Number of generated SR photons per event in mag_D2ER
  KEY: TH1F	h11_7;1	Energy sum of generated SR photons per event in mag_D2ER
  KEY: TH1F	h10_8;1	Number of generated SR photons per event in mag_Q3ER
  KEY: TH1F	h11_8;1	Energy sum of generated SR photons per event in mag_Q3ER
  KEY: TH1F	h10_9;1	Number of generated SR photons per event in mag_Q4ER
  KEY: TH1F	h11_9;1	Energy sum of generated SR photons per event in mag_Q4ER
  KEY: TH1F	h10_10;1	Number of generated SR photons per event in mag_Q5ER
  KEY: TH1F	h11_10;1	Energy sum of generated SR photons per event in mag_Q5ER
  KEY: TH1F	h10_11;1	Number of generated SR photons per event in mag_MARCO
  KEY: TH1F	h11_11;1	Energy sum of generated SR photons per event in mag_MARCO
  KEY: TH1D	h1_ene_g4;1	SR photon spectrum
  KEY: TH1D	h1_dirx_g4;1	SR photon momDir X
  KEY: TH1D	h1_diry_g4;1	SR photon momDir Y
  KEY: TH1D	h1_dirz_g4;1	SR photon momDir Z
  KEY: TH1D	h1_posx_g4;1	SR photon position X
  KEY: TH1D	h1_posy_g4;1	SR photon position Y
  KEY: TH1D	h1_posz_g4;1	SR photon position Z
  KEY: TH2D	h1_ene_dirx_g4;1	SR photon energy vs momDir X
  KEY: TH2D	h1_ene_diry_g4;1	SR photon energy vs momDir Y
  KEY: TH2D	h1_ene_dirz_g4;1	SR photon energy vs momDir Z
*/
