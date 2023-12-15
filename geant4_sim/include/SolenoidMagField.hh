#ifndef SolenoidMagField_hh
#define SolenoidMagField_hh 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include <string>
#include <fstream>
#include <vector>
#include <utility> // pair
#include <stdexcept> // runtime_error
#include <sstream> // stringstream
#include <stdio.h>
#include <map>

using namespace std;

class SolenoidMagField : public G4MagneticField
{
public:
    	SolenoidMagField(	G4String pName,
				G4double pRmax,
				G4double pZmin,
				G4double pZmax,
				G4bool pPrint,
				const G4ThreeVector& pOrigin,
				G4RotationMatrix* pMatrix);
	~SolenoidMagField() override;
	
	void GetFieldValue(	const G4double yTrack[],
				G4double B[]     ) const override;
	G4Field* Clone() const override;

private:
	G4String 		fName 		=  "../geometry/MARCO.txt";
	G4double		fRmax 		=  DBL_MAX;
	G4double		fZmin 		= -DBL_MAX;
	G4double		fZmax 		=  DBL_MAX;
	G4bool 			fPrint 		=  false;
	G4ThreeVector		fOrigin 	=  G4ThreeVector(0.0, 0.0, 0.0);
	G4RotationMatrix*	fMatrix 	=  nullptr;

	vector<pair<double, vector<double>>> _Br;
	vector<pair<double, vector<double>>> _Bz;
	vector<double> _R;
	vector<double> _Z;

	void ReadSolenoidFieldData();
	G4int FindIndexinVector(vector<double> myVector, G4double targetElement);
	void PrintSolenoidFieldData();
	void FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2) const;
	void FindSolenoidFieldValue(double r_val, double z_val, double &Br_val, double &Bz_val) const;
};

#endif
