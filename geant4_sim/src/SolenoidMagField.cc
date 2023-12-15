#include "SolenoidMagField.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

namespace
{
	G4RotationMatrix IdentityMatrix; 
}

SolenoidMagField::SolenoidMagField(	G4String pName,
					G4double pRmax,
					G4double pZmin,
					G4double pZmax,
					G4bool pPrint,
					const G4ThreeVector& pOrigin,
					G4RotationMatrix* pMatrix)
{
	// set values
	fName = pName;
	fRmax = pRmax;
	fZmin = pZmin;
	fZmax = pZmax;
	fPrint = pPrint;
	fOrigin = pOrigin;
	fMatrix = pMatrix;

	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SolenoidMagField::SolenoidMagField"<<G4endl;
	G4cout<<"File name = "<<fName<<G4endl;
	G4cout<<"Max R = "<<fRmax/cm<<" [cm]"<<G4endl;
	G4cout<<"Min Z = "<<fZmin/cm<<" [cm]"<<G4endl;
	G4cout<<"Max Z = "<<fZmax/cm<<" [cm]"<<G4endl;
	G4cout<<"Print data flag = "<<fPrint<<G4endl;
	G4cout<<"Origin (x;y;z) = ("<<fOrigin.x()/cm<<";"<<fOrigin.y()/cm<<";"<<fOrigin.z()/cm<<") [cm]"<<G4endl;
	G4cout<<"Rotation matrix (theta;phi) = ("<<fMatrix->phi()/rad<<";"<<fMatrix->theta()/rad<<") [rad]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	ReadSolenoidFieldData();
	if(fPrint){PrintSolenoidFieldData();}
}

G4Field* SolenoidMagField::Clone() const
{
	return new SolenoidMagField(fName, fRmax, fZmin, fZmax, fPrint, fOrigin, fMatrix);
}

SolenoidMagField::~SolenoidMagField() = default;

void SolenoidMagField::GetFieldValue(	const G4double y[],	// [7]
					G4double B[]) const 	// [3]
{
	//  with displaced origin and rotation
	G4ThreeVector r_global = G4ThreeVector(	y[0] - fOrigin.x(),
                                         	y[1] - fOrigin.y(),
                                         	y[2] - fOrigin.z());
  
	const G4ThreeVector r_local = (*fMatrix) * r_global;
	// x,y,z --> rho,z,phi
	double zpos = r_local[2];
	double rho = hypot(r_local[0],r_local[1]);
	double phi = atan2(r_local[1],r_local[0]);

	// get the field
	double Br, Bz;
	FindSolenoidFieldValue(rho/cm,zpos/cm,Br,Bz); 

	// limit the field in local coordinates
	if(rho > fRmax || zpos < fZmin || zpos > fZmax){Br = 0; Bz = 0;}

	// rho,z,phi --> x,y,z
	const G4ThreeVector B_local(	Br * cos(phi) * tesla,
                               		Br * sin(phi) * tesla, 
					Bz * tesla);
	const G4ThreeVector B_global = fMatrix->inverse() * B_local;

	B[0] = B_global.x();
	B[1] = B_global.y();
	B[2] = B_global.z();

	return;
}

void SolenoidMagField::ReadSolenoidFieldData()
{
	// clean vectors
	_Br.clear();	
	_Bz.clear();
	_R.clear();
	_Z.clear();

	// Create an input filestream
	ifstream myFile(fName);

	// make sure the file is open 
    	if(!myFile.is_open()) 
	{
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[INFO] SolenoidMagField::ReadSolenoidFieldData"<<G4endl;
		G4cout<<"solenoid field data file name = "<<fName<<G4endl;
		G4cout<<"==================================================================\n\n"<<G4endl;
		throw runtime_error("[ERROR] SolenoidMagField::ReadSolenoidFieldData ==> Cannot open or find the file\n");
	}

	// Helper vars
	G4double rr, zz, brr, bzz;
	string line;

    	// Read R coordinates
    	if(myFile.good())
    	{
		// loop over all lines in the file
		while(getline(myFile, line))
		{
        		// Create a stringstream from line
        		stringstream ss(line);

			// read values in the line
        		ss >> rr >> zz >> brr >> bzz;
		
			// fill R vectors and create R-Br/R-Bz pairs
			if(find(_R.begin(), _R.end(), rr) == _R.end())
			{
				_R.push_back(rr);
				_Br.push_back({rr, vector<double> {}});
				_Bz.push_back({rr, vector<double> {}});
			}
			// fill Z vectors
			if(find(_Z.begin(), _Z.end(), zz) == _Z.end())
			{
				_Z.push_back(zz);
			}
		}
    	}
	// clear eof and fail flags
	myFile.clear();
	myFile.seekg(0);

	// Read R and Field coordinates
	while(getline(myFile, line)) 
	{
       		// Create a stringstream from line
       		stringstream ss(line);

		// read values in the line
		ss >> rr >> zz >> brr >> bzz;

		// find R and Z indexes
		G4int rIndx = FindIndexinVector(_R,rr);		
		G4int zIndx = FindIndexinVector(_Z,zz);		
		
		// fill R-Br/R-Bz pairs
		if(rIndx >= 0 && zIndx >= 0)
		{
			_Br.at(rIndx).second.push_back(brr);
			_Bz.at(rIndx).second.push_back(bzz);
		}
		else
		{
			G4cout<<"\n\n=================================================================="<<G4endl;
			G4cout<<"[INFO] SolenoidMagField::ReadSolenoidFieldData"<<G4endl;
			G4cout<<"rr = "<<rr<<G4endl;
			G4cout<<"zz = "<<zz<<G4endl;
			G4cout<<"brr = "<<brr<<G4endl;
			G4cout<<"bzz = "<<bzz<<G4endl;
			G4cout<<"R: "<<G4endl; for (G4double element : _R) {G4cout<<element<<" ";} G4cout<<G4endl;
			G4cout<<"Z: "<<G4endl; for (G4double element : _Z) {G4cout<<element<<" ";} G4cout<<G4endl;
			G4cout<<"==================================================================\n\n"<<G4endl; 
			throw runtime_error("[ERROR] SolenoidMagField::ReadSolenoidFieldData ==> Wrong R or Z coordinates\n");
		}
	}

	// Close the file
	myFile.close();
	
	return;
}

