/****************************************************************************
*  Based on CTPPSGeometryESModule.cc by:
*  Jan Kaspar (jan.kaspar@gmail.com)
*  Dominik Mierzejewski <dmierzej@cern.ch>
*
*  Rewritten + Moved out common functionailities to DetGeomDesc(Builder) by Gabrielle Hugo.
*  Migrated to DD4hep by Wagner Carvalho and Gabrielle Hugo.
*
****************************************************************************/

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/DDCMS/interface/DDCompactView.h"
#include "CondFormats/PPSObjects/interface/CTPPSRPAlignmentCorrectionsData.h"

#include "CondFormats/GeometryObjects/interface/PDetGeomDesc.h"
#include "CondFormats/AlignmentRecord/interface/RPRealAlignmentRecord.h"
#include "CondFormats/AlignmentRecord/interface/RPMisalignedAlignmentRecord.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/Records/interface/VeryForwardIdealGeometryRecord.h"
#include "Geometry/Records/interface/VeryForwardMisalignedGeometryRecord.h"
#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"

#include "Geometry/VeryForwardGeometryBuilder/interface/DetGeomDescBuilder.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSDDDNames.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/DetGeomDesc.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSGeometry.h"

#include <regex>

/**
 * \brief Builds ideal, real and misaligned geometries.
 *
 * First, it creates a tree of DetGeomDesc from DDCompView. For real and misaligned geometries,
 * it applies alignment corrections (RPAlignmentCorrections) found in corresponding ...GeometryRecord.
 *
 * Second, it creates CTPPSGeometry from DetGeoDesc tree.
 **/

using RotationMatrix = ROOT::Math::Rotation3D;
using Translation = ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>>;

class CTPPSGeometryESModule : public edm::ESProducer {
public:
  CTPPSGeometryESModule(const edm::ParameterSet&);
  ~CTPPSGeometryESModule() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  std::unique_ptr<DetGeomDesc> produceIdealGD(const IdealGeometryRecord&);
  std::unique_ptr<DetGeomDesc> produceIdealGDFromDB(const VeryForwardIdealGeometryRecord&);
  std::vector<int> fillCopyNos(TGeoIterator& it);

  std::unique_ptr<DetGeomDesc> produceRealGD(const VeryForwardRealGeometryRecord&);
  std::unique_ptr<DetGeomDesc> produceRealGDfromDB(const VeryForwardRealGeometryRecord&);
  std::unique_ptr<CTPPSGeometry> produceRealTG(const VeryForwardRealGeometryRecord&);

  std::unique_ptr<DetGeomDesc> produceMisalignedGD(const VeryForwardMisalignedGeometryRecord&);
  std::unique_ptr<DetGeomDesc> produceMisalignedGDfromDB(const VeryForwardMisalignedGeometryRecord&);
  std::unique_ptr<CTPPSGeometry> produceMisalignedTG(const VeryForwardMisalignedGeometryRecord&);

  template <typename REC , typename GEO>
  std::unique_ptr<DetGeomDesc> produceGD(const GEO&,
                                         const std::optional<REC>&,
                                         edm::ESGetToken<DetGeomDesc,GEO> const&,
                                         edm::ESGetToken<CTPPSRPAlignmentCorrectionsData,REC> const&,
                                         const char* name);

  static std::unique_ptr<DetGeomDesc> applyAlignments(const DetGeomDesc&, const CTPPSRPAlignmentCorrectionsData*);

  const unsigned int verbosity_;

  edm::ESGetToken<DDCompactView, IdealGeometryRecord> ddToken_;
  edm::ESGetToken<cms::DDCompactView, IdealGeometryRecord> dd4hepToken_;
  edm::ESGetToken<PDetGeomDesc, VeryForwardIdealGeometryRecord> dbToken_;
  const bool fromDB_, fromDD4hep_;

