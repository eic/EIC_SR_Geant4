#include "GammaReflectionProcess.hh"
#include "G4VParticleChange.hh"
#include "G4VSolid.hh"
#include "TROOT.h"


GammaReflectionProcess::GammaReflectionProcess() : G4VProcess("GammaReflectionProcess")
{
	_reflectFileNameMap["Cu"] = "../materials/02-Copper.csv";
	_reflectFileNameMap["Au"] = "../materials/04-Gold.csv";

	_reflectDataMap["Cu"] = ReadReflectData(_reflectFileNameMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"]);
	_reflectDataMap["Au"] = ReadReflectData(_reflectFileNameMap["Au"],_grazingAngleMap["Au"],_photonEnergyMap["Au"]);

	cout<<"\n\n[DEBUG] GammaReflectionProcess::GammaReflectionProcess ==> Cu"<<endl;
	PrintReflectData(_reflectDataMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"]);
	cout<<"\n\n[DEBUG] GammaReflectionProcess::GammaReflectionProcess ==> Au"<<endl;
	PrintReflectData(_reflectDataMap["Au"],_grazingAngleMap["Au"],_photonEnergyMap["Au"]);
}

GammaReflectionProcess::~GammaReflectionProcess()
{}

G4double GammaReflectionProcess::PostStepGetPhysicalInteractionLength(const G4Track&,G4double,G4ForceCondition* condition)
{
  	*condition = Forced;
  	return DBL_MAX;
}
  
G4VParticleChange * 
GammaReflectionProcess::PostStepDoIt(const G4Track& track, const G4Step & step)
{

  	aParticleChange.Initialize(track);

    	G4StepPoint*          	preStep		= step.GetPreStepPoint();
    	G4StepPoint*          	postStep	= step.GetPostStepPoint();
    	G4VPhysicalVolume*    	prePV		= preStep->GetPhysicalVolume();
    	G4VPhysicalVolume*	postPV		= postStep->GetPhysicalVolume();

	// check if the next step is not outside the world and skip everything except for gamma
	if(!postPV || track.GetParticleDefinition()->GetPDGEncoding() != G4Gamma::Gamma()->GetPDGEncoding())
		return &aParticleChange;


	// get tracks at the edge of the vacuum beam pipe and world
	if(	postStep->GetStepStatus() == fGeomBoundary && postPV->GetName() == "world" &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
		//..............-----------------------------------------
		//..............|..................Volume...............|
		//.pre-step.....0....<--.step.status.on.boundary.-->....0.post-step.point
		//.point........|\...................................../|.steps.ends.on.boundary
		//..............|.\.................................../.|
		//..............|..\................................./..|
		//..............|...\.............................../...|
		//..............|....0.post-step.point....pre-step.0....|
		//..............|.........................point.........|
		//..............-----------------------------------------

		G4double gammaEnergy_eV = track.GetTotalEnergy()/CLHEP::eV; // MeV --> eV
		G4ThreeVector gammaDir_i = postStep->GetMomentum()/postStep->GetMomentum().mag(); 
		G4ThreeVector stepPos = postStep->GetPosition();

		const G4VTouchable* touchable = preStep->GetTouchable();
		const G4RotationMatrix* rotation = touchable->GetRotation();
		G4RotationMatrix rotation_inv = rotation->inverse();
		G4ThreeVector translation = touchable->GetTranslation();
		G4VSolid* solid = touchable->GetSolid();
		G4String name = touchable->GetVolume()->GetName();
		G4ThreeVector posLoc = *rotation * (stepPos - translation);
		// the norm vector looks from inside to outside of the volume
		G4ThreeVector normLoc = solid->SurfaceNormal(posLoc); 
		G4ThreeVector gammaDirLoc_i = *rotation * gammaDir_i;
		G4double incidentAngle_rad = acos( normLoc.dot(gammaDirLoc_i) );
		G4double grazingAngle_rad = M_PI_2 - incidentAngle_rad; 

		G4ThreeVector norm = rotation_inv*normLoc + translation;

		//- Reflection
		// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
		// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
		// where i is the incoming ray direction, n the surface normal, and r the
		// reflection direction. The normal direction is expected to be
		// normalized (equal to a length of one) in this form. The normal can point in 
		// either direction (e.g., upward or downward) and the formula still works. The 
		// incoming ray direction does not have to be normalized, and the outgoing 
		// direction vector will be computed having the same length as the incoming one.

		G4ThreeVector gammaDirLoc_r = gammaDirLoc_i - 2.0 * normLoc.dot(gammaDirLoc_i) * normLoc;
		G4ThreeVector gammaDir_r = rotation_inv * gammaDirLoc_r + translation;

		// check that the half-angle between reflected and incident angles is equal to the grazing angle
		if(abs(grazingAngle_rad - acos(gammaDir_i.dot(gammaDir_r))/2.)/grazingAngle_rad > 1e-4)
		{
			G4cout<<"\n\n[DEBUG] GammaReflectionProcess::PostStepDoIt"<<G4endl;
			G4cout<<"- grazingAngle_rad = "<<grazingAngle_rad<<G4endl;
			G4cout<<"- acos(gammaDir_i.dot(gammaDir_r))/2. = "<<acos(gammaDir_i.dot(gammaDir_r))/2.<<G4endl;
			G4cout<<"- abs(grazingAngle_rad - acos(gammaDir_i.dot(gammaDir_r))/2.)/grazingAngle_rad = "<<abs(grazingAngle_rad - acos(gammaDir_i.dot(gammaDir_r))/2.)/grazingAngle_rad<<G4endl;
			G4cout<<"- gammaDir_i(x,y,z) = "<<gammaDir_i.x()<<" , "<<gammaDir_i.y()<<" , "<<gammaDir_i.z()<<G4endl;
			G4cout<<"- gammaDir_r(x,y,z) = "<<gammaDir_r.x()<<" , "<<gammaDir_r.y()<<" , "<<gammaDir_r.z()<<G4endl;
			G4cout<<"- norm (x,y,z) = "<<norm.x()<<" , "<<norm.y()<<" , "<<norm.z()<<G4endl;
			throw runtime_error("\n\n[ERROR] GammaReflectionProcess::PostStepDoIt ==> Wrong reflection direction\n");
		}

		// get probability
		G4double prob = FindReflectionProbability(_reflectDataMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingAngle_rad/1000.0,gammaEnergy_eV);

		//- Roughtness
		// In the old (default) Synrad+ reflection model, reflection is specular (mirror-like), but as surface roughness is taken into account, 
		// the theta and phi angles (between the reflected direction and the surface normal) are perturbated with a gaussian distributied offset
		G4double sigmaRatio = 0.005;
		G4double thetaOffset 	= atan(sigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		G4double phiOffset 	= atan(sigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		// where thetaOffset and phiOffset are the random angles added to the calculated ones (those of mirror reflection), and sigmaRation is the value 
		// the user inputs in the Synrad+ Material Roughness field (the ratio of the sigmaH and sigmaX values, where sigmaX is the standard deviation of the size 
		// of cavities on the surface, and sigmaH is the st.dev. of the depth of those). Essentially, the larger the "roughness" value (theoretically the ratio 
		// of RMS roughness and autocorrelation length), the more the reflection direction is perturbated around the perfect mirror direction.

		G4ThreeVector gammaDir_tmp = gammaDir_r;

		gammaDir_tmp.setTheta(gammaDir_r.getTheta() + thetaOffset);
		gammaDir_tmp.setPhi(gammaDir_r.getPhi() + phiOffset);

		// the offsetted reflected gamma is within physical ranges; since the norm vector looks from inside to outside of the vacuum volume,
		// the theta should be (pi/2;pi); see above "SurfaceNormal": SurfaceNormal > 0 --> (pi/2;pi) OR SurfaceNormal < 0 --> (0;pi/2)
		while(acos(norm.dot(gammaDir_tmp)) < M_PI_2)
		{
			thetaOffset = atan(sigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
			gammaDir_tmp.setTheta(gammaDir_r.getTheta() + thetaOffset);
		}

		gammaDir_r = gammaDir_tmp;

		if(G4UniformRand() > prob)
    			aParticleChange.ProposeTrackStatus(fStopAndKill);
		else
			aParticleChange.ProposeMomentumDirection(gammaDir_r);
	}

	// get tracks at the edge of the vacuum beam pipe and absorber
	if(	postStep->GetStepStatus() == fGeomBoundary && G4StrUtil::contains(postPV->GetName(),"abs_") &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
		aParticleChange.ProposeTrackStatus(fStopAndKill);
	}

  	return &aParticleChange;
}

const G4String &GammaReflectionProcess::GetName() const 
{
	return theProcessName;
}


G4double GammaReflectionProcess::AlongStepGetPhysicalInteractionLength(const G4Track&,G4double,G4double,G4double&,G4GPILSelection*) 
{
  	return -1.0;
}

G4double GammaReflectionProcess::AtRestGetPhysicalInteractionLength(const G4Track&,G4ForceCondition*) 
{
  	return -1.0;
}

G4VParticleChange* GammaReflectionProcess::AtRestDoIt(const G4Track&,const G4Step&) 
{
  	return 0;
}

G4VParticleChange* GammaReflectionProcess::AlongStepDoIt(const G4Track&,const G4Step&) 
{
  	return 0;
}

void GammaReflectionProcess::PrintReflectData(vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy)
{
	// print column names
	printf("Angl/Ene : ");
	for(int j = 0; j < angle.size(); ++j){printf("%.2e  ",angle.at(j));}
	printf("\n");

	printf("Angl/Ene : ");
    	for(int j = 0; j < dataset.size(); ++j)
    	{
        	printf("%.2e  ",dataset.at(j).first);
    	}
	printf("\n");

	// print data
    	for(int i = 0; i < dataset.at(0).second.size(); ++i)
    	{
		printf("%.2e  : ",energy.at(i));
        	for(int j = 0; j < dataset.size(); ++j)
        	{
           		printf("%.2e  ",dataset.at(j).second.at(i));
        	}
		printf("\n");
    	}

	return;
}

vector<pair<double, vector<double>>> GammaReflectionProcess::ReadReflectData(string filename,
	vector<double> &angle, vector<double> &energy)
{
	// Reads a CSV file into a vector of <string, vector<double>> pairs where
	// each pair represents <column name, column values>

	vector<pair<double, vector<double>>> dataset;

	// Create an input filestream
    	ifstream myFile(filename);

    	// make sure the file is open
    	if(!myFile.is_open()) 
		throw runtime_error(
			Form("\n\n[ERROR] GammaReflectionProcess::ReadReflectData ==> Cannot open of find the file: %s\n",
				filename.c_str()));

    	// Helper vars
    	string line, colname;
    	double val;

    	// Read the column names
    	if(myFile.good())
    	{
        	// Extract the first line in the file
        	getline(myFile, line);

        	// Create a stringstream from line
        	stringstream ss(line);

        	// Extract each column name
		getline(ss, colname, ','); // skip the first column
        	while(getline(ss, colname, ','))
		{
			val = stod(colname);
			// Initialize and add <colname, double vector> pairs to dataset
            		dataset.push_back({val, vector<double> {}});
			angle.push_back(val);
        	}
    	}

    	// Read data, line by line
    	while(getline(myFile, line))
    	{
        	// Create a stringstream of the current line
        	stringstream ss(line);
        
        	// Keep track of the current column index
        	int colIdx = 0;
        
        	// Extract each value
        	while(ss >> val)
		{
            		// Add the current integer to the 'colIdx' column's values vector
            		if(colIdx == 0) energy.push_back(val);
			else dataset.at(colIdx-1).second.push_back(val);
            
            		// If the next token is a comma, ignore it and move on
            		if(ss.peek() == ',') ss.ignore();
            
            		// Increment the column index
            		colIdx++;
        	}
    	}

    	// Close file
    	myFile.close();

	return dataset;
}

void GammaReflectionProcess::FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2)
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

double GammaReflectionProcess::FindReflectionProbability(vector<pair<double, vector<double>>> dataset,
								vector<double> angle, vector<double> energy, 
								double angle_val, double energy_val)
{
	int ang_i, ang_j, ene_i, ene_j;

	FindClosestIndexesInVec(angle,angle_val,ang_i,ang_j);
	FindClosestIndexesInVec(energy,energy_val,ene_i,ene_j);

	double prob_ii = dataset.at(ang_i).second.at(ene_i);
	double prob_ji = dataset.at(ang_j).second.at(ene_i);
	double prob_ij = dataset.at(ang_i).second.at(ene_j);
	double prob_jj = dataset.at(ang_j).second.at(ene_j);

	double prob_ii_ji = (ang_i == ang_j) ? prob_ii : (prob_ji - prob_ii)*(angle_val - angle.at(ang_i))/(angle.at(ang_j) - angle.at(ang_i)) + prob_ii;
	double prob_ij_jj = (ang_i == ang_j) ? prob_jj : (prob_jj - prob_ij)*(angle_val - angle.at(ang_j))/(angle.at(ang_i) - angle.at(ang_j)) + prob_ij;

	double prob = (ene_i == ene_j) ? prob_ii_ji : (prob_ij_jj - prob_ii_ji)*(energy_val - energy.at(ene_i))/(energy.at(ene_j) - energy.at(ene_i)) + prob_ii_ji;

	return prob;
}
