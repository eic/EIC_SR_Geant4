/**********************************************************************
 * Maximize the combined pixel coverage by two taggers
 **********************************************************************/

Int_t GetAcceptance(TString fName, TString tagName1, TString tagName2);

void ana4()
{
	TGraph* gr1_tag12 = new TGraph();

	// TaggerScan1
	for(Int_t PosZ = -995; PosZ <= 995; PosZ += 5)
	{
		TString fName = Form("/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm/ana1_output_%d.root",PosZ);
		cout<<fName<<endl;
		Double_t acp_tag12 = GetAcceptance(fName,"tag1","tag2");

		gr1_tag12->SetPoint(gr1_tag12->GetN(),PosZ,acp_tag12);
	}

/*
	// AnteChamberScan
	for(Int_t aH = 1; aH <= 10; aH += 1)
	{
		TString fName = Form("/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-150.0cm_ChamberHscan/ana1_output_%d.root",aH);
		cout<<fName<<endl;
		Double_t acp_tag1 = GetAcceptance(fName,"tag1");
		Double_t acp_tag2 = GetAcceptance(fName,"tag2");

		gr1_tag1->SetPoint(gr1_tag1->GetN(),aH,acp_tag1);
		gr1_tag2->SetPoint(gr1_tag2->GetN(),aH,acp_tag2);
	}
*/
/*
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
*/
	TCanvas* c1= new TCanvas("c1","c1",900,900);
	c1->cd();
	gr1_tag12->SetLineColor(kBlue);
	gr1_tag12->SetLineWidth(3);

	gr1_tag12->Draw("APL");
	gPad->SetGrid();

	c1->SaveAs("pics/c1.png");
	c1->SaveAs("pics/c1.root");

	return;
}

Int_t GetAcceptance(TString fName, TString tagName1, TString tagName2)
{
	TFile *_file = TFile::Open(fName.Data());
	TH1F* h9_etag1 = (TH1F*)_file->Get(Form("h9_e%s",tagName1.Data()));
	TH1F* h9_etag2 = (TH1F*)_file->Get(Form("h9_e%s",tagName2.Data()));

	h9_etag1->Add(h9_etag2);

	Int_t nonZeroBins = 0;
	for(Int_t bini = 1; bini <= h9_etag1->GetNbinsX(); bini++)
	{
		for(Int_t binj = 1; binj <= h9_etag1->GetNbinsY(); binj++)
		{
			if(h9_etag1->GetBinContent(bini,binj) > 0){nonZeroBins++;}
		}
	}
	_file->Close();
	return nonZeroBins;
}
