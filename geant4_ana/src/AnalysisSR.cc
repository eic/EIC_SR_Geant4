#include "AnalysisSR.hh"

AnalysisSR::AnalysisSR() : _fChain(0), _fHepmc(0) 
{}

AnalysisSR::~AnalysisSR()
{
   	if(_fChain)
		delete _fChain->GetCurrentFile();

	if(_fHepmc)
		delete _fHepmc;
}

void AnalysisSR::Init(TString treeName = "tree")
{
	_fChain = new TChain(treeName.Data());
   	if(!_fChain){return;};
	_fChain->Add(_inputFileName.Data());

	cout<<"\n[INFO] AnalysisSR::Init ==> Added files:"<<endl;
	_fChain->ls();
	cout<<"\n"<<endl;

   	// set object pointer
	monName = new string();
	gammaVtxPosX_cm = nullptr;
	gammaVtxPosY_cm = nullptr;
	gammaVtxPosZ_cm = nullptr;
	gammaVtxDirX = nullptr;
	gammaVtxDirY = nullptr;
	gammaVtxDirZ = nullptr;
	gammaPosX_cm = nullptr;
	gammaPosY_cm = nullptr;
	gammaPosZ_cm = nullptr;
	gammaMomX_eV = nullptr;
	gammaMomY_eV = nullptr;
	gammaMomZ_eV = nullptr;
	gammaEnergy_eV = nullptr;
	gammaTime_ms = nullptr;

	// set branch addresses and branch pointers
   	_fChain->SetBranchAddress("event", &event, &b_event);
   	_fChain->SetBranchAddress("beamPdg", &beamPdg, &b_beamPdg);
   	_fChain->SetBranchAddress("beamPosX_cm", &beamPosX_cm, &b_beamPosX_cm);
   	_fChain->SetBranchAddress("beamPosY_cm", &beamPosY_cm, &b_beamPosY_cm);
   	_fChain->SetBranchAddress("beamPosZ_cm", &beamPosZ_cm, &b_beamPosZ_cm);
   	_fChain->SetBranchAddress("beamMomX_eV", &beamMomX_eV, &b_beamMomX_eV);
   	_fChain->SetBranchAddress("beamMomY_eV", &beamMomY_eV, &b_beamMomY_eV);
   	_fChain->SetBranchAddress("beamMomZ_eV", &beamMomZ_eV, &b_beamMomZ_eV);
   	_fChain->SetBranchAddress("beamEnergy_eV", &beamEnergy_eV, &b_beamEnergy_eV);
   	_fChain->SetBranchAddress("beamIniPosX_cm", &beamIniPosX_cm, &b_beamIniPosX_cm);
   	_fChain->SetBranchAddress("beamIniPosY_cm", &beamIniPosY_cm, &b_beamIniPosY_cm);
   	_fChain->SetBranchAddress("beamIniPosZ_cm", &beamIniPosZ_cm, &b_beamIniPosZ_cm);
   	_fChain->SetBranchAddress("beamIniAngle_rad", &beamIniAngle_rad, &b_beamIniAngle_rad);

   	_fChain->SetBranchAddress("gammaVtxPosX_cm", &gammaVtxPosX_cm, &b_gammaVtxPosX_cm);
   	_fChain->SetBranchAddress("gammaVtxPosY_cm", &gammaVtxPosY_cm, &b_gammaVtxPosY_cm);
   	_fChain->SetBranchAddress("gammaVtxPosZ_cm", &gammaVtxPosZ_cm, &b_gammaVtxPosZ_cm);
   	_fChain->SetBranchAddress("gammaVtxDirX", &gammaVtxDirX, &b_gammaVtxDirX);
   	_fChain->SetBranchAddress("gammaVtxDirY", &gammaVtxDirY, &b_gammaVtxDirY);
   	_fChain->SetBranchAddress("gammaVtxDirZ", &gammaVtxDirZ, &b_gammaVtxDirZ);

   	_fChain->SetBranchAddress("gammaPosX_cm", &gammaPosX_cm, &b_gammaPosX_cm);
   	_fChain->SetBranchAddress("gammaPosY_cm", &gammaPosY_cm, &b_gammaPosY_cm);
   	_fChain->SetBranchAddress("gammaPosZ_cm", &gammaPosZ_cm, &b_gammaPosZ_cm);
   	_fChain->SetBranchAddress("gammaMomX_eV", &gammaMomX_eV, &b_gammaMomX_eV);
   	_fChain->SetBranchAddress("gammaMomY_eV", &gammaMomY_eV, &b_gammaMomY_eV);
   	_fChain->SetBranchAddress("gammaMomZ_eV", &gammaMomZ_eV, &b_gammaMomZ_eV);

   	_fChain->SetBranchAddress("gammaEnergy_eV", &gammaEnergy_eV, &b_gammaEnergy_eV);
   	_fChain->SetBranchAddress("gammaTime_ms", &gammaTime_ms, &b_gammaTime_ms);

   	_fChain->SetBranchAddress("monName", &monName, &b_monName);
   	_fChain->SetBranchAddress("monPosX_cm", &monPosX_cm, &b_monPosX_cm);
   	_fChain->SetBranchAddress("monPosY_cm", &monPosY_cm, &b_monPosY_cm);
   	_fChain->SetBranchAddress("monPosZ_cm", &monPosZ_cm, &b_monPosZ_cm);
   	_fChain->SetBranchAddress("monMomX_eV", &monMomX_eV, &b_monMomX_eV);
   	_fChain->SetBranchAddress("monMomY_eV", &monMomY_eV, &b_monMomY_eV);
   	_fChain->SetBranchAddress("monMomZ_eV", &monMomZ_eV, &b_monMomZ_eV);

	_fChain->GetEntry(0);
	// histograms
	_h1 = new TH1F("h1","SR photon energy;E_{#gamma} [eV];# of photons",10100,-1000,100000);
	_h2 = new TH2F("h2","SR photon position;Z_{#gamma} [cm];X_{#gamma} [cm]",600,-4000,2000,300,-30,30);
	_h3 = new TH2F("h3","SR photon vertex position;Z_{vtx} [cm];X_{vtx} [cm]",600,-4000,2000,300,-30,30);

	_h4 = new TH2F("h4",Form("Monitor [%s]: Px vs X;X [cm];X'",monName->c_str()),1000,-4,4,1000,-1e-3,1e-3);
	_h5 = new TH2F("h5",Form("Monitor [%s]: Py vs Y;Y [cm];Y'",monName->c_str()),1000,-4,4,1000,-1e-4,1e-4);
	_h6 = new TH1F("h6",Form("Monitor [%s]: Momentum;P [GeV]",monName->c_str()),20000,0,20);

	_h7 = new TH2F("h7",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Px vs X;X [cm];X'",
		beamIniPosX_cm,beamIniPosY_cm,beamIniPosZ_cm,beamIniAngle_rad),1000,-4,4,1000,-1e-3,1e-3);
	_h8 = new TH2F("h8",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Py vs Y;Y [cm];Y'",
		beamIniPosX_cm,beamIniPosY_cm,beamIniPosZ_cm,beamIniAngle_rad),1000,-4,4,1000,-1e-4,1e-4);
	_h9 = new TH1F("h9",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Momentum;P [GeV]",
		beamIniPosX_cm,beamIniPosY_cm,beamIniPosZ_cm,beamIniAngle_rad),20000,0,20);

	// ascii file to store generated events
	_fHepmc = new WriterAscii(GetOutputFileNameHepmc().Data());

	return;
}

