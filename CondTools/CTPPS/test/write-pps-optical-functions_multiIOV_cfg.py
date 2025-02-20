import FWCore.ParameterSet.Config as cms

process = cms.Process("writePPSOpticalFunctions")

#from MyConditions.OpticalFunctions.ppsOpticalFunctionsWriter_cff import *

# Load CondDB service
process.load("CondCore.CondDB.CondDB_cfi")

# output database (in this case local sqlite file)
process.CondDB.connect = 'sqlite_file:PPSOpticalFunctions_2016-2022.db'

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
    # timetype = cms.untracked.string('timestamp'),
    # timetype = cms.untracked.string('lumiid'),
    toPut = cms.VPSet(cms.PSet(
        record = cms.string('CTPPSOpticsRcd'),
        tag = cms.string('PPSOpticalFunctions_test')
      )
    )
)

dummy_opt_functions = cms.PSet(
  dummy = cms.bool(True),
  validityRange = cms.EventRange("0:min - 1:max"),
  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(185), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2016_preTS2/version2/185urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
  )
)

config_base_2016_preTS2 = cms.PSet(
  dummy = cms.bool(False),
  validityRange = cms.EventRange("273725:min - 280385:max"),
  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(185), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2016_preTS2/version2/185urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(0x76100000), dirName = cms.string("XRPH_C6L5_B2"), z = cms.double(-20382.6) ),  # RP 002, strip
    cms.PSet( rpId = cms.uint32(0x76180000), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.1) ),  # RP 003, strip
    cms.PSet( rpId = cms.uint32(0x77100000), dirName = cms.string("XRPH_C6R5_B1"), z = cms.double(+20382.6) ),  # RP 102, strip
    cms.PSet( rpId = cms.uint32(0x77180000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.1) ),  # RP 103, strip
  )
)

config_base_2016_postTS2 = cms.PSet(
  dummy = cms.bool(False),
  validityRange = cms.EventRange("282730:min - 284044:max"),
  opticalFunctions = cms.VPSet(
    # fake placeholder at the moment
    cms.PSet( xangle = cms.double(140), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2016_postTS2/version2/140urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(0x76100000), dirName = cms.string("XRPH_C6L5_B2"), z = cms.double(-20382.6) ),  # RP 002, strip
    cms.PSet( rpId = cms.uint32(0x76180000), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.1) ),  # RP 003, strip
    cms.PSet( rpId = cms.uint32(0x77100000), dirName = cms.string("XRPH_C6R5_B1"), z = cms.double(+20382.6) ),  # RP 102, strip
    cms.PSet( rpId = cms.uint32(0x77180000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.1) ),  # RP 103, strip
  )
)

