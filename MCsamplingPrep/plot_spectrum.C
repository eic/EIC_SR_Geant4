void plot_spectrum()
{
	const int nBins = 400;
    	double binEdges[nBins + 1];

    	double logMin = -1;
    	double logMax = +3;

    	for (int i = 0; i <= nBins; ++i) 
	{
        	binEdges[i] = TMath::Power(10, logMin + i * (logMax - logMin) / nBins);
    	}

	const Int_t nFiles = 4;
	TH1D* h1[nFiles];
	//____________________________________________________________
	const TString mainDir = "/gpfs02/eic/anatochii/SynradG4_HepMC_Files_SR_on_IP6/data/synrad/dataprod_rel_1.0.0";

	const TString fName[nFiles] = {
		mainDir + "/18x275/dataprod_rel_1.0.0_synrad_18x275_run001.hepmc3.tree.root",
		mainDir + "/18x275/dataprod_rel_1.0.0_synrad_18x275_run001.preproc_10000repeats.hepmc3.tree.root",
		mainDir + "/10x275/dataprod_rel_1.0.0_synrad_10x275_run001.preproc_10000repeats.hepmc3.tree.root",
		mainDir + "/5x100/dataprod_rel_1.0.0_synrad_5x100_run001.preproc_10000repeats.hepmc3.tree.root",
	};
	const TString sample[nFiles] = {
		"In-Pipe:  18 GeV",
		"Out-Pipe: 18 GeV",
		"Out-Pipe: 10 GeV",
		"Out-Pipe:  5 GeV"
	};
	const Double_t normFactor[nFiles] = {
		0.227/(TMath::Qe()*1e11),
		1./2e-6 * 28./10000.0,
		1./2e-6 * 28./10000.0 * 2.5/0.277,
		1./2e-6 * 28./10000.0 * 2.5/0.277
	};

	for(Int_t iFile = 0; iFile < nFiles; iFile++)
	{
		h1[iFile] = new TH1D(Form("h1_%d",iFile), "Spectrum;Energy [GeV];Counts", nBins, binEdges);

		cout<<"[INFO] fName = "<<fName[iFile]<<endl;
		TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fName[iFile].Data());
		if (!f || !f->IsOpen()) 
		{
			f = new TFile(fName[iFile].Data());
		}
		TTree* tree;
		f->GetObject("hepmc3_tree",tree);

		static constexpr Int_t kMaxparticles = 2;
		Double_t particles_momentum_m_v4[kMaxparticles];   //[particles_]
		TBranch *b_particles_momentum_m_v4;   //!
		tree->SetBranchAddress("particles.momentum.m_v4", particles_momentum_m_v4, &b_particles_momentum_m_v4);
		Long64_t nEntries = tree->GetEntries();
		cout<<"[INFO] nEntries = "<<nEntries<<endl;
		
		for(Long64_t iEntry = 0; iEntry < nEntries; iEntry++)
		{
			tree->GetEntry(iEntry);
			h1[iFile]->Fill(particles_momentum_m_v4[1] * 1e6); // GeV -> keV
		}

		f->Close();
	}

	gStyle->SetOptStat(0);
	gStyle->SetTitleFontSize(0.08);

	const Int_t color[] = {1,2,4,6};
	const Int_t style[] = {0,1,3345,3354};
	TCanvas* c1 = new TCanvas("c1","c1",1800,900);
	c1->cd();
	TPad* p1 = new TPad("p1","p1",0,0,1,1);
	p1->SetTopMargin(0.1);
	p1->SetBottomMargin(0.1);
	p1->SetLeftMargin(0.1);
	p1->SetRightMargin(0.1);
	p1->Draw();
	p1->cd();

	TLegend* leg = new TLegend(0.6,0.7,0.95,0.95);

	for(Int_t iFile = 0; iFile < nFiles; iFile++)
	{
		h1[iFile]->Scale(normFactor[iFile]);
		h1[iFile]->SetLineColor(color[iFile]);
		h1[iFile]->SetLineWidth(3);
		h1[iFile]->SetFillStyle(style[iFile]);
		h1[iFile]->SetFillColorAlpha(color[iFile],0.2);
		h1[iFile]->SetMinimum(1e3);
		h1[iFile]->SetMaximum(1e12);
		h1[iFile]->GetXaxis()->SetTitle("Total Energy [keV]");
		h1[iFile]->GetYaxis()->SetTitle("Rate [Hz]");

		if(iFile == 0){h1[iFile]->Draw("HIST & E");}
		else{h1[iFile]->Draw("SAME & HIST & E");}

		leg->AddEntry(
			h1[iFile],
			Form("%s: N_{part.} = %.f",
				sample[iFile].Data(),
				h1[iFile]->GetEntries()),
			"fe");
	}
	leg->Draw();
	gPad->SetLogx();
	gPad->SetLogy();
	c1->SaveAs("c1_spectrum.png");

	return;
}
