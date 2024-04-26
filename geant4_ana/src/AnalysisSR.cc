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

void AnalysisSR::Init(TString treeName = "tree", Double_t minZ = -1e9, Double_t maxZ = 1e9)
{
	// set Z coord limits
	_minZ = minZ;
	_maxZ = maxZ;

	_fChain = new TChain(treeName.Data());
   	if(!_fChain){return;};
	_fChain->Add(_inputFileName.Data());

	cout<<"\n\n=================================================================="<<endl;
	cout<<"\n[INFO] AnalysisSR::Init ==> Added files:"<<endl;
	_fChain->ls();
	cout<<"==================================================================\n\n"<<endl;

   	// set object pointer
	beamPdg = nullptr;
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
   	_fChain->SetBranchAddress("beamRefPosX_cm", &beamRefPosX_cm, &b_beamRefPosX_cm);
   	_fChain->SetBranchAddress("beamRefPosY_cm", &beamRefPosY_cm, &b_beamRefPosY_cm);
   	_fChain->SetBranchAddress("beamRefPosZ_cm", &beamRefPosZ_cm, &b_beamRefPosZ_cm);
   	_fChain->SetBranchAddress("beamRefAngle_rad", &beamRefAngle_rad, &b_beamRefAngle_rad);

   	_fChain->SetBranchAddress("magName", &magName, &b_magName);
   	_fChain->SetBranchAddress("magNph", &magNph, &b_magNph);
   	_fChain->SetBranchAddress("magSumEph_eV", &magSumEph_eV, &b_magSumEph_eV);

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
   	_fChain->SetBranchAddress("monRefPosX_cm", &monRefPosX_cm, &b_monRefPosX_cm);
   	_fChain->SetBranchAddress("monRefPosY_cm", &monRefPosY_cm, &b_monRefPosY_cm);
   	_fChain->SetBranchAddress("monRefPosZ_cm", &monRefPosZ_cm, &b_monRefPosZ_cm);
   	_fChain->SetBranchAddress("monRefAngle_rad", &monRefAngle_rad, &b_monRefAngle_rad);

	_fChain->GetEntry(0);
	// histograms
	_h0 = new TH1F("h0","SR photon energy (abs(gammaPosZ_cm) < 65 cm);E_{#gamma} [eV];# of photons",
		101000,-1000,100000);
	_h1 = new TH1F("h1","SR photon energy;E_{#gamma} [eV];# of photons",101000,-1000,100000);
	_h2 = new TH2F("h2","SR photon position;Z_{#gamma} [cm];X_{#gamma} [cm]",40000,-4000,4000,2000,-100,100);
	_h3x = new TH2F("h3x","SR photon vertex position;Z_{vtx} [cm];X_{vtx} [cm]",40000,-4000,4000,2000,-100,100);
	_h3y = new TH2F("h3y","SR photon vertex position;Z_{vtx} [cm];Y_{vtx} [cm]",40000,-4000,4000,2000,-100,100);

	_h4 = new TH2F("h4",Form("Monitor [%s]: Px vs X;X [cm];X'",monName->c_str()),1000,-4,4,10000,-1e-1,1e-1);
	_h5 = new TH2F("h5",Form("Monitor [%s]: Py vs Y;Y [cm];Y'",monName->c_str()),1000,-4,4,10000,-1e-1,1e-1);
	_h6 = new TH1F("h6",Form("Monitor [%s]: Momentum;P [GeV/c]",monName->c_str()),20000,0,20);

	_h7 = new TH2F("h7",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Px vs X;X [cm];X'",
		beamRefPosX_cm,beamRefPosY_cm,beamRefPosZ_cm,beamRefAngle_rad),1000,-4,4,10000,-1e-1,1e-1);
	_h8 = new TH2F("h8",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Py vs Y;Y [cm];Y'",
		beamRefPosX_cm,beamRefPosY_cm,beamRefPosZ_cm,beamRefAngle_rad),1000,-4,4,10000,-1e-2,1e-2);
	_h9 = new TH1F("h9",Form("Beam [(%.1e,%.1e,%.1e) cm, %.1e rad]: Momentum;P [GeV/c]",
		beamRefPosX_cm,beamRefPosY_cm,beamRefPosZ_cm,beamRefAngle_rad),20000,0,20);

	for(Int_t iMag = 0; iMag < magName->size(); iMag++)
	{
		_h10[iMag] = new TH1F(Form("h10_%d",iMag),
			Form("Number of generated SR photons per event in %s",magName->at(iMag).c_str()),10000,0,10000);
		_h11[iMag] = new TH1F(Form("h11_%d",iMag),
			Form("Energy sum of generated SR photons per event in %s;#Sigma_{#gamma} [eV]",
			magName->at(iMag).c_str()),1e6,0,100e6);
	}	

	// ascii file to store generated events
	_fHepmc = new WriterAscii(GetOutputFileNameHepmc().Data());

	// for benchmark/debug
	h1_ene = new TH1D("h1_ene_g4","SR photon spectrum;E_{#gamma} [eV];Flux [ph/s]",1e6,0,1e6);
	h1_dirx = new TH1D("h1_dirx_g4","SR photon momDir X; Px_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
	h1_diry = new TH1D("h1_diry_g4","SR photon momDir Y; Py_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
	h1_dirz = new TH1D("h1_dirz_g4","SR photon momDir Z; Pz_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2);
	h1_posx = new TH1D("h1_posx_g4","SR photon position X; X_{#gamma} [cm];Flux [ph/s]",20000,-10,10);
	h1_posy = new TH1D("h1_posy_g4","SR photon position Y; Y_{#gamma} [cm];Flux [ph/s]",20000,-10,10);
	h1_posz = new TH1D("h1_posz_g4","SR photon position Z; Z_{#gamma} [cm];Flux [ph/s]",20000,-100,100);
	h1_ene_dirx = 
		new TH2D("h1_ene_dirx_g4",
		"SR photon energy vs momDir X;Px_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
		4e3,-2,2,4e3,0,40e3);
	h1_ene_diry = 
		new TH2D("h1_ene_diry_g4",
		"SR photon energy vs momDir Y;Py_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
		4e3,-2,2,4e3,0,40e3);
	h1_ene_dirz = 
		new TH2D("h1_ene_dirz_g4",
		"SR photon energy vs momDir Z;Pz_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",
		4e3,-2,2,4e3,0,40e3);

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
			// for benchmark/debug
//			if(	-3350.0 < gammaPosZ_cm->at(i) && gammaPosZ_cm->at(i) < -2350.0 &&
//				gammaPosX_cm->at(i) < -4.925) // two facets 28755 28756
//			if(	-67.0 < gammaPosZ_cm->at(i) && gammaPosZ_cm->at(i) < 68.69231 &&
//				2.684679 < gammaPosX_cm->at(i) && gammaPosX_cm->at(i) < 2.913047 &&
//				-1.55 < gammaPosY_cm->at(i) && gammaPosY_cm->at(i) < -1.060262 &&
//				gammaEnergy_eV->at(i) > 10000.0) // IP facet 26455
//			if(	-67.0 < gammaPosZ_cm->at(i) && gammaPosZ_cm->at(i) < 68.69231	) // IP beam pipe
			if(	-65.0 < gammaPosZ_cm->at(i) && gammaPosZ_cm->at(i) < 65.0	) // around the IP
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

			//---------------------------------------------------------------------------
			// fill histograms

			// only SR photons around the IP6
			if(TMath::Abs(gammaPosZ_cm->at(i)) < 65.0)
				_h0->Fill(gammaEnergy_eV->at(i));

			_h1->Fill(gammaEnergy_eV->at(i));
			_h2->Fill(gammaPosZ_cm->at(i),gammaPosX_cm->at(i));
			_h3x->Fill(gammaVtxPosZ_cm->at(i),gammaVtxPosX_cm->at(i));
			_h3y->Fill(gammaVtxPosZ_cm->at(i),gammaVtxPosY_cm->at(i));

			//---------------------------------------------------------------------------
			// generate a HEPMC event

			// flip XZ coordinates from Geant4 to EIC
			x = (-1)*gammaPosX_cm->at(i)*1e1; // cm -> mm
			y = ( 1)*gammaPosY_cm->at(i)*1e1; // cm -> mm
			z = (-1)*gammaPosZ_cm->at(i)*1e1; // cm -> mm

			// flip PxPz coordinates from Geant4 to EIC
			px = (-1)*gammaMomX_eV->at(i)/1e9; // eV -> GeV
			py = ( 1)*gammaMomY_eV->at(i)/1e9; // eV -> GeV
			pz = (-1)*gammaMomZ_eV->at(i)/1e9; // eV -> GeV

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
		if(!event.vertices_empty())
			_fHepmc->write_event(event);

		// monitor data
		TVector3 monMom(monMomX_eV,monMomY_eV,monMomZ_eV);
		TVector3 monPos(monPosX_cm-monRefPosX_cm,monPosY_cm-monRefPosY_cm,monPosZ_cm-monRefPosZ_cm);
		monMom.RotateY(-monRefAngle_rad);
		monPos.RotateY(-monRefAngle_rad);

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
		}

		// loop over magnets
		for(Int_t iMag = 0; iMag < magName->size(); iMag++) 
		{
			_h10[iMag]->Fill(magNph->at(iMag));
			_h11[iMag]->Fill(magSumEph_eV->at(iMag));
		}
   	}
 	printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jentry/nEntries);

	TFile* f = new TFile(GetOutputFileNameRoot().Data(),"recreate");
	f->cd();
	_h0->Write();
	_h1->Write();
	_h2->Write();
	_h3x->Write();
	_h3y->Write();
	_h4->Write();
	_h5->Write();
	_h6->Write();
	_h7->Write();
	_h8->Write();
	_h9->Write();
	for(Int_t iMag = 0; iMag < magName->size(); iMag++)
	{
		_h10[iMag]->Write();
		_h11[iMag]->Write();
	}
	// for benchmark/debug
	h1_ene->Write();
	h1_dirx->Write();
	h1_diry->Write();
	h1_dirz->Write();
	h1_posx->Write();
	h1_posy->Write();
	h1_posz->Write();
	h1_ene_dirx->Write();
	h1_ene_diry->Write();
	h1_ene_dirz->Write();

	f->Close();

	return;
}
