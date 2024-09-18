#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TVector3.h>

#include "vector"
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/ReaderAscii.h"

int main(int argc, char* argv[])
{
	string fNameHepMC = argv[1];
	cout<<"[INFO] Input file name: "<<fNameHepMC<<endl;
	TString fNameOutput = fNameHepMC + ".root";

	HepMC3::GenEvent event(HepMC3::Units::GEV,HepMC3::Units::MM);
	HepMC3::ReaderAscii reader(fNameHepMC);

	Int_t pdg;
	Double_t posx, posy, posz, momx, momy, momz;

	auto file = new TFile(fNameOutput.Data(),"recreate");
	file->cd();
	auto tree = new TTree("tree","HepMC3 SR data");	
	tree->Branch("pdg",	&pdg);
	tree->Branch("posx",	&posx);
	tree->Branch("posy",	&posy);
	tree->Branch("posz",	&posz);
	tree->Branch("momx",	&momx);
	tree->Branch("momy",	&momy);
	tree->Branch("momz",	&momz);

	while (!reader.failed())
	{
		reader.read_event(event);
		if (reader.failed()) break;
		for (const auto& vertex : event.vertices())
		{
			for(const auto& particle : vertex->particles_out())
			{
				pdg = particle->pid();
				HepMC3::FourVector pos = vertex->position();
				HepMC3::FourVector mom = particle->momentum();
				posx = pos.x();
				posy = pos.y();
				posz = pos.z();
				momx = mom.x();
				momy = mom.y();
				momz = mom.z();
				tree->Fill();
			}
		}
		event.clear();
	}
	cout<<"[INFO] Number of events: "<<tree->GetEntries()<<endl;

	file->cd();
	tree->Write();
	delete file;

	reader.close();

	cout<<"[INFO] Output file name: "<<fNameOutput<<endl;

	return 0;
}
