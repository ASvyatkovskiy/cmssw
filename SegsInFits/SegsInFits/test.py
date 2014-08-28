import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process('Rec2')

#from RecoMuon.TrackingTools.MuonServiceProxy_cff import *

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
#process.load('Configuration.StandardSequences.ReMixingSeeds_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
#/RelValSingleMuPt1000/CMSSW_5_2_0_pre6-START60_V4-v3/GEN-SIM-DIGI-RAW-HLTDEBUG
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(
'/store/group/phys_muon/giovanni/CMSSW_7_1_0_pre7/SingleMuPt1000_UP15/default/SingleMuPt1000_UP15_CMSSW_7_1_0_pre7_default_GEN-SIM-DIGI-RAW-HLTDEBUG.root'
#'file:SingleMuPt1000_cfi_GEN_SIM_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco.root'
 )
)

import SimGeneral.MixingModule.mixNoPU_cfi
process.muonMix = SimGeneral.MixingModule.mixNoPU_cfi.mix.clone()

process.muonMix.playback = True
#process.GlobalTag.globaltag = 'START71_V8A::All'
#process.GlobalTag.globaltag = 'MC_71_V1::All'
#process.GlobalTag.globaltag = 'auto:run2_mc'
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# This is to load new CondDB
from CondCore.DBCommon.CondDBSetup_cfi import *

# Reading APEs 
#process.muonAPEs = cms.ESSource("PoolDBESSource", CondDBSetup,
# connect = cms.string('sqlite_file:/afs/cern.ch/user/p/pakhotin/public/muonAPEs_2014-06-05.db'),
# toGet = cms.VPSet(cms.PSet(record = cms.string("DTAlignmentRcd"),
# tag = cms.string("DTAlignmentRcd")),
# #cms.PSet(record = cms.string("DTAlignmentErrorRcd"),
# #tag = cms.string("DTAlignmentErrorRcd")),
# cms.PSet(record = cms.string("CSCAlignmentRcd"),
# tag = cms.string("CSCAlignmentRcd")),
# #cms.PSet(record = cms.string("CSCAlignmentErrorRcd"),
# #tag = cms.string("CSCAlignmentErrorRcd"))))
#))
#process.es_prefer_muonAPEs = cms.ESPrefer("PoolDBESSource","muonAPEs")


if 'debugdumps' in sys.argv:
    process.MessageLogger.cerr.threshold = 'DEBUG'
    process.MessageLogger.debugModules = cms.untracked.vstring('tevMuons')
    process.MessageLogger.categories.append('TrackFitters')
    process.MessageLogger.categories.append('GlobalMuonRefitter')


from RecoMuon.StandAloneMuonProducer import *
process.standAloneMuons.STATrajBuilderParameters.FilterParameters.MuonTrajectoryUpdatorParameters.Granularity = 1
process.standAloneMuons.STATrajBuilderParameters.BWFilterParameters.MuonTrajectoryUpdatorParameters.Granularity = 1

process.STAfromSegs = process.standAloneMuons.clone()
process.STAfromSegs.STATrajBuilderParameters.FilterParameters.MuonTrajectoryUpdatorParameters.Granularity = 0
process.STAfromSegs.STATrajBuilderParameters.BWFilterParameters.MuonTrajectoryUpdatorParameters.Granularity = 0

process.GLBfromSegs = process.globalMuons.clone()
process.GLBfromSegs.MuonCollectionLabel = cms.InputTag('STAfromSegs', 'UpdatedAtVtx')

process.TEVfromSegs = process.tevMuons.clone()
process.TEVfromSegs.MuonCollectionLabel = cms.InputTag('GLBfromSegs')

#adding picky
process.PickysegsInFits = cms.EDAnalyzer('SegsInFits',
                                       srcs = cms.VInputTag(cms.InputTag("tevMuons",    "picky", "Rec2"),
                                                            cms.InputTag("TEVfromSegs", "picky", "Rec2")
                                                            ),
                                       is_StA_run =  cms.bool(False),
                                       doPhiCorr = cms.bool(False),
                                       signSelection = cms.double(1.),
                                       )

process.GLBsegsInFits = cms.EDAnalyzer('SegsInFits',
                                       srcs = cms.VInputTag(cms.InputTag("globalMuons", "", "Rec2"),
                                                            cms.InputTag("GLBfromSegs", "", "Rec2")
                                                            ),
                                       is_StA_run =  cms.bool(False),
                                       doPhiCorr = cms.bool(False),
                                       signSelection = cms.double(1.),
                                       )

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               # save only events passing the full path
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
                               # save PAT Layer 1 output; you need a '*' to                               # unpack the list of commands 'patEventContent'                      
                               outputCommands = cms.untracked.vstring('drop *'),
                               )

#process.TFileService = cms.Service('TFileService', fileName=cms.string('zp2mu_histos_1000ON.root'))

process.p = cms.Path(process.RawToDigi* process.reconstruction) #* process.STAfromSegs * process.GLBfromSegs * process.TEVfromSegs * process.PickysegsInFits * process.GLBsegsInFits)
process.outpath = cms.EndPath(process.out)

# rename output file
process.out.fileName = cms.untracked.string('rcandidates.root')

process.out.outputCommands += (['keep *_*_*_*'])
