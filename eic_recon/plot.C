// hadd -f /group/belle2/TMP/anatochi/eic/output_eicrecon_hist_all.root /group/belle2/TMP/anatochi/eic/output_eicrecon/output_eicrecon_*.hist.root
// hadd -f /group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_hist_all.root /group/belle2/TMP/anatochi/eic/output_eicrecon_ip6/output_eicrecon_*.hist.root

void plot()
{
	TString fName = "/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_hist_all.root";
	TFile* file = TFile::Open(fName.Data());
	TDirectory* dir = file->GetDirectory("analysisPlugin");
	dir->ls();
	TNtuple* ntraw_nHits = (TNtuple*)dir->Get("ntraw_nHits");	
	TNtuple* ntraw_VertexBarrelHits = (TNtuple*)dir->Get("ntraw_VertexBarrelHits");

	Int_t nEntries_nHits = ntraw_nHits->GetEntries();
	Int_t nEntries_VertexBarrelHits = ntraw_VertexBarrelHits->GetEntries();

	cout<<"nEntries_nHits = "<<nEntries_nHits<<endl;
	cout<<"nEntries_VertexBarrelHits = "<<nEntries_VertexBarrelHits<<endl;

	const Int_t bNum = 11;
	const Double_t time = 1; // [sec]
	TString bName[bNum] = {	"TrackerEndcapHits","HcalEndcapNHits","EcalEndcapNHits",
				"VertexBarrelHits","TOFBarrelHits","SiBarrelHits",
				"MPGDBarrelHits","EcalEndcapPHits","B0TrackerHits",
				"HcalBarrelHits","LumiSpecCALHits"};
	Float_t nHits[bNum], nHitsSum[bNum];
	Float_t x_VertexBarrelHits, y_VertexBarrelHits, z_VertexBarrelHits;

	for(Int_t i = 0; i < bNum; i++)
	{
		ntraw_nHits->SetBranchAddress(Form("nHits_%s",bName[i].Data()),&nHits[i]);
		nHitsSum[i] = 0;
	}
	ntraw_VertexBarrelHits->SetBranchAddress("x",&x_VertexBarrelHits);
	ntraw_VertexBarrelHits->SetBranchAddress("y",&y_VertexBarrelHits);
	ntraw_VertexBarrelHits->SetBranchAddress("z",&z_VertexBarrelHits);

	TH1D* hHits = new TH1D("hHits","Hit frequency for different sub-detectors (t_{int.} = 100 nsec | 8000 Events);Subdetector;Hits freq. [Hz]",bNum,0,bNum);
	TH2D* hVertexBarrelHits = new TH2D("hVertexBarrelHits","Scatter plot of SR hits in VertexBarrel (t_{int.} = 100 nsec | 8000 Events);x [mm];y [mm]",240,-120,120,240,-120,120);


	for(Int_t i = 0; i < nEntries_nHits; i++)
	{
		ntraw_nHits->GetEntry(i);
		for(Int_t j = 0; j < bNum; j++)
		{
			nHitsSum[j] += nHits[j];
		}
	}

	for(Int_t i = 0; i < bNum; i++)
	{
		hHits->SetBinContent(i+1,nHitsSum[i]/(time*nEntries_nHits));
		hHits->SetBinError(i+1,TMath::Sqrt(nHitsSum[i])/(time*nEntries_nHits));
		hHits->GetXaxis()->SetBinLabel(i+1,bName[i].Data());
	}

	for(Int_t i = 0; i < nEntries_VertexBarrelHits; i++)
	{
		ntraw_VertexBarrelHits->GetEntry(i);
		hVertexBarrelHits->Fill(x_VertexBarrelHits,y_VertexBarrelHits);
	}

	gStyle->SetErrorX(0);
	gStyle->SetOptStat(0);

	TCanvas* c1 = new TCanvas("c1","c1",1800,600);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
	pad1->SetBottomMargin(0.2);
	pad1->SetTopMargin(0.12);
	pad1->SetRightMargin(0.12);
	pad1->SetLeftMargin(0.12);
	pad1->Draw();
	pad1->cd();
	hHits->Draw("bar1 & e");
	hHits->SetFillColorAlpha(kBlue,0.4);
	hHits->SetLineWidth(2);
	hHits->SetLineColor(kBlack);
	hHits->SetBarWidth(0.8);
//	hHits->SetMinimum(1e1);
	hHits->GetYaxis()->SetTitleOffset(1.2);
	hHits->GetXaxis()->SetTitleOffset(3.0);
	hHits->GetXaxis()->CenterTitle(true);
	hHits->GetYaxis()->CenterTitle(true);
	hHits->GetXaxis()->SetLabelSize(0.05);
	hHits->GetXaxis()->SetLabelOffset(0.02);
//	hHits->LabelsOption("v");
	gPad->SetLogy();

	TCanvas* c2 = new TCanvas("c2","c2",600,600);
	c2->cd();
	TPad *pad2 = new TPad("pad2","pad2",0,0,1,1);
	pad2->SetBottomMargin(0.12);
	pad2->SetTopMargin(0.12);
	pad2->SetRightMargin(0.12);
	pad2->SetLeftMargin(0.12);
	pad2->Draw();
	pad2->cd();
	hVertexBarrelHits->SetMarkerStyle(21);
	hVertexBarrelHits->Draw();
	hVertexBarrelHits->GetYaxis()->SetTitleOffset(1.4);
	hVertexBarrelHits->GetXaxis()->SetTitleOffset(1.4);
	hVertexBarrelHits->GetYaxis()->CenterTitle(true);
	hVertexBarrelHits->GetXaxis()->CenterTitle(true);

	Double_t VertexBarrel_rmin[3] = {36.0-2.0/2.0,
					36.0+12.0-2.0/2.0,
					120.0-2.0/2.0}; // [mm]
	for(Int_t i = 0; i < 3; i++)
	{
		TArc *arc = new TArc(0,0,VertexBarrel_rmin[i]);
   		arc->SetLineColor(kRed);
   		arc->SetLineWidth(4);
   		arc->SetLineStyle(9);
   		arc->SetLineColorAlpha(kRed,0.4);
   		arc->SetFillStyle(0);
   		arc->Draw();
	}

	return;
}
