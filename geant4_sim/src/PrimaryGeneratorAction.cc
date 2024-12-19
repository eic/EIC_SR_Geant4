#include "PrimaryGeneratorAction.hh"

// Beam tail function 
// Ref.[Christoph Montag, "Transverse Beam Tails and Beam Lifetime in the EIC Electron Storage Ring" (July 1, 2021)]
Double_t getBeamTailDistribution(Double_t *x, Double_t *par){return ( par[0]/pow(x[0],3) );}

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
    	fParticleGun = new G4ParticleGun(1);
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintGenerator();

	if(simPar->GetGenerator() == "Beam")
	{
		if(simPar->GetBeamType() == "tailCubic")
		{
			fBeamTailX = new TF1("fBeamTailX",getBeamTailDistribution,
				simPar->GetBeamTailXmin(),simPar->GetBeamTailXmax(),1);
			fBeamTailY = new TF1("fBeamTailY",getBeamTailDistribution,
				simPar->GetBeamTailYmin(),simPar->GetBeamTailYmax(),1);
			fBeamTailX->SetParameter(0,simPar->GetBeamTailKx());
			fBeamTailY->SetParameter(0,simPar->GetBeamTailKy());
		}
		else if(simPar->GetBeamType() == "tailUnif")
		{
			fBeamTailX = new TF1("fBeamTailX", "[0]", 0, simPar->GetBeamTailXmax());
			fBeamTailY = new TF1("fBeamTailY", "[0]", 0, simPar->GetBeamTailYmax());
			// Set normalization factor: 
			// 	ensures the integral of the function over the range is 1, 
			// 	making it a valid probability density function.
			fBeamTailX->SetParameter(0, 1.0 / (simPar->GetBeamTailXmax() - 0));
			fBeamTailY->SetParameter(0, 1.0 / (simPar->GetBeamTailYmax() - 0));
		}
	}
	else if(G4StrUtil::contains(simPar->GetGenerator(),"HepMC"))
	{
		fHepMcAsciiInput = new HepMC3::ReaderAscii(simPar->GetHepMcFileName());
	}
	else
	{
		throw runtime_error(
			"[ERROR] PrimaryGeneratorAction::PrimaryGeneratorAction ==> Wrong particle generator name\n");
	}
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	if(G4StrUtil::contains(simPar->GetGenerator(),"HepMC"))
	{
		fHepMcAsciiInput->close();
		delete fHepMcAsciiInput;
	}
    	delete fParticleGun;
	delete simPar;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	if(simPar->GetGenerator() == "Beam")
	{
		GenerateBeamPrimaries(anEvent);
	}
	else if(simPar->GetGenerator() == "HepMC")
	{
		GenerateHepMcPrimaries(anEvent);
	}
	else if(simPar->GetGenerator() == "HepMC_All")
	{
		GenerateHepMcAllPrimaries(anEvent);
	}
	else if(simPar->GetGenerator() == "HepMC_GETaLM")
	{
		GenerateHepMcGETaLMPrimaries(anEvent);
	}
	else
	{
		throw runtime_error(
			"[ERROR] PrimaryGeneratorAction::PrimaryGeneratorAction ==> Wrong particle generator name\n");
	}

	return;
}

void PrimaryGeneratorAction::StorePrimaryCoordinates(
	const G4Event* anEvent, const G4ThreeVector magVector, 
	const G4double angle, const G4double Q2, const G4double x)
{
	// Loop over vertices
	for(G4int ivtx = 0; ivtx < anEvent->GetNumberOfPrimaryVertex(); ivtx++)
	{
		G4PrimaryVertex* vtx = anEvent->GetPrimaryVertex(ivtx);
		// Loop over particles
		for(G4int iptc = 0; iptc < vtx->GetNumberOfParticle(); iptc++)
		{
			G4PrimaryParticle* ptc = vtx->GetPrimary(iptc);

			runAction->_beamPdg.push_back(ptc->GetPDGcode());
			runAction->_beamPosX.push_back(vtx->GetX0()/cm);
			runAction->_beamPosY.push_back(vtx->GetY0()/cm);
			runAction->_beamPosZ.push_back(vtx->GetZ0()/cm);
			runAction->_beamMomX.push_back(ptc->GetPx()/eV);
			runAction->_beamMomY.push_back(ptc->GetPy()/eV);
			runAction->_beamMomZ.push_back(ptc->GetPz()/eV);
			runAction->_beamEnergy.push_back(ptc->GetTotalEnergy()/eV);
			runAction->_beamRefPosX.push_back(magVector.x()/cm);
			runAction->_beamRefPosY.push_back(magVector.y()/cm);
			runAction->_beamRefPosZ.push_back(magVector.z()/cm);
			runAction->_beamRefAngle.push_back(angle/rad);
			runAction->_beamTransfMomSqr.push_back(Q2/GeV/GeV);
			runAction->_beamBjorkenX.push_back(x);
		}
	}

	return;
}

