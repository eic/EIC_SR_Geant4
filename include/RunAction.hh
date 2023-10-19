#ifndef RunAction_h
#define RunAction_h 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TH1.h"
#pragma GCC diagnostic pop

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4Timer.hh"
#include "RunMessenger.hh"

#include <string>
#include <fstream>
#include <vector>
#include <utility> // pair
#include <stdexcept> // runtime_error
#include <sstream> // stringstream
#include <stdio.h>

using namespace std;

class G4Run;
class RunMessenger;

class RunAction: public G4UserRunAction
{
public:
	RunAction();
    	~RunAction();

    	void BeginOfRunAction(const G4Run*);
    	void EndOfRunAction(const G4Run*);

    	G4String GetOutputFileName() { return _output_filename;}
    	void SetOutputFilename(G4String name){_output_filename = name;}

    	TTree* _tree;

    	G4int _event;
    	G4int _beamPdg;
	G4float _beamPosX;
	G4float _beamPosY;
	G4float _beamPosZ;
	G4float _beamMomX;
	G4float _beamMomY;
	G4float _beamMomZ;
	G4float _beamEnergy;

	vector<G4float> _gammaPosX;
	vector<G4float> _gammaPosY;
	vector<G4float> _gammaPosZ;
	vector<G4float> _gammaMomX;
	vector<G4float> _gammaMomY;
	vector<G4float> _gammaMomZ;
	vector<G4float> _gammaEnergy;

private:
    	G4Timer* _timer;
    	G4String _output_filename;

    	RunMessenger* fRunMessenger;
};

#endif
