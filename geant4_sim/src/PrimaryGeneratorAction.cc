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
	simPar->PrintBeamParameters();

	if(simPar->GetBeamType() == "tail")
	{
		fBeamTailX = new TF1("fBeamTailX",getBeamTailDistribution,simPar->GetBeamTailMin(),simPar->GetBeamTailMax(),1);
		fBeamTailY = new TF1("fBeamTailY",getBeamTailDistribution,simPar->GetBeamTailMin(),simPar->GetBeamTailMax(),1);
		fBeamTailX->SetParameter(0,simPar->GetBeamTailKx());
		fBeamTailY->SetParameter(0,simPar->GetBeamTailKy());
	}
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    	delete fParticleGun;
	delete simPar;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
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
	G4ThreeVector magVector1(	simPar->GetBeamStartPosX() - simPar->GetIpPosX(),
					simPar->GetBeamStartPosY() - simPar->GetIpPosY(),
					simPar->GetBeamStartPosZ() - simPar->GetIpPosZ());
	// vector of end coordinates in the global ref. system shifted w.r.t. the ip
	G4ThreeVector magVector2(	simPar->GetBeamEndPosX() - simPar->GetIpPosX(),
					simPar->GetBeamEndPosY() - simPar->GetIpPosY(),
					simPar->GetBeamEndPosZ() - simPar->GetIpPosZ());
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
	G4ThreeVector posXYZrot = rotation * G4ThreeVector(X_real,Y_real,0.0) + translation;
	G4ThreeVector dirXYZrot = rotation * 
		G4ThreeVector(Xprime_real,Yprime_real,sqrt(1.0 - (pow(Xprime_real,2) + pow(Yprime_real,2))));

	// store beam coordinates
    	runAction->_beamPdg = particleDef->GetPDGEncoding();
	runAction->_beamPosX = posXYZrot.x()/cm;
	runAction->_beamPosY = posXYZrot.y()/cm;
	runAction->_beamPosZ = posXYZrot.z()/cm;
	runAction->_beamMomX = Mom*dirXYZrot.x()/eV;
	runAction->_beamMomY = Mom*dirXYZrot.y()/eV;
	runAction->_beamMomZ = Mom*dirXYZrot.z()/eV;
	runAction->_beamEnergy = (Ekin + mass)/eV;
	runAction->_beamRefPosX = magVector2.x()/cm;
	runAction->_beamRefPosY = magVector2.y()/cm;
	runAction->_beamRefPosZ = (Float_t)magVector2.z()/cm;
	runAction->_beamRefAngle = angle/rad;

	// generate the primary beam 
    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(dirXYZrot);
    	fParticleGun->SetParticlePosition(posXYZrot);

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
}