  edm::ESGetToken<DetGeomDesc, IdealGeometryRecord> idealGDToken_;
  edm::ESGetToken<DetGeomDesc, VeryForwardIdealGeometryRecord> idealDBGDToken_;
  edm::ESGetToken<CTPPSRPAlignmentCorrectionsData, RPRealAlignmentRecord> realAlignmentToken_;
  edm::ESGetToken<CTPPSRPAlignmentCorrectionsData, RPMisalignedAlignmentRecord> misAlignmentToken_;

  const edm::ESGetToken<DetGeomDesc, VeryForwardRealGeometryRecord> dgdRealToken_;
  const edm::ESGetToken<DetGeomDesc, VeryForwardMisalignedGeometryRecord> dgdMisToken_;
};

CTPPSGeometryESModule::CTPPSGeometryESModule(const edm::ParameterSet& iConfig)
    : verbosity_(iConfig.getUntrackedParameter<unsigned int>("verbosity")),
      fromDB_(iConfig.getUntrackedParameter<bool>("fromDB", false)),
      fromDD4hep_(iConfig.getUntrackedParameter<bool>("fromDD4hep", false)),
      dgdRealToken_{
          setWhatProduced(this, &CTPPSGeometryESModule::produceRealTG).consumes<DetGeomDesc>(edm::ESInputTag())},
      dgdMisToken_{
          setWhatProduced(this, &CTPPSGeometryESModule::produceMisalignedTG).consumes<DetGeomDesc>(edm::ESInputTag())} {
  if (fromDB_) {
    auto c = setWhatProduced(this, &CTPPSGeometryESModule::produceIdealGDFromDB);
    dbToken_ = c.consumes<PDetGeomDesc>(edm::ESInputTag("", iConfig.getParameter<std::string>("dbTag")));
    
    auto c1 = setWhatProduced(this, &CTPPSGeometryESModule::produceRealGDfromDB);
    idealDBGDToken_ = c1.consumesFrom<DetGeomDesc, VeryForwardIdealGeometryRecord>(edm::ESInputTag());
    realAlignmentToken_ = c1.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPRealAlignmentRecord>(edm::ESInputTag());
    
    auto c2 = setWhatProduced(this, &CTPPSGeometryESModule::produceMisalignedGDfromDB);
    misAlignmentToken_ = c2.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPMisalignedAlignmentRecord>(edm::ESInputTag());
  } else if (!fromDD4hep_) {
    auto c = setWhatProduced(this, &CTPPSGeometryESModule::produceIdealGD);
    ddToken_ = c.consumes<DDCompactView>(edm::ESInputTag("", iConfig.getParameter<std::string>("compactViewTag")));
    
    auto c1 = setWhatProduced(this, &CTPPSGeometryESModule::produceRealGD);
    idealGDToken_ = c1.consumesFrom<DetGeomDesc, IdealGeometryRecord>(edm::ESInputTag());
    realAlignmentToken_ = c1.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPRealAlignmentRecord>(edm::ESInputTag());
    
    auto c2 = setWhatProduced(this, &CTPPSGeometryESModule::produceMisalignedGD);
    misAlignmentToken_ = c2.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPMisalignedAlignmentRecord>(edm::ESInputTag());
  } else {
    auto c = setWhatProduced(this, &CTPPSGeometryESModule::produceIdealGD);
    dd4hepToken_ =
        c.consumes<cms::DDCompactView>(edm::ESInputTag("", iConfig.getParameter<std::string>("compactViewTag")));
    
    auto c1 = setWhatProduced(this, &CTPPSGeometryESModule::produceRealGD);
    idealGDToken_ = c1.consumesFrom<DetGeomDesc, IdealGeometryRecord>(edm::ESInputTag());
    realAlignmentToken_ = c1.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPRealAlignmentRecord>(edm::ESInputTag());
    
    auto c2 = setWhatProduced(this, &CTPPSGeometryESModule::produceMisalignedGD);
    misAlignmentToken_ = c2.consumesFrom<CTPPSRPAlignmentCorrectionsData, RPMisalignedAlignmentRecord>(edm::ESInputTag());
  }
}

