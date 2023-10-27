#ifndef SimParameters_h
#define SimParameters_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

using namespace std;

class SimParameters
{
public: 
	SimParameters();
	~SimParameters();
private:
    	G4double _world_dim_x;
    	G4double _world_dim_y;
    	G4double _world_dim_z;

	G4double _ext_length;
	G4double _ext_radius;
	G4double _ext_endpos;	

	vector<G4String> _magName;
	vector<G4String> _magType;
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

	vector<G4double> _absSizeX;
	vector<G4double> _absSizeY;
	vector<G4double> _absSizeZ;
	vector<G4double> _absPosX;
	vector<G4double> _absPosY;
	vector<G4double> _absPosZ;

    	G4double _beamPosX;
    	G4double _beamPosY;
    	G4double _beamPosZ;
	G4double _beamEmitX;
	G4double _beamEmitY;
	G4double _beamBetaX;
	G4double _beamBetaY;
	G4double _beamEtaX;
	G4double _beamEtaY;
	G4double _beamEtaPrimeX;
	G4double _beamEtaPrimeY;

	G4double _beamMomSpread; // _deltaP
    	G4double _beamMom;;
    	G4double _beamDirX;
    	G4double _beamDirY;
    	G4double _beamDirZ;
    	G4String _beamName;

	G4int _SRtype;

	G4double _gammaEcut;

public:
	// print functions
	void InitDefault();	
	void PrintGeoParameters();
	void PrintBeamParameters();
	void PrintMagParameters();
	void CleanMagParameters();
	void PrintAbsParameters();
	void CleanAbsParameters();
	void PrintSRtype();
	void PrintGammaEcut();
	void ReadXML(G4String fName);

	// get functions
	G4double GetWorldSizeX(){return _world_dim_x;}
	G4double GetWorldSizeY(){return _world_dim_y;}
	G4double GetWorldSizeZ(){return _world_dim_z;}

	G4double GetExtL(){return _ext_length;}
	G4double GetExtR(){return _ext_radius;}
	G4double GetExtE(){return _ext_endpos;}

	G4String GetMagName(G4int i){return _magName.at(i);}
	G4String GetMagType(G4int i){return _magType.at(i);}
	G4int GetMagNum(){return _magName.size();}
	G4double GetMagLength(G4int i){return _magLength.at(i);}
	G4double GetMagStartPosX(G4int i){return _magStartPosX.at(i);}
	G4double GetMagStartPosY(G4int i){return _magStartPosY.at(i);}
	G4double GetMagStartPosZ(G4int i){return _magStartPosZ.at(i);}
	G4double GetMagStartDirX(G4int i){return _magStartDirX.at(i);}
	G4double GetMagStartDirY(G4int i){return _magStartDirY.at(i);}
	G4double GetMagStartDirZ(G4int i){return _magStartDirZ.at(i);}
	G4double GetMagBx(G4int i){return _magBx.at(i);}
	G4double GetMagBy(G4int i){return _magBy.at(i);}
	G4double GetMagBz(G4int i){return _magBz.at(i);}
	G4double GetMagK1(G4int i){return _magK1.at(i);}
	
	G4int GetAbsNum(){return _absSizeX.size();}
	G4double GetAbsSizeX(G4int i){return _absSizeX.at(i);}
	G4double GetAbsSizeY(G4int i){return _absSizeY.at(i);}
	G4double GetAbsSizeZ(G4int i){return _absSizeZ.at(i);}
	G4double GetAbsPosX(G4int i){return _absPosX.at(i);}
	G4double GetAbsPosY(G4int i){return _absPosY.at(i);}
	G4double GetAbsPosZ(G4int i){return _absPosZ.at(i);}

    	G4String GetBeamName(){return _beamName;}
	G4double GetBeamPosX(){return _beamPosX;}
	G4double GetBeamPosY(){return _beamPosY;}
	G4double GetBeamPosZ(){return _beamPosZ;}
    	G4double GetBeamDirX(){return _beamDirX;}
    	G4double GetBeamDirY(){return _beamDirY;}
    	G4double GetBeamDirZ(){return _beamDirZ;}
    	G4double GetBeamMom(){return _beamMom;}
	G4double GetBeamMomSpread(){return _beamMomSpread;}
	G4double GetBeamBetaX(){return _beamBetaX;}
	G4double GetBeamBetaY(){return _beamBetaY;}
	G4double GetBeamEtaX(){return _beamEtaX;}
	G4double GetBeamEtaY(){return _beamEtaY;}
	G4double GetBeamEtaPrimeX(){return _beamEtaPrimeX;}
	G4double GetBeamEtaPrimeY(){return _beamEtaPrimeY;}
	G4double GetBeamEmitX(){return _beamEmitX;}
	G4double GetBeamEmitY(){return _beamEmitY;}

	G4int GetSRtype(){return _SRtype;}

	G4double GetGammaEcut(){return _gammaEcut;}

private:
	// set functions
	void SetWorldSizeX(G4double val){_world_dim_x = val; return;}
	void SetWorldSizeY(G4double val){_world_dim_y = val; return;}
	void SetWorldSizeZ(G4double val){_world_dim_z = val; return;}

	void SetExtL(G4double val){_ext_length = val; return;}
	void SetExtR(G4double val){_ext_radius = val; return;}
	void SetExtE(G4double val){_ext_endpos = val; return;}

	void SetMagParameters(G4String magName, G4String magType, G4double magLength, 
	G4double magStartPosX, G4double magStartPosY, G4double magStartPosZ,
	G4double magStartDirX, G4double magStartDirY, G4double magStartDirZ,
	G4double magBx, G4double magBy, G4double magBz, G4double magK1);

	void SetAbsParameters(G4double absSizeX, G4double absSizeY, G4double absSizeZ,
	G4double absPosX, G4double absPosY, G4double absPosZ);

    	void SetBeamName(G4String val){_beamName = val; return;}
	void SetBeamPosX(G4double val){_beamPosX = val; return;}
	void SetBeamPosY(G4double val){_beamPosY = val; return;}
	void SetBeamPosZ(G4double val){_beamPosZ = val; return;}
    	void SetBeamDirX(G4double val){_beamDirX = val; return;}
    	void SetBeamDirY(G4double val){_beamDirY = val; return;}
    	void SetBeamDirZ(G4double val){_beamDirZ = val; return;}
    	void SetBeamMom(G4double val){_beamMom = val; return;}
	void SetBeamMomSpread(G4double val){_beamMomSpread = val; return;}
	void SetBeamBetaX(G4double val){_beamBetaX = val; return;}
	void SetBeamBetaY(G4double val){_beamBetaY = val; return;}
	void SetBeamEtaX(G4double val){_beamEtaX = val; return;}
	void SetBeamEtaY(G4double val){_beamEtaY = val; return;}
	void SetBeamEtaPrimeX(G4double val){_beamEtaPrimeX = val; return;}
	void SetBeamEtaPrimeY(G4double val){_beamEtaPrimeY = val; return;}
	void SetBeamEmitX(G4double val){_beamEmitX = val; return;}
	void SetBeamEmitY(G4double val){_beamEmitY = val; return;}

	void SetSRtype(G4int val){_SRtype = val; return;}

	void SetGammaEcut(G4double val){_gammaEcut = val; return;}

	G4bool FileExists(const G4String& fName);
};

#endif
