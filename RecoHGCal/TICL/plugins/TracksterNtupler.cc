// Thorben Quast
// 29 June 2021


#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/HGCalReco/interface/Trackster.h"

class TracksterNtupler : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit TracksterNtupler(const edm::ParameterSet&);
  ~TracksterNtupler();
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void endJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // some options
  const edm::EDGetTokenT<std::vector<ticl::Trackster>> tracksters_token_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layer_clusters_token_;
  const std::string outfilePath_;

  // Output tree
  TTree* tree_;

  void clearVariables();  // function to clear tree variables/vectors

  unsigned int event_index;

  // Variables for branches
  unsigned int ev_event_;
  unsigned int ntracksters_;

  std::vector<uint16_t> trackster_iteration_index;
  std::vector<float_t> trackster_time;
  std::vector<float_t> trackster_timeError;
  std::vector<float_t> trackster_regressed_energy;
  std::vector<float_t> trackster_raw_energy;
  std::vector<float_t> trackster_raw_em_energy;
  std::vector<float_t> trackster_raw_pt;
  std::vector<float_t> trackster_raw_em_pt;
  std::vector<float_t> trackster_barycenter_x;
  std::vector<float_t> trackster_barycenter_y;
  std::vector<float_t> trackster_barycenter_z;
  std::vector<float_t> trackster_EV1;
  std::vector<float_t> trackster_EV2;
  std::vector<float_t> trackster_EV3;
  std::vector<float_t> trackster_sigmaPCA1;
  std::vector<float_t> trackster_sigmaPCA2;
  std::vector<float_t> trackster_sigmaPCA3;
  std::vector<std::vector<float_t>> trackster_id_probabilities;
  
  std::vector<std::vector<uint16_t> > trackster_vertices_indexes;
  std::vector<std::vector<float_t> > trackster_vertices_x;
  std::vector<std::vector<float_t> > trackster_vertices_y;
  std::vector<std::vector<float_t> > trackster_vertices_z;
  std::vector<std::vector<float_t> > trackster_vertices_energy;
};

void TracksterNtupler::clearVariables() {
  // event info
  ev_event_ = 0;
  ntracksters_ = 0;
  trackster_iteration_index.clear();
  trackster_time.clear();
  trackster_timeError.clear();
  trackster_regressed_energy.clear();
  trackster_raw_energy.clear();
  trackster_raw_em_energy.clear();
  trackster_raw_pt.clear();
  trackster_raw_em_pt.clear();
  trackster_barycenter_x.clear();
  trackster_barycenter_y.clear();
  trackster_barycenter_z.clear();
  trackster_EV1.clear();
  trackster_EV2.clear();
  trackster_EV3.clear();
  trackster_sigmaPCA1.clear();
  trackster_sigmaPCA2.clear();
  trackster_sigmaPCA3.clear();
  trackster_id_probabilities.clear();
  
  trackster_vertices_indexes.clear();
  trackster_vertices_x.clear();
  trackster_vertices_y.clear();
  trackster_vertices_z.clear();
  trackster_vertices_energy.clear();
};

TracksterNtupler::TracksterNtupler(const edm::ParameterSet& ps)
    : tracksters_token_(consumes<std::vector<ticl::Trackster>>(ps.getParameter<edm::InputTag>("tracksters"))),
      layer_clusters_token_(consumes<std::vector<reco::CaloCluster>>(ps.getUntrackedParameter<edm::InputTag>("layerClusters"))),
      outfilePath_(ps.getUntrackedParameter<std::string>("outfilePath", "test.root")){

      };

TracksterNtupler::~TracksterNtupler() { clearVariables(); };

