#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"
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

#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    	// Detect interactive mode (if no arguments) and define UI session
    	//
    	G4UIExecutive* ui = 0;
    	if ( argc == 1 )
    	{
        	ui = new G4UIExecutive(argc, argv);
    	}


    	// Construct the default run manager
    	//

    	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    	CLHEP::HepRandom::setTheSeed(time(NULL));
    	G4Random::setTheSeed(time(NULL));

    	G4RunManager* runManager = new G4RunManager;
	PhysicsList* phys = new PhysicsList();
    	// G4PhysListFactory factory;
    	// G4String physName = "FTFP_BERT_HP";
    	// G4VModularPhysicsList* phys = factory.GetReferencePhysList(physName);
    	runManager->SetUserInitialization(phys);
    	RunAction* runAction = new RunAction();
    	DetectorConstruction* detConstr = new DetectorConstruction();
    	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(runAction);
    	EventAction* eventAction = new EventAction(runAction);
    	SteppingAction* stepAction = new SteppingAction(runAction);
    	StackingAction* stackAction = new StackingAction();

    	runManager->SetUserInitialization(detConstr);
    	runManager->SetUserAction(runAction);
    	runManager->SetUserAction(primary);
    	runManager->SetUserAction(eventAction);
    	runManager->SetUserAction(stepAction);
    	runManager->SetUserAction(stackAction);

    	// Initialize visualization
    	//
    	G4VisManager* visManager = new G4VisExecutive;
    	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    	// G4VisManager* visManager = new G4VisExecutive("Quiet");
    	visManager->Initialize();

    	// Get the pointer to the User Interface manager
    	G4UImanager* UImanager = G4UImanager::GetUIpointer();

    	// Process macro or start UI session
    	//
    	if ( ! ui )
    	{
        	// batch mode
        	G4String command = "/control/execute ";
        	G4String fileName = argv[1];
        	UImanager->ApplyCommand(command+fileName);
    	}
    	else
    	{
        	// interactive mode
        	UImanager->ApplyCommand("/control/execute vis.mac");
        	ui->SessionStart();
        	delete ui;
    	}

    	// Job termination
    	// Free the store: user actions, physics_list and detector_description are
    	// owned and deleted by the run manager, so they should not be deleted
    	// in the main() program !

    	delete visManager;
    	delete runManager;

    	return 0;
}
