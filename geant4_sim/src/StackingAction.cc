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
        simPar->PrintGammaCut();
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

		// kill straight tracks
		if(simPar->GetStraightTracksKillStatus())
		{
			G4double x0 = track->GetPosition().getX();
			G4double y0 = track->GetPosition().getY();
			G4double r_min = simPar->GetStraightTracksDmin()/2; // minimum radius of the beam pipe
			// select only the photons inside the minimum radius cylinder
			if(sqrt(x0*x0 + y0*y0) <= r_min)
			{
				G4double z0 = track->GetPosition().getZ();
				G4double dirx = track->GetMomentumDirection().getX();
				G4double diry = track->GetMomentumDirection().getY();
				G4double z = simPar->GetStraightTracksZend();
				G4double x = x0 + (z - z0) * dirx;
				G4double y = y0 + (z - z0) * diry;
				if(sqrt(x*x + y*y) <= r_min){status = fKill;}
			}
		}
	}

	// kill secondaries
    	if(_kill_secondaries == 1 && track->GetTrackID() > 1){status = fKill;}

	// kill gammas
	if(track->GetTotalEnergy() < simPar->GetGammaEcut()){status = fKill;}

    	return status;
}
