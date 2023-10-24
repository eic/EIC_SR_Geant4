#include "SimParameters.hh"  

SimParameters::SimParameters()
{}    	

SimParameters::~SimParameters()
{}

void SimParameters::InitDefault()
{
	_world_dim_x =  10.0*m;
    	_world_dim_y =  10.0*m;
    	_world_dim_z =  80.0*m;

	_ext_length = 370*cm;
	_ext_radius = 20.0*cm;
	_ext_endpos = -3350.0*cm;	

	_magName = {"D2EF2","D2EF1","D1EF","Q1EF","Q0EF"};
	_magType = {"D-pole","D-pole","D-pole","Q-pole","Q-pole"};
	_magLength = {2.726015431*m,2.726015431*m,0.8914000836*m,1.61*m,1.20*m};
	_magStartPosX = {7.61776450600000E+00*cm,2.10318264100000E+00*cm,6.68549937300000E-02*cm,0.0*cm,0.0*cm};
	_magStartPosY = {0,0,0,0,0};
	_magStartPosZ = {-3.71513783800000E+03*cm,-3.41670739300000E+03*cm,-3.11826997500000E+03*cm,-1.23700000000000E+03*cm,-7.00000000000000E+02*cm};
	_magStartDirX = {-2.48090543441244E-02,-1.31554205131084E-02,-1.49999943750006E-03*cm,0,0};
	_magStartDirY = {0,0,0,0,0};
	_magStartDirZ = {9.99692208043331E-01,9.99913463711297E-01,9.99998875000211E-01*cm,0,0};
	_magBx = {0,0,0,0,0};
	_magBy = {2.54524403400000E-01*tesla,2.54524403400000E-01*tesla,1.00169207900000E-01*tesla,0,0};
	_magBz = {0,0,0,0,0};
	_magK1 = {0,0,0,5.957841642*tesla/(1.*meter),-12.98838344*tesla/(1.*meter)};

	
	_absSizeX = {30*cm,10*cm,10*cm,50*cm};
	_absSizeY = {30*cm,10*cm,10*cm,50*cm};
	_absSizeZ = {5*cm,5*cm,5*cm,5*cm};
	_absPosX = {0*cm,-12*cm,14*cm,0*cm};
	_absPosY = {0*cm,0*cm,0*cm,0*cm};
	_absPosZ = {1527*cm,475*cm,-550*cm,-3720*cm};
	
    	_beam_pos_x =  7.61776450600000E+00*cm;
    	_beam_pos_y =  0.00000000000000E+00*cm;
    	_beam_pos_z = -3.71513783800000E+03*cm;

	_beamEmitX = 24.0*nm;
	_beamEmitY =  2.0*nm;
	_beamBetaX = 202.4452538*m;
	_beamBetaY = 128.4636335*m;
	G4double beamEtaX = -0.02443133971*m;
	G4double beamEtaY = 0.0*m;
	G4double beamEtaPrimeX = 0.01315593225;
	G4double beamEtaPrimeY = 0.0;

	return;
}

void SimParameters::PrintGeoParameters()
{
	G4cout<<"world_dim_x = "<<_world_dim_x/m<<" [m]"<<G4endl;
	G4cout<<"world_dim_y = "<<_world_dim_y/m<<" [m]"<<G4endl;
	G4cout<<"world_dim_z = "<<_world_dim_z/m<<" [m]"<<G4endl;
	
	G4cout<<"ext_length = "<<_ext_length/cm<<" [cm]"<<G4endl;
	G4cout<<"ext_radius = "<<_ext_radius/cm<<" [cm]"<<G4endl;
	G4cout<<"ext_endpos = "<<_ext_endpos/cm<<" [cm]"<<G4endl;

	return;
}
