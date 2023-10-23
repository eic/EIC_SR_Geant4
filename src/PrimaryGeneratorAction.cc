#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4Electron.hh"
#include "Constants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* runAct) : runAction(runAct)
{
    	//default values;
    	_deltaP = 0.0;
    	_P = 1.0;
    	_PdirX = 0.0;
    	_PdirY = 0.0;
    	_PdirZ = 1.0;
    	_pName = "e-";

    	fPrimaryMessenger = new PrimaryMessenger(this);
    	fParticleGun = new G4ParticleGun(1);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    	delete fParticleGun;
    	delete fPrimaryMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    	G4ParticleDefinition* particleDef;
    	particleDef = particleTable->FindParticle(_pName.c_str());

    	runAction->_beamPdg = particleDef->GetPDGEncoding();
	G4double mass = particleDef->GetPDGMass();
    	G4double Mom = G4RandGauss::shoot(_P,_deltaP*_P);
    	G4double Ekin = std::sqrt(Mom*Mom + mass*mass) - mass;

	G4double beamEmitX = 24.0*nm;
	G4double beamEmitY =  2.0*nm;
	G4double beamBetaX = 202.4452538*m;
	G4double beamBetaY = 128.4636335*m;
	G4double beamEtaX = -0.02443133971*m;
	G4double beamEtaY = 0.0*m;
	G4double beamEtaPrimeX = 0.01315593225;
	G4double beamEtaPrimeY = 0.0;
	G4double sigmaX = sqrt(beamEmitX*beamBetaX + pow(beamEtaX*_deltaP,2));
	G4double sigmaY = sqrt(beamEmitY*beamBetaY + pow(beamEtaY*_deltaP,2));
	G4double sigmaXprime = sqrt(beamEmitX/beamBetaX + pow(beamEtaPrimeX*_deltaP,2));
	G4double sigmaYprime = sqrt(beamEmitY/beamBetaY + pow(beamEtaPrimeY*_deltaP,2));

	// generate primary particles at the IP
	G4double posX = G4RandGauss::shoot(0.0,sigmaX);
	G4double posY = G4RandGauss::shoot(0.0,sigmaY);
	G4double posZ = 0.0;

  	// create a rotation matrix
  	G4RotationMatrix rotation = G4RotationMatrix();
  	rotation.rotateX(atan(_PdirY/_PdirZ)*rad);
  	rotation.rotateY(atan(_PdirX/_PdirZ)*rad);
  	rotation.rotateZ(atan(_PdirY/_PdirX)*rad);
	G4ThreeVector translation = G4ThreeVector(Constants::beam_pos_x,Constants::beam_pos_y,Constants::beam_pos_z); 

	G4ThreeVector posXYZrot = rotation * G4ThreeVector(posX,posY,posZ) + translation;

	_PdirX = G4RandGauss::shoot(_PdirX,sigmaXprime);
	_PdirY = G4RandGauss::shoot(_PdirY,sigmaYprime);

	runAction->_beamPosX = posXYZrot.x();
	runAction->_beamPosY = posXYZrot.y();
	runAction->_beamPosZ = posXYZrot.z();
	runAction->_beamMomX = Mom*_PdirX;
	runAction->_beamMomY = Mom*_PdirY;
	runAction->_beamMomZ = Mom*_PdirZ;
	runAction->_beamEnergy = std::sqrt(Mom*Mom + mass*mass);

    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(_PdirX,_PdirY,_PdirZ));
    	fParticleGun->SetParticlePosition(posXYZrot);

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
}
