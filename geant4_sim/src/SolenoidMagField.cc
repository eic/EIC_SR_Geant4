#include "SolenoidMagField.hh"
#include "G4RotationMatrix.hh"

namespace
{
	G4RotationMatrix IdentityMatrix; 
}

SolenoidMagField::SolenoidMagField()
{
	fpMatrix  = &IdentityMatrix;
}

SolenoidMagField::SolenoidMagField(	const G4ThreeVector& pOrigin,
					G4RotationMatrix* pMatrix)
{
	fOrigin   = pOrigin;
	fpMatrix  = pMatrix;
}

G4Field* SolenoidMagField::Clone() const
{
	return new SolenoidMagField(fOrigin, fpMatrix);
}

SolenoidMagField::~SolenoidMagField() = default;

void SolenoidMagField::GetFieldValue(	const G4double y[],	// [7]
					G4double B[]) const 	// [3]
{
	//  with displaced origin and rotation
	G4ThreeVector r_global = G4ThreeVector(	y[0] - fOrigin.x(),
                                         	y[1] - fOrigin.y(),
                                         	y[2] - fOrigin.z());
  
	const G4ThreeVector r_local = (*fpMatrix) * r_global;
	const G4ThreeVector B_local(	fGradient * r_local.y(),
                               		fGradient * r_local.x(), 
					0);
  	const G4ThreeVector B_global = fpMatrix->inverse() * B_local;

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
	string line

    	// Read R coordinates
    	if(myFile.good())
    	{
		// loop over all lines in the file
		while(getline(myFile, line))
		{
        		// Create a stringstream from line
        		stringstream ss(line);

			// read values in the line
        		myFile >> rr >> zz >> brr >> bzz;
			
			// fill R vectors and create R-Br/R-Bz pairs
			if(find(_R.begin(), _R.end(), rr) != _R.end())
			{
				_R.push_back(rr);
				_Br.push_back({rr, vector<double> {}});
				_Bz.push_back({rr, vector<double> {}});
			}
			// fill Z vectors
			if(find(_Z.begin(), _Z.end(), zz) != _Z.end())
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
		// read values in the line
		myFile >> rr >> zz >> brr >> bzz;

		// find R and Z indexes
		G4int rIndx = FindIndexinVector(_R,rr);		
		G4int zIndx = FindIndexinVector(_Z,zz);		
		
		// fill R-Br/R-Bz pairs
		if(rIndx > 0 && zIndx > 0)
		{
			_Br.at(rIndx).second.push_back(brr);
			_Bz.at(rIndx).second.push_back(bzz);
		}
		else
		{
			G4cout<<"\n\n=================================================================="<<G4endl;
			G4cout<<"[INFO] SolenoidMagField::ReadSolenoidFieldData"<<G4endl;
			G4cout<<"R = "<<rr<<G4endl;
			G4cout<<"Z = "<<zz<<G4endl;
			G4cout<<"Br = "<<brr<<G4endl;
			G4cout<<"Bz = "<<bzz<<G4endl;
			G4cout<<"==================================================================\n\n"<<G4endl; 
			throw runtime_error("[ERROR] SolenoidMagField::ReadSolenoidFieldData ==> Wrong R or Z coordinates\n");
		}
		
		
	
	}
	

	return;
}




int SolenoidMagField::FindIndexinVector(vector<double> myVector, G4double targetElement)
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

