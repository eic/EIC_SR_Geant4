
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
//    	ntraw_VertexBarrelHits = new TNtuple("ntraw_VertexBarrelHits","VertexBarrelHits","x:y:z");
//    	ntraw_SiBarrelHits = new TNtuple("ntraw_SiBarrelHits","SiBarrelHits","x:y:z");

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
	nHits[ 0] = raw_TrackerEndcapHits().size();
	nHits[ 1] = raw_HcalEndcapNHits().size();
	nHits[ 2] = raw_EcalEndcapNHits().size();
	nHits[ 3] = raw_VertexBarrelHits().size();
	nHits[ 4] = raw_TOFBarrelHits().size();
	nHits[ 5] = raw_SiBarrelHits().size();
	nHits[ 6] = raw_MPGDBarrelHits().size();
	nHits[ 7] = raw_EcalEndcapPHits().size();
	nHits[ 8] = raw_B0TrackerHits().size();
	nHits[ 9] = raw_HcalBarrelHits().size();
	nHits[10] = raw_LumiSpecCALHits().size();
	nHits[11] = raw_TaggerTrackerHits().size();
	nHits[12] = raw_ZDCEcalHits().size();
	nHits[13] = raw_PFRICHHits().size();
	nHits[14] = raw_LumiSpecTrackerHits().size();
	nHits[15] = raw_LumiDirectPCALHits().size();
	nHits[16] = raw_LFHCALHits().size();

	ntraw_nHits->Fill();

/*
	// Alternative method
        const auto & raw_TrackerEndcapHits   = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("TrackerEndcapHits"));
        const auto & raw_VertexBarrelHits    = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("VertexBarrelHits"));
        const auto & raw_TOFBarrelHits       = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("TOFBarrelHits"));
        const auto & raw_SiBarrelHits        = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("SiBarrelHits"));
        const auto & raw_MPGDBarrelHits      = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("MPGDBarrelHits"));
        const auto & raw_B0TrackerHits       = *static_cast<const edm4hep::SimTrackerHitCollection*>(
		event->GetCollectionBase("B0TrackerHits"));

        const auto & raw_HcalBarrelHits      = *static_cast<const edm4hep::SimCalorimeterHitCollection*>(
		event->GetCollectionBase("HcalBarrelHits"));
        const auto & raw_HcalEndcapNHits     = *static_cast<const edm4hep::SimCalorimeterHitCollection*>(
		event->GetCollectionBase("HcalEndcapNHits"));
        const auto & raw_EcalEndcapNHits     = *static_cast<const edm4hep::SimCalorimeterHitCollection*>(
		event->GetCollectionBase("EcalEndcapNHits"));
        const auto & raw_EcalEndcapPHits     = *static_cast<const edm4hep::SimCalorimeterHitCollection*>(
		event->GetCollectionBase("EcalEndcapPHits"));
        const auto & raw_LumiSpecCALHits     = *static_cast<const edm4hep::SimCalorimeterHitCollection*>(
		event->GetCollectionBase("LumiSpecCALHits"));

    	// Fill histograms and ntuples
    	ntraw_nHits->Fill(	raw_TrackerEndcapHits.size(),
		    		raw_HcalEndcapNHits.size(),
				raw_EcalEndcapNHits.size(),
				raw_VertexBarrelHits.size(),
				raw_TOFBarrelHits.size(),
				raw_SiBarrelHits.size(),
				raw_MPGDBarrelHits.size(),
				raw_EcalEndcapPHits.size(),
				raw_B0TrackerHits.size(),
				raw_HcalBarrelHits.size(),
				raw_LumiSpecCALHits.size());

    	for( auto hit : raw_VertexBarrelHits )
	{
		ntraw_VertexBarrelHits->Fill(	hit.getPosition().x, 
						hit.getPosition().y,
						hit.getPosition().z);
	}
    	for( auto hit : raw_SiBarrelHits )
	{
		ntraw_SiBarrelHits->Fill(	hit.getPosition().x, 
						hit.getPosition().y,
						hit.getPosition().z);
	}
*/

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

