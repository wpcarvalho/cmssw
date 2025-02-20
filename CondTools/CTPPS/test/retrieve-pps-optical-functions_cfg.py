import FWCore.ParameterSet.Config as cms

process = cms.Process("retrievePPSOpticalFunctions")

# Load CondDB service
process.load("CondCore.CondDB.CondDB_cfi")

# Input database (in this case local sqlite file)
process.CondDB.connect = 'sqlite_file:PPSOpticalFunctions_2025_v1.db'

process.MessageLogger = cms.Service("MessageLogger",
                                    cout = cms.untracked.PSet(threshold = cms.untracked.string('INFO')),
                                    destinations = cms.untracked.vstring('cout')
                                    )

process.PoolDBESSource = cms.ESSource("PoolDBESSource",
    process.CondDB,
    DumpStat=cms.untracked.bool(True),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('CTPPSOpticsRcd'),
        tag = cms.string("PPSOpticalFunctions_test")
    )),
)

process.get = cms.EDAnalyzer("PPSOpticalFunctionsSetCollectionReader",
    verbose = cms.bool(False)
)

# A data source must always be defined. We don't need it, so here's a dummy one.
process.source = cms.Source("EmptyIOVSource",
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)

process.path = cms.Path(process.get)
