#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"
#include "TrackingAction.hh"
#include "G4RunManager.hh"
#include "G4eMultipleScattering.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"
#include "G4PhysListFactory.hh"
#include "EventAction.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BIC.hh"
#include "FTFP_BERT_HP.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "SimParameters.hh"

#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
	G4String macFileName;
	G4String xmlFileName;
	G4long randomSeed;

	// check input parameters
	if( argc == 4 )
	{
       		macFileName = argv[1];
		xmlFileName = argv[2];
		randomSeed = time(NULL)+atoi(argv[3]);
		
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[INFO] main ==> Input parameters"<<G4endl;
		G4cout<<"argv[1] macFileName = "<<macFileName<<G4endl;
		G4cout<<"argv[2] xmlFileName = "<<xmlFileName<<G4endl;
		G4cout<<"argv[3] randomSeed = "<<randomSeed<<G4endl;
		G4cout<<"==================================================================\n"<<G4endl;
	}
	else
	{
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[ERROR] main ==> Wrong number of input parameters: argc = "<<argc<<G4endl;
		G4cout<<"Usage: ./exe run.mac ../geometry/simParameters.xml 21011993"<<G4endl;
		G4cout<<"==================================================================\n"<<G4endl;

		return -1;
	}

	// Choose the Random engine
	//
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(randomSeed);
	G4Random::showEngineStatus();

    	// Construct the default run manager
    	//
    	G4RunManager* runManager = new G4RunManager;

	// Set initialization classes
	//
	PhysicsList* phys = new PhysicsList(xmlFileName);
    	runManager->SetUserInitialization(phys);

    	RunAction* runAction = new RunAction(xmlFileName,randomSeed);
    	runManager->SetUserAction(runAction);

    	DetectorConstruction* detConstr = new DetectorConstruction(
		runAction,xmlFileName);
    	runManager->SetUserInitialization(detConstr);

    	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(runAction,xmlFileName);
    	runManager->SetUserAction(primary);

    	EventAction* eventAction = new EventAction(runAction,xmlFileName);
    	runManager->SetUserAction(eventAction);

    	SteppingAction* stepAction = new SteppingAction(runAction,xmlFileName);
    	runManager->SetUserAction(stepAction);

    	StackingAction* stackAction = new StackingAction(runAction,xmlFileName);
    	runManager->SetUserAction(stackAction);

    	TrackingAction* trackAction = new TrackingAction(runAction,xmlFileName);
    	runManager->SetUserAction(trackAction);

	// Initialize G4 kernel
	//
	runManager->Initialize();

    	// Initialize visualization
    	//
    	auto visManager = new G4VisExecutive;
    	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    	// G4VisManager* visManager = new G4VisExecutive("Quiet");
    	visManager->Initialize();

    	// Get the pointer to the User Interface manager
    	auto UImanager = G4UImanager::GetUIpointer();

    	// Process macro or start session
	if(G4StrUtil::contains(macFileName,"vis"))
	{
		// interactive mode
		G4UIExecutive* ui = new G4UIExecutive(argc,argv);

		UImanager->ApplyCommand("/control/execute " + macFileName);
		ui->SessionStart();
		delete ui;
	}
	else
	{
       		// batch mode
       		UImanager->ApplyCommand("/control/execute " + macFileName);
	}

    	// Job termination
    	// Free the store: user actions, physics_list and detector_description are
    	// owned and deleted by the run manager, so they should not be deleted
    	// in the main() program !

    	delete visManager;
    	delete runManager;

    	return 0;
}
