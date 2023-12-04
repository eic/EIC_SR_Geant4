#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PhysicalConstants.hh"
#include "RunAction.hh"
#include "SimParameters.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4Electron.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"
#include "TROOT.h"
#include "TF1.h"

using namespace std;

class G4GeneralParticleSource;
class G4Event;
class G4ParticleGun;
class RunAction;
class SimParameters;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
    	PrimaryGeneratorAction(RunAction*,G4String);
    	virtual ~PrimaryGeneratorAction();

    	void GeneratePrimaries(G4Event* anEvent);

private:
    	G4ParticleGun* fParticleGun;
	SimParameters* simPar;
	RunAction* runAction;	

	TF1* fBeamTailX;
	TF1* fBeamTailY;
};

#endif


