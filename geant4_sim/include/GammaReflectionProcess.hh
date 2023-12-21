#ifndef GammaReflectionProcess_hh 
#define GammaReflectionProcess_hh 1

#include "G4ParticleChange.hh"
#include "G4VProcess.hh"
#include "G4VTrackTerminator.hh"
#include "G4Gamma.hh"
#include "G4NistManager.hh"

#include <string>
#include <fstream>
#include <vector>
#include <utility> // pair
#include <stdexcept> // runtime_error
#include <sstream> // stringstream
#include <stdio.h>
#include <map>

using namespace std;

class GammaReflectionProcess : public G4VProcess
{
public:
	explicit GammaReflectionProcess();
	virtual ~GammaReflectionProcess();

	virtual G4double PostStepGetPhysicalInteractionLength(	const G4Track& aTrack,
				       				G4double   previousStepSize,
				       				G4ForceCondition* condition);
    	// make processed being forced

  	virtual G4VParticleChange * PostStepDoIt(const G4Track&, const G4Step&);
    	// message "scorer" with  G4Step and a G4GeometryCellStep from the "mass" 
    	// geometry


    	// to be called by the importance process if the track should
    	// be killed after scoring
  	virtual const G4String &GetName() const ;

public:  // without description

  	// no operation in  AtRestDoIt and  AlongStepDoIt
  	virtual G4double 
  	AlongStepGetPhysicalInteractionLength(	const G4Track&,
						G4double  ,
						G4double  ,
						G4double& ,
						G4GPILSelection*);
  
  	virtual G4double 
  	AtRestGetPhysicalInteractionLength(	const G4Track&,
				     		G4ForceCondition*);
  	virtual G4VParticleChange* AtRestDoIt(	const G4Track&,
						const G4Step&);
  	virtual G4VParticleChange* AlongStepDoIt(	const G4Track&,
					   		const G4Step&);
  
private:

  	GammaReflectionProcess(const GammaReflectionProcess &);
  	GammaReflectionProcess &operator=(const GammaReflectionProcess &);

	void DoGammaReflection(const G4double GamEner, G4double &prob, G4ThreeVector &fNewMomentum);
	void DoXrayReflection(const G4double GamEner, G4double &prob, G4ThreeVector &fNewMomentum);

	map<string, string> _reflectFileNameMap;
	G4double _sigmaRatio;
	map<string, vector<pair<double, vector<double>>>> _reflectDataMap;
	map<string, vector<double>> _grazingAngleMap;
	map<string, vector<double>> _photonEnergyMap;

	vector<pair<double, vector<double>>> ReadReflectData(string filename,
		vector<double> &angle, vector<double> &energy);
	void PrintReflectData(string name, vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy);
	void FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2);
	double FindReflectionProbability(vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy, 
		double angle_val, double energy_val);

	//
	G4double Reflectivity(const G4double GamEner, const G4double SinIncidentAngle, const G4Material* theMat) const;
	void SaveHenkeDataAsMaterialProperty();
	G4int ReadHenkeXrayData(std::string ElName, std::vector<G4double>& Ephot, std::vector<G4double>& f1, std::vector<G4double>& f2);

	static G4double fSurfaceRoughness;  // same value in all threads
	G4NistManager* fNistManager;
	G4Material* theMatIP;
	G4Material* theMatIR;
};

#endif
