
// hadd -f /gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_podio_all.root /gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6/output_eicrecon_*.podio.root

#define MINX -400 // [cm]
#define MAXX  400 // [cm]
#define MINY -400 // [cm]
#define MAXY  400 // [cm]
#define MINZ -800 // [cm]
#define MAXZ  800 // [cm]

void plot()
{	
	cout<<"Loading ..."<<endl;
	
	TH2F* hx0_xz = new TH2F("hx0_xz","XZ;Z [cm];X [cm]",1000,MINZ,MAXZ,1000,MINX,MAXX);
	TH2F* hx0_yz = new TH2F("hx0_yz","YZ;Z [cm];Y [cm]",1000,MINZ,MAXZ,1000,MINY,MAXY);
	TH2F* hx0_yx = new TH2F("hx0_yx","YX;X [cm];Y [cm]",1000,MINX,MAXX,1000,MINY,MAXY);

	TH2F* hmc_xz = new TH2F("hmc_xz","MC XZ;Z [cm];X [cm]",1000,MINZ,MAXZ,1000,MINX,MAXX);
	TH2F* hmc_yz = new TH2F("hmc_yz","MC YZ;Z [cm];Y [cm]",1000,MINZ,MAXZ,1000,MINY,MAXY);
	TH2F* hmc_yx = new TH2F("hmc_yx","MC YX;X [cm];Y [cm]",1000,MINX,MAXX,1000,MINY,MAXY);
	//-----------------------------------------------------------//
	//----------------------- MCparticles -----------------------//
	//-----------------------------------------------------------//
/*
	TString eicreconFileName = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_podio_all.root";
	TFile* file = new TFile(eicreconFileName.Data());
	TTreeReader myReader("events", file);
	TTreeReaderArray<Int_t> 	pdg_mc(myReader,"MCParticles.PDG");
	TTreeReaderArray<Int_t>		generatorStatus_mc(myReader,"MCParticles.generatorStatus");
	TTreeReaderArray<Int_t>		simulatorStatus_mc(myReader,"MCParticles.simulatorStatus");
	TTreeReaderArray<Float_t>	charge_mc(myReader,"MCParticles.charge");
	TTreeReaderArray<Float_t>	time_mc(myReader,"MCParticles.time");
	TTreeReaderArray<Double_t>	mass_mc(myReader,"MCParticles.mass");
	TTreeReaderArray<Double_t> 	vx_mc(myReader, "MCParticles.vertex.x");
	TTreeReaderArray<Double_t> 	vy_mc(myReader, "MCParticles.vertex.y");
	TTreeReaderArray<Double_t> 	vz_mc(myReader, "MCParticles.vertex.z");

	cout<<"MC particles calculation ..."<<endl;
	while(myReader.Next())
	{
		for (Int_t imcPart = 0; imcPart < pdg_mc.GetSize(); imcPart++)
		{
			if(simulatorStatus_mc[imcPart])
			{
				hmc_xz->Fill(vz_mc[imcPart]/1e1,vx_mc[imcPart]/1e1); // mm -> cm
				hmc_yz->Fill(vz_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
				hmc_yx->Fill(vx_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
			}
		}
	}
	file->Close();
*/
	//-----------------------------------------------------------//
	//---------------------- Material Scan ----------------------//
	//-----------------------------------------------------------//
	TString g4matFileName = "/gpfs/group/belle2/TMP/anatochi/eic/output_matscan/g4mat_scan_*.root";

	// read the root file with material scan results
	TChain* fChain = new TChain("tree");
	fChain->Add(g4matFileName.Data());
	fChain->ls();

	Float_t x0, lambda, posx, posy, posz;
	fChain->SetBranchAddress("x0", 		&x0);
	fChain->SetBranchAddress("lambda", 	&lambda);
	fChain->SetBranchAddress("posx", 	&posx);
	fChain->SetBranchAddress("posy", 	&posy);
	fChain->SetBranchAddress("posz", 	&posz);

	Long64_t nEntries = fChain->GetEntries();
	cout<<"nEntries = "<<nEntries<<endl;

	cout<<"Material scan building ..."<<endl;
	Int_t binx, biny;
	Float_t binc;
	Long64_t jEntry = 0;
	while(jEntry < nEntries)
	{
		fChain->GetEntry(jEntry);

                if(jEntry%100000 == 0)
                {
                        printf("\r[INFO] Progress: %.3f [%%]",100.0*jEntry/nEntries);
                        fflush(stdout);
                }
                jEntry++;

		// XZ
		binx = hx0_xz->GetXaxis()->FindBin(posz);
		biny = hx0_xz->GetYaxis()->FindBin(posx);
		binc = hx0_xz->GetBinContent(binx,biny);
		hx0_xz->SetBinContent(binx,biny,(binc > 0) ? (binc+x0)/2. : (x0));

		// YZ
		binx = hx0_yz->GetXaxis()->FindBin(posz);
		biny = hx0_yz->GetYaxis()->FindBin(posy);
		binc = hx0_yz->GetBinContent(binx,biny);
		hx0_yz->SetBinContent(binx,biny,(binc > 0) ? (binc+x0)/2. : (x0));

		// YX
		binx = hx0_yx->GetXaxis()->FindBin(posx);
		biny = hx0_yx->GetYaxis()->FindBin(posy);
		binc = hx0_yx->GetBinContent(binx,biny);
		hx0_yx->SetBinContent(binx,biny,(binc > 0) ? (binc+x0)/2. : (x0));
	}
        printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jEntry/nEntries);
	delete fChain->GetCurrentFile();
	//-----------------------------------------------------------//
	//-----------------------------------------------------------//
	//-----------------------------------------------------------//

	cout<<"Writing to a file ..."<<endl;
	TFile* f = new TFile("g4mat_scan_histo.root","recreate");
	f->cd();
	hmc_xz->Write();
	hmc_yz->Write();
	hmc_yx->Write();
	hx0_xz->Write();
	hx0_yz->Write();
	hx0_yx->Write();
	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}
