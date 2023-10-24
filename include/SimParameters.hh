#ifndef SimParameters_h
#define SimParameters_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "string"

using namespace std;

class SimParameters
{
public: 
	SimParameters();
	~SimParameters();
private:
	// geometry
    	G4double _world_dim_x;
    	G4double _world_dim_y;
    	G4double _world_dim_z;

    	G4double _beam_pos_x;
    	G4double _beam_pos_y;
    	G4double _beam_pos_z;

	G4double _ext_length;
	G4double _ext_radius;
	G4double _ext_endpos;	

	vector<string> _magName;
	vector<string> _magType;
	vector<G4double> _magLength;
	vector<G4double> _magStartPosX;
	vector<G4double> _magStartPosY;
	vector<G4double> _magStartPosZ;
	vector<G4double> _magStartDirX;
	vector<G4double> _magStartDirY;
	vector<G4double> _magStartDirZ;
	vector<G4double> _magBx;
	vector<G4double> _magBy;
	vector<G4double> _magBz;
	vector<G4double> _magK1;
	

public:
	void InitDefault();	
	void PrintGeoParameters();

	G4double GetWorldSizeX(){return _world_dim_x;}
	G4double GetWorldSizeY(){return _world_dim_y;}
	G4double GetWorldSizeZ(){return _world_dim_z;}

	G4double GetBeamPosX(){return _beam_pos_x;}
	G4double GetBeamPosY(){return _beam_pos_y;}
	G4double GetBeamPosZ(){return _beam_pos_z;}

	G4double GetExtL(){return _ext_length;}
	G4double GetExtR(){return _ext_radius;}
	G4double GetExtE(){return _ext_endpos;}

	string GetMagName(int i){return _magName.at(i);}
	string GetMagType(int i){return _magType.at(i);}
	G4int GetMagNum(){return _magName.size();}
	G4double GetMagLength(int i){return _magLength.at(i);}
	G4double GetMagStartPosX(int i){return _magStartPosX.at(i);}
	G4double GetMagStartPosY(int i){return _magStartPosY.at(i);}
	G4double GetMagStartPosZ(int i){return _magStartPosZ.at(i);}
	G4double GetMagStartDirX(int i){return _magStartDirX.at(i);}
	G4double GetMagStartDirY(int i){return _magStartDirY.at(i);}
	G4double GetMagStartDirZ(int i){return _magStartDirZ.at(i);}
	G4double GetMagBx(int i){return _magBx.at(i);}
	G4double GetMagBy(int i){return _magBy.at(i);}
	G4double GetMagBz(int i){return _magBz.at(i);}
	G4double GetMagK1(int i){return _magK1.at(i);}
	
};

#endif
