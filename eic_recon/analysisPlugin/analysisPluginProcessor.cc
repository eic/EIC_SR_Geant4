
//
// Template for this file generated with eicmkplugin.py
//

#include "analysisPluginProcessor.h"
#include "services/rootfile/RootFile_service.h"

// The following just makes this a JANA plugin
extern "C" 
{
	void InitPlugin(JApplication *app) 
	{
        	InitJANAPlugin(app);
        	app->Add(new analysisPluginProcessor);
    	}
}

//-------------------------------------------
// InitWithGlobalRootLock
//-------------------------------------------
void analysisPluginProcessor::InitWithGlobalRootLock()
{
    	auto rootfile_svc = GetApplication()->GetService<RootFile_service>();
    	auto rootfile = rootfile_svc->GetHistFile();
    	rootfile->mkdir("analysisPlugin")->cd();

    	// Create histograms
    	ntraw_nHits = new TTree("ntraw_nHits","nHits");
	ntraw_nHits->Branch("nHits",	nHits,	"nHits[100]/F");

	return;
}

//-------------------------------------------
// ProcessSequential
//-------------------------------------------
void analysisPluginProcessor::ProcessSequential(const std::shared_ptr<const JEvent>& event) 
{
    	// Fill histograms and ntuples
	nHits[ 0] = raw_B0ECalHits().size();
	nHits[ 1] = raw_B0TrackerHits().size();
	nHits[ 2] = raw_BackwardMPGDEndcapHits().size();
	nHits[ 3] = raw_DIRCBarHits().size();
	nHits[ 4] = raw_DRICHHits().size();
	nHits[ 5] = raw_EcalBarrelImagingHits().size();
	nHits[ 6] = raw_EcalBarrelScFiHits().size();
	nHits[ 7] = raw_EcalEndcapNHits().size();
	nHits[ 8] = raw_EcalEndcapPHits().size();
	nHits[ 9] = raw_EcalEndcapPInsertHits().size();
	nHits[10] = raw_EcalFarForwardZDCHits().size();
	nHits[11] = raw_EcalLumiSpecHits().size();
	nHits[12] = raw_ForwardMPGDEndcapHits().size();
	nHits[13] = raw_ForwardOffMTrackerHits().size();
	nHits[14] = raw_ForwardRomanPotHits().size();
	nHits[15] = raw_HcalBarrelHits().size();
	nHits[16] = raw_HcalEndcapNHits().size();
	nHits[17] = raw_HcalEndcapPInsertHits().size();
	nHits[18] = raw_HcalFarForwardZDCHits().size();
	nHits[19] = raw_LFHCALHits().size();
	nHits[20] = raw_LumiDirectPCALHits().size();
	nHits[21] = raw_LumiSpecTrackerHits().size();
	nHits[22] = raw_MPGDBarrelHits().size();
	nHits[23] = raw_OuterMPGDBarrelHits().size();
	nHits[24] = raw_PFRICHHits().size();
	nHits[25] = raw_SiBarrelHits().size();
	nHits[26] = raw_TaggerTrackerHits().size();
	nHits[27] = raw_TOFBarrelHits().size();
	nHits[28] = raw_TOFEndcapHits().size();
	nHits[29] = raw_TrackerEndcapHits().size();
	nHits[30] = raw_VertexBarrelHits().size();

	ntraw_nHits->Fill();
	return;
}

//-------------------------------------------
// FinishWithGlobalRootLock
//-------------------------------------------
void analysisPluginProcessor::FinishWithGlobalRootLock() 
{
	// Do any final calculations here.
	return;
}

