import FWCore.ParameterSet.Config as cms

process = cms.Process("DDCMSDetectorTest")

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

process.MessageLogger = cms.Service("MessageLogger",
                                    cout = cms.untracked.PSet(threshold = cms.untracked.string('DEBUG')),
                                    destinations = cms.untracked.vstring('cout')
                                    )

process.DDDetectorESProducer = cms.ESSource("DDDetectorESProducer",
                                            confGeomXMLFiles = cms.FileInPath('DetectorDescription/DDCMS/data/cms-pps-geometry-2016.xml'),
                                            appendToDataLabel = cms.string('CMS')
                                            )

process.DDVectorRegistryESProducer = cms.ESProducer("DDVectorRegistryESProducer",
                                                    appendToDataLabel = cms.string('CMS')
                                                    )

process.test = cms.EDAnalyzer("DDCMSDetector",
                              DDDetector = cms.ESInputTag('','CMS')
                              )

process.testVectors = cms.EDAnalyzer("DDTestVectors",
                                     DDDetector = cms.ESInputTag('','CMS')
                                     )

process.testGeoIter = cms.EDAnalyzer("DDTestDumpGeometry",
                                     DDDetector = cms.ESInputTag('','CMS')
                                     )

process.p = cms.Path(
    process.test
    +process.testVectors
    +process.testGeoIter)