G4int SolenoidMagField::FindIndexinVector(vector<double> myVector, G4double targetElement)
{
	// Find the iterator pointing to the target element
    	auto it = find(myVector.begin(), myVector.end(), targetElement);

    	// Check if the element was found
    	if (it != myVector.end()) 
	{
        	// Calculate the index using std::distance and cast to int
        	return static_cast<int>(std::distance(myVector.begin(), it));
    	} 

	return -1;
}

void SolenoidMagField::PrintSolenoidFieldData()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	cout<<"[INFO] SolenoidMagField::PrintSolenoidFieldData"<<endl;

	G4int iLine = 0;
	for(int i = 0; i < _R.size(); i++)
	{
		G4cout<<"# \t Br->R/Bz->R/R[cm] \t Z[cm] \t Br[T] \t Bz[T]"<<G4endl;
		for(int j = 0; j < _Z.size(); j++)
		{
			printf("%d \t %7.1f/%7.1f/%7.1f \t %7.1f \t %7.4f \t %7.4f\n",
				iLine+1,
				_Br.at(i).first,
				_Bz.at(i).first,
				_R.at(i),
				_Z.at(j),
				_Br.at(i).second.at(j),
				_Bz.at(i).second.at(j));
		
			iLine++;
		}
	}
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SolenoidMagField::FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2) const
{
	id1 = id2 = 0;
	// find the index of the element of the vecotr that is nearest-above to val
	auto i = lower_bound(vec.begin(), vec.end(), val); 
	// if the vector values are in decreasing order use:
	//auto i = lower_bound(vec.rbegin(), vec.rend(), val);
	id2 = i - vec.begin(); // nearest index

	if (i == vec.end())
  	{
		--id2;  // extrapolating above
	}
	else if (*i == val)
	{
		id1 = id2;
  		return;
	}

	id1 = id2 ? id2 - 1 : id2; //nearest-below index, except when extrapolating downward
	id1 = (val > vec.at(id2)) ? id2 : id1; //outside the data range

	return;
}

void SolenoidMagField::FindSolenoidFieldValue(double r_val, double z_val, double &Br_val, double &Bz_val) const
{
	int r_i, r_j, z_i, z_j;

	FindClosestIndexesInVec(_R,r_val,r_i,r_j);
	FindClosestIndexesInVec(_Z,z_val,z_i,z_j);

	// Br
	double Br_ii = _Br.at(r_i).second.at(z_i);
	double Br_ji = _Br.at(r_j).second.at(z_i);
	double Br_ij = _Br.at(r_i).second.at(z_j);
	double Br_jj = _Br.at(r_j).second.at(z_j);

	// linear interpolation between two points
	double Br_ii_ji = (r_i == r_j) ? Br_ii : 
		(Br_ji - Br_ii)*(r_val - _R.at(r_i))/(_R.at(r_j) - _R.at(r_i)) + Br_ii;
	double Br_ij_jj = (r_i == r_j) ? Br_jj : 
		(Br_jj - Br_ij)*(r_val - _R.at(r_j))/(_R.at(r_i) - _R.at(r_j)) + Br_ij;

	Br_val = (z_i == z_j) ? Br_ii_ji : 
		(Br_ij_jj - Br_ii_ji)*(z_val - _Z.at(z_i))/(_Z.at(z_j) - _Z.at(z_i)) + 
		Br_ii_ji;

	// Bz
	double Bz_ii = _Bz.at(r_i).second.at(z_i);
	double Bz_ji = _Bz.at(r_j).second.at(z_i);
	double Bz_ij = _Bz.at(r_i).second.at(z_j);
	double Bz_jj = _Bz.at(r_j).second.at(z_j);

	// linear interpolation between two points
	double Bz_ii_ji = (r_i == r_j) ? Bz_ii : 
		(Bz_ji - Bz_ii)*(r_val - _R.at(r_i))/(_R.at(r_j) - _R.at(r_i)) + Bz_ii;
	double Bz_ij_jj = (r_i == r_j) ? Bz_jj : 
		(Bz_jj - Bz_ij)*(r_val - _R.at(r_j))/(_R.at(r_i) - _R.at(r_j)) + Bz_ij;

	Bz_val = (z_i == z_j) ? Bz_ii_ji : 
		(Bz_ij_jj - Bz_ii_ji)*(z_val - _Z.at(z_i))/(_Z.at(z_j) - _Z.at(z_i)) + 
		Bz_ii_ji;

	return;
}