void CTPPSGeometryESModule::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.addUntracked<unsigned int>("verbosity", 1);
  desc.add<std::string>("dbTag", std::string());
  desc.add<std::string>("compactViewTag", std::string());
  desc.addUntracked<bool>("fromDB", false);
  desc.addUntracked<bool>("fromDD4hep", false);
  descriptions.add("CTPPSGeometryESModule", desc);
}

//----------------------------------------------------------------------------------------------------
/*
 * Apply alignments by doing a BFS on idealGD tree.
 */
std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::applyAlignments(const DetGeomDesc& idealDetRoot,
                                                                    const CTPPSRPAlignmentCorrectionsData* alignments) {
  std::deque<const DetGeomDesc*> bufferIdealGeo;
  bufferIdealGeo.emplace_back(&idealDetRoot);

  std::deque<DetGeomDesc*> bufferAlignedGeo;
  DetGeomDesc* alignedDetRoot = new DetGeomDesc(idealDetRoot, DetGeomDesc::cmWithoutChildren);
  bufferAlignedGeo.emplace_back(alignedDetRoot);

  while (!bufferIdealGeo.empty()) {
    const DetGeomDesc* idealDet = bufferIdealGeo.front();
    DetGeomDesc* alignedDet = bufferAlignedGeo.front();
    bufferIdealGeo.pop_front();
    bufferAlignedGeo.pop_front();

    const std::string name = alignedDet->name();

    // Is it sensor? If yes, apply full sensor alignments
    if (name == DDD_TOTEM_RP_SENSOR_NAME || name == DDD_CTPPS_DIAMONDS_SEGMENT_NAME ||
        name == DDD_CTPPS_UFSD_SEGMENT_NAME || name == DDD_CTPPS_PIXELS_SENSOR_NAME ||
        name == DDD_CTPPS_PIXELS_SENSOR_NAME_2x2 || std::regex_match(name, std::regex(DDD_TOTEM_TIMING_SENSOR_TMPL))) {
      unsigned int plId = alignedDet->geographicalID();

      if (alignments) {
        const auto& ac = alignments->getFullSensorCorrection(plId);
        alignedDet->applyAlignment(ac);
      }
    }

    // Is it RP box? If yes, apply RP alignments
    if (name == DDD_TOTEM_RP_RP_NAME || name == DDD_CTPPS_DIAMONDS_RP_NAME || name == DDD_CTPPS_PIXELS_RP_NAME ||
        name == DDD_TOTEM_TIMING_RP_NAME) {
      unsigned int rpId = alignedDet->geographicalID();

      if (alignments) {
        const auto& ac = alignments->getRPCorrection(rpId);
        alignedDet->applyAlignment(ac);
      }
    }

    // create and add children
    const auto& idealDetChildren = idealDet->components();
    for (unsigned int i = 0; i < idealDetChildren.size(); i++) {
      const DetGeomDesc* idealDetChild = idealDetChildren[i];
      bufferIdealGeo.emplace_back(idealDetChild);

      // create new node with the same information as in idealDetChild and add it as a child of alignedDet
      DetGeomDesc* alignedDetChild = new DetGeomDesc(*idealDetChild, DetGeomDesc::cmWithoutChildren);
      alignedDet->addComponent(alignedDetChild);

      bufferAlignedGeo.emplace_back(alignedDetChild);
    }
  }
  return std::unique_ptr<DetGeomDesc>(alignedDetRoot);
}

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceIdealGD(const IdealGeometryRecord& iRecord) {
  
  if (!fromDD4hep_) {
    // Get the DDCompactView from EventSetup
    auto const& myCompactView = iRecord.get(ddToken_);

    // Build geo from compact view.
    return detgeomdescbuilder::buildDetGeomDescFromCompactView(myCompactView);
  }

  else {
    // Get the DDCompactView from EventSetup
    auto const& myCompactView = iRecord.get(dd4hepToken_);

    // Build geo from compact view.
    return detgeomdescbuilder::buildDetGeomDescFromCompactView(myCompactView);
  }
}

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceIdealGDFromDB(const VeryForwardIdealGeometryRecord& iRecord) {

    // Get the PDetGeomDesc from EventSetup
    auto const& myDB = iRecord.get(dbToken_);
    
    edm::LogInfo("CTPPSGeometryESModule") << " myDB size = " << myDB.container_.size();

    // Build geo from PDetGeomDesc DB object.
    auto pdet = std::make_unique<DetGeomDesc>(myDB);
    return pdet;

}

