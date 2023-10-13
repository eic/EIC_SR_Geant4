#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PhysicalConstants.hh"
#include "RunAction.hh"
#include "PrimaryMessenger.hh"

using namespace std;

class PrimaryMessenger;
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

    void SetMomentumSpread(G4double value){_deltaP = value;}
    void SetMomentum(G4double value){_P = value;}
    void SetMomentumDirectionX(G4double value){_PdirX = value;}
    void SetMomentumDirectionY(G4double value){_PdirY = value;}
    void SetMomentumDirectionZ(G4double value){_PdirZ = value;}
    void SetParticleName(G4String name){_pName = name;}

    G4double GetMomentumSpread() {return _deltaP;}
    G4double GetMomentum() {return _P;}
    G4double GetMomentumDirectionX(){return _PdirX;}
    G4double GetMomentumDirectionY(){return _PdirY;}
    G4double GetMomentumDirectionZ(){return _PdirZ;}
    G4String GetParticleName() {return _pName;}

private:
    G4ParticleGun* fParticleGun;
    RunAction* runAction;

    G4double _deltaP;
    G4double _P;
    G4double _PdirX;
    G4double _PdirY;
    G4double _PdirZ;
    G4String _pName;

    PrimaryMessenger* fPrimaryMessenger;
};

#endif


