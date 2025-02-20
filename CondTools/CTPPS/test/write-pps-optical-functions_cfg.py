import FWCore.ParameterSet.Config as cms

process = cms.Process("writePPSOpticalFunctions")

# Load CondDB service
process.load("CondCore.CondDB.CondDB_cfi")

# output database (in this case local sqlite file)
process.CondDB.connect = 'sqlite_file:PPSOpticalFunctions_2025_v2.db'  #  with vertical pots

process.MessageLogger = cms.Service("MessageLogger",
                                    cout = cms.untracked.PSet(threshold = cms.untracked.string('INFO')),
                                    destinations = cms.untracked.vstring('cout')
                                    )

# A data source must always be defined. We don't need it, so here's a dummy one.
process.source = cms.Source("EmptyIOVSource",
    timetype = cms.string('runnumber'),
    # timetype = cms.string('lumiid'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)

# We define the output service.
process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.CondDB,
    timetype = cms.untracked.string('runnumber'),
    toPut = cms.VPSet(cms.PSet(
        record = cms.string('CTPPSOpticsRcd'),
        tag = cms.string('PPSOpticalFunctions_test')
    ))
)

process.cond_maker = cms.EDAnalyzer("PPSOpticalFunctionsSetCollectionWriter",
  opticalFunctionsEras = cms.VPSet(
    cms.PSet(
      dummy = cms.bool(False),
      validityRange = cms.EventRange("1:min - 999999:max"),
      opticalFunctions = cms.VPSet(
        # Check the appropriate source root files at and copy them to the working area:
        #
        #     /eos/cms/store/group/phys_pps/reconstruction/optical_functions/
        #
        cms.PSet( xangle = cms.double(120.000), fileName = cms.FileInPath("/afs/cern.ch/work/w/wcarvalh/pps/optical_functions/2025/CMSSW_15_0_0_pre3/src/CondTools/CTPPS/120urad.root") ),
        cms.PSet( xangle = cms.double(160.000), fileName = cms.FileInPath("/afs/cern.ch/work/w/wcarvalh/pps/optical_functions/2025/CMSSW_15_0_0_pre3/src/CondTools/CTPPS/160urad.root") )
      ),
      # z in cm
      scoringPlanes = cms.VPSet(
        # Horizontals
        cms.PSet( rpId = cms.uint32(2014838784), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.0) ),  # RP 003, pixel
        cms.PSet( rpId = cms.uint32(2056257536), dirName = cms.string("XRPH_A6L5_B2"), z = cms.double(-21507.8) ),  # RP 022, diamond
        cms.PSet( rpId = cms.uint32(2054160384), dirName = cms.string("XRPH_E6L5_B2"), z = cms.double(-21570.0) ),  # RP 016, diamond
#        cms.PSet( rpId = cms.uint32(2023227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel
        cms.PSet( rpId = cms.uint32(2025227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel
        # Verticals (2 pots per scoring plane)
        cms.PSet( rpId = cms.uint32(1979711488), dirName = cms.string("XRPV_C6L5_B2"), z = cms.double(-20337.7) ),  # RP 000, strip
        cms.PSet( rpId = cms.uint32(1980235776), dirName = cms.string("XRPV_C6L5_B2"), z = cms.double(-20337.7) ),  # RP 001, strip
        cms.PSet( rpId = cms.uint32(1981808640), dirName = cms.string("XRPV_D6L5_B2"), z = cms.double(-21300.0) ),  # RP 004, strip
        cms.PSet( rpId = cms.uint32(1982332928), dirName = cms.string("XRPV_D6L5_B2"), z = cms.double(-21300.0) ),  # RP 005, strip
        cms.PSet( rpId = cms.uint32(1988100096), dirName = cms.string("XRPV_A6L5_B2"), z = cms.double(-21462.8) ),  # RP 020, strip
        cms.PSet( rpId = cms.uint32(1988624384), dirName = cms.string("XRPV_A6L5_B2"), z = cms.double(-21462.8) ),  # RP 021, strip
        cms.PSet( rpId = cms.uint32(1990197248), dirName = cms.string("XRPV_B6L5_B2"), z = cms.double(-22000.0) ),  # RP 024, strip
        cms.PSet( rpId = cms.uint32(1990721536), dirName = cms.string("XRPV_B6L5_B2"), z = cms.double(-22000.0) ),  # RP 025, strip
        # Horizontals
        cms.PSet( rpId = cms.uint32(2031616000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.0) ),  # RP 103, pixel
        cms.PSet( rpId = cms.uint32(2073034752), dirName = cms.string("XRPH_A6R5_B1"), z = cms.double(+21507.8) ),  # RP 122, diamond
        cms.PSet( rpId = cms.uint32(2070937600), dirName = cms.string("XRPH_E6R5_B1"), z = cms.double(+21570.0) ),  # RP 116, diamond
        cms.PSet( rpId = cms.uint32(2040004608), dirName = cms.string("XRPH_B6R5_B1"), z = cms.double(+21955.0) ),  # RP 123, pixel
        # Verticals (2 pots per scoring plane)
        cms.PSet( rpId = cms.uint32(1996488704), dirName = cms.string("XRPV_C6R5_B1"), z = cms.double(+20337.7) ),  # RP 100, strip
        cms.PSet( rpId = cms.uint32(1997012992), dirName = cms.string("XRPV_C6R5_B1"), z = cms.double(+20337.7) ),  # RP 101, strip
        cms.PSet( rpId = cms.uint32(1998585856), dirName = cms.string("XRPV_D6R5_B1"), z = cms.double(+21300.0) ),  # RP 104, strip
        cms.PSet( rpId = cms.uint32(1999110144), dirName = cms.string("XRPV_D6R5_B1"), z = cms.double(+21300.0) ),  # RP 105, strip
        cms.PSet( rpId = cms.uint32(2004877312), dirName = cms.string("XRPV_A6R5_B1"), z = cms.double(+21462.8) ),  # RP 120, strip
        cms.PSet( rpId = cms.uint32(2005401600), dirName = cms.string("XRPV_A6R5_B1"), z = cms.double(+21462.8) ),  # RP 121, strip
        cms.PSet( rpId = cms.uint32(2006974464), dirName = cms.string("XRPV_B6R5_B1"), z = cms.double(+22000.0) ),  # RP 124, strip
        cms.PSet( rpId = cms.uint32(2007498752), dirName = cms.string("XRPV_B6R5_B1"), z = cms.double(+22000.0) ),  # RP 125, strip
      )
    )
  ),
  record = cms.string('CTPPSOpticsRcd'),
  loggingOn= cms.untracked.bool(True),
  SinceAppendMode=cms.bool(True),
  Source=cms.PSet(
    IOVRun=cms.untracked.uint32(1)
  )
)

process.path = cms.Path(process.cond_maker)