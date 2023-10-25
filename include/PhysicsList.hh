#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

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

private:

  void ConstructEM();

  G4DecayPhysics*         fDecayPhysics;
};

#endif

