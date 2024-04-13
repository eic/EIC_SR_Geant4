/********************************/
/********************************/
/* Plot detector hit rates	*/
/* Run in eic-shell		*/
/********************************/
/********************************/
#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTreeReaderArray.h"
#include "TTreeReader.h"
#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <filesystem>

using namespace std;

void getAllHitsPerFile(	
			const string inputfile  = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask4_1_farBWD_newMaterialsAndCoating/output_eicrecon_1.podio.root", 
			const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask4_1_farBWD_newMaterialsAndCoating/output_eicrecon_1.hist.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	cout<<"--> inputfile = "<<inputfile<<endl;
	cout<<"--> outputfile = "<<outputfile<<endl;
	
	const Int_t nDet = 31;
	Int_t nSimHitSum[nDet];
	Int_t nRawHitSum[nDet];
	Int_t nRecHitSum[nDet];
	for(Int_t i = 0; i < nDet; i++)
	{
		nSimHitSum[i] = 0;
		nRawHitSum[i] = 0;
		nRecHitSum[i] = 0;
	}

	const TString nSimHitsArray[] = {"B0ECalHits","B0TrackerHits","BackwardMPGDEndcapHits","DIRCBarHits","DRICHHits","EcalBarrelImagingHits","EcalBarrelScFiHits","EcalEndcapNHits","EcalEndcapPHits","EcalEndcapPInsertHits","EcalFarForwardZDCHits","EcalLumiSpecHits","ForwardMPGDEndcapHits","ForwardOffMTrackerHits","ForwardRomanPotHits","HcalBarrelHits","HcalEndcapNHits","HcalEndcapPInsertHits","HcalFarForwardZDCHits","LFHCALHits","LumiDirectPCALHits","LumiSpecTrackerHits","MPGDBarrelHits","OuterMPGDBarrelHits","PFRICHHits","SiBarrelHits","TaggerTrackerHits","TOFBarrelHits","TOFEndcapHits","TrackerEndcapHits","VertexBarrelHits"};

	const TString nRawHitsArray[] = {"B0ECalRawHits","B0TrackerRawHits","BackwardMPGDEndcapRawHits","DIRCRawHits","DRICHRawHits","EcalBarrelImagingRawHits","EcalBarrelScFiRawHits","EcalEndcapNRawHits","EcalEndcapPRawHits","EcalEndcapPInsertRawHits","EcalFarForwardZDCRawHits","EcalLumiSpecRawHits","ForwardMPGDEndcapRawHits","ForwardOffMTrackerRawHits","ForwardRomanPotRawHits","HcalBarrelRawHits","HcalEndcapNRawHits","HcalEndcapPInsertRawHits","HcalFarForwardZDCRawHits","LFHCALRawHits","LumiDirectPCALRawHits","LumiSpecTrackerRawHits","MPGDBarrelRawHits","OuterMPGDBarrelRawHits","PFRICHRawHits","SiBarrelRawHits","TaggerTrackerRawHits","TOFBarrelRawHit","TOFEndcapRawHits","SiEndcapTrackerRawHits","SiBarrelVertexRawHits"};

	const TString nRecHitsArray[] = {"B0ECalRecHits","B0TrackerRecHits","BackwardMPGDEndcapRecHits","DIRCBarRecHits","DRICHRecHits","EcalBarrelImagingRecHits","EcalBarrelScFiRecHits","EcalEndcapNRecHits","EcalEndcapPRecHits","EcalEndcapPInsertRecHits","EcalFarForwardZDCRecHits","EcalLumiSpecRecHits","ForwardMPGDEndcapRecHits","ForwardOffMTrackerRecHits","ForwardRomanPotRecHits","HcalBarrelRecHits","HcalEndcapNRecHits","HcalEndcapPInsertRecHits","HcalFarForwardZDCRecHits","LFHCALRecHits","LumiDirectPCALRecHits","LumiSpecTrackerRecHits","MPGDBarrelRecHits","OuterMPGDBarrelRecHits","PFRICHRecHits","SiBarrelTrackerRecHits","TaggerTrackerRecHits","TOFBarrelRecHits","TOFEndcapRecHits","SiEndcapTrackerRecHits","SiBarrelVertexRecHits"};

//- Print a code
//for(int i = 0; i < 31; i++)
//{cout<<"TTreeReaderArray<Float_t> var_"<<nSimHitsArray[i]<<"(myReader, \""<<nSimHitsArray[i]<<"\");"<<endl;}
//{cout<<"nSimHitSum["<<i<<"] += var_"<<nSimHitsArray[i]<<".GetSize();"<<endl;}
//{cout<<"TTreeReaderArray<Float_t> var_"<<nRawHitsArray[i]<<"(myReader, \""<<nRawHitsArray[i]<<"\");"<<endl;}
//{cout<<"nRawHitSum["<<i<<"] += var_"<<nRawHitsArray[i]<<".GetSize();"<<endl;}
//{cout<<"TTreeReaderArray<Float_t> var_"<<nRecHitsArray[i]<<"(myReader, \""<<nRecHitsArray[i]<<"\");"<<endl;}
//{cout<<"nRecHitSum["<<i<<"] += var_"<<nRecHitsArray[i]<<".GetSize();"<<endl;}
//exit(-1);
	//-----------------------------------------------------------//
	TFile* file = new TFile(inputfile.c_str());
	if(!file) return;

	TTreeReader myReader("events", file);
	
	//- SimHits
	TTreeReaderArray<Float_t> var_B0ECalHits(myReader, "B0ECalHits.energy");
	TTreeReaderArray<Float_t> var_B0TrackerHits(myReader, "B0TrackerHits.EDep");
	TTreeReaderArray<Float_t> var_BackwardMPGDEndcapHits(myReader, "BackwardMPGDEndcapHits.EDep");
	TTreeReaderArray<Float_t> var_DIRCBarHits(myReader, "DIRCBarHits.EDep");
	TTreeReaderArray<Float_t> var_DRICHHits(myReader, "DRICHHits.EDep");
	TTreeReaderArray<Float_t> var_EcalBarrelImagingHits(myReader, "EcalBarrelImagingHits.energy");
	TTreeReaderArray<Float_t> var_EcalBarrelScFiHits(myReader, "EcalBarrelScFiHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapNHits(myReader, "EcalEndcapNHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapPHits(myReader, "EcalEndcapPHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapPInsertHits(myReader, "EcalEndcapPInsertHits.energy");
	TTreeReaderArray<Float_t> var_EcalFarForwardZDCHits(myReader, "EcalFarForwardZDCHits.energy");
	TTreeReaderArray<Float_t> var_EcalLumiSpecHits(myReader, "EcalLumiSpecHits.energy");
	TTreeReaderArray<Float_t> var_ForwardMPGDEndcapHits(myReader, "ForwardMPGDEndcapHits.EDep");
	TTreeReaderArray<Float_t> var_ForwardOffMTrackerHits(myReader, "ForwardOffMTrackerHits.EDep");
	TTreeReaderArray<Float_t> var_ForwardRomanPotHits(myReader, "ForwardRomanPotHits.EDep");
	TTreeReaderArray<Float_t> var_HcalBarrelHits(myReader, "HcalBarrelHits.energy");
	TTreeReaderArray<Float_t> var_HcalEndcapNHits(myReader, "HcalEndcapNHits.energy");
	TTreeReaderArray<Float_t> var_HcalEndcapPInsertHits(myReader, "HcalEndcapPInsertHits.energy");
	TTreeReaderArray<Float_t> var_HcalFarForwardZDCHits(myReader, "HcalFarForwardZDCHits.energy");
	TTreeReaderArray<Float_t> var_LFHCALHits(myReader, "LFHCALHits.energy");
	TTreeReaderArray<Float_t> var_LumiDirectPCALHits(myReader, "LumiDirectPCALHits.energy");
	TTreeReaderArray<Float_t> var_LumiSpecTrackerHits(myReader, "LumiSpecTrackerHits.EDep");
	TTreeReaderArray<Float_t> var_MPGDBarrelHits(myReader, "MPGDBarrelHits.EDep");
	TTreeReaderArray<Float_t> var_OuterMPGDBarrelHits(myReader, "OuterMPGDBarrelHits.EDep");
//		TTreeReaderArray<Float_t> var_PFRICHHits(myReader, "PFRICHHits.EDep");
	TTreeReaderArray<Float_t> var_SiBarrelHits(myReader, "SiBarrelHits.EDep");
	TTreeReaderArray<Float_t> var_TaggerTrackerHits(myReader, "TaggerTrackerHits.EDep");
	TTreeReaderArray<Float_t> var_TOFBarrelHits(myReader, "TOFBarrelHits.EDep");
	TTreeReaderArray<Float_t> var_TOFEndcapHits(myReader, "TOFEndcapHits.EDep");
	TTreeReaderArray<Float_t> var_TrackerEndcapHits(myReader, "TrackerEndcapHits.EDep");
	TTreeReaderArray<Float_t> var_VertexBarrelHits(myReader, "VertexBarrelHits.EDep");

	//- RawHits 
	TTreeReaderArray<Int_t> var_B0ECalRawHits(myReader, "B0ECalRawHits.amplitude");
	TTreeReaderArray<Int_t> var_B0TrackerRawHits(myReader, "B0TrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_BackwardMPGDEndcapRawHits(myReader, "BackwardMPGDEndcapRawHits.charge");
	TTreeReaderArray<Int_t> var_DIRCRawHits(myReader, "DIRCRawHits.charge");
	TTreeReaderArray<Int_t> var_DRICHRawHits(myReader, "DRICHRawHits.charge");
	TTreeReaderArray<Int_t> var_EcalBarrelImagingRawHits(myReader, "EcalBarrelImagingRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalBarrelScFiRawHits(myReader, "EcalBarrelScFiRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalEndcapNRawHits(myReader, "EcalEndcapNRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalEndcapPRawHits(myReader, "EcalEndcapPRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalEndcapPInsertRawHits(myReader, "EcalEndcapPInsertRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalFarForwardZDCRawHits(myReader, "EcalFarForwardZDCRawHits.amplitude");
	TTreeReaderArray<Int_t> var_EcalLumiSpecRawHits(myReader, "EcalLumiSpecRawHits.amplitude");
	TTreeReaderArray<Int_t> var_ForwardMPGDEndcapRawHits(myReader, "ForwardMPGDEndcapRawHits.charge");
	TTreeReaderArray<Int_t> var_ForwardOffMTrackerRawHits(myReader, "ForwardOffMTrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_ForwardRomanPotRawHits(myReader, "ForwardRomanPotRawHits.charge");
	TTreeReaderArray<Int_t> var_HcalBarrelRawHits(myReader, "HcalBarrelRawHits.amplitude");
	TTreeReaderArray<Int_t> var_HcalEndcapNRawHits(myReader, "HcalEndcapNRawHits.amplitude");
	TTreeReaderArray<Int_t> var_HcalEndcapPInsertRawHits(myReader, "HcalEndcapPInsertRawHits.amplitude");
	TTreeReaderArray<Int_t> var_HcalFarForwardZDCRawHits(myReader, "HcalFarForwardZDCRawHits.amplitude");
	TTreeReaderArray<Int_t> var_LFHCALRawHits(myReader, "LFHCALRawHits.amplitude");
//		TTreeReaderArray<Int_t> var_LumiDirectPCALRawHits(myReader, "LumiDirectPCALRawHits.amplitude");
//		TTreeReaderArray<Int_t> var_LumiSpecTrackerRawHits(myReader, "LumiSpecTrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_MPGDBarrelRawHits(myReader, "MPGDBarrelRawHits.charge");
	TTreeReaderArray<Int_t> var_OuterMPGDBarrelRawHits(myReader, "OuterMPGDBarrelRawHits.charge");
//		TTreeReaderArray<Int_t> var_PFRICHRawHits(myReader, "PFRICHRawHits.charge");
	TTreeReaderArray<Int_t> var_SiBarrelRawHits(myReader, "SiBarrelRawHits.charge");
	TTreeReaderArray<Int_t> var_TaggerTrackerRawHits(myReader, "TaggerTrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_TOFBarrelRawHit(myReader, "TOFBarrelRawHit.charge");
	TTreeReaderArray<Int_t> var_TOFEndcapRawHits(myReader, "TOFEndcapRawHits.charge");
	TTreeReaderArray<Int_t> var_SiEndcapTrackerRawHits(myReader, "SiEndcapTrackerRawHits.charge");
	TTreeReaderArray<Int_t> var_SiBarrelVertexRawHits(myReader, "SiBarrelVertexRawHits.charge");

	//- RecHits
	TTreeReaderArray<Float_t> var_B0ECalRecHits(myReader, "B0ECalRecHits.energy");
	TTreeReaderArray<Float_t> var_B0TrackerRecHits(myReader, "B0TrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_BackwardMPGDEndcapRecHits(myReader, "BackwardMPGDEndcapRecHits.edep");
//		TTreeReaderArray<Float_t> var_DIRCBarRecHits(myReader, "DIRCBarRecHits.edep");
//		TTreeReaderArray<Float_t> var_DRICHRecHits(myReader, "DRICHRecHits.edep");
	TTreeReaderArray<Float_t> var_EcalBarrelImagingRecHits(myReader, "EcalBarrelImagingRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalBarrelScFiRecHits(myReader, "EcalBarrelScFiRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapNRecHits(myReader, "EcalEndcapNRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapPRecHits(myReader, "EcalEndcapPRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalEndcapPInsertRecHits(myReader, "EcalEndcapPInsertRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalFarForwardZDCRecHits(myReader, "EcalFarForwardZDCRecHits.energy");
	TTreeReaderArray<Float_t> var_EcalLumiSpecRecHits(myReader, "EcalLumiSpecRecHits.energy");
	TTreeReaderArray<Float_t> var_ForwardMPGDEndcapRecHits(myReader, "ForwardMPGDEndcapRecHits.edep");
	TTreeReaderArray<Float_t> var_ForwardOffMTrackerRecHits(myReader, "ForwardOffMTrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_ForwardRomanPotRecHits(myReader, "ForwardRomanPotRecHits.edep");
	TTreeReaderArray<Float_t> var_HcalBarrelRecHits(myReader, "HcalBarrelRecHits.energy");
	TTreeReaderArray<Float_t> var_HcalEndcapNRecHits(myReader, "HcalEndcapNRecHits.energy");
	TTreeReaderArray<Float_t> var_HcalEndcapPInsertRecHits(myReader, "HcalEndcapPInsertRecHits.energy");
	TTreeReaderArray<Float_t> var_HcalFarForwardZDCRecHits(myReader, "HcalFarForwardZDCRecHits.energy");
	TTreeReaderArray<Float_t> var_LFHCALRecHits(myReader, "LFHCALRecHits.energy");
//		TTreeReaderArray<Float_t> var_LumiDirectPCALRecHits(myReader, "LumiDirectPCALRecHits.energy");
//		TTreeReaderArray<Float_t> var_LumiSpecTrackerRecHits(myReader, "LumiSpecTrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_MPGDBarrelRecHits(myReader, "MPGDBarrelRecHits.edep");
	TTreeReaderArray<Float_t> var_OuterMPGDBarrelRecHits(myReader, "OuterMPGDBarrelRecHits.edep");
//		TTreeReaderArray<Float_t> var_PFRICHRecHits(myReader, "PFRICHRecHits.edep");
	TTreeReaderArray<Float_t> var_SiBarrelTrackerRecHits(myReader, "SiBarrelTrackerRecHits.edep");
//		TTreeReaderArray<Float_t> var_TaggerTrackerRecHits(myReader, "TaggerTrackerRecHits.edep");
//		TTreeReaderArray<Float_t> var_TOFBarrelRecHits(myReader, "TOFBarrelRecHits.edep");
	TTreeReaderArray<Float_t> var_TOFEndcapRecHits(myReader, "TOFEndcapRecHits.edep");
	TTreeReaderArray<Float_t> var_SiEndcapTrackerRecHits(myReader, "SiEndcapTrackerRecHits.edep");
	TTreeReaderArray<Float_t> var_SiBarrelVertexRecHits(myReader, "SiBarrelVertexRecHits.edep");
	//-----------------------------------------------------------//
	// loop over events
	while(myReader.Next())
	{
		//- SimHits
		nSimHitSum[0] += var_B0ECalHits.GetSize();
		nSimHitSum[1] += var_B0TrackerHits.GetSize();
		nSimHitSum[2] += var_BackwardMPGDEndcapHits.GetSize();
		nSimHitSum[3] += var_DIRCBarHits.GetSize();
		nSimHitSum[4] += var_DRICHHits.GetSize();
		nSimHitSum[5] += var_EcalBarrelImagingHits.GetSize();
		nSimHitSum[6] += var_EcalBarrelScFiHits.GetSize();
		nSimHitSum[7] += var_EcalEndcapNHits.GetSize();
		nSimHitSum[8] += var_EcalEndcapPHits.GetSize();
		nSimHitSum[9] += var_EcalEndcapPInsertHits.GetSize();
		nSimHitSum[10] += var_EcalFarForwardZDCHits.GetSize();
		nSimHitSum[11] += var_EcalLumiSpecHits.GetSize();
		nSimHitSum[12] += var_ForwardMPGDEndcapHits.GetSize();
		nSimHitSum[13] += var_ForwardOffMTrackerHits.GetSize();
		nSimHitSum[14] += var_ForwardRomanPotHits.GetSize();
		nSimHitSum[15] += var_HcalBarrelHits.GetSize();
		nSimHitSum[16] += var_HcalEndcapNHits.GetSize();
		nSimHitSum[17] += var_HcalEndcapPInsertHits.GetSize();
		nSimHitSum[18] += var_HcalFarForwardZDCHits.GetSize();
		nSimHitSum[19] += var_LFHCALHits.GetSize();
		nSimHitSum[20] += var_LumiDirectPCALHits.GetSize();
		nSimHitSum[21] += var_LumiSpecTrackerHits.GetSize();
		nSimHitSum[22] += var_MPGDBarrelHits.GetSize();
		nSimHitSum[23] += var_OuterMPGDBarrelHits.GetSize();
//			nSimHitSum[24] += var_PFRICHHits.GetSize();
		nSimHitSum[25] += var_SiBarrelHits.GetSize();
		nSimHitSum[26] += var_TaggerTrackerHits.GetSize();
		nSimHitSum[27] += var_TOFBarrelHits.GetSize();
		nSimHitSum[28] += var_TOFEndcapHits.GetSize();
		nSimHitSum[29] += var_TrackerEndcapHits.GetSize();
		nSimHitSum[30] += var_VertexBarrelHits.GetSize();

		//- RawHits
		nRawHitSum[0] += var_B0ECalRawHits.GetSize();
		nRawHitSum[1] += var_B0TrackerRawHits.GetSize();
		nRawHitSum[2] += var_BackwardMPGDEndcapRawHits.GetSize();
		nRawHitSum[3] += var_DIRCRawHits.GetSize();
		nRawHitSum[4] += var_DRICHRawHits.GetSize();
		nRawHitSum[5] += var_EcalBarrelImagingRawHits.GetSize();
		nRawHitSum[6] += var_EcalBarrelScFiRawHits.GetSize();
		nRawHitSum[7] += var_EcalEndcapNRawHits.GetSize();
		nRawHitSum[8] += var_EcalEndcapPRawHits.GetSize();
		nRawHitSum[9] += var_EcalEndcapPInsertRawHits.GetSize();
		nRawHitSum[10] += var_EcalFarForwardZDCRawHits.GetSize();
		nRawHitSum[11] += var_EcalLumiSpecRawHits.GetSize();
		nRawHitSum[12] += var_ForwardMPGDEndcapRawHits.GetSize();
		nRawHitSum[13] += var_ForwardOffMTrackerRawHits.GetSize();
		nRawHitSum[14] += var_ForwardRomanPotRawHits.GetSize();
		nRawHitSum[15] += var_HcalBarrelRawHits.GetSize();
		nRawHitSum[16] += var_HcalEndcapNRawHits.GetSize();
		nRawHitSum[17] += var_HcalEndcapPInsertRawHits.GetSize();
		nRawHitSum[18] += var_HcalFarForwardZDCRawHits.GetSize();
		nRawHitSum[19] += var_LFHCALRawHits.GetSize();
//			nRawHitSum[20] += var_LumiDirectPCALRawHits.GetSize();
//			nRawHitSum[21] += var_LumiSpecTrackerRawHits.GetSize();
		nRawHitSum[22] += var_MPGDBarrelRawHits.GetSize();
		nRawHitSum[23] += var_OuterMPGDBarrelRawHits.GetSize();
//			nRawHitSum[24] += var_PFRICHRawHits.GetSize();
		nRawHitSum[25] += var_SiBarrelRawHits.GetSize();
		nRawHitSum[26] += var_TaggerTrackerRawHits.GetSize();
		nRawHitSum[27] += var_TOFBarrelRawHit.GetSize();
		nRawHitSum[28] += var_TOFEndcapRawHits.GetSize();
		nRawHitSum[29] += var_SiEndcapTrackerRawHits.GetSize();
		nRawHitSum[30] += var_SiBarrelVertexRawHits.GetSize();

		//- RecHits
		nRecHitSum[0] += var_B0ECalRecHits.GetSize();
		nRecHitSum[1] += var_B0TrackerRecHits.GetSize();
		nRecHitSum[2] += var_BackwardMPGDEndcapRecHits.GetSize();
//			nRecHitSum[3] += var_DIRCBarRecHits.GetSize();
//			nRecHitSum[4] += var_DRICHRecHits.GetSize();
		nRecHitSum[5] += var_EcalBarrelImagingRecHits.GetSize();
		nRecHitSum[6] += var_EcalBarrelScFiRecHits.GetSize();
		nRecHitSum[7] += var_EcalEndcapNRecHits.GetSize();
		nRecHitSum[8] += var_EcalEndcapPRecHits.GetSize();
		nRecHitSum[9] += var_EcalEndcapPInsertRecHits.GetSize();
		nRecHitSum[10] += var_EcalFarForwardZDCRecHits.GetSize();
		nRecHitSum[11] += var_EcalLumiSpecRecHits.GetSize();
		nRecHitSum[12] += var_ForwardMPGDEndcapRecHits.GetSize();
		nRecHitSum[13] += var_ForwardOffMTrackerRecHits.GetSize();
		nRecHitSum[14] += var_ForwardRomanPotRecHits.GetSize();
		nRecHitSum[15] += var_HcalBarrelRecHits.GetSize();
		nRecHitSum[16] += var_HcalEndcapNRecHits.GetSize();
		nRecHitSum[17] += var_HcalEndcapPInsertRecHits.GetSize();
		nRecHitSum[18] += var_HcalFarForwardZDCRecHits.GetSize();
		nRecHitSum[19] += var_LFHCALRecHits.GetSize();
//			nRecHitSum[20] += var_LumiDirectPCALRecHits.GetSize();
//			nRecHitSum[21] += var_LumiSpecTrackerRecHits.GetSize();
		nRecHitSum[22] += var_MPGDBarrelRecHits.GetSize();
		nRecHitSum[23] += var_OuterMPGDBarrelRecHits.GetSize();
//			nRecHitSum[24] += var_PFRICHRecHits.GetSize();
		nRecHitSum[25] += var_SiBarrelTrackerRecHits.GetSize();
//			nRecHitSum[26] += var_TaggerTrackerRecHits.GetSize();
//			nRecHitSum[27] += var_TOFBarrelRecHits.GetSize();
		nRecHitSum[28] += var_TOFEndcapRecHits.GetSize();
		nRecHitSum[29] += var_SiEndcapTrackerRecHits.GetSize();
		nRecHitSum[30] += var_SiBarrelVertexRecHits.GetSize();
	}
	file->Close();
	//-----------------------------------------------------------//
	cout<<"Writing to a file ..."<<endl;
	TFile* f = new TFile(outputfile.c_str(),"recreate");
	if(!f)
	{
		cout<<"[ERROR] Cannot recreate the output file: "<<outputfile<<endl;
		return;
	}
	f->cd();

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
	hSimHits->Write();
	hRawHits->Write();
	hRecHits->Write();
	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}
