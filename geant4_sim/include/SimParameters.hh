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

	G4String _beampipeFileName;

	G4double _ext_length;
	G4double _ext_radius;
	G4double _ext_endpos;	

	G4double _ipPosX;
	G4double _ipPosY;
	G4double _ipPosZ;
	G4double _ipTheta;

	vector<G4String> _magName;
	vector<G4String> _magType;
	vector<G4double> _magLength;
	vector<G4double> _magStartPosX;
	vector<G4double> _magStartPosY;
	vector<G4double> _magStartPosZ;
	vector<G4double> _magEndPosX;
	vector<G4double> _magEndPosY;
	vector<G4double> _magEndPosZ;
	vector<G4double> _magAngle;
	vector<G4double> _magK1L;

	vector<G4double> _absSizeX;
	vector<G4double> _absSizeY;
	vector<G4double> _absSizeZ;
	vector<G4double> _absPosX;
	vector<G4double> _absPosY;
	vector<G4double> _absPosZ;

    	G4double _beamStartPosX;
    	G4double _beamStartPosY;
    	G4double _beamStartPosZ;
    	G4double _beamEndPosX;
    	G4double _beamEndPosY;
    	G4double _beamEndPosZ;
	G4double _beamEmitX;
	G4double _beamEmitY;
	G4double _beamBetaX;
	G4double _beamBetaY;
	G4double _beamEtaX;
	G4double _beamEtaY;
	G4double _beamEtaPrimeX;
	G4double _beamEtaPrimeY;

	G4double _beamMomSpread;
    	G4double _beamGamma;
    	G4String _beamName;

	G4int _SRtype;

	G4double _gammaEcut;
	G4double _trackTcut;

	G4double _ipBeampipeZmin;
	G4double _ipBeampipeZmax;

public:
	// print functions
	void InitDefault();	
	void PrintGeoParameters();
	void PrintBeampipeFileName();
	void PrintBeamParameters();
	void PrintMagParameters();
	void CleanMagParameters();
	void PrintAbsParameters();
	void PrintIpBeampipeParameters();
	void CleanAbsParameters();
	void PrintSRtype();
	void PrintGammaEcut();
	void PrintTrackTcut();
	void ReadXML(G4String fName);

	// get functions
	G4double GetWorldSizeX(){return _world_dim_x;}
	G4double GetWorldSizeY(){return _world_dim_y;}
	G4double GetWorldSizeZ(){return _world_dim_z;}

	G4String GetBeampipeFileName(){return _beampipeFileName;}

	G4double GetExtL(){return _ext_length;}
	G4double GetExtR(){return _ext_radius;}
	G4double GetExtE(){return _ext_endpos;}

	G4double GetIpBeampipeZmin(){return _ipBeampipeZmin;}
	G4double GetIpBeampipeZmax(){return _ipBeampipeZmax;}

	G4double GetIpPosX(){return _ipPosX;}
	G4double GetIpPosY(){return _ipPosY;}
	G4double GetIpPosZ(){return _ipPosZ;}
	G4double GetIpTheta(){return _ipTheta;}

	G4String GetMagName(G4int i)
	{
		if(i < 0)	{return _magName.back();}
		else 		{return _magName.at(i);}
	}
	G4String GetMagType(G4int i){return _magType.at(i);}
	G4int GetMagNum(){return _magName.size();}
	G4double GetMagLength(G4int i){return _magLength.at(i);}
	G4double GetMagStartPosX(G4int i){return _magStartPosX.at(i);}
	G4double GetMagStartPosY(G4int i){return _magStartPosY.at(i);}
	G4double GetMagStartPosZ(G4int i){return _magStartPosZ.at(i);}
	G4double GetMagEndPosX(G4int i){return _magEndPosX.at(i);}
	G4double GetMagEndPosY(G4int i){return _magEndPosY.at(i);}
	G4double GetMagEndPosZ(G4int i){return _magEndPosZ.at(i);}
	G4double GetMagAngle(G4int i){return _magAngle.at(i);}
	G4double GetMagK1L(G4int i){return _magK1L.at(i);}
	
	G4int GetAbsNum(){return _absSizeX.size();}
	G4double GetAbsSizeX(G4int i){return _absSizeX.at(i);}
	G4double GetAbsSizeY(G4int i){return _absSizeY.at(i);}
	G4double GetAbsSizeZ(G4int i){return _absSizeZ.at(i);}
	G4double GetAbsPosX(G4int i){return _absPosX.at(i);}
	G4double GetAbsPosY(G4int i){return _absPosY.at(i);}
	G4double GetAbsPosZ(G4int i){return _absPosZ.at(i);}

    	G4String GetBeamName(){return _beamName;}
	G4double GetBeamStartPosX(){return _beamStartPosX;}
	G4double GetBeamStartPosY(){return _beamStartPosY;}
	G4double GetBeamStartPosZ(){return _beamStartPosZ;}
	G4double GetBeamEndPosX(){return _beamEndPosX;}
	G4double GetBeamEndPosY(){return _beamEndPosY;}
	G4double GetBeamEndPosZ(){return _beamEndPosZ;}
    	G4double GetBeamGamma(){return _beamGamma;}
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

	G4double GetTrackTcut(){return _trackTcut;}

private:
	// set functions
	void SetWorldSizeX(G4double val){_world_dim_x = val; return;}
	void SetWorldSizeY(G4double val){_world_dim_y = val; return;}
	void SetWorldSizeZ(G4double val){_world_dim_z = val; return;}

	void SetBeampipeFileName(G4String val){_beampipeFileName = val; return;}

	void SetExtL(G4double val){_ext_length = val; return;}
	void SetExtR(G4double val){_ext_radius = val; return;}
	void SetExtE(G4double val){_ext_endpos = val; return;}

	void SetIpBeampipeZmin(G4double val){_ipBeampipeZmin = val; return;}
	void SetIpBeampipeZmax(G4double val){_ipBeampipeZmax = val; return;}

	void SetIpPosX(G4double val){_ipPosX = val; return;}
	void SetIpPosY(G4double val){_ipPosY = val; return;}
	void SetIpPosZ(G4double val){_ipPosZ = val; return;}
	void SetIpTheta(G4double val){_ipTheta = val; return;}

	void SetMagParameters(G4String magName, G4String magType, G4double magLength, 
		G4double magStartPosX, G4double magStartPosY, G4double magStartPosZ,
		G4double magEndPosX, G4double magEndPosY, G4double magEndPosZ,
		G4double magAngle, G4double magK1L);

	void SetAbsParameters(G4double absSizeX, G4double absSizeY, G4double absSizeZ,
	G4double absPosX, G4double absPosY, G4double absPosZ);

    	void SetBeamName(G4String val){_beamName = val; return;}
	void SetBeamStartPosX(G4double val){_beamStartPosX = val; return;}
	void SetBeamStartPosY(G4double val){_beamStartPosY = val; return;}
	void SetBeamStartPosZ(G4double val){_beamStartPosZ = val; return;}
	void SetBeamEndPosX(G4double val){_beamEndPosX = val; return;}
	void SetBeamEndPosY(G4double val){_beamEndPosY = val; return;}
	void SetBeamEndPosZ(G4double val){_beamEndPosZ = val; return;}
    	void SetBeamGamma(G4double val){_beamGamma = val; return;}
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

	void SetTrackTcut(G4double val){_trackTcut = val; return;}

	G4bool FileExists(const G4String& fName);
};

#endif
