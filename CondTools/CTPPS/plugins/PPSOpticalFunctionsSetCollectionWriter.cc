// -*- C++ -*-
//
// Class:      PPSOpticalFunctionsSetCollectionWriter
//
// Description: Test analyzer for PPS optical functions condition data
//
//              Writes one CTPPSOpticsRcd record into a sql database file.
//
// Original Author:  Wagner De Paula Carvalho
//         Created:  Wed, 20 Mar 2019
//         LHCOpticalFunctionsSetCollection access and filling adapted from 
//         CalibPPS/ESProducers/plugins/CTPPSOpticalFunctionsESSource.cc  by Jan Kašpar
//
//==================================================================================

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

#include "CondFormats/PPSObjects/interface/LHCOpticalFunctionsSetCollection.h"
#include "CondFormats/DataRecord/interface/CTPPSOpticsRcd.h"

#include <cstdint>

class PPSOpticalFunctionsSetCollectionWriter : public edm::one::EDAnalyzer<>
{
  public:
    PPSOpticalFunctionsSetCollectionWriter(const edm::ParameterSet&);
    ~PPSOpticalFunctionsSetCollectionWriter() override = default;

  private:
    void analyze(const edm::Event&, const edm::EventSetup&) override;

    // Introduced to allow "null" optical functions to be implemented for some IOV ranges
    bool m_dummyFunction;

    edm::EventRange m_validityRange;

    struct FileInfo
    {
      double xangle;
      std::string fileName;
    };
    std::vector<FileInfo> m_fileInfo;

    struct RPInfo
    {
      std::string dirName;
      double scoringPlaneZ;
    };
    
    struct AllInfo
    {
      bool dummy;
      edm::EventRange validity;
      std::vector<FileInfo> files;
      std::unordered_map<unsigned int, RPInfo> rp;
    };
    std::vector<AllInfo> m_allInfo;
};

//---------------------------------------------------------------------------------------

PPSOpticalFunctionsSetCollectionWriter::PPSOpticalFunctionsSetCollectionWriter(const edm::ParameterSet& conf) 
{
  for (const auto &pall : conf.getParameter<std::vector<edm::ParameterSet>>("opticalFunctionsEras"))
  {
    m_dummyFunction = pall.getParameter<bool>("dummyFunction");
    m_validityRange = pall.getParameter<edm::EventRange>("validityRange");
    edm::LogInfo("PPSOpticalFunctionsSetCollectionWriter::analyze") 
      << " startRun = " << m_validityRange.startRun() << " , startLumi = " << m_validityRange.startLumi()  
      << " endRun = " << m_validityRange.endRun() << " , endLumi = " << m_validityRange.endLumi() ; 
    
    m_fileInfo.clear();
    for (const auto &pset : pall.getParameter<std::vector<edm::ParameterSet>>("opticalFunctions"))
    {
      const double &xangle = pset.getParameter<double>("xangle");
      const std::string &fileName = pset.getParameter<edm::FileInPath>("fileName").fullPath();
      m_fileInfo.push_back({xangle, fileName});
    }

    std::unordered_map<unsigned int, RPInfo> m_rpInfo;
    for (const auto &pset : pall.getParameter<std::vector<edm::ParameterSet>>("scoringPlanes"))
    {
      const unsigned int rpId = pset.getParameter<unsigned int>("rpId");
      const std::string dirName = pset.getParameter<std::string>("dirName");
      const double z = pset.getParameter<double>("z");
      const RPInfo entry = {dirName, z};
      m_rpInfo.emplace(rpId, entry);
    }
    
    m_allInfo.push_back({m_dummyFunction , m_validityRange , m_fileInfo , m_rpInfo});
  }
}

//---------------------------------------------------------------------------------------

void PPSOpticalFunctionsSetCollectionWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  for (const auto &ai : m_allInfo) {
  LHCOpticalFunctionsSetCollection p;
  cond::Time_t iov = ai.validity.startRun();
    for (const auto &fi : ai.files)
    {
      std::unordered_map<unsigned int, LHCOpticalFunctionsSet> xa_data;
      if(!ai.dummy) {
        std::stringstream outstr;
        outstr << "\n xAngle = " << fi.xangle;
        for (const auto &rpi : ai.rp)
        {
          LHCOpticalFunctionsSet fcn(fi.fileName, rpi.second.dirName, rpi.second.scoringPlaneZ);
          xa_data.emplace(rpi.first, std::move(fcn));
          // edm::LogInfo("PPSOpticalFunctionsSetCollectionWriter::analyze") << "ScoringPlaneZ = " << fcn.getScoringPlaneZ();
          outstr << "\n ScoringPlaneZ = " << fcn.getScoringPlaneZ();
        }
        p.emplace(fi.xangle, xa_data);
        edm::LogInfo("PPSOpticalFunctionsSetCollectionWriter::analyze") << outstr.str();
      }
    }
    // Write to database or sqlite file
    edm::Service<cond::service::PoolDBOutputService> poolDbService;
    if(poolDbService.isAvailable())
      poolDbService->writeOneIOV( p,iov,"CTPPSOpticsRcd");
    else
      throw std::runtime_error("PoolDBService required.");
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(PPSOpticalFunctionsSetCollectionWriter);