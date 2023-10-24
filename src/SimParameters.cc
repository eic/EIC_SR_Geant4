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

    	_beam_pos_x =  7.61776450600000E+00*cm;
    	_beam_pos_y =  0.00000000000000E+00*cm;
    	_beam_pos_z = -3.71513783800000E+03*cm;

	_ext_length = 370*cm;
	_ext_radius = 20.0*cm;
	_ext_endpos = -3350.0*cm;	

	_magName.push_back("D2EF2");
	_magType.push_back("Dipole");
	_magLength.push_back(2.726015431*m);
	_magStartPosX.push_back(7.61776450600000E+00*cm);
	_magStartPosY.push_back(0.0*cm);
	_magStartPosZ.push_back(-3.71513783800000E+03*cm);
	_magStartDirX.push_back(-2.48090543441244E-02);
	_magStartDirY.push_back(0.0);
	_magStartDirZ.push_back(9.99692208043331E-01);
	_magBx.push_back(0.0*tesla);
	_magBy.push_back(2.54524403400000E-01*tesla);
	_magBz.push_back(0.0*tesla);
	_magK1.push_back(0.0*tesla/(1.*meter));

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
