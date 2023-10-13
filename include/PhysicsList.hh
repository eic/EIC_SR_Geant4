//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PhysicsListMessenger;
class G4DecayPhysics;

class PhysicsList: public G4VUserPhysicsList
{
public:
  PhysicsList();
  virtual ~PhysicsList();

  // Construct particles
  virtual void ConstructParticle();

  // Construct processes and register them
  virtual void ConstructProcess();

  inline void SetAnalyticSR(G4bool val) {fSRType = val;};

private:

  void ConstructEM();

  G4bool                  fSRType;
  PhysicsListMessenger*   fMess;
  G4DecayPhysics*         fDecayPhysics;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