template <typename REC , typename GEO>
std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceGD(GEO const& iIdealRec,
                                        std::optional<REC> const& iAlignRec,
                                        edm::ESGetToken<DetGeomDesc,GEO> const& iGDToken,
                                        edm::ESGetToken<CTPPSRPAlignmentCorrectionsData,REC> const& iAlignToken,
                                        const char* name) {
  // get the input GeometricalDet
  auto const& idealGD = iIdealRec.get(iGDToken);

  // load alignments
  CTPPSRPAlignmentCorrectionsData const* alignments = nullptr;
  if (iAlignRec) {
    auto alignmentsHandle = iAlignRec->getHandle(iAlignToken);
    if (alignmentsHandle.isValid()) {
      alignments = alignmentsHandle.product();
    }
  }

  if (verbosity_) {
    if (alignments) {
      edm::LogVerbatim(name) << ">> " << name << " > Real geometry: " << alignments->getRPMap().size() << " RP and "
                             << alignments->getSensorMap().size() << " sensor alignments applied.";
    } else {
      edm::LogVerbatim(name) << ">> " << name << " > Real geometry: No alignment applied.";
    }
  }

  return applyAlignments(idealGD, alignments);
}

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceRealGDfromDB(const VeryForwardRealGeometryRecord& iRecord) {
  return produceGD(iRecord.getRecord<VeryForwardIdealGeometryRecord>(),
                   iRecord.tryToGetRecord<RPRealAlignmentRecord>(),
                   idealDBGDToken_,
                   realAlignmentToken_,
                   "CTPPSGeometryESModule::produceRealGDfromDB");
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceMisalignedGDfromDB(
    const VeryForwardMisalignedGeometryRecord& iRecord) {
  return produceGD(iRecord.getRecord<VeryForwardIdealGeometryRecord>(),
                   iRecord.tryToGetRecord<RPMisalignedAlignmentRecord>(),
                   idealDBGDToken_,
                   misAlignmentToken_,
                   "CTPPSGeometryESModule::produceMisalignedGDfromDB");
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceRealGD(const VeryForwardRealGeometryRecord& iRecord) {
  return produceGD(iRecord.getRecord<IdealGeometryRecord>(),
                   iRecord.tryToGetRecord<RPRealAlignmentRecord>(),
                   idealGDToken_,
                   realAlignmentToken_,
                   "CTPPSGeometryESModule::produceRealGD");
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<DetGeomDesc> CTPPSGeometryESModule::produceMisalignedGD(
    const VeryForwardMisalignedGeometryRecord& iRecord) {
  return produceGD(iRecord.getRecord<IdealGeometryRecord>(),
                   iRecord.tryToGetRecord<RPMisalignedAlignmentRecord>(),
                   idealGDToken_,
                   misAlignmentToken_,
                   "CTPPSGeometryESModule::produceMisalignedGD");
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<CTPPSGeometry> CTPPSGeometryESModule::produceRealTG(const VeryForwardRealGeometryRecord& iRecord) {
  auto const& gD = iRecord.get(dgdRealToken_);

  return std::make_unique<CTPPSGeometry>(&gD, verbosity_);
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<CTPPSGeometry> CTPPSGeometryESModule::produceMisalignedTG(
    const VeryForwardMisalignedGeometryRecord& iRecord) {
  auto const& gD = iRecord.get(dgdMisToken_);

  return std::make_unique<CTPPSGeometry>(&gD, verbosity_);
}

DEFINE_FWK_EVENTSETUP_MODULE(CTPPSGeometryESModule);
