#ifndef GammaReflectionProcess_hh 
#define GammaReflectionProcess_hh

#include "G4VProcess.hh"
#include "G4VTrackTerminator.hh"

class G4VScorer;

class GammaReflectionProcess : public G4VProcess
{
public:
	explicit GammaReflectionProcess();
	virtual ~GammaReflectionProcess();

	virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
				       				G4double   previousStepSize,
				       				G4ForceCondition* condition);
    // make processed being forced

  virtual G4VParticleChange * PostStepDoIt(const G4Track&, const G4Step&);
    // message "scorer" with  G4Step and a G4GeometryCellStep from the "mass" 
    // geometry


    // to be called by the importance process if the track should
    // be killed after scoring
  virtual const G4String &GetName() const ;

public:  // without description

  // no operation in  AtRestDoIt and  AlongStepDoIt

  virtual G4double 
  AlongStepGetPhysicalInteractionLength(const G4Track&,
					G4double  ,
					G4double  ,
					G4double& ,
					G4GPILSelection*);
  
  virtual G4double 
  AtRestGetPhysicalInteractionLength(const G4Track&,
				     G4ForceCondition*);
  
  virtual G4VParticleChange* AtRestDoIt(const G4Track&,
					const G4Step&);
  virtual G4VParticleChange* AlongStepDoIt(const G4Track&,
					   const G4Step&);
  
private:

  GammaReflectionProcess(const GammaReflectionProcess &);
  GammaReflectionProcess &operator=(const GammaReflectionProcess &);
  
};

#endif
