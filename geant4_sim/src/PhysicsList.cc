#include "PhysicsList.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4SynchrotronRadiation.hh"
#include "SynchrotronRadiation.hh"
#include "G4SynchrotronRadiationInMat.hh"

#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4DecayPhysics.hh"

#include "G4SystemOfUnits.hh"
#include "GammaReflectionProcess.hh"

PhysicsList::PhysicsList(G4String xmlFileName): G4VUserPhysicsList()
{
  	SetDefaultCutValue(1.*km);
  	fDecayPhysics = new G4DecayPhysics();

	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintSRprocParameters();
	simPar->PrintMeanFreePathFactor();
}

PhysicsList::~PhysicsList()
{ 
  	delete fDecayPhysics;
	delete simPar;
}

void PhysicsList::ConstructParticle()
{
  	fDecayPhysics->ConstructParticle();
	return;
}

void PhysicsList::ConstructProcess()
{
  	AddTransportation();
  	ConstructEM();
  	fDecayPhysics->ConstructProcess();
	return;
}

void PhysicsList::ConstructEM()
{
  	auto particleIterator=GetParticleIterator();
  	particleIterator->reset();
  	while( (*particleIterator)() )
	{
    		G4ParticleDefinition* particle = particleIterator->value();
    		G4ProcessManager* pmanager = particle->GetProcessManager();

		// set special limits defined by the user; see DetectorConstruction [SetUserLimits]
		pmanager->AddProcess(new G4UserSpecialCuts(),-1,-1,1);

		// get particle name
    		G4String particleName = particle->GetParticleName();

    		if (particleName == "gamma") 
		{
      			// gamma
			
			// turn off standard gamma processes
			//pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
			//pmanager->AddDiscreteProcess(new G4ComptonScattering);
			//pmanager->AddDiscreteProcess(new G4GammaConversion);
			//pmanager->AddDiscreteProcess(new G4RayleighScattering);
	
			// turn on the custom gamma process
			pmanager->AddDiscreteProcess(new GammaReflectionProcess(simPar->GetReflType(),
										simPar->GetReflPrint(),
										simPar->GetSurfRoughness(),
										simPar->GetAutoCorrLength(),
										simPar->GetSigmaRatio()));
    		} 
		else if (particleName == "e-") 
		{
      			//electron
      			pmanager->AddProcess(new G4eMultipleScattering,       -1, 1, -1);
      			pmanager->AddProcess(new G4eIonisation,               -1, 2, 1);
      			pmanager->AddProcess(new G4eBremsstrahlung,           -1, 3, 2);

      			switch(simPar->GetSRtype())
			{ 
				case 1:
					// G4SynchrotronRadiation
        				// pmanager->AddProcess(new G4SynchrotronRadiation,	-1,-1, 3); // Geant4
        				pmanager->AddProcess(
						new SynchrotronRadiation("SynRad",fElectromagnetic,
							simPar->GetMeanFreePathFactor()), // Geant4-biased
						-1,-1, 3);
					break;
				case 2:
					// G4SynchrotronRadiationInMat
        				pmanager->AddProcess(new G4SynchrotronRadiationInMat,	-1,-1, 3);
					break;
				default:
					throw runtime_error("[ERROR] PhysicsList::ConstructEM ==> Wrong SR model\n"); 
			}

      			pmanager->AddProcess(new G4StepLimiter,               	-1,-1, 4);
    		} 
		else if (particleName == "e+") 
		{
      			//positron
	      		pmanager->AddProcess(new G4eMultipleScattering,		-1, 1, -1);
	      		pmanager->AddProcess(new G4eIonisation,               	-1, 2, 1);
	      		pmanager->AddProcess(new G4eBremsstrahlung,           	-1, 3, 2);
	      		pmanager->AddProcess(new G4eplusAnnihilation,     	0,-1, 3);

      			switch(simPar->GetSRtype())
			{ 
				case 1:
					// G4SynchrotronRadiation
        				// pmanager->AddProcess(new G4SynchrotronRadiation,	-1,-1, 4); // Geant4
        				pmanager->AddProcess(
						new SynchrotronRadiation("SynRad",fElectromagnetic,
							simPar->GetMeanFreePathFactor()), // Geant4-biased
						-1,-1, 4);
					break;
				case 2:
					// G4SynchrotronRadiationInMat
        				pmanager->AddProcess(new G4SynchrotronRadiationInMat,	-1,-1, 4); 
					break;
				default:
					throw runtime_error("[ERROR] PhysicsList::ConstructEM ==> Wrong SR model\n"); 
			}

	      		pmanager->AddProcess(new G4StepLimiter,               -1,-1, 5);
	    	} 
		else if (particleName == "mu+" || particleName == "mu-") 
		{
	      		//muon
	      		pmanager->AddProcess(new G4MuMultipleScattering, 	-1, 1, -1);
	      		pmanager->AddProcess(new G4MuIonisation,         	-1, 2, 1);
	      		pmanager->AddProcess(new G4MuBremsstrahlung,     	-1, 3, 2);
	      		pmanager->AddProcess(new G4MuPairProduction,     	-1, 4, 3);
	      		pmanager->AddProcess(new G4SynchrotronRadiation,	-1,-1, 4); 
	      		pmanager->AddProcess(new G4StepLimiter,			-1,-1, 5);
	    	} 
		else if (particleName == "proton") 	
		{
	      		//proton
	      		pmanager->AddProcess(new G4hMultipleScattering, 	-1, 1, -1);
	      		pmanager->AddProcess(new G4hIonisation,         	-1, 2, 1);
	      		pmanager->AddProcess(new G4hBremsstrahlung,     	-1, 3, 2);
	      		pmanager->AddProcess(new G4hPairProduction,     	-1, 4, 3);
	      		pmanager->AddProcess(new G4SynchrotronRadiation, 	-1,-1, 4); 
	      		pmanager->AddProcess(new G4StepLimiter,          	-1,-1, 5);
	    	}
	    	else if (particle->GetPDGCharge() != 0.0 && !particle->IsShortLived()) 
	    	{
	      		pmanager->AddProcess(new G4SynchrotronRadiation,	-1,-1, 1);  
		}
	}

	return;
}
