#ifndef MYSTEPPINGACTION_H
#define MYSTEPPINGACTION_H

// DD4HEP
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StepHandler.h"

// ROOT
#include "TROOT.h"

// C++
#include <iostream>
#include <string>
using namespace std;

// HepMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"
using namespace HepMC3;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep 
{
        /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
	namespace sim 
	{
		class MySteppingAction;
		class MySteppingAction : public Geant4SteppingAction 
		{
			protected:
			DDG4_DEFINE_ACTION_CONSTRUCTORS(MySteppingAction);

			private:
			string _OutputFileNameWithSteppingInfo;
			WriterAscii* _fHepmc;
			shared_ptr<GenRunInfo> _run;
		 	bool _initialized = false;
			int _nEvents = 0;
			
			public:
 			/// Standard constructor with initializing arguments
			MySteppingAction(Geant4Context* context, const std::string& name = "") : 
			Geant4SteppingAction(context, name) 
			{
				declareProperty("OutputFileNameWithSteppingInfo", _OutputFileNameWithSteppingInfo);
			};

			/// Default destructor
			virtual ~MySteppingAction() 
			{
				// write a non-empty event
				if(_fHepmc)
				{
					cout<<"[MySteppingAction] Number of events in the output file: "<<_nEvents<<endl; 
					delete _fHepmc;
				}
			};

			/// Initialize the output file and HepMC objects
			void Init() 
			{
				// ascii file to store generated events
				cout<<"[MySteppingAction] Output file name: "<<_OutputFileNameWithSteppingInfo<<endl; 
				_fHepmc = new WriterAscii(_OutputFileNameWithSteppingInfo.c_str());

				// main bookkeeping type that holds meta-information about the generated events: list of used tools, 
				// names of used event weights and arbitrary attributes (see https://arxiv.org/pdf/1912.08005.pdf)
				_run = make_shared<GenRunInfo>();
				vector<string> weight_names = {"NormFactor"};
				_run->set_weight_names(weight_names);
				_fHepmc->set_run_info(_run);
				_fHepmc->write_run_info();

				_initialized = true;
			};

			/// Pre-track action callback
			virtual void operator()(const G4Step* step, G4SteppingManager* /*mgr*/) override 
			{
				if(!_initialized){Init();}

				G4Track* track = step->GetTrack();

				G4StepPoint*            postStep        = step->GetPostStepPoint();
				G4VPhysicalVolume*      postPV          = postStep->GetPhysicalVolume();

				if(
					postStep->GetStepStatus() == fGeomBoundary &&
					G4StrUtil::contains(postPV->GetName(),"BeamPipeSens")
				)
				{
					// data type that holds the position of the primary inter- action, and lists of vertices, 
					// particles and attributes
					GenEvent* event = new GenEvent(HepMC3::Units::GEV,HepMC3::Units::MM);
					event->set_run_info(_run);
					vector<float> weights = {1};
					event->weights().clear(); // Clear existing weights
					event->weights().insert(event->weights().end(), weights.begin(), weights.end());

					// Get particle info
					G4ThreeVector position = postStep->GetPosition();
					G4double energy = track->GetTotalEnergy(); 
					G4double mass = track->GetParticleDefinition()->GetPDGMass();
					G4ThreeVector momentum = postStep->GetMomentum();
					G4int pdg = track->GetParticleDefinition()->GetPDGEncoding();
					
					// particles going into the vertex: 
					// (px,py,pz,e),pdgid,status [https://arxiv.org/pdf/1912.08005.pdf]
					GenParticlePtr pin = make_shared<GenParticle>( 
						FourVector(
							momentum.getX()/CLHEP::GeV,
							momentum.getY()/CLHEP::GeV,
							momentum.getZ()/CLHEP::GeV,
							energy/CLHEP::GeV),pdg,4);	
					pin->set_generated_mass(mass/CLHEP::GeV);
					event->add_particle(pin);

					// particles going out of the vertex: 
					GenParticlePtr pout = make_shared<GenParticle>( 
						FourVector(
							momentum.getX()/CLHEP::GeV,
							momentum.getY()/CLHEP::GeV,
							momentum.getZ()/CLHEP::GeV,
							energy/CLHEP::GeV),pdg,1);	
					pin->set_generated_mass(mass/CLHEP::GeV);
					event->add_particle(pout);

					// generate the vertex
					GenVertexPtr vertex = make_shared<GenVertex>(
						FourVector(
							position.getX()/CLHEP::mm,
							position.getY()/CLHEP::mm,
							position.getZ()/CLHEP::mm,
							0)
					);
					vertex->add_particle_in(pin);
					vertex->add_particle_out(pout);

					// add the vertex to the event
					event->add_vertex(vertex);
					if(!event->vertices_empty() && _fHepmc)
					{
						// write the event: 1 event = 1 particle
						_fHepmc->write_event(*event);
						_nEvents++;
					}
					delete event;

					// stop and kill the track
					track->SetTrackStatus(fStopAndKill);
				}
			} //operator function
		}; //class definition
	} // End namespace sim
} // End namespace dd4hep
#endif
