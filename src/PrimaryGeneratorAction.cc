#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4Electron.hh"
#include "SimParameters.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* runAct) : runAction(runAct)
{
    	fParticleGun = new G4ParticleGun(1);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    	delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	SimParameters simPar;
	simPar.InitDefault();
	simPar.PrintBeamParameters();

    	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    	G4ParticleDefinition* particleDef;
    	particleDef = particleTable->FindParticle(simPar.GetBeamName().c_str());

    	runAction->_beamPdg = particleDef->GetPDGEncoding();
	G4double mass = particleDef->GetPDGMass();
    	G4double Mom = G4RandGauss::shoot(simPar.GetBeamMom(),simPar.GetBeamMomSpread()*simPar.GetBeamMom());
    	G4double Ekin = std::sqrt(Mom*Mom + mass*mass) - mass;

	G4double sigmaX = sqrt(simPar.GetBeamEmitX()*simPar.GetBeamBetaX() + 
				pow(simPar.GetBeamEtaX()*simPar.GetBeamMomSpread(),2));
	G4double sigmaY = sqrt(simPar.GetBeamEmitY()*simPar.GetBeamBetaY() + 
				pow(simPar.GetBeamEtaY()*simPar.GetBeamMomSpread(),2));
	G4double sigmaXprime = sqrt(simPar.GetBeamEmitX()/simPar.GetBeamBetaX() + 
				pow(simPar.GetBeamEtaPrimeX()*simPar.GetBeamMomSpread(),2));
	G4double sigmaYprime = sqrt(simPar.GetBeamEmitY()/simPar.GetBeamBetaY() + 
				pow(simPar.GetBeamEtaPrimeY()*simPar.GetBeamMomSpread(),2));

	// generate primary particles at the IP
	G4ThreeVector posXYZ = {	G4RandGauss::shoot(0.0,sigmaX),
					G4RandGauss::shoot(0.0,sigmaY),
					0.0};
  	// create a rotation matrix
  	G4RotationMatrix rotation = G4RotationMatrix();
  	rotation.rotateX(atan(simPar.GetBeamDirY()/simPar.GetBeamDirZ())*rad);
  	rotation.rotateY(atan(simPar.GetBeamDirX()/simPar.GetBeamDirZ())*rad);
  	rotation.rotateZ(atan(simPar.GetBeamDirY()/simPar.GetBeamDirX())*rad);
	G4ThreeVector translation = G4ThreeVector(	simPar.GetBeamPosX(),
							simPar.GetBeamPosY(),
							simPar.GetBeamPosZ()); 

	G4ThreeVector posXYZrot = rotation * posXYZ + translation;

	G4ThreeVector dirXYZ = {	G4RandGauss::shoot(simPar.GetBeamDirX(),sigmaXprime),
					G4RandGauss::shoot(simPar.GetBeamDirY(),sigmaYprime),
					simPar.GetBeamDirZ()};

	runAction->_beamPosX = posXYZrot.x();
	runAction->_beamPosY = posXYZrot.y();
	runAction->_beamPosZ = posXYZrot.z();
	runAction->_beamMomX = Mom*dirXYZ.x();
	runAction->_beamMomY = Mom*dirXYZ.y();
	runAction->_beamMomZ = Mom*dirXYZ.z();
	runAction->_beamEnergy = std::sqrt(Mom*Mom + mass*mass);

    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(dirXYZ);
    	fParticleGun->SetParticlePosition(posXYZrot);

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
}
