import FWCore.ParameterSet.Config as cms
process = cms.Process("GeometryInfo")

# minimum of logs
process.MessageLogger = cms.Service("MessageLogger",
    statistics = cms.untracked.vstring(),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO')
    )
)

# geometry
process.load("CondCore.CondDB.CondDB_cfi")
# input database (in this case local sqlite file)
process.CondDB.connect = 'sqlite_file:/afs/cern.ch/work/w/wcarvalh/pps/geometry/db_payload/CMSSW_11_2_0_pre9/src/CondTools/Geometry/PPSGeometry_2018_fromDD4hep.db'

process.PoolDBESSource = cms.ESSource("PoolDBESSource",
    process.CondDB,
    DumpStat=cms.untracked.bool(True),
    toGet = cms.VPSet(
      cms.PSet(
        record = cms.string('VeryForwardIdealGeometryRecord'),
        tag = cms.string("PPSGeometry_test")
      )
    )
)

process.ctppsGeometryESModule = cms.ESProducer("CTPPSGeometryESModule",
    fromDB = cms.untracked.bool(True),
    fromDD4hep = cms.untracked.bool(False),
    verbosity = cms.untracked.uint32(1),
#    dbTag = cms.string('PPSGeometry_test'),
    dbTag = cms.string(''),
    compactViewTag = cms.string('XMLIdealGeometryESSource_CTPPS')
)


# load alignment correction
process.load("CalibPPS.ESProducers.ctppsRPAlignmentCorrectionsDataESSourceXML_cfi")
process.ctppsRPAlignmentCorrectionsDataESSourceXML.RealFiles = cms.vstring(
    "Geometry/VeryForwardGeometryBuilder/test/alignment_file_1.xml",
    "Geometry/VeryForwardGeometryBuilder/test/alignment_file_2.xml",
)
process.ctppsRPAlignmentCorrectionsDataESSourceXML.verbosity = 1

# no events to process
process.source = cms.Source("EmptySource",
  firstRun = cms.untracked.uint32(123),
  firstLuminosityBlock = cms.untracked.uint32(1),
  firstEvent = cms.untracked.uint32(1),
  numberEventsInLuminosityBlock = cms.untracked.uint32(3),
  numberEventsInRun = cms.untracked.uint32(30)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.ctppsGeometryInfo = cms.EDAnalyzer("CTPPSGeometryInfo",
    geometryType = cms.untracked.string("real"),
    printRPInfo = cms.untracked.bool(True),
    printSensorInfo = cms.untracked.bool(True)
)

process.p = cms.Path(
    process.ctppsGeometryInfo
)
