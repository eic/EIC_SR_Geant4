#ifndef SolenoidMagField_hh
#define SolenoidMagField_hh 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class SolenoidMagField : public G4MagneticField
{
public:
	SolenoidMagField();
    	SolenoidMagField(	const G4ThreeVector& pOrigin,
				G4RotationMatrix* pMatrix);
	~SolenoidMagField() override;
	
	void GetFieldValue(	const G4double yTrack[],
				G4double B[]     ) const override;
	G4Field* Clone() const override;

	void SetFileName(G4String fName){_fileName = fName; return;}
	G4String GetFileName(){return _fileName;}
	void ReadSolenoidFieldData();
	int FindIndexinVector(vector<double> myVector, G4double targetElement);

private:
	G4ThreeVector		fOrigin = G4ThreeVector(0.0, 0.0, 0.0);
	G4RotationMatrix*	fpMatrix = nullptr;

	G4String _fileName;
	vector<pair<double, vector<double>>> _Br;
	vector<pair<double, vector<double>>> _Bz;
	vector<double> _R;
	vector<double> _Z;

/*
	void PrintReflectData(string name, vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy);
	void FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2);
	double FindReflectionProbability(vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy, 
		double angle_val, double energy_val);
*/
};

#endif
