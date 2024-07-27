#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
	_timer = new G4Timer;

        simPar = new SimParameters();
        simPar->InitDefault();
        simPar->ReadXML(xmlFileName);
	simPar->PrintMagParameters();

	// fill magnet data vectors
	for(G4int iMag = 0; iMag < simPar->GetMagNum(); iMag++)
	{
		runAction->_magName.push_back("mag_"+simPar->GetMagName(iMag));
		runAction->_magNph.push_back(0.0);
		runAction->_magSumEph.push_back(0.0);
	}
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* event)
{
	_timer->Start();
    	runAction->_event = event->GetEventID();

	// clean magnet data vectors
	for(G4int iMag = 0; iMag < runAction->_magName.size(); iMag++)
	{
		runAction->_magNph.at(iMag) = 0;
		runAction->_magSumEph.at(iMag) = 0;
	}

    	if((runAction->_event)%1000 == 0)
        	printf("[INFO] EventAction::BeginOfEventAction ==> BeginOfEvent: %12d\n",runAction->_event);

	return;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    	runAction->_tree->Fill();

	// clean variables
	runAction->_beamPdg.clear();
	runAction->_beamPosX.clear();
	runAction->_beamPosY.clear();
	runAction->_beamPosZ.clear();
	runAction->_beamMomX.clear();
	runAction->_beamMomY.clear();
	runAction->_beamMomZ.clear();
	runAction->_beamEnergy.clear();
	runAction->_beamRefPosX.clear();
	runAction->_beamRefPosY.clear();
	runAction->_beamRefPosZ.clear();
	runAction->_beamRefAngle.clear();
	runAction->_beamTransfMomSqr.clear();
	runAction->_beamBjorkenX.clear();
	runAction->_beamKillPosX.clear();
	runAction->_beamKillPosY.clear();
	runAction->_beamKillPosZ.clear();

	runAction->_gammaVtxPosX.clear();
	runAction->_gammaVtxPosY.clear();
	runAction->_gammaVtxPosZ.clear();
	runAction->_gammaVtxDirX.clear();
	runAction->_gammaVtxDirY.clear();
	runAction->_gammaVtxDirZ.clear();

	runAction->_gammaPostPV.clear();
	runAction->_gammaPrePV.clear();
	runAction->_gammaPosX.clear();
	runAction->_gammaPosY.clear();
	runAction->_gammaPosZ.clear();
	runAction->_gammaMomX.clear();
	runAction->_gammaMomY.clear();
	runAction->_gammaMomZ.clear();
	runAction->_gammaEnergy.clear();
	runAction->_gammaTime.clear();

	runAction->_sensName.clear();
	runAction->_sensPDG.clear();
	runAction->_sensPosX.clear();
	runAction->_sensPosY.clear();
	runAction->_sensPosZ.clear();
	runAction->_sensMomX.clear();
	runAction->_sensMomY.clear();
	runAction->_sensMomZ.clear();
	runAction->_sensEnergy.clear();
	runAction->_sensTime.clear();

	runAction->_lumiPosX.clear();
	runAction->_lumiPosY.clear();
	runAction->_lumiPosZ.clear();
	runAction->_lumiMomX.clear();
	runAction->_lumiMomY.clear();
	runAction->_lumiMomZ.clear();
	runAction->_lumiEnergy.clear();
	runAction->_lumiTime.clear();

    	_timer->Stop();
	// 'real' is wall clock time - time from start to finish of the call. This is all elapsed time 
	// including time slices used by other processes and time the process spends blocked (for example 
	// if it is waiting for I/O to complete).
	runAction->_avgEventTime += _timer->GetRealElapsed();

	return;
}