void PrimaryGeneratorAction::GenerateHepMcAllPrimaries(G4Event* g4event)
{
	// [https://gitlab.cern.ch/hepmc/HepMC3/-/tree/master/include/HepMC3?ref_type=heads]
	// [https://pythia.org/latest-manual/ParticleProperties.html]
	// [https://twiki.cern.ch/twiki/bin/view/Main/MC_status_code]

    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4PrimaryParticle* g4prim;
	G4PrimaryVertex* g4vtx;

	// Construct an event
	HepMC3::GenEvent hepmcevt(HepMC3::Units::GEV,HepMC3::Units::MM);
	// Check if there are available events
	if(fHepMcAsciiInput->failed())
		G4RunManager::GetRunManager()->AbortRun();
	// Read event from input file
	fHepMcAsciiInput->read_event(hepmcevt);
	// Print the first event info
	if(g4event->GetEventID() == 0)
	{
		HepMC3::Print::listing(hepmcevt);
		HepMC3::Print::content(hepmcevt);
	}

	bool gotParticles = false;
	// Loop over vertices
	for (const auto& vertex : hepmcevt.vertices())
	{
		// Loop over outcoming particles
		// Only for a real vertex (i.e., stable particle or the one which will decay later)
		for(const auto& particle : vertex->particles_out()) 
		{
			// Select only electrons
			if (!particle->end_vertex() && particle->status() == 1) 
			{
				// Check world boundary
				HepMC3::FourVector pos = vertex->position();
				G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
				if (! CheckVertexInsideWorld(xvtx.vect() * mm)) continue;

				// Create G4PrimaryVertex and associated G4PrimaryParticles
				g4vtx = new G4PrimaryVertex(
					xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm,xvtx.t()*mm/c_light);

				G4int pdgcode = particle->pid();
				HepMC3::FourVector mom = particle->momentum();

				g4prim = new G4PrimaryParticle(pdgcode, mom.px()*GeV, mom.py()*GeV, mom.pz()*GeV);
				g4vtx->SetPrimary(g4prim);

				// generate the primary beam 
				G4ParticleDefinition* particleDef = particleTable->FindParticle(pdgcode);
				fParticleGun->SetParticleDefinition(particleDef);
				fParticleGun->SetParticleEnergy(g4prim->GetKineticEnergy());
				fParticleGun->SetParticleMomentumDirection(g4prim->GetMomentumDirection());
				fParticleGun->SetParticlePosition(g4vtx->GetPosition());

				fParticleGun->GeneratePrimaryVertex(g4event);

				gotParticles = true;
      			}
    		}
	}
	// Abort the run if there are no more particles to generate
	if(!gotParticles)
	{
		G4cout<<"[WARNING] No more particle to generate ==> Abort the Run!"<<G4endl;
		G4RunManager::GetRunManager()->AbortRun(false);
	}

	// Store Coordinates
	StorePrimaryCoordinates(g4event,G4ThreeVector(0,0,0),0,0,0);

	return;
}

