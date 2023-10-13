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

SteppingAction::SteppingAction(RunAction* runAct) : runAction(runAct)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
    G4Track* track = step->GetTrack();

    G4StepPoint*          prePoint  = step->GetPreStepPoint();
    G4VPhysicalVolume*    prePV     = prePoint->GetPhysicalVolume();

    G4ThreeVector coord     = prePoint->GetPosition();
    G4ThreeVector p         = track->GetMomentum();
    G4int parentID          = track->GetParentID();
    G4double Ekin           = track->GetKineticEnergy();

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
}
