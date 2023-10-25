#include "SimParameters.hh"  
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;

SimParameters::SimParameters()
{}    	

SimParameters::~SimParameters()
{}

void SimParameters::ReadXML(G4String fName)
{
    	// Create empty property tree object
    	ptree tree;

    	// Parse the XML into the property tree.
    	read_xml(fName, tree);

    	// Use get_child to find the node containing the a specific data, and iterate over
    	// its children. If the path cannot be resolved, get_child throws.
    	// A C++11 for-range loop would also work.
	// world
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.world"))
	{
        	// The data function is used to access the data stored in a node.
		if(child_node.first == "sec" && child_node.second.get<string>("<xmlattr>.unit") == "m")
		{
			if(child_node.second.get<string>("<xmlattr>.name") == "x")
				SetWorldSizeX(stod(child_node.second.data())*m);
			else if(child_node.second.get<string>("<xmlattr>.name") == "y")
				SetWorldSizeY(stod(child_node.second.data())*m);
			else if(child_node.second.get<string>("<xmlattr>.name") == "z")
				SetWorldSizeZ(stod(child_node.second.data())*m);
			else
				throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n"); 
		}
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
    	}
	// extension
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.extension"))
	{
        	// The data function is used to access the data stored in a node.
		if(child_node.first == "sec" && child_node.second.get<string>("<xmlattr>.unit") == "cm")
		{
			if(child_node.second.get<string>("<xmlattr>.name") == "l")
				SetExtL(stod(child_node.second.data())*cm);
			else if(child_node.second.get<string>("<xmlattr>.name") == "r")
				SetExtR(stod(child_node.second.data())*cm);
			else if(child_node.second.get<string>("<xmlattr>.name") == "e")
				SetExtE(stod(child_node.second.data())*cm);
			else
				throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n"); 
		}
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
    	}

	return;
}


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
	
    	_beamPosX =  7.61776450600000E+00*cm;
    	_beamPosY =  0.00000000000000E+00*cm;
    	_beamPosZ = -3.71513783800000E+03*cm;
	_beamEmitX = 24.0*nm;
	_beamEmitY =  2.0*nm;
	_beamBetaX = 202.4452538*m;
	_beamBetaY = 128.4636335*m;
	_beamEtaX = -0.02443133971*m;
	_beamEtaY = 0.0*m;
	_beamEtaPrimeX = 0.01315593225;
	_beamEtaPrimeY = 0.0;
	_beamMomSpread = 0.109e-2;
	_beamMom = 18*GeV;
	_beamDirX = -2.48090543441244E-02;
	_beamDirY = 0.00000000000000E+00;
	_beamDirZ = 9.99692208043331E-01;
	_beamName = "e-";

	_SRtype = true;

	_gammaEcut = 5*keV;

	return;
}

void SimParameters::PrintGeoParameters()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintGeoParameters ==> Geometry parameters"<<G4endl;
	G4cout<<"World size X = "<<GetWorldSizeX()/m<<" [m]"<<G4endl;
	G4cout<<"World size Y = "<<GetWorldSizeY()/m<<" [m]"<<G4endl;
	G4cout<<"World size Z = "<<GetWorldSizeZ()/m<<" [m]"<<G4endl;
	G4cout<<"Extension length = "<<GetExtL()/cm<<" [cm]"<<G4endl;
	G4cout<<"Extension radius = "<<GetExtR()/cm<<" [cm]"<<G4endl;
	G4cout<<"Extension end position = "<<GetExtE()/cm<<" [cm]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintBeamParameters()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintBeamParameters ==> Beam parameters"<<G4endl;
	G4cout<<"Position X = "<<GetBeamPosX()/cm<<" [cm]"<<G4endl;
	G4cout<<"Position Y = "<<GetBeamPosY()/cm<<" [cm]"<<G4endl;
	G4cout<<"Position Z = "<<GetBeamPosZ()/cm<<" [cm]"<<G4endl;
	G4cout<<"Emittance X = "<<GetBeamEmitX()/nm<<" [nm]"<<G4endl;
	G4cout<<"Emittance Y = "<<GetBeamEmitY()/nm<<" [nm]"<<G4endl;
	G4cout<<"Beta X = "<<GetBeamBetaX()/m<<" [m]"<<G4endl;
	G4cout<<"Beta Y = "<<GetBeamBetaY()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion X = "<<GetBeamEtaX()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion Y = "<<GetBeamEtaY()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion PX = "<<GetBeamEtaPrimeX()/m<<" []"<<G4endl;
	G4cout<<"Dispersion PY = "<<GetBeamEtaPrimeY()/m<<" []"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintSRtype()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintSRtype ==> "<<GetSRtype()<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintGammaEcut()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintGammaEcut ==> Ecut = "<<GetGammaEcut()/eV<<" [eV]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}