void PrimaryGeneratorAction::GenerateHepMcPrimaries(G4Event* g4event)
{
	// [https://gitlab.cern.ch/hepmc/HepMC3/-/tree/master/include/HepMC3?ref_type=heads]
	// [https://pythia.org/latest-manual/ParticleProperties.html]
	// [https://twiki.cern.ch/twiki/bin/view/Main/MC_status_code]

    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    	G4ParticleDefinition* particleDef;
    	particleDef = particleTable->FindParticle(simPar->GetBeamName().c_str());
	G4PrimaryParticle* g4prim;
	G4PrimaryVertex* g4vtx;
	G4double Q2 = 0, E_i = 0, E_f = 0, targetMass = 0, E_2 = 0;
	G4LorentzVector q, p2;

	// Construct an event
	HepMC3::GenEvent hepmcevt(HepMC3::Units::GEV,HepMC3::Units::MM);
	// Check if there are available events
	if(fHepMcAsciiInput->failed())
		G4RunManager::GetRunManager()->AbortRun();
	// Read event from input file
	fHepMcAsciiInput->read_event(hepmcevt);
	// Print the first event info
	if(g4event->GetEventID() == 0)
	{
		HepMC3::Print::listing(hepmcevt);
		HepMC3::Print::content(hepmcevt);
	}

	bool gotParticles = false;
	// Loop over vertices
	for (const auto& vertex : hepmcevt.vertices())
	{
		// Loop over outcoming particles
		// Only for a real vertex (i.e., stable particle or the one which will decay later)
		for(const auto& particle : vertex->particles_out()) 
		{
			// Select only electrons
			if (!particle->end_vertex() && particle->status() == 1 && particle->pid() == 11) 
			{
				// Select the particle from the beam only (find a grandparent)
				bool fromBeam = false;
				HepMC3::FourVector momBeam;
				G4double massBeam;
				for(const auto& parent : particle->parents())
					for(const auto& grandparent : parent->parents()) 
						if(grandparent->status() == 4 && grandparent->pid() == 11) 
						{
							momBeam = grandparent->momentum();
							massBeam = grandparent->generated_mass();
							fromBeam = true;
						}
				if(!fromBeam){continue;}

				// Check world boundary
				HepMC3::FourVector pos = vertex->position();
				G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
				if (! CheckVertexInsideWorld(xvtx.vect() * mm)) continue;

				// Create G4PrimaryVertex and associated G4PrimaryParticles
				g4vtx = new G4PrimaryVertex(
					xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm,xvtx.t()*mm/c_light);

				G4int pdgcode = particle->pid();
				HepMC3::FourVector mom = particle->momentum();
				// Note the possible loss of precision in the calculation of E^2 - p^2;
				// for particles the correct mass is stored separately to avoid such problems.
				// [https://pythia.org/latest-manual/Welcome.html]
				G4double mass = particle->generated_mass();
				E_f = sqrt(
						pow(mom.x(),2) +
						pow(mom.y(),2) + 
						pow(mom.z(),2) + 
						pow(mass,2));
				E_i = sqrt(	pow(momBeam.x(),2) + 
						pow(momBeam.y(),2) + 
						pow(momBeam.z(),2) + 
						pow(massBeam,2));
				G4LorentzVector k_f(mom.px(), mom.py(), mom.pz(), E_f);
				G4LorentzVector k_i(momBeam.px(), momBeam.py(), momBeam.pz(), E_i);
				q = k_i - k_f;
				Q2 = -q.m2();

				g4prim = new G4PrimaryParticle(pdgcode, k_f.x()*GeV, k_f.y()*GeV, k_f.z()*GeV);
				g4vtx->SetPrimary(g4prim);

				gotParticles = true;
      			}
			// we need only one scattered electron, skip the rest of particles
			if(gotParticles){break;}
    		}
                // Loop over incoming particles
                // Only for a real vertex (i.e., stable particle or the one which will decay later)
                for(const auto& particle : vertex->particles_in()) 
                {
                        // Select only the beam proton
                        if (particle->status() == 4 && particle->pid() == 2212)
                        {
				targetMass = particle->generated_mass();
				HepMC3::FourVector targetMom = particle->momentum();
				E_2 = sqrt(     pow(targetMom.x(),2) +
						pow(targetMom.y(),2) +
						pow(targetMom.z(),2) +
						pow(targetMass,2));

				p2 = G4LorentzVector(targetMom.px(), targetMom.py(), targetMom.pz(), E_2);
				break;
			}
                }
		// we need only one scattered electron, skip the rest of vertices
		if(gotParticles){break;}
	}

	// generate the primary beam 
    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(g4prim->GetKineticEnergy());
    	fParticleGun->SetParticleMomentumDirection(g4prim->GetMomentumDirection());
    	fParticleGun->SetParticlePosition(g4vtx->GetPosition());

    	fParticleGun->GeneratePrimaryVertex(g4event);

	// Store Coordinates
	StorePrimaryCoordinates(g4event,G4ThreeVector(0,0,0),0,Q2*GeV*GeV,Q2/(2.*p2*q));

	return;
}

