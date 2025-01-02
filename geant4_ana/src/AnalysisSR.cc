#include "AnalysisSR.hh"
#define NMAGMAX 100 // maximum number of magnets

AnalysisSR::AnalysisSR() : _fHepmc(0) 
{
	cout<<"[FUNCTION] "<<__FUNCTION__<<endl;
	gSystem->GetProcInfo(&fProcInfo);
}

AnalysisSR::~AnalysisSR()
{
	cout<<"[FUNCTION] "<<__FUNCTION__<<endl;

	if(_fHepmc)
		delete _fHepmc;

	if(_fOut)
		delete _fOut;
}

// Function to print memory usage
void AnalysisSR::PrintMemoryUsage(const char* description) 
{
	cout << description << ": " << endl;
	cout << "  Resident Memory: " << fProcInfo.fMemResident / 1024.0 << " MB" << endl;
	cout << "  Virtual Memory: " << fProcInfo.fMemVirtual / 1024.0 << " MB" << endl;

	return;
}

void AnalysisSR::Init(
		Double_t minZ = -1e9, Double_t maxZ = 1e9, 
		Double_t minVtxZ = -1e9, Double_t maxVtxZ = 1e9)
{
	cout<<"[FUNCTION] "<<__FUNCTION__<<endl;
	// set Z coord limits
	_minZ = minZ;
	_maxZ = maxZ;
	_minVtxZ = minVtxZ;
	_maxVtxZ = maxVtxZ;

   	// set object pointer
	beamPdg = nullptr;
	beamX_norm = nullptr;
	beamXprime_norm = nullptr;
	beamY_norm = nullptr;
	beamYprime_norm = nullptr;
	beamPosX_cm = nullptr;
	beamPosY_cm = nullptr;
	beamPosZ_cm = nullptr;
	beamMomX_eV = nullptr;
	beamMomY_eV = nullptr;
	beamMomZ_eV = nullptr;
	beamEnergy_eV = nullptr;
	beamRefPosX_cm = nullptr;
	beamRefPosY_cm = nullptr;
	beamRefPosZ_cm = nullptr;
	beamRefAngle_rad = nullptr;

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
	magName = nullptr;
	magNph = nullptr;
	magSumEph_eV = nullptr;

	cout<<"[INFO] Data prod. flag: "<<GetDataProdFlag()<<endl;

	if(!GetDataProdFlag())
	{
		// histograms
		_fOut = new TFile(GetOutputFileNameRoot().Data(),"recreate","",ROOT::kZLIB);

		_h0 = new TH1F("h0","SR photon energy (abs(gammaPosZ_cm) < 65 cm);E_{#gamma} [eV];# of photons",
			101000,-1000,100000);
		_h1 = new TH1F("h1","SR photon energy;E_{#gamma} [eV];# of photons",101000,-1000,100000);
		_h2 = new TH2F("h2","SR photon position;Z_{#gamma} [cm];X_{#gamma} [cm]",2000,-6000,4000,2000,-100,100);
		_h21 = new TH2F("h21","SR photon position vs energy;Z_{#gamma} [cm];X_{#gamma} [cm];E_{#gamma} [eV]",2000,-6000,4000,2000,-100,100);
		_h3x = new TH2F("h3x","SR photon vertex position;Z_{vtx} [cm];X_{vtx} [cm]",2000,-6000,4000,2000,-100,100);
		_h3y = new TH2F("h3y","SR photon vertex position;Z_{vtx} [cm];Y_{vtx} [cm]",2000,-6000,4000,2000,-100,100);

		_h4 = new TH2F("h4","",1000,-4,4,10000,-1e-2,1e-2);
		_h5 = new TH2F("h5","",1000,-4,4,10000,-1e-2,1e-2);
		_h6 = new TH1F("h6","",20000,0,20);

		_h7 = new TH2F("h7","",1000,-4,4,10000,-1e-2,1e-2);
		_h8 = new TH2F("h8","",1000,-4,4,10000,-1e-2,1e-2);
		_h9 = new TH1F("h9","",20000,0,20);

		_h21_hepmc = new TH2F("h21_hepmc","SR photon X-Z position vs energy (HepMC);Z_{#gamma} [cm];X_{#gamma} [cm];E_{#gamma} [eV]",2000,-6000,4000,2000,-100,100);
		_h22_hepmc = new TH2F("h22_hepmc","SR photon Y-Z position vs energy (HepMC);Z_{#gamma} [cm];Y_{#gamma} [cm];E_{#gamma} [eV]",2000,-6000,4000,2000,-100,100);
		_h23_hepmc = new TH2F("h23_hepmc","SR photon Y-X position vs energy (HepMC);X_{#gamma} [cm];Y_{#gamma} [cm];E_{#gamma} [eV]",2000,-100,100,2000,-100,100);

		for(Int_t iMag = 0; iMag < NMAGMAX; iMag++)
		{
			_h10[iMag] = new TH1F(Form("h10_%d",iMag),"",10000,0,10000);
			_h11[iMag] = new TH1F(Form("h11_%d",iMag),"",1e6,0,100e6);
		}	

		_h0_Xnorm = new TH2F("h0_Xnorm","XX';X[#sigma];X'[#sigma]",1000,-50,50,1000,-50,50);
		_h1_Xnorm = new TH2F("h1_Xnorm","XX' (XX' > 5#sigma);X[#sigma];X'[#sigma]",1000,-50,50,1000,-50,50);
		_h0_Ynorm = new TH2F("h0_Ynorm","YY';Y[#sigma];Y'[#sigma]",1000,-50,50,1000,-50,50);
		_h1_Ynorm = new TH2F("h1_Ynorm","YY' (YY' > 5#sigma);Y[#sigma];Y'[#sigma]",1000,-50,50,1000,-50,50);

		// for benchmark/debug
		h1_ene = new TH1F("h1_ene_g4","SR photon spectrum;E_{#gamma} [eV];Flux [ph/s]",1e6,0,1e6);
		h1_dirx = new TH1F("h1_dirx_g4","SR photon momDir X; Px_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
		h1_diry = new TH1F("h1_diry_g4","SR photon momDir Y; Py_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
		h1_dirz = new TH1F("h1_dirz_g4","SR photon momDir Z; Pz_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
		h1_posx = new TH1F("h1_posx_g4","SR photon position X; X_{#gamma} [cm];Flux [ph/s]",20000,-10,10);
		h1_posy = new TH1F("h1_posy_g4","SR photon position Y; Y_{#gamma} [cm];Flux [ph/s]",20000,-10,10);
		h1_posz = new TH1F("h1_posz_g4","SR photon position Z; Z_{#gamma} [cm];Flux [ph/s]",20000,-100,100);

		h1_ene_dirx = 
			new TH2F("h1_ene_dirx_g4",
			"SR photon energy vs momDir X;Px_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
			4e3,-2,2,4e3,0,40e3);
		h1_ene_diry = 
			new TH2F("h1_ene_diry_g4",
			"SR photon energy vs momDir Y;Py_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
			4e3,-2,2,4e3,0,40e3);
		h1_ene_dirz = 
			new TH2F("h1_ene_dirz_g4",
			"SR photon energy vs momDir Z;Pz_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
			4e3,-2,2,4e3,0,40e3);

		// Detach from memory management
		_h0->SetDirectory(0);
		_h1->SetDirectory(0);
		_h2->SetDirectory(0);
		_h21->SetDirectory(0);
		_h3x->SetDirectory(0);
		_h3y->SetDirectory(0);
		_h4->SetDirectory(0);
		_h5->SetDirectory(0);
		_h6->SetDirectory(0);
		_h7->SetDirectory(0);
		_h8->SetDirectory(0);
		_h9->SetDirectory(0);
		for(Int_t iMag = 0; iMag < NMAGMAX; iMag++)
		{
			_h10[iMag]->SetDirectory(0);
			_h11[iMag]->SetDirectory(0);
		}
		_h21_hepmc->SetDirectory(0);
		_h22_hepmc->SetDirectory(0);
		_h23_hepmc->SetDirectory(0);
		_h0_Xnorm->SetDirectory(0);
		_h1_Xnorm->SetDirectory(0);
		_h0_Ynorm->SetDirectory(0);
		_h1_Ynorm->SetDirectory(0);
		h1_ene->SetDirectory(0);	
		h1_dirx->SetDirectory(0);	
		h1_diry->SetDirectory(0);	
		h1_dirz->SetDirectory(0);	
		h1_posx->SetDirectory(0);	
		h1_posy->SetDirectory(0);
		h1_posz->SetDirectory(0);
		h1_ene_dirx->SetDirectory(0);
		h1_ene_diry->SetDirectory(0);
		h1_ene_dirz->SetDirectory(0);
	}

	// ascii file to store generated events
	_fHepmc = new WriterAscii(GetOutputFileNameHepmc().Data());

	// main bookkeeping type that holds meta-information about the generated events: list of used tools, 
	// names of used event weights and arbitrary attributes (see https://arxiv.org/pdf/1912.08005.pdf)
	_run = make_shared<GenRunInfo>();
	vector<string> weight_names = {"NormFactor"};
	_run->set_weight_names(weight_names);
	_fHepmc->set_run_info(_run);
	_fHepmc->write_run_info();

	return;
}

void AnalysisSR::Loop()
{
	cout<<"[FUNCTION] "<<__FUNCTION__<<endl;

	TFile *file = TFile::Open(_inputFileName.Data(), "READ");
	if (!file || file->IsZombie())
	{
		cout<<"[WARNING] Unable to open the file: "<<_inputFileName<<endl;
		return;
	}
	cout<<"[INFO] File name: "<<_inputFileName<<endl;

	TTree *tree = (TTree*)file->Get("tree");
	if (!tree)
	{
		cout<<"[WARNING] Tree not found in the file"<<endl;
		return;
	}

	// set branch addresses and branch pointers
   	tree->SetBranchAddress("event", &event, &b_event);
   	tree->SetBranchAddress("beamPdg", &beamPdg, &b_beamPdg);
	tree->SetBranchAddress("beamX_norm", &beamX_norm, &b_beamX_norm);
	tree->SetBranchAddress("beamXprime_norm", &beamXprime_norm, &b_beamXprime_norm);
	tree->SetBranchAddress("beamY_norm", &beamY_norm, &b_beamY_norm);
	tree->SetBranchAddress("beamYprime_norm", &beamYprime_norm, &b_beamYprime_norm);
   	tree->SetBranchAddress("beamPosX_cm", &beamPosX_cm, &b_beamPosX_cm);
   	tree->SetBranchAddress("beamPosY_cm", &beamPosY_cm, &b_beamPosY_cm);
   	tree->SetBranchAddress("beamPosZ_cm", &beamPosZ_cm, &b_beamPosZ_cm);
   	tree->SetBranchAddress("beamMomX_eV", &beamMomX_eV, &b_beamMomX_eV);
   	tree->SetBranchAddress("beamMomY_eV", &beamMomY_eV, &b_beamMomY_eV);
   	tree->SetBranchAddress("beamMomZ_eV", &beamMomZ_eV, &b_beamMomZ_eV);
   	tree->SetBranchAddress("beamEnergy_eV", &beamEnergy_eV, &b_beamEnergy_eV);
   	tree->SetBranchAddress("beamRefPosX_cm", &beamRefPosX_cm, &b_beamRefPosX_cm);
   	tree->SetBranchAddress("beamRefPosY_cm", &beamRefPosY_cm, &b_beamRefPosY_cm);
   	tree->SetBranchAddress("beamRefPosZ_cm", &beamRefPosZ_cm, &b_beamRefPosZ_cm);
   	tree->SetBranchAddress("beamRefAngle_rad", &beamRefAngle_rad, &b_beamRefAngle_rad);

   	tree->SetBranchAddress("magName", &magName, &b_magName);
   	tree->SetBranchAddress("magNph", &magNph, &b_magNph);
   	tree->SetBranchAddress("magSumEph_eV", &magSumEph_eV, &b_magSumEph_eV);

   	tree->SetBranchAddress("gammaVtxPosX_cm", &gammaVtxPosX_cm, &b_gammaVtxPosX_cm);
   	tree->SetBranchAddress("gammaVtxPosY_cm", &gammaVtxPosY_cm, &b_gammaVtxPosY_cm);
   	tree->SetBranchAddress("gammaVtxPosZ_cm", &gammaVtxPosZ_cm, &b_gammaVtxPosZ_cm);
   	tree->SetBranchAddress("gammaVtxDirX", &gammaVtxDirX, &b_gammaVtxDirX);
   	tree->SetBranchAddress("gammaVtxDirY", &gammaVtxDirY, &b_gammaVtxDirY);
   	tree->SetBranchAddress("gammaVtxDirZ", &gammaVtxDirZ, &b_gammaVtxDirZ);

   	tree->SetBranchAddress("gammaPosX_cm", &gammaPosX_cm, &b_gammaPosX_cm);
   	tree->SetBranchAddress("gammaPosY_cm", &gammaPosY_cm, &b_gammaPosY_cm);
   	tree->SetBranchAddress("gammaPosZ_cm", &gammaPosZ_cm, &b_gammaPosZ_cm);
   	tree->SetBranchAddress("gammaMomX_eV", &gammaMomX_eV, &b_gammaMomX_eV);
   	tree->SetBranchAddress("gammaMomY_eV", &gammaMomY_eV, &b_gammaMomY_eV);
   	tree->SetBranchAddress("gammaMomZ_eV", &gammaMomZ_eV, &b_gammaMomZ_eV);

   	tree->SetBranchAddress("gammaEnergy_eV", &gammaEnergy_eV, &b_gammaEnergy_eV);
   	tree->SetBranchAddress("gammaTime_ms", &gammaTime_ms, &b_gammaTime_ms);

   	tree->SetBranchAddress("monName", &monName, &b_monName);
   	tree->SetBranchAddress("monPosX_cm", &monPosX_cm, &b_monPosX_cm);
   	tree->SetBranchAddress("monPosY_cm", &monPosY_cm, &b_monPosY_cm);
   	tree->SetBranchAddress("monPosZ_cm", &monPosZ_cm, &b_monPosZ_cm);
   	tree->SetBranchAddress("monMomX_eV", &monMomX_eV, &b_monMomX_eV);
   	tree->SetBranchAddress("monMomY_eV", &monMomY_eV, &b_monMomY_eV);
   	tree->SetBranchAddress("monMomZ_eV", &monMomZ_eV, &b_monMomZ_eV);
   	tree->SetBranchAddress("monRefPosX_cm", &monRefPosX_cm, &b_monRefPosX_cm);
   	tree->SetBranchAddress("monRefPosY_cm", &monRefPosY_cm, &b_monRefPosY_cm);
   	tree->SetBranchAddress("monRefPosZ_cm", &monRefPosZ_cm, &b_monRefPosZ_cm);
   	tree->SetBranchAddress("monRefAngle_rad", &monRefAngle_rad, &b_monRefAngle_rad);

   	Long64_t nEntries = tree->GetEntries();
	cout<<"[INFO] AnalysisSR::Loop ==> nEntries = "<<nEntries<<endl;

	Float_t x, y, z, pinx, piny, pinz, px, py, pz, energy;
	Int_t pdg;
	
	Long64_t jentry = 0;
   	while(jentry < nEntries) 
	{
		// load the entry
		tree->GetEntry(jentry);
		
		if(jentry%1000 == 0)
        	{
           	 	printf("\r[INFO] Progress: %.3f [%%]",100.0*jentry/nEntries);
            		fflush(stdout);
        	}
		jentry++;
		
		// data type that holds the position of the primary inter- action, and lists of vertices, 
		// particles and attributes
		GenEvent* event = new GenEvent(HepMC3::Units::GEV,HepMC3::Units::MM);
		event->set_run_info(_run);
		vector<float> weights = {1};
		event->weights().clear();               // Clear existing weights
		event->weights().insert(event->weights().end(), weights.begin(), weights.end());
		
		// loop over SR photons
		for(Int_t i = 0; i < gammaEnergy_eV->size(); i++)
		{
			// for benchmark/debug
			if(	!GetDataProdFlag() &&
				-80.0 < gammaPosZ_cm->at(i) && gammaPosZ_cm->at(i) < 67.0	) // around the IP
			{
				h1_ene->Fill(gammaEnergy_eV->at(i));
				h1_dirx->Fill(gammaMomX_eV->at(i)/gammaEnergy_eV->at(i));
				h1_diry->Fill(gammaMomY_eV->at(i)/gammaEnergy_eV->at(i));
				h1_dirz->Fill(gammaMomZ_eV->at(i)/gammaEnergy_eV->at(i));
				h1_posx->Fill(gammaPosX_cm->at(i));
				h1_posy->Fill(gammaPosY_cm->at(i));
				h1_posz->Fill(gammaPosZ_cm->at(i));
				h1_ene_dirx->Fill(	gammaMomX_eV->at(i)/gammaEnergy_eV->at(i),
							gammaEnergy_eV->at(i));
				h1_ene_diry->Fill(	gammaMomY_eV->at(i)/gammaEnergy_eV->at(i),
							gammaEnergy_eV->at(i));
				h1_ene_dirz->Fill(	gammaMomZ_eV->at(i)/gammaEnergy_eV->at(i),
							gammaEnergy_eV->at(i));
			}
			
			// geometry cut for SR photons
			if(gammaPosZ_cm->at(i) < _minZ || _maxZ < gammaPosZ_cm->at(i)){continue;} 
			if(gammaVtxPosZ_cm->at(i) < _minVtxZ || _maxVtxZ < gammaVtxPosZ_cm->at(i)){continue;} 

			//---------------------------------------------------------------------------
			// fill histograms
			if(!GetDataProdFlag())
			{
				// only SR photons around the IP6
				if(TMath::Abs(gammaPosZ_cm->at(i)) < 65.0)
					_h0->Fill(gammaEnergy_eV->at(i));

				_h1->Fill(gammaEnergy_eV->at(i));
				_h2->Fill(gammaPosZ_cm->at(i),gammaPosX_cm->at(i));
				_h21->Fill(gammaPosZ_cm->at(i),gammaPosX_cm->at(i),gammaEnergy_eV->at(i));
				_h3x->Fill(gammaVtxPosZ_cm->at(i),gammaVtxPosX_cm->at(i));
				_h3y->Fill(gammaVtxPosZ_cm->at(i),gammaVtxPosY_cm->at(i));
			}

			//---------------------------------------------------------------------------
			// generate a HEPMC event

			x = gammaPosX_cm->at(i)*1e1; // cm -> mm
			y = gammaPosY_cm->at(i)*1e1; // cm -> mm
			z = gammaPosZ_cm->at(i)*1e1; // cm -> mm

			px = gammaMomX_eV->at(i)/1e9; // eV -> GeV
			py = gammaMomY_eV->at(i)/1e9; // eV -> GeV
			pz = gammaMomZ_eV->at(i)/1e9; // eV -> GeV

			//---------------------------------------------------------------------------
			// fill histograms in HepMC
			if(!GetDataProdFlag())
			{
				_h21_hepmc->Fill(z/1e1,x/1e1,TMath::Sqrt(px*px+py*py+pz*pz)*1e9); // mm -> cm, GeV -> eV
				_h22_hepmc->Fill(z/1e1,y/1e1,TMath::Sqrt(px*px+py*py+pz*pz)*1e9); // mm -> cm, GeV -> eV
				_h23_hepmc->Fill(x/1e1,y/1e1,TMath::Sqrt(px*px+py*py+pz*pz)*1e9); // mm -> cm, GeV -> eV
			}
			//---------------------------------------------------------------------------

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
			event->add_particle(pin);

			// particles coming out of the vertex
			GenParticlePtr pout = make_shared<GenParticle>( 
					FourVector(px,py,pz,energy),pdg,1);	
			pout->set_generated_mass(0.);
			event->add_particle(pout);
			
			// generate the vertex
			GenVertexPtr vertex = make_shared<GenVertex>(FourVector(x,y,z,0.));
			vertex->add_particle_in(pin);
			vertex->add_particle_out(pout);

			// add the vertex to the event
			event->add_vertex(vertex);
		}

		// write a non-empty event
		if(!event->vertices_empty())
			_fHepmc->write_event(*event);
		delete event;

		if(!GetDataProdFlag())
		{
			// monitor data
			TVector3 monMom(monMomX_eV,monMomY_eV,monMomZ_eV);
			TVector3 monPos(monPosX_cm-monRefPosX_cm,monPosY_cm-monRefPosY_cm,monPosZ_cm-monRefPosZ_cm);
			monMom.RotateY(-monRefAngle_rad);
			monPos.RotateY(-monRefAngle_rad);

			_h4->SetTitle(Form("Monitor [%s]: Px vs X;X [cm];X'",monName->c_str()));
			_h5->SetTitle(Form("Monitor [%s]: Py vs Y;Y [cm];Y'",monName->c_str()));
			_h6->SetTitle(Form("Monitor [%s]: Momentum;P [GeV/c]",monName->c_str()));
			_h4->Fill(monPos.x(),monMom.x()/monMom.Mag());
			_h5->Fill(monPos.y(),monMom.y()/monMom.Mag());
			_h6->Fill(monMom.Mag()/1e9); // eV --> GeV

			// primary beam data
			for(Int_t iBeam = 0; iBeam < beamPdg->size(); iBeam++)
			{
				TVector3 beamMom(beamMomX_eV->at(iBeam),beamMomY_eV->at(iBeam),beamMomZ_eV->at(iBeam));
				TVector3 beamPos(	beamPosX_cm->at(iBeam)-beamRefPosX_cm->at(iBeam),
							beamPosY_cm->at(iBeam)-beamRefPosY_cm->at(iBeam),
							beamPosZ_cm->at(iBeam)-beamRefPosZ_cm->at(iBeam));
				beamMom.RotateY(-beamRefAngle_rad->at(iBeam));
				beamPos.RotateY(-beamRefAngle_rad->at(iBeam));

				_h7->Fill(beamPos.x(),beamMom.x()/beamMom.Mag());
				_h8->Fill(beamPos.y(),beamMom.y()/beamMom.Mag());
				_h9->Fill(beamMom.Mag()/1e9); // eV --> GeV

				// normalized coordinates
				_h0_Xnorm->Fill(beamX_norm->at(iBeam),beamXprime_norm->at(iBeam));
				_h0_Ynorm->Fill(beamY_norm->at(iBeam),beamYprime_norm->at(iBeam));
				Double_t rx = TMath::Sqrt(
						TMath::Power(beamX_norm->at(iBeam),2) +
						TMath::Power(beamXprime_norm->at(iBeam),2));
				Double_t ry = TMath::Sqrt(
						TMath::Power(beamY_norm->at(iBeam),2) +
						TMath::Power(beamYprime_norm->at(iBeam),2));
				if(ry < 5 || (rx > 5 && ry > 5)){_h1_Xnorm->Fill(beamX_norm->at(iBeam),beamXprime_norm->at(iBeam));}
				if(rx < 5 || (rx > 5 && ry > 5)){_h1_Ynorm->Fill(beamY_norm->at(iBeam),beamYprime_norm->at(iBeam));}
			}

			// loop over magnets
			for(Int_t iMag = 0; iMag < magName->size(); iMag++) 
			{
				_h10[iMag]->SetTitle(
						Form("Number of generated SR photons per event in %s",
							magName->at(iMag).c_str()));
				_h11[iMag]->SetTitle(
						Form("Energy sum of generated SR photons per event in %s;#Sigma_{#gamma} [eV]",
							magName->at(iMag).c_str()));
				_h10[iMag]->Fill(magNph->at(iMag));
				_h11[iMag]->Fill(magSumEph_eV->at(iMag));
			}
		}
   	}
 	printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jentry/nEntries);

	file->Close();
	delete file;
	file = nullptr;

	return;
}


void AnalysisSR::WriteHisto()
{
	cout<<__FUNCTION__<<endl;

	if(!GetDataProdFlag())
	{
		_fOut->cd();
		
		_h0->Write();
		delete _h0;
		_h1->Write();
		delete _h1;
		_h2->Write();
		delete _h2;
		_h3x->Write();
		delete _h3x;
		_h3y->Write();
		delete _h3y;
		_h4->Write();
		delete _h4;
		_h5->Write();
		delete _h5;
		_h6->Write();
		delete _h6;
		_h7->Write();
		delete _h7;
		_h8->Write();
		delete _h8;
		_h9->Write();
		delete _h9;
		_fOut->Flush();
		_h21->Write();
		delete _h21;
		_h21_hepmc->Write();
		delete _h21_hepmc;
		_h22_hepmc->Write();
		delete _h22_hepmc;
		_h23_hepmc->Write();
		delete _h23_hepmc;
		for(Int_t iMag = 0; iMag < NMAGMAX; iMag++)
		{
			_h10[iMag]->Write();
			delete _h10[iMag];
			_h11[iMag]->Write();
			delete _h11[iMag];
		}
		_h0_Xnorm->Write();
		delete _h0_Xnorm;
		_h1_Xnorm->Write();
		delete _h1_Xnorm;
		_h0_Ynorm->Write();
		delete _h0_Ynorm;
		_h1_Ynorm->Write();
		delete _h1_Ynorm;
		// for benchmark/debug
		h1_ene->Write();
		delete h1_ene;
		h1_dirx->Write();
		delete h1_dirx;
		h1_diry->Write();
		delete h1_diry;
		h1_dirz->Write();
		delete h1_dirz;
		h1_posx->Write();
		delete h1_posx;
		h1_posy->Write();
		delete h1_posy;
		h1_posz->Write();
		delete h1_posz;
		h1_ene_dirx->Write();
		delete h1_ene_dirx;
		h1_ene_diry->Write();
		delete h1_ene_diry;
		h1_ene_dirz->Write();
		delete h1_ene_dirz;

		_fOut->Close();
	}

	return;
}
