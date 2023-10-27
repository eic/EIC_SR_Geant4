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

	// check input parameters
	if( argc == 3 )
	{
       		macFileName = argv[1];
		xmlFileName = argv[2];

		
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[INFO] main ==> Input parameters"<<G4endl;
		G4cout<<"argv[1] = "<<macFileName<<G4endl;
		G4cout<<"argv[2] = "<<xmlFileName<<G4endl;
		G4cout<<"==================================================================\n"<<G4endl;
	}
	else if( argc == 2 )
	{
		macFileName = argv[1];
		xmlFileName = "";

		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[INFO] main ==> Input parameters"<<G4endl;
		G4cout<<"argv[1] = "<<macFileName<<G4endl;
		G4cout<<"==================================================================\n"<<G4endl;

		G4cout<<"\n\n      =================================================================================="<<G4endl;
		G4cout<<"      =================================================================================="<<G4endl;
		G4cout<<"      || [WARNING] main ==> The XML file with simulation parameters was not provided. ||"<<G4endl;
		G4cout<<"      || Default simulation parameters will be used instead. Please note that this    ||"<<G4endl;
		G4cout<<"      || may lead to unexpected simulation results or reproducibility issues.         ||"<<G4endl;
		G4cout<<"      =================================================================================="<<G4endl;
		G4cout<<"      ==================================================================================\n"<<G4endl;
	}
	else
	{
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[ERROR] main ==> Wrong number of input parameters: argc = "<<argc<<G4endl;
		G4cout<<"Usage: ./exe run.mac ../geometry/simParameters.xml"<<G4endl;
		G4cout<<"==================================================================\n"<<G4endl;

		return -1;
	}
    	// Construct the default run manager
    	//

    	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    	CLHEP::HepRandom::setTheSeed(time(NULL));
    	G4Random::setTheSeed(time(NULL));

    	G4RunManager* runManager = new G4RunManager;
	PhysicsList* phys = new PhysicsList(xmlFileName);
    	// G4PhysListFactory factory;
    	// G4String physName = "FTFP_BERT_HP";
    	// G4VModularPhysicsList* phys = factory.GetReferencePhysList(physName);
    	runManager->SetUserInitialization(phys);
    	RunAction* runAction = new RunAction(xmlFileName);
    	DetectorConstruction* detConstr = new DetectorConstruction(xmlFileName);
    	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(runAction,xmlFileName);
    	EventAction* eventAction = new EventAction(runAction);
    	SteppingAction* stepAction = new SteppingAction(runAction);
    	StackingAction* stackAction = new StackingAction(xmlFileName);

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

    	// Process macro
       	// batch mode
       	UImanager->ApplyCommand("/control/execute " + macFileName);

    	// Job termination
    	// Free the store: user actions, physics_list and detector_description are
    	// owned and deleted by the run manager, so they should not be deleted
    	// in the main() program !

    	delete visManager;
    	delete runManager;

    	return 0;
}
