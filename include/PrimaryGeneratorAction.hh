#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PhysicalConstants.hh"
#include "RunAction.hh"

using namespace std;

class G4GeneralParticleSource;
class G4Event;
class G4ParticleGun;
class RunAction;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(RunAction*);
    virtual ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent);

private:
    G4ParticleGun* fParticleGun;
    RunAction* runAction;
};

#endif