void PrimaryGeneratorAction::GenerateHepMcGETaLMPrimaries(G4Event* g4event)
{
	// [https://gitlab.cern.ch/hepmc/HepMC3/-/tree/master/include/HepMC3?ref_type=heads]
	// [https://pythia.org/latest-manual/ParticleProperties.html]
	// [https://twiki.cern.ch/twiki/bin/view/Main/MC_status_code]

    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    	G4ParticleDefinition* particleDef;
    	particleDef = particleTable->FindParticle(simPar->GetBeamName().c_str());
	G4PrimaryParticle* g4prim;
	G4PrimaryVertex* g4vtx;
	HepMC3::FourVector momBeam;
	G4double Q2 = 0, E_i = 0, E_f = 0, targetMass = 0, E_2 = 0, massBeam = 0, x = 0;
	G4LorentzVector q, p2;

	// Construct an event
	HepMC3::GenEvent hepmcevt(HepMC3::Units::GEV,HepMC3::Units::MM);
	// Check if there are available events
	if(fHepMcAsciiInput->failed())
		G4RunManager::GetRunManager()->AbortRun();
	// Read event from input file
	fHepMcAsciiInput->read_event(hepmcevt);
	// Print the first event info
	if(g4event->GetEventID() == 0)
	{
		HepMC3::Print::listing(hepmcevt);
		HepMC3::Print::content(hepmcevt);
	}

	// Get event attributions
	auto trueQ2 = hepmcevt.attribute<HepMC3::DoubleAttribute>("trueQ2");
	auto truex = hepmcevt.attribute<HepMC3::DoubleAttribute>("truex");
	auto true_el_theta = hepmcevt.attribute<HepMC3::DoubleAttribute>("true_el_theta");
	auto true_el_px = hepmcevt.attribute<HepMC3::DoubleAttribute>("true_el_px");
	auto true_el_py = hepmcevt.attribute<HepMC3::DoubleAttribute>("true_el_py");
	auto true_el_pz = hepmcevt.attribute<HepMC3::DoubleAttribute>("true_el_pz");
	auto true_el_en = hepmcevt.attribute<HepMC3::DoubleAttribute>("true_el_en");

	bool gotParticles = false;
	// Loop over vertices
	for (const auto& vertex : hepmcevt.vertices())
	{
		// Loop over incoming particles
		// Get beam particles
		for(const auto& particle : vertex->particles_in())
		{
                        // Select only the beam electron
			if (particle->status() == 4 && particle->pid() == 11)
			{
				momBeam = particle->momentum();
				massBeam = particle->generated_mass();
			}

                        // Select only the beam proton
                        if (particle->status() == 4 && particle->pid() == 2212)
                        {
				targetMass = particle->generated_mass();
				HepMC3::FourVector targetMom = particle->momentum();
				E_2 = sqrt(     pow(targetMom.x(),2) +
						pow(targetMom.y(),2) +
						pow(targetMom.z(),2) +
						pow(targetMass,2));

				p2 = G4LorentzVector(targetMom.px(), targetMom.py(), targetMom.pz(), E_2);
			}
		}
		// Loop over outcoming particles
		// Only for a real vertex (i.e., stable particle or the one which will decay later)
		for(const auto& particle : vertex->particles_out()) 
		{
			if (!particle->end_vertex() && particle->status() == 1 && particle->pid() == 11) 
			{
				// Check world boundary
				HepMC3::FourVector pos = vertex->position();
				G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
				if (! CheckVertexInsideWorld(xvtx.vect() * mm)) continue;

				// Create G4PrimaryVertex and associated G4PrimaryParticles
				g4vtx = new G4PrimaryVertex(
					xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm,xvtx.t()*mm/c_light);

				G4int pdgcode = particle->pid();
				HepMC3::FourVector mom = particle->momentum();
				// Note the possible loss of precision in the calculation of E^2 - p^2;
				// for particles the correct mass is stored separately to avoid such problems.
				// [https://pythia.org/latest-manual/Welcome.html]
				G4double mass = particle->generated_mass();
				E_f = sqrt(
						pow(mom.x(),2) +
						pow(mom.y(),2) + 
						pow(mom.z(),2) + 
						pow(mass,2));
				E_i = sqrt(	pow(momBeam.x(),2) + 
						pow(momBeam.y(),2) + 
						pow(momBeam.z(),2) + 
						pow(massBeam,2));
				G4LorentzVector k_f(mom.px(), mom.py(), mom.pz(), E_f);
				G4LorentzVector k_i(momBeam.px(), momBeam.py(), momBeam.pz(), E_i);
				if(trueQ2 && truex) // DIS
				{
					Q2 = trueQ2->value();
					x = truex->value();
				}
				else if(true_el_theta && true_el_px && true_el_py && true_el_pz) // Brems
				{
					Q2 = 2.0 * E_i * E_f * (1.0 - cos(M_PI - true_el_theta->value())); // assuming: m/E -> 0
					G4LorentzVector k_f_true(
						true_el_px->value(), true_el_py->value(), true_el_pz->value(), E_f);
					q = k_i - k_f_true;
				}
				else
				{
					throw runtime_error(
						"[ERROR] PrimaryGeneratorAction::GenerateHepMcGETaLMPrimaries ==> Event attributions\n");
				}

				g4prim = new G4PrimaryParticle(pdgcode, k_f.x()*GeV, k_f.y()*GeV, k_f.z()*GeV);
				g4vtx->SetPrimary(g4prim);

				gotParticles = true;
      			}
			// we need only one scattered electron, skip the rest of particles
			if(gotParticles){break;}
    		}
		// we need only one scattered electron, skip the rest of vertices
		if(gotParticles){break;}
	}

	// generate the primary beam 
    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(g4prim->GetKineticEnergy());
    	fParticleGun->SetParticleMomentumDirection(g4prim->GetMomentumDirection());
    	fParticleGun->SetParticlePosition(g4vtx->GetPosition());

    	fParticleGun->GeneratePrimaryVertex(g4event);

	// Store Coordinates
	if(trueQ2 && truex)
		StorePrimaryCoordinates(g4event,G4ThreeVector(0,0,0),0,Q2*GeV*GeV,x);
	else
		StorePrimaryCoordinates(g4event,G4ThreeVector(0,0,0),0,Q2*GeV*GeV,Q2/(2.*p2*q));

	return;
}
G4bool PrimaryGeneratorAction::CheckVertexInsideWorld(const G4ThreeVector& pos) const
{
	G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
	G4VPhysicalVolume* world = navigator->GetWorldVolume();
	G4VSolid* solid = world->GetLogicalVolume()->GetSolid();
	EInside qinside = solid->Inside(pos);

	if( qinside != kInside) return false;
	else return true;
}

