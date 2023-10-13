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

    G4int _Event_ID;
    G4int _PDG;
    G4double _Q;
    G4double _Mass;

    G4int _Det0;
    G4double _X0;
    G4double _Y0;
    G4double _Z0;
    G4double _Mom_X0;
    G4double _Mom_Y0;
    G4double _Mom_Z0;
    G4double _Ekin0;

    G4int _Det1;
    G4double _X1;
    G4double _Y1;
    G4double _Z1;
    G4double _Mom_X1;
    G4double _Mom_Y1;
    G4double _Mom_Z1;
    G4double _Ekin1;

    G4double _targetL;
    G4double _targetW;
    G4double _targetH;

private:
    G4Timer* _timer;
    G4String _output_filename;

    RunMessenger* fRunMessenger;
};

#endif
