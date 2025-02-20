// -*- C++ -*-
//
// Class:      PPSOpticalFunctionsSetCollectionReader
//
// Description: Test analyzer for PPS optical functions condition data
//
//              Simple analyzer which dumps data from a CTPPSOpticsRcd record.
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

class PPSOpticalFunctionsSetCollectionReader : public edm::one::EDAnalyzer<>
{
  public:
    PPSOpticalFunctionsSetCollectionReader(const edm::ParameterSet&);
    ~PPSOpticalFunctionsSetCollectionReader() override = default;

  private:
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    edm::ESGetToken<LHCOpticalFunctionsSetCollection, CTPPSOpticsRcd> token_;
    bool m_verbose;
};

//---------------------------------------------------------------------------------------

PPSOpticalFunctionsSetCollectionReader::PPSOpticalFunctionsSetCollectionReader(const edm::ParameterSet& conf) 
  : token_(esConsumes<LHCOpticalFunctionsSetCollection, CTPPSOpticsRcd>()), 
  m_verbose(conf.getParameter<bool>("verbose")) {}

//---------------------------------------------------------------------------------------

void PPSOpticalFunctionsSetCollectionReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // Conditions data
  auto const& optics = iSetup.getData(token_);
  
  edm::LogInfo("PPSOpticalFunctionsSetCollectionReader") 
    << "LHCOpticalFunctionsSetCollection size = " << optics.size();
  
  // Iterate over collection
  for (const auto &ii : optics)
  {
    std::stringstream outstr;
    double xAngle = ii.first;
    std::unordered_map<unsigned int, LHCOpticalFunctionsSet> potMap = ii.second;
    for (const auto &ik : potMap)
    {
      const unsigned int potID = ik.first;
      LHCOpticalFunctionsSet lhcofs = ik.second;
//      std::stringstream outstr;
      outstr << "\n xAngle = " << xAngle 
             << " , rpID = " << std::hex << potID << " , z = " << lhcofs.getScoringPlaneZ() ;
      // Loop over xi and fcn values if verbose is set True
      if(m_verbose) {
        // xi
        const std::vector<double> xi_values = lhcofs.getXiValues();
        outstr << "\n xi = ";
        for (const auto &ixi : xi_values) outstr << ixi << "  ";
        // fcn values
        const std::vector<std::vector<double>> fcn_values = lhcofs.getFcnValues();
        outstr << "\n fcns = ";
        for (const auto &ifcn : fcn_values)
        {
          outstr << "\n";
          for (const auto &jv : ifcn) outstr << jv << "  ";
        }
      }
//      edm::LogInfo("PPSOpticalFunctionsSetCollectionReader") << outstr.str();
    }
    edm::LogInfo("PPSOpticalFunctionsSetCollectionReader") << outstr.str();
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(PPSOpticalFunctionsSetCollectionReader);

