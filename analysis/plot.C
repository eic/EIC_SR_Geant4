void plot()
{
	gStyle->SetOptStat(0);
	gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.035);

	TFile *file0 = TFile::Open("output/output_noEcut.root");
	TFile *file1 = TFile::Open("output/output_5keVcut.root");

	TH1F* h0_1 = (TH1F*)file0->Get("h1"); h0_1->SetName("h0_1");
	TH1F* h1_1 = (TH1F*)file1->Get("h1"); h1_1->SetName("h1_1");
	TH2F* h0_2 = (TH2F*)file0->Get("h2"); h0_2->SetName("h0_2");
	TH2F* h1_2 = (TH2F*)file1->Get("h2"); h1_2->SetName("h1_2");

	TCanvas* c1 = new TCanvas("c1","c1",1200,900);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
    	pad1->SetBottomMargin(0.1);
    	pad1->SetTopMargin(0.1);
    	pad1->SetRightMargin(0.1);
    	pad1->SetLeftMargin(0.1);
    	pad1->Draw();
    	pad1->cd();
	h0_1->Rebin(10);
	h1_1->Rebin(10);
	h0_1->SetLineWidth(4);
	h1_1->SetLineWidth(1);
	h1_1->SetFillStyle(3345);
	h1_1->SetFillColor(kRed);	
	h0_1->SetLineColor(kBlue);
	h1_1->SetLineColor(kRed);
	h0_1->Draw("hist");
	h1_1->Draw("same & hist");

	gPad->SetLogy();

	TLegend* leg1 = new TLegend(0.4,0.7,0.9,0.9);
	leg1->AddEntry(h0_1,Form("E_{#gamma} > 0 keV [nEntries = %.1e]",h0_1->GetEntries()),"f");
	leg1->AddEntry(h1_1,Form("E_{#gamma} > 5 keV [nEntries = %.1e]",h1_1->GetEntries()),"f");
	leg1->Draw();

	c1->SaveAs("c1.png");

	TCanvas* c2 = new TCanvas("c2","c2",1200,900);
	c2->Divide(1,2);

	c2->cd(1);
	TPad *pad21 = new TPad("pad21","pad21",0,0,1,1);
    	pad21->SetBottomMargin(0.1);
    	pad21->SetTopMargin(0.1);
    	pad21->SetRightMargin(0.1);
    	pad21->SetLeftMargin(0.1);
    	pad21->Draw();
    	pad21->cd();
	h0_2->SetTitle("SR photon position, E_{#gamma} > 0 keV");
	h0_2->Draw("colz");
	gPad->SetLogz();

	c2->cd(2);
	TPad *pad22 = new TPad("pad22","pad22",0,0,1,1);
    	pad22->SetBottomMargin(0.1);
    	pad22->SetTopMargin(0.1);
    	pad22->SetRightMargin(0.1);
    	pad22->SetLeftMargin(0.1);
    	pad22->Draw();
    	pad22->cd();
	h1_2->SetTitle("SR photon position, E_{#gamma} > 5 keV");
	h1_2->Draw("colz");
	h1_2->SetMaximum(h0_2->GetMaximum());
	gPad->SetLogz();

	c2->SaveAs("c2.png");

	return;
}
