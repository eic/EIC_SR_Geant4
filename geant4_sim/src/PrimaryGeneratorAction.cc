#include "PrimaryGeneratorAction.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
    	fParticleGun = new G4ParticleGun(1);
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintBeamParameters();
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

    	runAction->_beamPdg = particleDef->GetPDGEncoding();
	G4double mass = particleDef->GetPDGMass();
	G4double gamma = simPar->GetBeamGamma();
	G4double Mom = sqrt(pow(gamma,2) - 1.0)*mass;
	Mom = G4RandGauss::shoot(Mom,simPar->GetBeamMomSpread()*Mom);
	G4double Ekin = sqrt(pow(Mom,2) + pow(mass,2)) - mass;

	G4double sigmaX = sqrt(simPar->GetBeamEmitX()*simPar->GetBeamBetaX() + 
				pow(simPar->GetBeamEtaX()*simPar->GetBeamMomSpread(),2));
	G4double sigmaY = sqrt(simPar->GetBeamEmitY()*simPar->GetBeamBetaY() + 
				pow(simPar->GetBeamEtaY()*simPar->GetBeamMomSpread(),2));
	G4double sigmaXprime = sqrt(simPar->GetBeamEmitX()/simPar->GetBeamBetaX() + 
				pow(simPar->GetBeamEtaPrimeX()*simPar->GetBeamMomSpread(),2));
	G4double sigmaYprime = sqrt(simPar->GetBeamEmitY()/simPar->GetBeamBetaY() + 
				pow(simPar->GetBeamEtaPrimeY()*simPar->GetBeamMomSpread(),2));

	// generate primary particles at the IP
	G4ThreeVector posXYZ = {	G4RandGauss::shoot(0.0,sigmaX),
					G4RandGauss::shoot(0.0,sigmaY),
					0.0};

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
	// initial beam direction
	G4ThreeVector beamDirection(0,0,1);
	// rotate the direction vector
	beamDirection.rotateY(angle);

  	// create rotation matrix and translation vector
  	G4RotationMatrix rotation = G4RotationMatrix();
  	rotation.rotateY(angle);
	G4ThreeVector translation = magVector2; 
	// move initial coordinates to the new position
	G4ThreeVector posXYZrot = rotation * posXYZ + translation;
	// spread the direction
	G4ThreeVector dirXYZ = {	G4RandGauss::shoot(beamDirection.x(),sigmaXprime),
					G4RandGauss::shoot(beamDirection.y(),sigmaYprime),
					beamDirection.z()};

	runAction->_beamPosX = posXYZrot.x()/cm;
	runAction->_beamPosY = posXYZrot.y()/cm;
	runAction->_beamPosZ = posXYZrot.z()/cm;
	runAction->_beamMomX = Mom*dirXYZ.x()/eV;
	runAction->_beamMomY = Mom*dirXYZ.y()/eV;
	runAction->_beamMomZ = Mom*dirXYZ.z()/eV;
	runAction->_beamEnergy = (Ekin + mass)/eV;

    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(dirXYZ);
    	fParticleGun->SetParticlePosition(posXYZrot);

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
}
