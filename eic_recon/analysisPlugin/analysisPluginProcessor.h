#include <JANA/JEventProcessorSequentialRoot.h>
#include <TNtuple.h>

// Include appropirate class headers
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/SimTrackerHit.h>


class analysisPluginProcessor: public JEventProcessorSequentialRoot 
{
private:
    	// Data objects needed from JANA
   	PrefetchT<edm4hep::SimTrackerHit> 		raw_TrackerEndcapHits 	= {this, "TrackerEndcapHits"};
    	PrefetchT<edm4hep::SimTrackerHit> 		raw_VertexBarrelHits 	= {this, "VertexBarrelHits"};
    	PrefetchT<edm4hep::SimTrackerHit> 		raw_TOFBarrelHits 	= {this, "TOFBarrelHits"};
    	PrefetchT<edm4hep::SimTrackerHit> 		raw_SiBarrelHits 	= {this, "SiBarrelHits"};
    	PrefetchT<edm4hep::SimTrackerHit> 		raw_MPGDBarrelHits 	= {this, "MPGDBarrelHits"};
    	PrefetchT<edm4hep::SimTrackerHit> 		raw_B0TrackerHits 	= {this, "B0TrackerHits"};
    	PrefetchT<edm4hep::SimCalorimeterHit> 		raw_HcalBarrelHits   	= {this, "HcalBarrelHits"};
    	PrefetchT<edm4hep::SimCalorimeterHit> 		raw_HcalEndcapNHits   	= {this, "HcalEndcapNHits"};
    	PrefetchT<edm4hep::SimCalorimeterHit>       	raw_EcalEndcapNHits     = {this, "EcalEndcapNHits"};
    	PrefetchT<edm4hep::SimCalorimeterHit>       	raw_EcalEndcapPHits     = {this, "EcalEndcapPHits"};
    	PrefetchT<edm4hep::SimCalorimeterHit>       	raw_LumiSpecCALHits     = {this, "LumiSpecCALHits"};

    	// Declare histograms and ntuples
    	TNtuple *ntraw_TrackerEndcapHits = nullptr;
    	TNtuple *ntraw_VertexBarrelHits = nullptr;
    	TNtuple *ntraw_TOFBarrelHits = nullptr;
    	TNtuple *ntraw_SiBarrelHits = nullptr;
    	TNtuple *ntraw_MPGDBarrelHits = nullptr;
    	TNtuple *ntraw_B0TrackerHits = nullptr;
    	TNtuple *ntraw_HcalBarrelHits = nullptr;
    	TNtuple *ntraw_HcalEndcapNHits = nullptr;
    	TNtuple *ntraw_EcalEndcapNHits = nullptr;
    	TNtuple *ntraw_EcalEndcapPHits = nullptr;
    	TNtuple *ntraw_LumiSpecCALHits = nullptr;
    	TNtuple *ntraw_nHits = nullptr;

public:
    	analysisPluginProcessor() { SetTypeName(NAME_OF_THIS); }

    	void InitWithGlobalRootLock() override;
    	void ProcessSequential(const std::shared_ptr<const JEvent>& event) override;
    	void FinishWithGlobalRootLock() override;
};
