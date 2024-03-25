#include <JANA/JEventProcessorSequentialRoot.h>
#include <TNtuple.h>

// Include appropirate class headers
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>


class analysisPluginProcessor: public JEventProcessorSequentialRoot 
{
private:
    	// Data objects needed from JANA
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_B0ECalHits = {this,"B0ECalHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_B0TrackerHits = {this,"B0TrackerHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_BackwardMPGDEndcapHits = {this,"BackwardMPGDEndcapHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_DIRCBarHits = {this,"DIRCBarHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_DRICHHits = {this,"DRICHHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalBarrelImagingHits = {this,"EcalBarrelImagingHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalBarrelScFiHits = {this,"EcalBarrelScFiHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalEndcapNHits = {this,"EcalEndcapNHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalEndcapPHits = {this,"EcalEndcapPHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalEndcapPInsertHits = {this,"EcalEndcapPInsertHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalFarForwardZDCHits = {this,"EcalFarForwardZDCHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_EcalLumiSpecHits = {this,"EcalLumiSpecHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_ForwardMPGDEndcapHits = {this,"ForwardMPGDEndcapHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_ForwardOffMTrackerHits = {this,"ForwardOffMTrackerHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_ForwardRomanPotHits = {this,"ForwardRomanPotHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_HcalBarrelHits = {this,"HcalBarrelHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_HcalEndcapNHits = {this,"HcalEndcapNHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_HcalEndcapPInsertHits = {this,"HcalEndcapPInsertHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_HcalFarForwardZDCHits = {this,"HcalFarForwardZDCHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_LFHCALHits = {this,"LFHCALHits"};
	PrefetchT<edm4hep::SimCalorimeterHit>	raw_LumiDirectPCALHits = {this,"LumiDirectPCALHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_LumiSpecTrackerHits = {this,"LumiSpecTrackerHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_MPGDBarrelHits = {this,"MPGDBarrelHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_OuterMPGDBarrelHits = {this,"OuterMPGDBarrelHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_PFRICHHits = {this,"PFRICHHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_SiBarrelHits = {this,"SiBarrelHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_TaggerTrackerHits = {this,"TaggerTrackerHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_TOFBarrelHits = {this,"TOFBarrelHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_TOFEndcapHits = {this,"TOFEndcapHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_TrackerEndcapHits = {this,"TrackerEndcapHits"};
	PrefetchT<edm4hep::SimTrackerHit>	raw_VertexBarrelHits = {this,"VertexBarrelHits"};

	Float_t nHits[100];

    	// Declare histograms and ntuples
    	TTree *ntraw_nHits = nullptr;


public:
    	analysisPluginProcessor() { SetTypeName(NAME_OF_THIS); }

    	void InitWithGlobalRootLock() override;
    	void ProcessSequential(const std::shared_ptr<const JEvent>& event) override;
    	void FinishWithGlobalRootLock() override;
};
