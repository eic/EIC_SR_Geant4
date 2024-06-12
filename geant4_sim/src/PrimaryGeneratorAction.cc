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
		if(simPar->GetBeamType() == "tail")
		{
			fBeamTailX = new TF1("fBeamTailX",getBeamTailDistribution,
				simPar->GetBeamTailMin(),simPar->GetBeamTailMax(),1);
			fBeamTailY = new TF1("fBeamTailY",getBeamTailDistribution,
				simPar->GetBeamTailMin(),simPar->GetBeamTailMax(),1);
			fBeamTailX->SetParameter(0,simPar->GetBeamTailKx());
			fBeamTailY->SetParameter(0,simPar->GetBeamTailKy());
		}
	}
	else if(simPar->GetGenerator() == "HepMC")
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
	if(simPar->GetGenerator() == "HepMC")
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
	else
	{
		throw runtime_error(
			"[ERROR] PrimaryGeneratorAction::PrimaryGeneratorAction ==> Wrong particle generator name\n");
	}

	return;
}

void PrimaryGeneratorAction::StorePrimaryCoordinates(
	const G4Event* anEvent, const G4ThreeVector magVector, 
	const G4double angle, const G4double Q2, const G4double dE)
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
			runAction->_beamTransfEnergy.push_back(dE/GeV);
		}
	}

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
	G4double Q2 = 0, E_i = 0, E_f = 0;

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

	bool gotParticle = false;
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
				G4LorentzVector q = k_i - k_f;
				Q2 = -q.m2();

				g4prim = new G4PrimaryParticle(pdgcode, k_f.x()*GeV, k_f.y()*GeV, k_f.z()*GeV);
				g4vtx->SetPrimary(g4prim);

				gotParticle = true;
      			}
			// we need only one scattered electron, skip the rest of particles
			if(gotParticle){break;}
    		}
		// we need only one scattered electron, skip the rest of vertices
		if(gotParticle){break;}
	}

	// generate the primary beam 
    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(g4prim->GetKineticEnergy());
    	fParticleGun->SetParticleMomentumDirection(g4prim->GetMomentumDirection());
    	fParticleGun->SetParticlePosition(g4vtx->GetPosition());

    	fParticleGun->GeneratePrimaryVertex(g4event);

	// Store Coordinates
	StorePrimaryCoordinates(g4event,G4ThreeVector(0,0,0),0,Q2*GeV*GeV,(E_i-E_f)*GeV);

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
	G4double Mom = sqrt(pow(gamma,2) - 1.0)*mass;
	Mom = G4RandGauss::shoot(Mom,simPar->GetBeamMomSpread()*Mom);
	G4double Ekin = sqrt(pow(Mom,2) + pow(mass,2)) - mass;
	
	//- Normalized coordinates [see S.Y.Lee "Accelerator Physics" 4th edition, (Floquet Transformation) page 49]
	// X
	G4double EtaX_norm = (1./sqrt(simPar->GetBeamBetaX()))*simPar->GetBeamEtaX();
	G4double EtaPrimeX_norm = (simPar->GetBeamAlphaX()/sqrt(simPar->GetBeamBetaX()))*simPar->GetBeamEtaX() +
		sqrt(simPar->GetBeamBetaX())*simPar->GetBeamEtaPrimeX();
	G4double sigmaX_norm = sqrt(simPar->GetBeamEmitX() + pow(EtaX_norm*simPar->GetBeamMomSpread(),2));
	G4double sigmaPrimeX_norm = sqrt(simPar->GetBeamEmitX() + pow(EtaPrimeX_norm*simPar->GetBeamMomSpread(),2));
	// Y
	G4double EtaY_norm = (1./sqrt(simPar->GetBeamBetaY()))*simPar->GetBeamEtaY();
	G4double EtaPrimeY_norm = (simPar->GetBeamAlphaY()/sqrt(simPar->GetBeamBetaY()))*simPar->GetBeamEtaY() +
		sqrt(simPar->GetBeamBetaY())*simPar->GetBeamEtaPrimeY();
	G4double sigmaY_norm = sqrt(simPar->GetBeamEmitY() + pow(EtaY_norm*simPar->GetBeamMomSpread(),2));
	G4double sigmaPrimeY_norm = sqrt(simPar->GetBeamEmitY() + pow(EtaPrimeY_norm*simPar->GetBeamMomSpread(),2));

	// Initial normalized coordinates
	G4double X_norm, Y_norm, Xprime_norm, Yprime_norm;

	// beam core -- Gaussian
	// XX'
	X_norm = G4RandGauss::shoot(0.0,sigmaX_norm);
	Xprime_norm = G4RandGauss::shoot(0.0,sigmaPrimeX_norm);
	// YY'
	Yprime_norm = G4RandGauss::shoot(0.0,sigmaPrimeY_norm);
	Y_norm = G4RandGauss::shoot(0.0,sigmaY_norm);

	// beam tail -- non-Gaussian
	if(simPar->GetBeamType() == "tail")
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

	//- Real coordinates [see S.Y.Lee "Accelerator Physics" 4th edition, (Floquet Transformation) page 49]
	G4double X_real = X_norm*sqrt(simPar->GetBeamBetaX());
	G4double Xprime_real = (-simPar->GetBeamAlphaX()/sqrt(simPar->GetBeamBetaX()))*X_norm +
		(1./sqrt(simPar->GetBeamBetaX()))*Xprime_norm;
	G4double Y_real = Y_norm*sqrt(simPar->GetBeamBetaY());
	G4double Yprime_real = (-simPar->GetBeamAlphaY()/sqrt(simPar->GetBeamBetaY()))*Y_norm +
		(1./sqrt(simPar->GetBeamBetaY()))*Yprime_norm;

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
