void getAllHitsPerFile() 
{
	const string inputfile  = "./output/dataprod_rel_1.0.0_synrad_18x275_run001.preproc.hepmc3.tree.podio.root";
	const double dtime = 2e-6 * 10000./28.; // [sec] - time window

	cout<<"--> inputfile = "<<inputfile<<endl;
	
	const Int_t nDet = 3;
	Int_t nSimHitSum[nDet];
	Int_t nRawHitSum[nDet];
	Int_t nRecHitSum[nDet];
	for(Int_t i = 0; i < nDet; i++)
	{
		nSimHitSum[i] = 0;
		nRawHitSum[i] = 0;
		nRecHitSum[i] = 0;
	}

	const TString nSimHitsArray[] = {"SiBarrelHits","TrackerEndcapHits","VertexBarrelHits"};
	const TString nRawHitsArray[] = {"SiBarrelRawHits","SiEndcapTrackerRawHits","SiBarrelVertexRawHits"};
	const TString nRecHitsArray[] = {"SiBarrelTrackerRecHits","SiEndcapTrackerRecHits","SiBarrelVertexRecHits"};
	//-----------------------------------------------------------//
	TFile* file = new TFile(inputfile.c_str());
	if(!file) return;
	TTree *tree = nullptr;
	file->GetObject("events", tree);
	delete tree;

	TTreeReader myReader("events", file);
	
	//- SimHits
	TTreeReaderArray<unsigned long> var_SiBarrelHits(myReader, "SiBarrelHits.cellID");
	TTreeReaderArray<unsigned long> var_TrackerEndcapHits(myReader, "TrackerEndcapHits.cellID");
	TTreeReaderArray<unsigned long> var_VertexBarrelHits(myReader, "VertexBarrelHits.cellID");

	//- RawHits 
	TTreeReaderArray<Int_t> var_SiBarrelRawHits(myReader, "SiBarrelRawHits.charge");
	TTreeReaderArray<Int_t> var_SiEndcapTrackerRawHits(myReader, "SiEndcapTrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_SiBarrelVertexRawHits(myReader, "SiBarrelVertexRawHits.charge");

	//- RecHits
	TTreeReaderArray<Float_t> var_SiBarrelTrackerRecHits(myReader, "SiBarrelTrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_SiEndcapTrackerRecHits(myReader, "SiEndcapTrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_SiBarrelVertexRecHits(myReader, "SiBarrelVertexRecHits.edep");
	//-----------------------------------------------------------//
	// loop over events
	while(myReader.Next())
	{
		//- SimHits
		nSimHitSum[0] += var_SiBarrelHits.GetSize();
		nSimHitSum[1] += var_TrackerEndcapHits.GetSize();
		nSimHitSum[2] += var_VertexBarrelHits.GetSize();

		//- RawHits
		nRawHitSum[0] += var_SiBarrelRawHits.GetSize();
		nRawHitSum[1] += var_SiEndcapTrackerRawHits.GetSize();
		nRawHitSum[2] += var_SiBarrelVertexRawHits.GetSize();

		//- RecHits
		nRecHitSum[0] += var_SiBarrelTrackerRecHits.GetSize();
		nRecHitSum[1] += var_SiEndcapTrackerRecHits.GetSize();
		nRecHitSum[2] += var_SiBarrelVertexRecHits.GetSize();
	}
	file->Close();

	TH1D* hSimHits = new TH1D("hSimHits","Simulation Hits;Sub-detector;SimHits [counts/file]",nDet,0,nDet);
	TH1D* hRawHits = new TH1D("hRawHits","Digitalization Hits;Sub-detector;RawHits [counts/file]",nDet,0,nDet);
	TH1D* hRecHits = new TH1D("hRecHits","Reconstruction Hits;Sub-detector;RecHits [counts/file]",nDet,0,nDet);
	
	//- fill histograms
	for(Int_t i = 0; i < nDet; i++)
	{
		//- SimHits
		hSimHits->SetBinContent(i+1,nSimHitSum[i]);
		hSimHits->SetBinError(i+1,TMath::Sqrt(nSimHitSum[i]));
		hSimHits->GetXaxis()->SetBinLabel(i+1,nSimHitsArray[i].Data());

		//- RawHits
		hRawHits->SetBinContent(i+1,nRawHitSum[i]);
		hRawHits->SetBinError(i+1,TMath::Sqrt(nRawHitSum[i]));
		hRawHits->GetXaxis()->SetBinLabel(i+1,nRawHitsArray[i].Data());

		//- RecHits
		hRecHits->SetBinContent(i+1,nRecHitSum[i]);
		hRecHits->SetBinError(i+1,TMath::Sqrt(nRecHitSum[i]));
		hRecHits->GetXaxis()->SetBinLabel(i+1,nRecHitsArray[i].Data());
	}
	
	hSimHits->Scale(1.0/dtime);
	hRawHits->Scale(1.0/dtime);
	hRecHits->Scale(1.0/dtime);

	gStyle->SetOptStat(0);
	gStyle->SetTitleFontSize(0.05); 

	TCanvas* c1 = new TCanvas("c1","c1",1800,900);
	c1->Divide(3,1);

	c1->cd(1);
	TPad* p1 = new TPad("p1","p1",0,0,1,1);
   	p1->SetTopMargin(0.1);
	p1->SetBottomMargin(0.3);
	p1->SetLeftMargin(0.1);
	p1->SetRightMargin(0.1);
	p1->Draw();
	p1->cd();

	hSimHits->GetYaxis()->SetTitle("Rate [Hz]");
	hSimHits->SetTitle("SimHits");
	hSimHits->SetLineWidth(3);
	hSimHits->SetLineColor(kBlack);
	hSimHits->SetFillStyle(3001);
	hSimHits->GetXaxis()->SetTitle("");
	hSimHits->SetFillColorAlpha(kBlue,0.2);
	hSimHits->GetYaxis()->SetLabelSize(0.06);
	hSimHits->GetXaxis()->SetLabelSize(0.06);
	hSimHits->GetYaxis()->SetTitleSize(0.04);
	hSimHits->GetXaxis()->SetTitleSize(0.04);
	hSimHits->GetYaxis()->SetTitleOffset(1);
	hSimHits->GetXaxis()->LabelsOption("v");
	hSimHits->GetYaxis()->SetNdivisions(5);
	hSimHits->SetMinimum(1e6);
	hSimHits->SetMaximum(1e10);

	hSimHits->Draw("HIST & E");
	gPad->SetLogy();
	gPad->SetGrid();

	c1->cd(2);
	TPad* p2 = new TPad("p2","p2",0,0,1,1);
   	p2->SetTopMargin(0.1);
	p2->SetBottomMargin(0.3);
	p2->SetLeftMargin(0.1);
	p2->SetRightMargin(0.1);
	p2->Draw();
	p2->cd();

	hRawHits->GetYaxis()->SetTitle("Rate [Hz]");
	hRawHits->SetTitle("RawHits");
	hRawHits->SetLineWidth(3);
	hRawHits->SetLineColor(kBlack);
	hRawHits->SetFillStyle(3001);
	hRawHits->GetXaxis()->SetTitle("");
	hRawHits->SetFillColorAlpha(kBlue,0.2);
	hRawHits->GetYaxis()->SetLabelSize(0.06);
	hRawHits->GetXaxis()->SetLabelSize(0.06);
	hRawHits->GetYaxis()->SetTitleSize(0.04);
	hRawHits->GetXaxis()->SetTitleSize(0.04);
	hRawHits->GetYaxis()->SetTitleOffset(1);
	hRawHits->GetXaxis()->LabelsOption("v");
	hRawHits->GetYaxis()->SetNdivisions(5);
	hRawHits->SetMinimum(1e6);
	hRawHits->SetMaximum(1e10);

	hRawHits->Draw("HIST & E");
	gPad->SetLogy();
	gPad->SetGrid();

	c1->cd(3);
	TPad* p3 = new TPad("p3","p3",0,0,1,1);
   	p3->SetTopMargin(0.1);
	p3->SetBottomMargin(0.3);
	p3->SetLeftMargin(0.1);
	p3->SetRightMargin(0.1);
	p3->Draw();
	p3->cd();

	hRecHits->GetYaxis()->SetTitle("Rate [Hz]");
	hRecHits->SetTitle("RecHits");
	hRecHits->SetLineWidth(3);
	hRecHits->SetLineColor(kBlack);
	hRecHits->SetFillStyle(3001);
	hRecHits->GetXaxis()->SetTitle("");
	hRecHits->SetFillColorAlpha(kBlue,0.2);
	hRecHits->GetYaxis()->SetLabelSize(0.06);
	hRecHits->GetXaxis()->SetLabelSize(0.06);
	hRecHits->GetYaxis()->SetTitleSize(0.04);
	hRecHits->GetXaxis()->SetTitleSize(0.04);
	hRecHits->GetYaxis()->SetTitleOffset(1);
	hRecHits->GetXaxis()->LabelsOption("v");
	hRecHits->GetYaxis()->SetNdivisions(5);
	hRecHits->SetMinimum(1e6);
	hRecHits->SetMaximum(1e10);

	hRecHits->Draw("HIST & E");
	gPad->SetLogy();
	gPad->SetGrid();

	c1->SaveAs("c1.png");

	return;
}
