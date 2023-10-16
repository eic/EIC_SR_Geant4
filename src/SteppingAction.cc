#include "TMath.h"

#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleChange.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Transform3D.hh"
#include "Constants.hh"
#include "Constants.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

SteppingAction::SteppingAction(RunAction* runAct) : runAction(runAct)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
    	G4Track*		track		= step->GetTrack();
    	G4StepPoint*          	prePoint	= step->GetPreStepPoint();
    	G4StepPoint*          	postPoint	= step->GetPostStepPoint();
    	G4VPhysicalVolume*    	prePV		= prePoint->GetPhysicalVolume();
    	G4VPhysicalVolume*	postPV		= postPoint->GetPhysicalVolume();

	// check if the next step is not outside the world
	if(!postPV){return;}

	// skip everything except for gamma
	if(track->GetParticleDefinition()->GetPDGEncoding() != 22){return;}

	// check if the step is at the edge of the vacuum beam pipe
	if(	prePoint->GetStepStatus() == fGeomBoundary && postPV->GetName() == "world" &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
		G4Track* nonConstTrack = const_cast<G4Track*>(track);
               	nonConstTrack->SetTrackStatus(fStopAndKill);

/*		
		//Find the index of the element of xgdat that is nearest-above to x
auto i = lower_bound(xgdat.begin(), xgdat.end(), x); 
//If the vector values are in decreasing order use:
//auto i = lower_bound(xgdat.rbegin(), xgdat.rend(), x);
k = i - xgdat.begin(); //Nearest index
if (i == xgdat.end())
  --k;  // extrapolating above
else if (*i == x)
  return gdat[k];

l = k? k - 1: 1; //nearest-below index, except when extrapolating downward

// proceed with linear interpolation/extrapolation using l and k
*/
	}
    
	
/*
    if(parentID == 0 && prePoint->GetStepStatus() == fGeomBoundary && prePV->GetName().contains("phantom_0") && runAction->_Det0 == 0)
    {
        runAction->_X0         = coord.x()/m;
        runAction->_Y0         = coord.y()/m;
        runAction->_Z0         = coord.z()/m;
        runAction->_Mom_X0     = p.x()/GeV;
        runAction->_Mom_Y0     = p.y()/GeV;
        runAction->_Mom_Z0     = p.z()/GeV;
        runAction->_Ekin0      = Ekin/GeV;
        runAction->_Det0       = 1;
    }
    if(parentID == 0 && prePoint->GetStepStatus() == fGeomBoundary && prePV->GetName().contains("phantom_1") && runAction->_Det1 == 0)
    {
        runAction->_X1         = coord.x()/m;
        runAction->_Y1         = coord.y()/m;
        runAction->_Z1         = coord.z()/m;
        runAction->_Mom_X1     = p.x()/GeV;
        runAction->_Mom_Y1     = p.y()/GeV;
        runAction->_Mom_Z1     = p.z()/GeV;
        runAction->_Ekin1      = Ekin/GeV;
        runAction->_Det1       = 1;
    }
i*/
	return;
}