void PrimaryGeneratorAction::GenerateBeamPrimaries(G4Event* anEvent)
{
    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    	G4ParticleDefinition* particleDef;
    	particleDef = particleTable->FindParticle(simPar->GetBeamName().c_str());

	G4double mass = particleDef->GetPDGMass();
	G4double gamma = simPar->GetBeamGamma();
	G4double MomRef = sqrt(pow(gamma,2) - 1.0)*mass;
	G4double Mom = G4RandGauss::shoot(MomRef,simPar->GetBeamMomSpread()*MomRef);
	G4double Ekin = sqrt(pow(Mom,2) + pow(mass,2)) - mass;
	G4double delta = Mom/MomRef - 1.0;
	
	//- Normalized coordinates [see S.Y.Lee "Accelerator Physics" 4th edition, (Floquet Transformation) page 49]
	// X
//	G4double EtaX_norm = (1./sqrt(simPar->GetBeamBetaX()))*simPar->GetBeamEtaX();
//	G4double EtaPrimeX_norm = (simPar->GetBeamAlphaX()/sqrt(simPar->GetBeamBetaX()))*simPar->GetBeamEtaX() +
//		sqrt(simPar->GetBeamBetaX())*simPar->GetBeamEtaPrimeX();
//	G4double sigmaX_norm = sqrt(simPar->GetBeamEmitX() + pow(EtaX_norm*simPar->GetBeamMomSpread(),2));
//	G4double sigmaPrimeX_norm = sqrt(simPar->GetBeamEmitX() + pow(EtaPrimeX_norm*simPar->GetBeamMomSpread(),2));
	// Y
//	G4double EtaY_norm = (1./sqrt(simPar->GetBeamBetaY()))*simPar->GetBeamEtaY();
//	G4double EtaPrimeY_norm = (simPar->GetBeamAlphaY()/sqrt(simPar->GetBeamBetaY()))*simPar->GetBeamEtaY() +
//		sqrt(simPar->GetBeamBetaY())*simPar->GetBeamEtaPrimeY();
//	G4double sigmaY_norm = sqrt(simPar->GetBeamEmitY() + pow(EtaY_norm*simPar->GetBeamMomSpread(),2));
//	G4double sigmaPrimeY_norm = sqrt(simPar->GetBeamEmitY() + pow(EtaPrimeY_norm*simPar->GetBeamMomSpread(),2));

	//- Normalized coordinates
	// X
	G4double sigmaX_norm            = sqrt(simPar->GetBeamEmitX());
	G4double sigmaPrimeX_norm       = sqrt(simPar->GetBeamEmitX());
	// Y
	G4double sigmaY_norm            = sqrt(simPar->GetBeamEmitY());
	G4double sigmaPrimeY_norm       = sqrt(simPar->GetBeamEmitY());

	// Initial normalized coordinates
	G4double X_norm, Y_norm, Xprime_norm, Yprime_norm;

	// beam core -- Gaussian
	// XX'
	X_norm = G4RandGauss::shoot(0.0,sigmaX_norm);
	Xprime_norm = G4RandGauss::shoot(0.0,sigmaPrimeX_norm);
	// YY'
	Y_norm = G4RandGauss::shoot(0.0,sigmaY_norm);
	Yprime_norm = G4RandGauss::shoot(0.0,sigmaPrimeY_norm);

	// beam tail -- non-Gaussian
	if(simPar->GetBeamType() == "tailCubic")
	{
		// XX'
		if(simPar->GetBeamTailKx() > 0)
		{
			G4double rx = fBeamTailX->GetRandom(); // random radius following the given tail distribution
			G4double tx = 2.*G4UniformRand()*M_PI; // uniformly distributed angle in the phase space
			X_norm = rx*cos(tx)*sigmaX_norm;
			Xprime_norm = rx*sin(tx)*sigmaPrimeX_norm;
		}
		// YY'
		if(simPar->GetBeamTailKy() > 0)
		{
			G4double ry = fBeamTailY->GetRandom(); // random radius following the given tail distribution
			G4double ty = 2.*G4UniformRand()*M_PI; // uniformly distributed angle in the phase space
			Y_norm = ry*cos(ty)*sigmaY_norm;
			Yprime_norm = ry*sin(ty)*sigmaPrimeY_norm;
		}
	}
	else if(simPar->GetBeamType() == "tailUnif")
	{
		G4double rx = fBeamTailX->GetRandom(); // random radius following the given tail distribution
		G4double ry = fBeamTailY->GetRandom(); // random radius following the given tail distribution
		while(rx < simPar->GetBeamTailXmin() && ry < simPar->GetBeamTailYmin()) // ignore core particles
		{
			rx = fBeamTailX->GetRandom();
			ry = fBeamTailY->GetRandom();
		}
		G4double tx = 2.*G4UniformRand()*M_PI; // uniformly distributed angle in the phase space 
		G4double ty = 2.*G4UniformRand()*M_PI; // uniformly distributed angle in the phase space

		// normalized coordinates
		X_norm = rx*cos(tx)*sigmaX_norm;
		Xprime_norm = rx*sin(tx)*sigmaPrimeX_norm;
		Y_norm = ry*cos(ty)*sigmaY_norm;
		Yprime_norm = ry*sin(ty)*sigmaPrimeY_norm;
	}

	//- Store normalized coordinates in num. of sigmas
	runAction->_beamX_norm.push_back(X_norm/sigmaX_norm);
	runAction->_beamXprime_norm.push_back(Xprime_norm/sigmaPrimeX_norm);
	runAction->_beamY_norm.push_back(Y_norm/sigmaY_norm);
	runAction->_beamYprime_norm.push_back(Yprime_norm/sigmaPrimeY_norm);

	//- Real coordinates [see S.Y.Lee "Accelerator Physics" 4th edition, (Floquet Transformation) page 49]
//	G4double X_real = X_norm*sqrt(simPar->GetBeamBetaX());
//	G4double Xprime_real = (-simPar->GetBeamAlphaX()/sqrt(simPar->GetBeamBetaX()))*X_norm +
//		(1./sqrt(simPar->GetBeamBetaX()))*Xprime_norm;
//	G4double Y_real = Y_norm*sqrt(simPar->GetBeamBetaY());
//	G4double Yprime_real = (-simPar->GetBeamAlphaY()/sqrt(simPar->GetBeamBetaY()))*Y_norm +
//		(1./sqrt(simPar->GetBeamBetaY()))*Yprime_norm;

	//- Real coordinates
	// X
	G4double X_real = X_norm*sqrt(simPar->GetBeamBetaX()) + 
		simPar->GetBeamEtaX()*delta;
	G4double Xprime_real = (-simPar->GetBeamAlphaX()/sqrt(simPar->GetBeamBetaX()))*X_norm + 
		(1./sqrt(simPar->GetBeamBetaX()))*Xprime_norm + 
		simPar->GetBeamEtaPrimeX()*delta;
	// Y
	G4double Y_real = Y_norm*sqrt(simPar->GetBeamBetaY()) +
		simPar->GetBeamEtaY()*delta;
	G4double Yprime_real = (-simPar->GetBeamAlphaY()/sqrt(simPar->GetBeamBetaY()))*Y_norm +
		(1./sqrt(simPar->GetBeamBetaY()))*Yprime_norm +
		simPar->GetBeamEtaPrimeY()*delta;

	// vector of start coordinates in the global ref. system shifted w.r.t. the ip
	G4ThreeVector magVector1(	-simPar->GetBeamStartPosX() + simPar->GetIpPosX(),
					-simPar->GetBeamStartPosY() + simPar->GetIpPosY(),
					-simPar->GetBeamStartPosZ() + simPar->GetIpPosZ());
	// vector of end coordinates in the global ref. system shifted w.r.t. the ip
	G4ThreeVector magVector2(	-simPar->GetBeamEndPosX() + simPar->GetIpPosX(),
					-simPar->GetBeamEndPosY() + simPar->GetIpPosY(),
					-simPar->GetBeamEndPosZ() + simPar->GetIpPosZ());
	// rotate coordinate vectors from global to local/ip system
	magVector1.rotateY(-simPar->GetIpTheta());
	magVector2.rotateY(-simPar->GetIpTheta());
	// orientation angle of the incoming beam
	G4double angle = atan((magVector2.x()-magVector1.x())/(magVector2.z()-magVector1.z()))*rad;

  	// create rotation matrix and translation vector
  	G4RotationMatrix rotation = G4RotationMatrix();
  	rotation.rotateY(angle);
	G4ThreeVector translation = magVector2; 
	// move initial coordinates to the new position
	G4ThreeVector posXYZrot = 
		rotation * G4ThreeVector(-X_real,-Y_real,0.0) + translation;
	G4ThreeVector dirXYZrot = 
		rotation * G4ThreeVector(	-Xprime_real,
						-Yprime_real,
						-sqrt(1.0 - (pow(Xprime_real,2) + pow(Yprime_real,2))));

	// generate the primary beam 
    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(dirXYZrot);
    	fParticleGun->SetParticlePosition(posXYZrot);

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	// Store Coordinates
	StorePrimaryCoordinates(anEvent,magVector2,angle,0,0);

	return;
}
