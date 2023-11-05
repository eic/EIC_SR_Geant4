#include "SimParameters.hh"  
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <sys/stat.h>

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
	if(!FileExists(fName)){return;}
    	read_xml(fName, tree);

    	// Use get_child to find the node containing the a specific data, and iterate over
    	// its children. If the path cannot be resolved, get_child throws.
    	// A C++11 for-range loop would also work.

	// world
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.world"))
	{
        	// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "x" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetWorldSizeX(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "y" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetWorldSizeY(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "z" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetWorldSizeZ(stod(child_node.second.data())*m);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n"); 
    	}

	// beam pipe
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.beampipe"))
	{
		// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "filename")
			SetBeampipeFileName(child_node.second.data());
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// ip beam pipe
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.ipbeampipe"))
	{
		// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "zmin" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetIpBeampipeZmin(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "zmax" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetIpBeampipeZmax(stod(child_node.second.data())*cm);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// extension
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.extension"))
	{
        	// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "l" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetExtL(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "r" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetExtR(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "e" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetExtE(stod(child_node.second.data())*cm);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n"); 
    	}

	// SR model
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.srmodel"))
	{
		// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "model")
			SetSRtype(stoi(child_node.second.data()));
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// gamma energy cut
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.genecut"))
	{
		// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "ecut" &&
			child_node.second.get<string>("<xmlattr>.unit") == "eV")
			SetGammaEcut(stod(child_node.second.data())*eV);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// tracking time cut
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.timecut"))
	{
		// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "tcut" &&
			child_node.second.get<string>("<xmlattr>.unit") == "s")
			SetTrackTcut(stod(child_node.second.data())*s);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// beam
	BOOST_FOREACH(ptree::value_type &child_node, tree.get_child("xml.beam"))
	{
        	// The data function is used to access the data stored in a node.
		if(child_node.second.get<string>("<xmlattr>.name") == "part" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamName(child_node.second.data());
		else if(child_node.second.get<string>("<xmlattr>.name") == "posx" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetBeamPosX(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "posy" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetBeamPosY(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "posz" &&
			child_node.second.get<string>("<xmlattr>.unit") == "cm")
			SetBeamPosZ(stod(child_node.second.data())*cm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "dirx" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamDirX(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "diry" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamDirY(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "dirz" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamDirZ(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "mom" &&
			child_node.second.get<string>("<xmlattr>.unit") == "GeV")
			SetBeamMom(stod(child_node.second.data())*GeV);
		else if(child_node.second.get<string>("<xmlattr>.name") == "spread" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamMomSpread(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "betax" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetBeamBetaX(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "betay" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetBeamBetaY(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "etax" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetBeamEtaX(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "etay" &&
			child_node.second.get<string>("<xmlattr>.unit") == "m")
			SetBeamEtaY(stod(child_node.second.data())*m);
		else if(child_node.second.get<string>("<xmlattr>.name") == "etapx" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamEtaPrimeX(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "etapy" &&
			child_node.second.get<string>("<xmlattr>.unit") == "")
			SetBeamEtaPrimeY(stod(child_node.second.data()));
		else if(child_node.second.get<string>("<xmlattr>.name") == "emitx" &&
			child_node.second.get<string>("<xmlattr>.unit") == "nm")
			SetBeamEmitX(stod(child_node.second.data())*nm);
		else if(child_node.second.get<string>("<xmlattr>.name") == "emity" &&
			child_node.second.get<string>("<xmlattr>.unit") == "nm")
			SetBeamEmitY(stod(child_node.second.data())*nm);
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n"); 
    	}

	// absorbers
	// clean vectors
	CleanAbsParameters();
	G4double absSizeX, absSizeY, absSizeZ, absPosX, absPosY, absPosZ;
	
	BOOST_FOREACH(ptree::value_type &child_tree, tree.get_child("xml.absorbers"))
	{
		if (child_tree.first == "absorber")
		{
			BOOST_FOREACH(ptree::value_type &child_node, child_tree.second)
			{
				if(child_node.second.get<string>("<xmlattr>.name") == "x" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absSizeX = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "y" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absSizeY = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "z" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absSizeZ = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "posx" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absPosX = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "posy" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absPosY = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "posz" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					absPosZ = stod(child_node.second.data())*cm; 
				else
					throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
			}
			// fill vectors with the new numbers
			SetAbsParameters(absSizeX,absSizeY,absSizeZ,absPosX,absPosY,absPosZ);
		}
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}

	// magnets
	// clean vectors
	CleanMagParameters();
	G4String magName, magType;
	G4double magLength, magStartPosX, magStartPosY, magStartPosZ, magStartDirX, magStartDirY, magStartDirZ;
	G4double magBx, magBy, magBz, magK1;

	BOOST_FOREACH(ptree::value_type &child_tree, tree.get_child("xml.magnets"))
	{
		if (child_tree.first == "magnet")
		{
			BOOST_FOREACH(ptree::value_type &child_node, child_tree.second)
			{
				if(child_node.second.get<string>("<xmlattr>.name") == "name")
					magName = child_node.second.data();
				else if(child_node.second.get<string>("<xmlattr>.name") == "type")
					magType = child_node.second.data();
				else if(child_node.second.get<string>("<xmlattr>.name") == "length" &&
					child_node.second.get<string>("<xmlattr>.unit") == "m")
					magLength = stod(child_node.second.data())*m; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "posx" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					magStartPosX = stod(child_node.second.data())*cm; 
				else if(child_node.second.get<string>("<xmlattr>.name") == "posy" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					magStartPosY = stod(child_node.second.data())*cm;
				else if(child_node.second.get<string>("<xmlattr>.name") == "posz" &&
					child_node.second.get<string>("<xmlattr>.unit") == "cm")
					magStartPosZ = stod(child_node.second.data())*cm;
				else if(child_node.second.get<string>("<xmlattr>.name") == "dirx" &&
					child_node.second.get<string>("<xmlattr>.unit") == "")
					magStartDirX = stod(child_node.second.data());
				else if(child_node.second.get<string>("<xmlattr>.name") == "diry" &&
					child_node.second.get<string>("<xmlattr>.unit") == "")
					magStartDirY = stod(child_node.second.data());
				else if(child_node.second.get<string>("<xmlattr>.name") == "dirz" &&
					child_node.second.get<string>("<xmlattr>.unit") == "")
					magStartDirZ = stod(child_node.second.data());
				else if(child_node.second.get<string>("<xmlattr>.name") == "bx" &&
					child_node.second.get<string>("<xmlattr>.unit") == "tesla")
					magBx = stod(child_node.second.data())*tesla;
				else if(child_node.second.get<string>("<xmlattr>.name") == "by" &&
					child_node.second.get<string>("<xmlattr>.unit") == "tesla")
					magBy = stod(child_node.second.data())*tesla;
				else if(child_node.second.get<string>("<xmlattr>.name") == "bz" &&
					child_node.second.get<string>("<xmlattr>.unit") == "tesla")
					magBz = stod(child_node.second.data())*tesla;
				else if(child_node.second.get<string>("<xmlattr>.name") == "k1" &&
					child_node.second.get<string>("<xmlattr>.unit") == "tesla/m")
					magK1 = stod(child_node.second.data())*tesla/m;
				else
					throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
			}
			// fill vectors with the new numbers
			SetMagParameters(magName,magType,magLength,magStartPosX,magStartPosY,magStartPosZ,
				magStartDirX,magStartDirY,magStartDirZ,magBx,magBy,magBz,magK1);
		}
		else
			throw runtime_error("[ERROR] SimParameters::ReadXML ==> Bad XML data\n");
	}
	
	return;
}

void SimParameters::SetMagParameters(G4String magName, G4String magType, G4double magLength, 
	G4double magStartPosX, G4double magStartPosY, G4double magStartPosZ,
	G4double magStartDirX, G4double magStartDirY, G4double magStartDirZ,
	G4double magBx, G4double magBy, G4double magBz, G4double magK1)
{
	_magName.push_back(magName);
	_magType.push_back(magType);
	_magLength.push_back(magLength);
	_magStartPosX.push_back(magStartPosX);
	_magStartPosY.push_back(magStartPosY);
	_magStartPosZ.push_back(magStartPosZ);
	_magStartDirX.push_back(magStartDirX);
	_magStartDirY.push_back(magStartDirY);
	_magStartDirZ.push_back(magStartDirZ);
	_magBx.push_back(magBx);
	_magBy.push_back(magBy);
	_magBz.push_back(magBz);
	_magK1.push_back(magK1);

	return;
}

void SimParameters::CleanMagParameters()
{
	_magName.clear();
	_magType.clear();
	_magLength.clear();
	_magStartPosX.clear();
	_magStartPosY.clear();
	_magStartPosZ.clear();
	_magStartDirX.clear();
	_magStartDirY.clear();
	_magStartDirZ.clear();
	_magBx.clear();
	_magBy.clear();
	_magBz.clear();
	_magK1.clear();

	return;
}

void SimParameters::SetAbsParameters(G4double absSizeX, G4double absSizeY, G4double absSizeZ,
	G4double absPosX, G4double absPosY, G4double absPosZ)
{
	_absSizeX.push_back(absSizeX);
	_absSizeY.push_back(absSizeY);
	_absSizeZ.push_back(absSizeZ);
	_absPosX.push_back(absPosX);
	_absPosY.push_back(absPosY);
	_absPosZ.push_back(absPosZ);

	return;
}

void SimParameters::CleanAbsParameters()  
{
	_absSizeX.clear();
	_absSizeY.clear();
	_absSizeZ.clear();
	_absPosX.clear();
	_absPosY.clear();
	_absPosZ.clear();

	return;
}

void SimParameters::InitDefault()
{
	_world_dim_x =  10.0*m;
    	_world_dim_y =  10.0*m;
    	_world_dim_z =  80.0*m;

	_beampipeFileName = "../geometry/beam_pipe_20231011_Zup.obj";

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
	_magK1 = {0,0,0,5.957841642*tesla/m,-12.98838344*tesla/m};

	
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

	_SRtype = 1;

	_gammaEcut = -1*keV;
	_trackTcut =  1*s;

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
	G4cout<<"Beam particles = "<<GetBeamName()<<G4endl;
	G4cout<<"Position X = "<<GetBeamPosX()/cm<<" [cm]"<<G4endl;
	G4cout<<"Position Y = "<<GetBeamPosY()/cm<<" [cm]"<<G4endl;
	G4cout<<"Position Z = "<<GetBeamPosZ()/cm<<" [cm]"<<G4endl;
	G4cout<<"Direction X = "<<GetBeamDirX()<<" []"<<G4endl;
	G4cout<<"Direction Y = "<<GetBeamDirY()<<" []"<<G4endl;
	G4cout<<"Direction Z = "<<GetBeamDirZ()<<" []"<<G4endl;
	G4cout<<"Momentum = "<<GetBeamMom()/GeV<<" [GeV]"<<G4endl;
	G4cout<<"Momentum spread = "<<GetBeamMomSpread()<<" []"<<G4endl;
	G4cout<<"Beta X = "<<GetBeamBetaX()/m<<" [m]"<<G4endl;
	G4cout<<"Beta Y = "<<GetBeamBetaY()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion X = "<<GetBeamEtaX()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion Y = "<<GetBeamEtaY()/m<<" [m]"<<G4endl;
	G4cout<<"Dispersion PX = "<<GetBeamEtaPrimeX()/m<<" []"<<G4endl;
	G4cout<<"Dispersion PY = "<<GetBeamEtaPrimeY()/m<<" []"<<G4endl;
	G4cout<<"Emittance X = "<<GetBeamEmitX()/nm<<" [nm]"<<G4endl;
	G4cout<<"Emittance Y = "<<GetBeamEmitY()/nm<<" [nm]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintMagParameters()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintMagParameters ==> Magnet parameters"<<G4endl;
	for(G4int iMag = 0; iMag < GetMagNum(); iMag++)
	{
		G4cout<<"ID = "<<iMag<<G4endl;
		G4cout<<"  Name = "<<GetMagName(iMag)<<G4endl;
		G4cout<<"  Type = "<<GetMagType(iMag)<<G4endl;
		G4cout<<"  Length = "<<GetMagLength(iMag)/m<<" [m]"<<G4endl;
		G4cout<<"  Start position X = "<<GetMagStartPosX(iMag)/cm<<" [cm]"<<G4endl;
		G4cout<<"  Start position Y = "<<GetMagStartPosY(iMag)/cm<<" [cm]"<<G4endl;
		G4cout<<"  Start position Z = "<<GetMagStartPosZ(iMag)/cm<<" [cm]"<<G4endl;
		G4cout<<"  Start direction X = "<<GetMagStartDirX(iMag)<<G4endl;
		G4cout<<"  Start direction Y = "<<GetMagStartDirY(iMag)<<G4endl;
		G4cout<<"  Start direction Z = "<<GetMagStartDirZ(iMag)<<G4endl;
		G4cout<<"  Field Bx = "<<GetMagBx(iMag)/tesla<<" [T]"<<G4endl;
		G4cout<<"  Field By = "<<GetMagBy(iMag)/tesla<<" [T]"<<G4endl;
		G4cout<<"  Field Bz = "<<GetMagBz(iMag)/tesla<<" [T]"<<G4endl;
		G4cout<<"  Field K1 = "<<GetMagK1(iMag)/(tesla/m)<<" [T/m)]"<<G4endl;
	}
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintAbsParameters()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintAbsParameters ==> Absorber parameters"<<G4endl;
	for(G4int iAbs = 0; iAbs < GetAbsNum(); iAbs++)
	{
		G4cout<<"ID = "<<iAbs<<G4endl;
		G4cout<<"  Size X = "<<GetAbsSizeX(iAbs)/cm<<" [cm]"<<G4endl; 
		G4cout<<"  Size Y = "<<GetAbsSizeY(iAbs)/cm<<" [cm]"<<G4endl; 
		G4cout<<"  Size Z = "<<GetAbsSizeZ(iAbs)/cm<<" [cm]"<<G4endl; 
		G4cout<<"  Position X = "<<GetAbsPosX(iAbs)/cm<<" [cm]"<<G4endl; 
		G4cout<<"  Position Y = "<<GetAbsPosY(iAbs)/cm<<" [cm]"<<G4endl; 
		G4cout<<"  Position Z = "<<GetAbsPosZ(iAbs)/cm<<" [cm]"<<G4endl; 
	}
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintSRtype()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintSRtype ==> "<<GetSRtype()<<G4endl;
	G4cout<<" 1 - G4SynchrotronRadiation"<<G4endl;
	G4cout<<" 2 - G4SynchrotronRadiationInMat"<<G4endl;
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

void SimParameters::PrintTrackTcut()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintTrackTcut ==> Tcut = "<<GetTrackTcut()/s<<" [s]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

void SimParameters::PrintBeampipeFileName()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintBeampipeFileName() ==> "<<GetBeampipeFileName()<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

G4bool SimParameters::FileExists(const G4String& fName) 
{
  	struct stat buffer;   
  	return (stat (fName.c_str(), &buffer) == 0); 
}

void SimParameters::PrintIpBeampipeParameters()
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] SimParameters::PrintIpBeampipeParameters ==> IP beam pipe parameters"<<G4endl;
	G4cout<<"FWD Z (zmin) = "<<GetIpBeampipeZmin()/m<<" [m]"<<G4endl;
	G4cout<<"BWD Z (zmax) = "<<GetIpBeampipeZmax()/m<<" [m]"<<G4endl;
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}
