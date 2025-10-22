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
	TH1D* h2[nFiles];
	//____________________________________________________________
	const TString mainDir = "/gpfs02/eic/anatochii/SynradG4_HepMC_Files_SR_on_IP6/data/synrad/dataprod_rel_1.0.0";

	const TString fName[nFiles] = {
		mainDir + "/18x275/dataprod_rel_1.0.0_synrad_18x275_run001.hepmc3.tree.root",
		mainDir + "/5x100/dataprod_rel_1.0.0_synrad_5x100_run001.preproc_10000repeats.hepmc3.tree.root",
		mainDir + "/10x275/dataprod_rel_1.0.0_synrad_10x275_run001.preproc_10000repeats.hepmc3.tree.root",
		mainDir + "/18x275/dataprod_rel_1.0.0_synrad_18x275_run001.preproc_10000repeats.hepmc3.tree.root"
	};
	const TString sample[nFiles] = {
		"In-Pipe:  18 GeV",
		"Out-Pipe:  5 GeV",
		"Out-Pipe: 10 GeV",
		"Out-Pipe: 18 GeV"
	};
	const Double_t normFactor[nFiles] = {
		0.227/(TMath::Qe()*1e11),
		1./2e-6 * 28./10000.0 * 2.5/0.277,
		1./2e-6 * 28./10000.0 * 2.5/0.277,
		1./2e-6 * 28./10000.0
	};

	for(Int_t iFile = 0; iFile < nFiles; iFile++)
	{
		h1[iFile] = new TH1D(Form("h1_%d",iFile), "Spectrum;Energy [GeV];Counts", nBins, binEdges);
		h2[iFile] = new TH1D(Form("h2_%d",iFile), "SynRad Outside the Beam Pipe;Z [cm];Counts", 1000, -150, 100);

		cout<<"[INFO] fName = "<<fName[iFile]<<endl;
		TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fName[iFile].Data());
		if (!f || !f->IsOpen()) 
		{
			f = new TFile(fName[iFile].Data());
		}
		TTree* tree;
		f->GetObject("hepmc3_tree",tree);

		static constexpr Int_t kMaxParticles = 2;
		static constexpr Int_t kMaxVerticies = 1;
		Double_t particles_momentum_m_v4[kMaxParticles];   //[particles_]
		Double_t vertices_position_m_v3[kMaxVerticies];   //[vertices_]
		TBranch *b_particles_momentum_m_v4;   //!
		TBranch *b_vertices_position_m_v3;   //!
		tree->SetBranchAddress("particles.momentum.m_v4", particles_momentum_m_v4, &b_particles_momentum_m_v4);
		tree->SetBranchAddress("vertices.position.m_v3", vertices_position_m_v3, &b_vertices_position_m_v3);
		Long64_t nEntries = tree->GetEntries();
		cout<<"[INFO] nEntries = "<<nEntries<<endl;
		
		for(Long64_t iEntry = 0; iEntry < nEntries; iEntry++)
		{
			tree->GetEntry(iEntry);
			h1[iFile]->Fill(particles_momentum_m_v4[1] * 1e6); // GeV -> keV
			h2[iFile]->Fill(vertices_position_m_v3[0] * 1e-1); // mm -> cm
		}

		f->Close();
	}

	gStyle->SetOptStat(0);
	gStyle->SetTitleFontSize(0.05);

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

	TLegend* leg1 = new TLegend(0.6,0.7,0.95,0.95);

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

		leg1->AddEntry(
			h1[iFile],
			Form("%s: N_{part.} = %.f",
				sample[iFile].Data(),
				h1[iFile]->GetEntries()),
			"fe");
	}
	leg1->Draw();
	gPad->SetLogx();
	gPad->SetLogy();
	c1->SaveAs("c1_spectrum.png");

	TCanvas* c2 = new TCanvas("c2","c2",1800,450);
	c2->cd();
	TPad* p2 = new TPad("p2","p2",0,0,1,1);
	p2->SetTopMargin(0.1);
	p2->SetBottomMargin(0.16);
	p2->SetLeftMargin(0.10);
	p2->SetRightMargin(0.12);
	p2->Draw();
	p2->cd();

	TLegend* leg2 = new TLegend(0.15,0.65,0.45,0.98);

	for(Int_t iFile = 1; iFile < nFiles; iFile++)
	{
		h2[iFile]->Scale(normFactor[iFile]);
		h2[iFile]->Scale(1e-6); // Hz -> MHz
		h2[iFile]->SetLineColor(color[iFile]);
		h2[iFile]->SetLineWidth(3);
		h2[iFile]->SetFillStyle(style[iFile]);
		h2[iFile]->SetFillColorAlpha(color[iFile],0.2);
		h2[iFile]->SetTitle("");
		h2[iFile]->SetMinimum(1e-2);
		h2[iFile]->SetMaximum(1e4);
		h2[iFile]->GetXaxis()->SetTitle("Z [cm]");
		h2[iFile]->GetYaxis()->SetTitle(Form("Rate [MHz/%.1fcm]",h2[iFile]->GetXaxis()->GetBinWidth(1)));
		h2[iFile]->GetYaxis()->SetLabelSize(0.06);
		h2[iFile]->GetXaxis()->SetLabelSize(0.06);
		h2[iFile]->GetYaxis()->SetTitleSize(0.06);
		h2[iFile]->GetXaxis()->SetTitleSize(0.06);
		h2[iFile]->GetYaxis()->SetTitleOffset(0.6);
		h2[iFile]->GetYaxis()->SetNdivisions(6);

		if(iFile == 1){h2[iFile]->Draw("HIST & E");}
		else{h2[iFile]->Draw("SAME & HIST & E");}

		if(iFile == 1)leg2->AddEntry(h2[iFile],"5x100 GeV^{2} @ 2.500 A","fe");
		if(iFile == 2)leg2->AddEntry(h2[iFile],"10x275 GeV^{2} @ 2.500 A","fe");
		if(iFile == 3)leg2->AddEntry(h2[iFile],"18x275 GeV^{2} @ 0.277 A","fe");
	}
	leg2->Draw();
	gPad->SetLogy();
	gPad->SetGrid();
	c2->SaveAs("c2_hits.png");

	return;
}