config_base_2017 = cms.PSet(
  dummy = cms.bool(False),
  # pre-TS2 begin to Jul alig. run
  validityRange = cms.EventRange("297046:min - 297723:max"),
  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(120), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2017/version5tim/120urad.root") ),
    cms.PSet( xangle = cms.double(130), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2017/version5tim/130urad.root") ),
    cms.PSet( xangle = cms.double(140), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2017/version5tim/140urad.root") )
    #
    # "version5tim" optical functions in CalibPPS/ESProducers/data/optical_functions/2017/ are different from the 
    # ones previously used: 
    #
    # cms.PSet( xangle = cms.double(120), fileName = cms.FileInPath("MyConditions/OpticalFunctions/data/2017/version5/120urad.root") ),
    # cms.PSet( xangle = cms.double(130), fileName = cms.FileInPath("MyConditions/OpticalFunctions/data/2017/version5/130urad.root") ),
    # cms.PSet( xangle = cms.double(140), fileName = cms.FileInPath("MyConditions/OpticalFunctions/data/2017/version5/140urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(0x76180000), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.1) ),  # RP 003, strip
    cms.PSet( rpId = cms.uint32(2023227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel
    cms.PSet( rpId = cms.uint32(0x77180000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.1) ),  # RP 103, strip
    cms.PSet( rpId = cms.uint32(2040004608), dirName = cms.string("XRPH_B6R5_B1"), z = cms.double(+21955.0) ),  # RP 123, pixel
  )
)

config_base_2018 = cms.PSet(
  dummy = cms.bool(False),
  # beginning to 90m runs
  validityRange = cms.EventRange("314747:min - 319077:max"),
  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(120), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/120urad.root") ),
    cms.PSet( xangle = cms.double(130), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/130urad.root") ),
    cms.PSet( xangle = cms.double(140), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/140urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(2014838784), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.1) ),  # RP 003, pixel
    cms.PSet( rpId = cms.uint32(2023227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel
    cms.PSet( rpId = cms.uint32(2031616000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.1) ),  # RP 103, pixel
    cms.PSet( rpId = cms.uint32(2040004608), dirName = cms.string("XRPH_B6R5_B1"), z = cms.double(+21955.0) ),  # RP 123, pixel
  )
)

'''
optics_2018 = cms.PSet(
  validityRange = cms.EventRange("314747:min - 325175:max"),

  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(120), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/120urad.root") ),
    cms.PSet( xangle = cms.double(130), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/130urad.root") ),
    cms.PSet( xangle = cms.double(140), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2018/version6/140urad.root") )
  ),

  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(2014838784), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.0) ),  # RP 003, pixel
    cms.PSet( rpId = cms.uint32(2054160384), dirName = cms.string("XRPH_E6L5_B2"), z = cms.double(-21570.0) ),  # RP 016, diamond
    cms.PSet( rpId = cms.uint32(2023227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel

    cms.PSet( rpId = cms.uint32(2031616000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.0) ),  # RP 103, pixel
    cms.PSet( rpId = cms.uint32(2070937600), dirName = cms.string("XRPH_E6R5_B1"), z = cms.double(+21570.0) ),  # RP 116, diamond
    cms.PSet( rpId = cms.uint32(2040004608), dirName = cms.string("XRPH_B6R5_B1"), z = cms.double(+21955.0) ),  # RP 123, pixel
  )
)
'''

config_base_2022 = cms.PSet(
  dummy = cms.bool(False),
  validityRange = cms.EventRange("343890:min - 999999:max"),
  opticalFunctions = cms.VPSet(
    cms.PSet( xangle = cms.double(144.974), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2022/version_pre1/144.974urad.root") ),
    cms.PSet( xangle = cms.double(160.000), fileName = cms.FileInPath("CalibPPS/ESProducers/data/optical_functions/2022/version_pre1/160.000urad.root") )
  ),
  scoringPlanes = cms.VPSet(
    # z in cm
    cms.PSet( rpId = cms.uint32(2014838784), dirName = cms.string("XRPH_D6L5_B2"), z = cms.double(-21255.0) ),  # RP 003, pixel
    cms.PSet( rpId = cms.uint32(2056257536), dirName = cms.string("XRPH_A6L5_B2"), z = cms.double(-21507.8) ),  # RP 022, diamond
    cms.PSet( rpId = cms.uint32(2054160384), dirName = cms.string("XRPH_E6L5_B2"), z = cms.double(-21570.0) ),  # RP 016, diamond
    cms.PSet( rpId = cms.uint32(2023227392), dirName = cms.string("XRPH_B6L5_B2"), z = cms.double(-21955.0) ),  # RP 023, pixel

    cms.PSet( rpId = cms.uint32(2031616000), dirName = cms.string("XRPH_D6R5_B1"), z = cms.double(+21255.0) ),  # RP 103, pixel
    cms.PSet( rpId = cms.uint32(2073034752), dirName = cms.string("XRPH_A6R5_B1"), z = cms.double(+21507.8) ),  # RP 122, diamond
    cms.PSet( rpId = cms.uint32(2070937600), dirName = cms.string("XRPH_E6R5_B1"), z = cms.double(+21570.0) ),  # RP 116, diamond
    cms.PSet( rpId = cms.uint32(2040004608), dirName = cms.string("XRPH_B6R5_B1"), z = cms.double(+21955.0) ),  # RP 123, pixel
  )
)


process.cond_maker = cms.EDAnalyzer("PPSOpticalFunctionsSetCollectionWriter",
  opticalFunctionsEras = cms.VPSet(
    #
    # Dummy configuration for runs before #273725
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("1:min - 273724:max")),
    #
    # 2016 pre-TS2 configuration
    #
    config_base_2016_preTS2,
    #
    # Dummy configuration for runs between 2016 pre-TS2 and 2016 post-TS2
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("280386:min - 282729:max")),
    #
    # 2016 post-TS2 configuration
    #
    config_base_2016_postTS2,
    #
    # Dummy configuration for runs between 2016 post-TS2 and 2017
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("284045:min - 297045:max")),
    #
    # 2017 configuration
    #
    config_base_2017.clone(validityRange = cms.EventRange("297046:min - 297723:max")),
    #
    # Dummy configuration to exclude aligment runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("297724:min - 298652:max")),
    #
    # 2017 configuration
    #
    config_base_2017.clone(validityRange = cms.EventRange("298653:min - 302663:max")),
    #
    # Dummy configuration to exclude aligment runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("302664:min - 303718:max")),
    #
    # 2017 configuration
    #
    config_base_2017.clone(validityRange = cms.EventRange("303719:min - 306462:max")),
    #
    # Dummy configuration between post-TS2 and W-mass runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("306463:min - 306895:max")),
    #
    # 2017 configuration
    #
    config_base_2017.clone(validityRange = cms.EventRange("306896:min - 307082:max")),
    #
    # Dummy configuration for runs between 2017 and 2018
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("307083:min - 314746:max")),
    #
    # 2018 configuration
    #
    config_base_2018.clone(validityRange = cms.EventRange("314747:min - 319077:max")),
    #
    # Dummy configuration for 90 m runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("319078:min - 319336:max")),
    #
    # 2018 configuration - 90 m runs to 900 GeV runs
    #
    config_base_2018.clone(validityRange = cms.EventRange("319337:min - 322633:max")),
    #
    # Dummy configuration to exclude aligment runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("322634:min - 323362:max")),
    #
    # 2018 configuration - 90 m runs to 900 GeV runs
    #
    config_base_2018.clone(validityRange = cms.EventRange("323363:min - 324420:max")),
    #
    # Dummy configuration for 900 GeV runs
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("324421:min - 324611:max")),
    #
    # 2018 configuration - 900 GeV runs to end
    #
    config_base_2018.clone(validityRange = cms.EventRange("324612:min - 325175:max")),
    #
    # Dummy configuration beyond 2018
    #
    dummy_opt_functions.clone(validityRange = cms.EventRange("325176:min - 343889:max")),
    #
    # 2018 configuration - 900 GeV runs to end
    #
    config_base_2022,
  ),
    record = cms.string('CTPPSOpticsRcd'),
    loggingOn= cms.untracked.bool(True),
    SinceAppendMode=cms.bool(True),
    Source=cms.PSet(
        IOVRun=cms.untracked.uint32(1)
    )
)

process.path = cms.Path(process.cond_maker)