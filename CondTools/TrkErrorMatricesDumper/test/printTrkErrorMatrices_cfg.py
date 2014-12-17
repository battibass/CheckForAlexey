import FWCore.ParameterSet.Config as cms

process = cms.Process("PrintTrkErrors")

# CB : this runNumber is used to generate a dummy source with 1 empty 
# event but allows to get the database in an IOV basis, one hat to
# define run number on the basis of the IOVs listed with something like:
# cmscond_list_iov --connect frontier://FrontierProd/CMS_COND_31X_ALIGNMENT --tag TrackerAlignmentErr_2009_v2_express
# for ever run in since + 1 the run number has to be set and the comparison has to be run

runNumber = 231615

process.load("CondCore.DBCommon.CondDBSetup_cfi")

# CB here is the DB source to get the OLD error records
# the relevant tag has to be put there or a sqlite file could be used
process.errorSource = cms.ESSource("PoolDBESSource",
                                   process.CondDBSetup,
                                   toGet = cms.VPSet(cms.PSet(record = cms.string('TrackerAlignmentErrorRcd'),
                                                              tag = cms.string('TrackerAlignmentError_2011Realistic_v1_mc')
                                                              )
                                                     ),
                                   connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ALIGNMENT')
                                   )

# CB here is the DB source to get the NEW error records
# the relevant tag has to be put there or a sqlite file could be used
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
