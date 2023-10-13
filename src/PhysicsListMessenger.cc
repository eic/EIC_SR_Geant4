//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsListMessenger.hh"

#include "PhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* physL)
:G4UImessenger(),fPhysList(physL),
 fPhysDir(0),
 fSRTypeCmd(0)
{
  fPhysDir = new G4UIdirectory("/testem/phys/");
  fPhysDir->SetGuidance("physics list commands");
  
  fSRTypeCmd = new G4UIcmdWithABool("/testem/phys/analyticSR",this);
  fSRTypeCmd->SetGuidance("choose analytic synchrotron radiation");
  fSRTypeCmd->SetParameterName("SRType",true);
  fSRTypeCmd->SetDefaultValue(true);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fSRTypeCmd;
  delete fPhysDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  	if (command == fSRTypeCmd)
    	{ 
		fPhysList->SetAnalyticSR(fSRTypeCmd->GetNewBoolValue(newValue));
		G4cout<<"[DEBUG] PhysicsListMessenger: newValue = "<<newValue<<G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