void TracksterNtupler::beginJob() {
  // Define tree and branches
  tree_ = new TTree("tracksters", "TICL tracksters");

  // event info
  tree_->Branch("event", &ev_event_);
  tree_->Branch("NTracksters", &ntracksters_);
  tree_->Branch("iteration_index", &trackster_iteration_index);
  tree_->Branch("time", &trackster_time);
  tree_->Branch("timeError", &trackster_timeError);
  tree_->Branch("regressed_energy", &trackster_regressed_energy);
  tree_->Branch("raw_energy", &trackster_raw_energy);
  tree_->Branch("raw_em_energy", &trackster_raw_em_energy);
  tree_->Branch("raw_pt", &trackster_raw_pt);
  tree_->Branch("raw_em_pt", &trackster_raw_em_pt);
  tree_->Branch("barycenter_x", &trackster_barycenter_x);
  tree_->Branch("barycenter_y", &trackster_barycenter_y);
  tree_->Branch("barycenter_z", &trackster_barycenter_z);
  tree_->Branch("EV1", &trackster_EV1);
  tree_->Branch("EV2", &trackster_EV2);
  tree_->Branch("EV3", &trackster_EV3);
  tree_->Branch("sigmaPCA1", &trackster_sigmaPCA1);
  tree_->Branch("sigmaPCA2", &trackster_sigmaPCA2);
  tree_->Branch("sigmaPCA3", &trackster_sigmaPCA3);
  tree_->Branch("id_probabilities", &trackster_id_probabilities);

  tree_->Branch("vertices_indexes", &trackster_vertices_indexes);
  tree_->Branch("vertices_x", &trackster_vertices_x);
  tree_->Branch("vertices_y", &trackster_vertices_y);
  tree_->Branch("vertices_z", &trackster_vertices_z);
  tree_->Branch("vertices_energy", &trackster_vertices_energy);
  
  event_index = 0;
}

void TracksterNtupler::analyze(const edm::Event& event, const edm::EventSetup& setup) {
  event_index++;
  clearVariables();

  //get all the tracksters
  edm::Handle<std::vector<ticl::Trackster>> tracksters_handle;
  event.getByToken(tracksters_token_, tracksters_handle);
  const auto& tracksters = *tracksters_handle;

  //get all the layer clusters
  edm::Handle<std::vector<reco::CaloCluster>> layer_clusters_h;
  event.getByToken(layer_clusters_token_, layer_clusters_h);

  ev_event_ = event_index;
  ntracksters_ = tracksters.size();

  for (auto trackster_iterator = tracksters.begin(); trackster_iterator != tracksters.end(); ++trackster_iterator) {
      //per-trackster analysis
    trackster_iteration_index.push_back(trackster_iterator->ticlIteration());
    trackster_time.push_back(trackster_iterator->time());
    trackster_timeError.push_back(trackster_iterator->timeError());
    trackster_regressed_energy.push_back(trackster_iterator->regressed_energy());
    trackster_raw_energy.push_back(trackster_iterator->raw_energy());
    trackster_raw_em_energy.push_back(trackster_iterator->raw_em_energy());
    trackster_raw_pt.push_back(trackster_iterator->raw_pt());
    trackster_raw_em_pt.push_back(trackster_iterator->raw_em_pt());
    trackster_barycenter_x.push_back(trackster_iterator->barycenter().x());
    trackster_barycenter_y.push_back(trackster_iterator->barycenter().y());
    trackster_barycenter_z.push_back(trackster_iterator->barycenter().z());
    trackster_EV1.push_back(trackster_iterator->eigenvalues()[0]);
    trackster_EV2.push_back(trackster_iterator->eigenvalues()[1]);
    trackster_EV3.push_back(trackster_iterator->eigenvalues()[2]);
    trackster_sigmaPCA1.push_back(trackster_iterator->sigmasPCA()[0]);
    trackster_sigmaPCA2.push_back(trackster_iterator->sigmasPCA()[1]);
    trackster_sigmaPCA3.push_back(trackster_iterator->sigmasPCA()[2]);
    std::vector<float_t> id_probs;
    for (size_t i = 0; i < 8; i++)
      id_probs.push_back(trackster_iterator->id_probabilities(i));
    trackster_id_probabilities.push_back(id_probs);
    
    //loop over all associated clusters
    std::vector<uint16_t> vertices_indexes;
    std::vector<float_t> vertices_x;
    std::vector<float_t> vertices_y;
    std::vector<float_t> vertices_z;
    std::vector<float_t> vertices_energy;
    for (auto idx : trackster_iterator->vertices()) {
        vertices_indexes.push_back(idx);
        auto associated_cluster = (*layer_clusters_h)[idx];
        vertices_x.push_back(associated_cluster.x());
        vertices_y.push_back(associated_cluster.y());
        vertices_z.push_back(associated_cluster.z());
        vertices_energy.push_back(associated_cluster.energy());
    }
    trackster_vertices_indexes.push_back(vertices_indexes);
    trackster_vertices_x.push_back(vertices_x);
    trackster_vertices_y.push_back(vertices_y);
    trackster_vertices_z.push_back(vertices_z);
    trackster_vertices_energy.push_back(vertices_energy);
  }

  tree_->Fill();
}

void TracksterNtupler::endJob() {
  TFile* out = new TFile(outfilePath_.c_str(), "RECREATE");
  tree_->Write();
  out->Close();
}

void TracksterNtupler::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(TracksterNtupler);
