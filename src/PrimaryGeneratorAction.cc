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
    	G4double Mom = _P*(1.0 + _deltaP);
    	G4double Ekin = std::sqrt(Mom*Mom + mass*mass) - mass;

	runAction->_beamPosX = Constants::beam_pos_x/CLHEP::cm;
	runAction->_beamPosY = Constants::beam_pos_y/CLHEP::cm;
	runAction->_beamPosZ = Constants::beam_pos_z/CLHEP::cm;
	runAction->_beamMomX = Mom*_PdirX/CLHEP::GeV;
	runAction->_beamMomY = Mom*_PdirY/CLHEP::GeV;
	runAction->_beamMomZ = Mom*_PdirZ/CLHEP::GeV;
	runAction->_beamEnergy = std::sqrt(Mom*Mom + mass*mass)/CLHEP::GeV;

    	fParticleGun->SetParticleDefinition(particleDef);
    	fParticleGun->SetParticleEnergy(Ekin);
    	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(_PdirX,_PdirY,_PdirZ));
    	fParticleGun->SetParticlePosition(G4ThreeVector(Constants::beam_pos_x,Constants::beam_pos_y,Constants::beam_pos_z));

    	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
}
