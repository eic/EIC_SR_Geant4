/*********************************************************************************************************************/
/*********************************************************************************************************************/
/* Plot reconstructed detector hits and vertex detector hit-map                                                      */
/* One should merge all output ROOT files into one using the 'hadd' command:                                         */
/* hadd -f /path/output_eicrecon_hist_all.root /path/output_eicrecon/output_eicrecon_*.hist.root                     */
/*********************************************************************************************************************/
/*********************************************************************************************************************/

const Double_t BeamCurrent_A = 0.227; // [A]
const Double_t nElectrons_geant4 = 1e8;

Double_t getBeamCoreTailRatio(TString type = "core")
{
	const Double_t Kx = 0.0;
	const Double_t Ky = 25e-7;
	const Double_t tailmin = 5; 
	const Double_t tailmax = 20; 

	// core, integral of gauss [0;inf] = 1/2 x [-inf;inf]
	Double_t Ic = TMath::Sqrt(TMath::PiOver2());
	// tail, integral of K_r/r^3 [tailmin;tailmax]
	Double_t xIt = (-0.5)*Kx*(1./TMath::Power(tailmax,2)-1./TMath::Power(tailmin,2)); // x-axis
	Double_t yIt = (-0.5)*Ky*(1./TMath::Power(tailmax,2)-1./TMath::Power(tailmin,2)); // y-axis
	// core + tail
	Double_t xIct = Ic + xIt; // x-axis 
	Double_t yIct = Ic + yIt; // y-axis 
	// ratio = core/(core+tail)
	Double_t Rc = Ic*Ic/(xIct*yIct);
	
	return (type == "core") ? (Rc /*core*/) : (1.0 - Rc /*tail*/);
}

void plot(TString dName = "./", TString fName = "output_eicrecon_hist_all.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	TString fileName = dName + "/" + fName;
	TFile* file = TFile::Open(fileName.Data());
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

	TH1D* hHits = new TH1D("hHits",

		/* core */		
		Form("Hit frequency for different sub-detectors (N_{e-} = %.e | I_{beam} = %.3f A | Core/(Core+Tail) = %.3e);Subdetector;Hits freq. [Hz]",
		nElectrons_geant4,BeamCurrent_A,getBeamCoreTailRatio("core")),bNum,0,bNum);

		/* tail */
		// Form("Hit frequency for different sub-detectors (N_{e-} = %.e | I_{beam} = %.3f A | Tail/(Core+Tail) = %.3e);Subdetector;Hits freq. [Hz]",
		// nElectrons_geant4,BeamCurrent_A,getBeamCoreTailRatio("tail")),bNum,0,bNum);

	TH2D* hVertexBarrelHits = new TH2D("hVertexBarrelHits",
		Form("Scatter plot of SR hits in VertexBarrel (N_{e-} = %.e);x [mm];y [mm]",
		nElectrons_geant4),240,-120,120,240,-120,120);

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
		hVertexBarrelHits->Fill(x_VertexBarrelHits,y_VertexBarrelHits,1);
	}

	/* core */ hHits->Scale(getBeamCoreTailRatio("core")*BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));
	/* tail */ // hHits->Scale(getBeamCoreTailRatio("tail")*BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));

	gStyle->SetErrorX(0);
	gStyle->SetOptStat(0);

	TCanvas* c1 = new TCanvas("c1","c1",2000,2000);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
	pad1->SetBottomMargin(0.25);
	pad1->SetTopMargin(0.1);
	pad1->SetRightMargin(0.1);
	pad1->SetLeftMargin(0.12);
	pad1->Draw();
	pad1->cd();
	hHits->Draw("bar1 & e");
	hHits->SetFillColorAlpha(kBlue,0.4);
	hHits->SetLineWidth(2);
	hHits->SetLineColor(kBlack);
	hHits->SetBarWidth(0.8);
//	hHits->SetMinimum(1e3);
//	hHits->SetMaximum(1e8);
	hHits->GetYaxis()->SetTitleOffset(1.2);
	hHits->GetXaxis()->SetTitleOffset(3.0);
	hHits->GetXaxis()->CenterTitle(true);
	hHits->GetYaxis()->CenterTitle(true);
	hHits->GetXaxis()->SetLabelSize(0.04);
	hHits->GetXaxis()->SetLabelOffset(0.01);
	hHits->GetXaxis()->SetTitle("");
	hHits->LabelsOption("v");
	gPad->SetLogy();

	c1->SaveAs(Form("c1_%s.png",fName.Data()));

	TCanvas* c2 = new TCanvas("c2","c2",2000,2000);
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

	c2->SaveAs(Form("c2_%s.png",fName.Data()));

	return;
}
