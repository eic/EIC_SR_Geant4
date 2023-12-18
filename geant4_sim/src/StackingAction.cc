#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4Gamma.hh"
#include "G4ProcessManager.hh"

StackingAction::StackingAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
    	_kill_secondaries  = 0;
    	fStackingMessenger = new StackingMessenger(this);
        simPar = new SimParameters();
        simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
        simPar->PrintGammaEcut();
}

StackingAction::~StackingAction()
{
    	delete fStackingMessenger;
	delete simPar;
}

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{
    	G4ClassificationOfNewTrack status = fUrgent;
    
	// get new track info, if SR gamma -> store
	if(	track->GetParticleDefinition()->GetPDGEncoding() == G4Gamma::Gamma()->GetPDGEncoding() &&
		(track->GetCreatorProcess()->GetProcessName() == "SynRad" || 
		 track->GetCreatorProcess()->GetProcessName() == "SynchrotronRadiation"))
	{
		G4String volume_name = track->GetVolume()->GetName();
		// solenoid field in the IP beam pipe
		if(volume_name == "vac_IP" && simPar->IsBuildSolenoid()){volume_name = "mag_MARCO";}

		// find the iterator of the magnet by its name
		auto it = std::find(runAction->_magName.begin(),runAction->_magName.end(),volume_name);
		// check if the current PhysVolume is present in the list of magnets
		if (it != runAction->_magName.end())
		{
			// get the magnet index in the vector
			G4int iMag = distance(runAction->_magName.begin(),it);	
			runAction->_magNph[iMag]++;
			runAction->_magSumEph[iMag] += track->GetTotalEnergy()/eV;
		}
		else
		{
			G4cout<<"\n\n=================================================================="<<G4endl;
			G4cout<<"[WARNING] StackingAction::ClassifyNewTrack ==> ";
			G4cout<<"Cannot find the SR creation magnet"<<G4endl;
			G4cout<<"==================================================================\n"<<G4endl;
		}

	}

	// kill secondaries
    	if(_kill_secondaries == 1 && track->GetTrackID() > 1){status = fKill;}

	// kill gammas
	if(track->GetTotalEnergy() < simPar->GetGammaEcut()){status = fKill;}

    	return status;
}