void AnalysisSR::Loop()
{
  	if(!_fChain){return;}

   	Long64_t nEntries = _fChain->GetEntries();
	cout<<"[INFO] AnalysisSR::Loop ==> nEntries = "<<nEntries<<endl;

	Float_t x, y, z, pinx, piny, pinz, px, py, pz, energy;
	Int_t pdg;

	// main bookkeeping type that holds meta-information about the generated events: list of used tools, 
	// names of used event weights and arbitrary attributes (see https://arxiv.org/pdf/1912.08005.pdf)
	shared_ptr<GenRunInfo> run = make_shared<GenRunInfo>();
	vector<string> weight_names = {"NormFactor"};
	run->set_weight_names(weight_names);
	_fHepmc->set_run_info(run);
	_fHepmc->write_run_info();

	Long64_t jentry = 0;
   	while(jentry < nEntries) 
	{
		// load the entry
		_fChain->GetEntry(jentry);
		
		if(jentry%1000 == 0)
        	{
           	 	printf("\r[INFO] Progress: %.3f [%%]",100.0*jentry/nEntries);
            		fflush(stdout);
        	}
		jentry++;
		
		// data type that holds the position of the primary inter- action, and lists of vertices, 
		// particles and attributes
		GenEvent event(HepMC3::Units::GEV,HepMC3::Units::MM);
		event.set_run_info(run);
		vector<double> weights = {1};
		event.weights() = weights;
		
		// loop over SR photons
		for(Int_t i = 0; i < gammaEnergy_eV->size(); i++)
		{
			//---------------------------------------------------------------------------
			// fill histograms

			_h1->Fill(gammaEnergy_eV->at(i));
			_h2->Fill(gammaPosZ_cm->at(i),gammaPosX_cm->at(i));
			_h3->Fill(gammaVtxPosZ_cm->at(i),gammaVtxPosX_cm->at(i));

			//---------------------------------------------------------------------------
			// generate a HEPMC event

			x = gammaPosX_cm->at(i)*1e1; // cm -> mm
			y = gammaPosY_cm->at(i)*1e1; // cm -> mm
			z = gammaPosZ_cm->at(i)*1e1; // cm -> mm

			px = gammaMomX_eV->at(i)/1e9; // eV -> GeV
			py = gammaMomY_eV->at(i)/1e9; // eV -> GeV
			pz = gammaMomZ_eV->at(i)/1e9; // eV -> GeV

			energy = gammaEnergy_eV->at(i)/1e9; // eV -> GeV
			pdg = 22; // gamma

			pinx = energy*x/TMath::Sqrt(x*x+y*y+z*z);
			piny = energy*y/TMath::Sqrt(x*x+y*y+z*z);
			pinz = energy*z/TMath::Sqrt(x*x+y*y+z*z);

		    	// particles going into the vertex: 
			// (px,py,pz,e),pdgid,status [https://arxiv.org/pdf/1912.08005.pdf]
			GenParticlePtr pin = make_shared<GenParticle>( 
					FourVector(pinx,piny,pinz,energy),pdg,4);	
			pin->set_generated_mass(0.);
			event.add_particle(pin);

		    	// particles coming out of the vertex
			GenParticlePtr pout = make_shared<GenParticle>( 
					FourVector(px,py,pz,energy),pdg,1);	
			pout->set_generated_mass(0.);
			event.add_particle(pout);
			
			// generate the vertex
			GenVertexPtr vertex = make_shared<GenVertex>(FourVector(x,y,z,0.));
			vertex->add_particle_in(pin);
    			vertex->add_particle_out(pout);

			// add the vertex to the event
			event.add_vertex(vertex);
		}

		// write a non-empty event
		if(gammaEnergy_eV->size() > 0)
			_fHepmc->write_event(event);

		// monitor data
		Float_t monMom_eV = TMath::Sqrt(TMath::Power(monMomX_eV,2) + 
						TMath::Power(monMomY_eV,2) + 
						TMath::Power(monMomZ_eV,2));
		_h4->Fill(monPosX_cm,monMomX_eV/monMom_eV);
		_h5->Fill(monPosY_cm,monMomY_eV/monMom_eV);
		_h6->Fill(monMom_eV/1e9); // eV --> GeV

		// primary beam data
		TVector3 beamMom(beamMomX_eV,beamMomY_eV,beamMomZ_eV);
		TVector3 beamPos(beamPosX_cm-beamIniPosX_cm,beamPosY_cm-beamIniPosY_cm,beamPosZ_cm-beamIniPosZ_cm);
		beamMom.RotateY(-beamIniAngle_rad);
		beamPos.RotateY(-beamIniAngle_rad);

		_h7->Fill(beamPos.x(),beamMom.x()/beamMom.Mag());
		_h8->Fill(beamPos.y(),beamMom.y()/beamMom.Mag());
		_h9->Fill(beamMom.Mag()/1e9); // eV --> GeV
   	}
 	printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jentry/nEntries);

	TFile* f = new TFile(GetOutputFileNameRoot().Data(),"recreate");
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
	f->Close();

	return;
}
