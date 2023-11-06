
//
// Template for this file generated with eicmkplugin.py
//

#include "analysisPluginProcessor.h"
#include "services/rootfile/RootFile_service.h"
#include "DD4hep/Printout.h"

using namespace dd4hep;

// The following just makes this a JANA plugin
extern "C" {
    void InitPlugin(JApplication *app) {
        InitJANAPlugin(app);
        app->Add(new analysisPluginProcessor);
    }
}

//-------------------------------------------
// InitWithGlobalRootLock
//-------------------------------------------
void analysisPluginProcessor::InitWithGlobalRootLock(){
    auto rootfile_svc = GetApplication()->GetService<RootFile_service>();
    auto rootfile = rootfile_svc->GetHistFile();
    rootfile->mkdir("analysisPlugin")->cd();

    // Create histograms
    ntraw_VertexBarrelHits = new TNtuple("ntraw_VertexBarrelHits","VertexBarrelHits","x:y");
    ntraw_SiBarrelHits = new TNtuple("ntraw_SiBarrelHits","SiBarrelHits","x:y");
/*
    ntraw_TrackerEndcapHits = new TNtuple("ntraw_TrackerEndcapHits","TrackerEndcapHits","nHits");
    ntraw_TOFBarrelHits = new TNtuple("ntraw_TOFBarrelHits","TOFBarrelHits","nHits");
    ntraw_MPGDBarrelHits = new TNtuple("ntraw_MPGDBarrelHits","MPGDBarrelHits","nHits");
    ntraw_B0TrackerHits = new TNtuple("ntraw_B0TrackerHits","B0TrackerHits","nHits");
    ntraw_HcalBarrelHits = new TNtuple("ntraw_HcalBarrelHits","HcalBarrelHits","nHits");
    ntraw_HcalEndcapNHits = new TNtuple("ntraw_HcalEndcapNHits","HcalEndcapNHits","nHits");
    ntraw_EcalEndcapNHits = new TNtuple("ntraw_EcalEndcapNHits","EcalEndcapNHits","nHits");
    ntraw_EcalEndcapPHits = new TNtuple("ntraw_EcalEndcapPHits","EcalEndcapPHits","nHits");
    ntraw_LumiSpecCALHits = new TNtuple("ntraw_LumiSpecCALHits","LumiSpecCALHits","nHits");
*/
    ntraw_nHits = new TNtuple("ntraw_nHits","nHits",
	"nHits_TrackerEndcapHits:nHits_HcalEndcapNHits:nHits_EcalEndcapNHits:nHits_VertexBarrelHits:nHits_TOFBarrelHits:nHits_SiBarrelHits:nHits_MPGDBarrelHits:nHits_EcalEndcapPHits:nHits_B0TrackerHits:nHits_HcalBarrelHits:nHits_LumiSpecCALHits");
}

//-------------------------------------------
// ProcessSequential
//-------------------------------------------
void analysisPluginProcessor::ProcessSequential(const std::shared_ptr<const JEvent>& event) {

    // Fill histograms and ntuples
    ntraw_nHits->Fill(	raw_TrackerEndcapHits().size(),
		    	raw_HcalEndcapNHits().size(),
			raw_EcalEndcapNHits().size(),
			raw_VertexBarrelHits().size(),
			raw_TOFBarrelHits().size(),
			raw_SiBarrelHits().size(),
			raw_MPGDBarrelHits().size(),
			raw_EcalEndcapPHits().size(),
			raw_B0TrackerHits().size(),
			raw_HcalBarrelHits().size(),
			raw_LumiSpecCALHits().size());

    for( auto hit : raw_VertexBarrelHits() ){ntraw_VertexBarrelHits->Fill(hit->getPosition().x, hit->getPosition().y);}
    for( auto hit : raw_SiBarrelHits() ){ntraw_SiBarrelHits->Fill(hit->getPosition().x, hit->getPosition().y);}
}

//-------------------------------------------
// FinishWithGlobalRootLock
//-------------------------------------------
void analysisPluginProcessor::FinishWithGlobalRootLock() {

    // Do any final calculations here.
}

