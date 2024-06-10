Double_t GetAcceptance(TString fName, TString tagName);

void ana2()
{
	TGraph* gr1_tag1 = new TGraph();
	TGraph* gr1_tag2 = new TGraph();
/*
	// TaggerScan1
	for(Int_t PosZ = -995; PosZ <= 995; PosZ += 5)
	{
		TString fName = Form("/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm/ana1_output_%d.root",PosZ);
		cout<<fName<<endl;
		Double_t acp_tag1 = GetAcceptance(fName,"tag1");
		Double_t acp_tag2 = GetAcceptance(fName,"tag2");

		gr1_tag1->SetPoint(gr1_tag1->GetN(),PosZ,acp_tag1);
		gr1_tag2->SetPoint(gr1_tag2->GetN(),PosZ,acp_tag2);
	}
*/
/*
	// AnteChamberScan
	for(Int_t aH = 1; aH <= 10; aH += 1)
	{
		TString fName = Form("/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-197.2cm_ChamberHscan/ana1_output_%d.root",aH);
		cout<<fName<<endl;
		Double_t acp_tag1 = GetAcceptance(fName,"tag1");
		Double_t acp_tag2 = GetAcceptance(fName,"tag2");

		gr1_tag1->SetPoint(gr1_tag1->GetN(),aH,acp_tag1);
		gr1_tag2->SetPoint(gr1_tag2->GetN(),aH,acp_tag2);
	}
*/

	// TaggerScan2
	for(Int_t PosZ = -995; PosZ <= 995; PosZ += 5)
	{
		TString fName = Form("/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan2_ChamberH_4cm/ana1_output_%d.root",PosZ);
		cout<<fName<<endl;
		Double_t acp_tag1 = GetAcceptance(fName,"tag1");
		Double_t acp_tag2 = GetAcceptance(fName,"tag2");

		gr1_tag1->SetPoint(gr1_tag1->GetN(),PosZ,acp_tag1);
		gr1_tag2->SetPoint(gr1_tag2->GetN(),PosZ,acp_tag2);
	}

	TCanvas* c1= new TCanvas("c1","c1",900,900);
	c1->cd();
	gr1_tag1->SetLineColor(kBlue);
	gr1_tag2->SetLineColor(kRed);

	gr1_tag1->SetLineWidth(3);
	gr1_tag2->SetLineWidth(3);

	gr1_tag1->SetMinimum(0.00);
	gr1_tag1->SetMaximum(0.10);

	gr1_tag1->Draw("APL");
	gr1_tag2->Draw("SAME & PL");
	gPad->SetGrid();

	c1->SaveAs("pics/c1.png");
	c1->SaveAs("pics/c1.root");

	return;
}

Double_t GetAcceptance(TString fName, TString tagName)
{
	TFile *_file = TFile::Open(fName.Data());
	TH1F* h1 = (TH1F*)_file->Get("h1");
	TH1F* h1_etag = (TH1F*)_file->Get(Form("h1_e%s",tagName.Data()));
	Int_t nEntries0 = h1->GetEntries();
	Int_t nEntries1 = h1_etag->GetEntries();
	_file->Close();
	return (Double_t)nEntries1/nEntries0;
}
