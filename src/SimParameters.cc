#include "SimParameters.hh"  

SimParameters::SimParameters()
{}    	

SimParameters::~SimParameters()
{}

void SimParameters::InitDefault()
{
	world_dim_x =  10.0*m;
    	world_dim_y =  10.0*m;
    	world_dim_z =  80.0*m;

    	beam_pos_x =  7.61776450600000E+00*cm;
    	beam_pos_y =  0.00000000000000E+00*cm;
    	beam_pos_z = -3.71513783800000E+03*cm;

	ext_length = 370*cm;
	ext_radius = 20.0*cm;
	ext_endpos = -3350.0*cm;	

	return;
}

void SimParameters::PrintGeoParameters()
{
	G4cout<<"world_dim_x = "<<world_dim_x/m<<" [m]"<<G4endl;
	G4cout<<"world_dim_y = "<<world_dim_y/m<<" [m]"<<G4endl;
	G4cout<<"world_dim_z = "<<world_dim_z/m<<" [m]"<<G4endl;
	
	G4cout<<"ext_length = "<<ext_length/cm<<" [cm]"<<G4endl;
	G4cout<<"ext_radius = "<<ext_radius/cm<<" [cm]"<<G4endl;
	G4cout<<"ext_endpos = "<<ext_endpos/cm<<" [cm]"<<G4endl;

	return;
}
