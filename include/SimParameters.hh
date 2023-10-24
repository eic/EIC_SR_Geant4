#ifndef SimParameters_h
#define SimParameters_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class SimParameters
{
public: 
	SimParameters();
	~SimParameters();
private:
	// geometry
    	G4double world_dim_x;
    	G4double world_dim_y;
    	G4double world_dim_z;

    	G4double beam_pos_x;
    	G4double beam_pos_y;
    	G4double beam_pos_z;

	G4double ext_length;
	G4double ext_radius;
	G4double ext_endpos;	

	// beam
/*
	G4double beamEmitX = 24.0*nm;
	G4double beamEmitY =  2.0*nm;
	G4double beamBetaX = 202.4452538*m;
	G4double beamBetaY = 128.4636335*m;
	G4double beamEtaX = -0.02443133971*m;
	G4double beamEtaY = 0.0*m;
	G4double beamEtaPrimeX = 0.01315593225;
	G4double beamEtaPrimeY = 0.0;
*/

public:
	void InitDefault();	
	void PrintGeoParameters();

	G4double GetWorldSizeX(){return world_dim_x;}
	G4double GetWorldSizeY(){return world_dim_y;}
	G4double GetWorldSizeZ(){return world_dim_z;}

	G4double GetBeamPosX(){return beam_pos_x;}
	G4double GetBeamPosY(){return beam_pos_y;}
	G4double GetBeamPosZ(){return beam_pos_z;}

	G4double GetExtL(){return ext_length;}
	G4double GetExtR(){return ext_radius;}
	G4double GetExtE(){return ext_endpos;}
	
};

#endif
