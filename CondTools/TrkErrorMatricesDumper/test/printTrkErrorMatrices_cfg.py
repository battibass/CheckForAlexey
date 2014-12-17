import FWCore.ParameterSet.Config as cms

process = cms.Process("PrintTrkErrors")

runNumber = 231615

process.load("CondCore.DBCommon.CondDBSetup_cfi")

process.errorSource = cms.ESSource("PoolDBESSource",
                                   process.CondDBSetup,
                                   toGet = cms.VPSet(cms.PSet(record = cms.string('TrackerAlignmentErrorRcd'),
                                                              tag = cms.string('TrackerAlignmentError_2011Realistic_v1_mc')
                                                              )
                                                     ),
                                   connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ALIGNMENT')
                                   )

process.extendedErrorSource = cms.ESSource("PoolDBESSource",
                                   process.CondDBSetup,
                                   toGet = cms.VPSet(cms.PSet(record = cms.string('TrackerAlignmentErrorExtendedRcd'),
                                                              tag = cms.string('TrackerAlignmentError_2011Realistic_v1_mc')
                                                              )
                                                     ),
                                   connect = cms.string('frontier://FrontierProd/CMS_COND_ALIGN_000')
                                   )

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("EmptyIOVSource",
                            timetype = cms.string('runnumber'),
                            firstValue = cms.uint64(runNumber),
                            lastValue = cms.uint64(runNumber),
                            interval = cms.uint64(1)
                            )

process.printTrkErrors = cms.EDAnalyzer('TrkErrorMatricesDumper',
                               readErrors = cms.bool(True),
                               readExtendedErrors = cms.bool(True),
                               compare = cms.bool(True)
                               )


process.p = cms.Path(process.printTrkErrors)
