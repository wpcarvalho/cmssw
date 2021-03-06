#include "PhysicsTools/PatAlgos/interface/MuonMvaEstimator.h"

#include "CondFormats/EgammaObjects/interface/GBRForest.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"

#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"

using namespace pat;

namespace {
  constexpr char muon_mva_name[] = "BDTG";
}

MuonMvaEstimator::MuonMvaEstimator(const std::string& weightsfile, float dRmax):
  dRmax_(dRmax)
{
  TMVA::Reader tmvaReader("!Color:!Silent:Error");
  tmvaReader.AddVariable("LepGood_pt",                    &pt_               );
  tmvaReader.AddVariable("LepGood_eta",                   &eta_              );
  tmvaReader.AddVariable("LepGood_jetNDauChargedMVASel",  &jetNDauCharged_   );
  tmvaReader.AddVariable("LepGood_miniRelIsoCharged",     &miniRelIsoCharged_);
  tmvaReader.AddVariable("LepGood_miniRelIsoNeutral",     &miniRelIsoNeutral_);
  tmvaReader.AddVariable("LepGood_jetPtRelv2",            &jetPtRel_         );
  tmvaReader.AddVariable("min(LepGood_jetPtRatiov2,1.5)", &jetPtRatio_       );
  tmvaReader.AddVariable("max(LepGood_jetBTagCSV,0)",     &jetBTagCSV_       );
  tmvaReader.AddVariable("LepGood_sip3d",                 &sip_              );
  tmvaReader.AddVariable("log(abs(LepGood_dxy))",         &log_abs_dxyBS_    );
  tmvaReader.AddVariable("log(abs(LepGood_dz))",          &log_abs_dzPV_     );
  tmvaReader.AddVariable("LepGood_segmentCompatibility",  &segmentCompatibility_);

  std::unique_ptr<TMVA::IMethod> temp( tmvaReader.BookMVA(muon_mva_name, weightsfile.c_str()) );
  gbrForest_.reset(new GBRForest( dynamic_cast<TMVA::MethodBDT*>( tmvaReader.FindMVA(muon_mva_name) ) ) );
}

MuonMvaEstimator::~MuonMvaEstimator() { }

namespace {

  enum inputIndexes {
      kPt,
      kEta,
      kJetNDauCharged,
      kMiniRelIsoCharged,
      kMiniRelIsoNeutral,
      kJetPtRel,
      kJetPtRatio,
      kJetBTagCSV,
      kSip,
      kLog_abs_dxyBS,
      kLog_abs_dzPV,
      kSegmentCompatibility,
      kLast
  };

  float ptRel(const reco::Candidate::LorentzVector& muP4, const reco::Candidate::LorentzVector& jetP4,
              bool subtractMuon=true)
  {
    reco::Candidate::LorentzVector jp4 = jetP4;
    if (subtractMuon) jp4-=muP4;
    float dot = muP4.Vect().Dot( jp4.Vect() );
    float ptrel = muP4.P2() - dot*dot/jp4.P2();
    ptrel = ptrel>0 ? sqrt(ptrel) : 0.0;
    return ptrel;
  }
}

float MuonMvaEstimator::computeMva(const pat::Muon& muon,
				  const reco::Vertex& vertex,
				  const reco::JetTagCollection& bTags,
                                  float& jetPtRatio,
                                  float& jetPtRel,
				  const reco::JetCorrector* correctorL1,
				  const reco::JetCorrector* correctorL1L2L3Res) const
{
  float var[kLast]{};

  var[kPt] = muon.pt();
  var[kEta] = muon.eta();
  var[kSegmentCompatibility] = muon.segmentCompatibility();
  var[kMiniRelIsoCharged] = muon.miniPFIsolation().chargedHadronIso();
  var[kMiniRelIsoNeutral] = muon.miniPFIsolation().neutralHadronIso();

  double dB2D  = fabs(muon.dB(pat::Muon::BS2D));
  double dB3D  = muon.dB(pat::Muon::PV3D);
  double edB3D = muon.edB(pat::Muon::PV3D);
  double dz    = fabs(muon.muonBestTrack()->dz(vertex.position()));
  var[kSip]  = edB3D>0?fabs(dB3D/edB3D):0.0;
  var[kLog_abs_dxyBS]     = dB2D>0?log(dB2D):0;
  var[kLog_abs_dzPV]      = dz>0?log(dz):0;

  //Initialise loop variables
  double minDr = 9999;
  double jecL1L2L3Res = 1.;
  double jecL1 = 1.;

  var[kJetPtRatio] = -99;
  var[kJetPtRel]   = -99;
  var[kJetBTagCSV] = -999;
  var[kJetNDauCharged] = -1;

  for (const auto& tagI: bTags){
    // for each muon with the lepton 
    double dr = deltaR(*(tagI.first), muon);
    if(dr > minDr) continue;  
    minDr = dr;
      
    const reco::Candidate::LorentzVector& muP4(muon.p4()); 
    reco::Candidate::LorentzVector jetP4(tagI.first->p4());

    if (correctorL1 && correctorL1L2L3Res){
      jecL1L2L3Res = correctorL1L2L3Res->correction(*(tagI.first));
      jecL1 = correctorL1->correction(*(tagI.first));
    }

    // Get b-jet info
    var[kJetBTagCSV] = tagI.second;
    var[kJetNDauCharged] = 0;
    for (auto jet: tagI.first->getJetConstituentsQuick()){
      const reco::PFCandidate *pfcand = dynamic_cast<const reco::PFCandidate*>(jet);
      if (pfcand==nullptr) throw cms::Exception("ConfigurationError") << "Cannot get jet constituents";
      if (pfcand->charge()==0) continue;
      auto bestTrackPtr = pfcand->bestTrack();
      if (!bestTrackPtr) continue;
      if (!bestTrackPtr->quality(reco::Track::highPurity)) continue;
      if (bestTrackPtr->pt()<1.) continue;
      if (bestTrackPtr->hitPattern().numberOfValidHits()<8) continue;
      if (bestTrackPtr->hitPattern().numberOfValidPixelHits()<2) continue;
      if (bestTrackPtr->normalizedChi2()>=5) continue;

      if (std::fabs(bestTrackPtr->dxy(vertex.position())) > 0.2) continue;
      if (std::fabs(bestTrackPtr->dz(vertex.position())) > 17) continue;
      var[kJetNDauCharged]++;
    }

    if(minDr < dRmax_){
      if ((jetP4-muP4).Rho()<0.0001){ 
	var[kJetPtRel] = 0;
	var[kJetPtRatio] = 1;
      } else {
	jetP4 -= muP4/jecL1;
	jetP4 *= jecL1L2L3Res;
	jetP4 += muP4;
      
	var[kJetPtRatio] = muP4.pt()/jetP4.pt();
	var[kJetPtRel] = ptRel(muP4,jetP4);
      }
    }
  }

  if (var[kJetPtRatio] > 1.5) var[kJetPtRatio] = 1.5;
  if (var[kJetBTagCSV] < 0) var[kJetBTagCSV] = 0;
  jetPtRatio = var[kJetPtRatio];
  jetPtRel = var[kJetPtRel];
  return gbrForest_->GetClassifier(var);
};
