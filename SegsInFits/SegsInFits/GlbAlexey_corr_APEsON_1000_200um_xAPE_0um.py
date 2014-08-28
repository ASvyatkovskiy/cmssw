
# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --conditions auto:run2_mc -s RAW2DIGI,L1Reco,RECO,EI,VALIDATION,DQM --datatier GEN-SIM-RECO,DQMIO -n 10 --magField 38T_PostLS1 --eventcontent RECOSIM,DQM --no_exec --python_filename=step3.py
import FWCore.ParameterSet.Config as cms

process = cms.Process('Rec2')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('CommonTools.ParticleFlow.EITopPAG_cff')
process.load('Configuration.StandardSequences.Validation_cff')
process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
#    fileNames = cms.untracked.vstring('file:step2_DIGI.root')
    fileNames = cms.untracked.vstring(
'/store/group/phys_muon/giovanni/CMSSW_7_1_0_pre7/SingleMuPt1000_UP15/default/SingleMuPt1000_UP15_CMSSW_7_1_0_pre7_default_GEN-SIM-DIGI-RAW-HLTDEBUG.root'
    )
)

# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# new APEs
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.muonAPEs = cms.ESSource("PoolDBESSource", 
 process.CondDBSetup,
 connect = cms.string('sqlite_file:/afs/cern.ch/user/p/pakhotin/public/2014-06-20_ArtificialMuonGeometriesWithAPEs/muonGeometry_xSigma_200um_xAPE_0um.db'),
 toGet = cms.VPSet(cms.PSet(record = cms.string("DTAlignmentRcd"),
 tag = cms.string("DTAlignmentRcd")),
 cms.PSet(record = cms.string("DTAlignmentErrorRcd"),
 tag = cms.string("DTAlignmentErrorRcd")),
 cms.PSet(record = cms.string("CSCAlignmentRcd"),
 tag = cms.string("CSCAlignmentRcd")),
 cms.PSet(record = cms.string("CSCAlignmentErrorRcd"),
 tag = cms.string("CSCAlignmentErrorRcd"))
))
process.es_prefer_muonAPEs = cms.ESPrefer("PoolDBESSource","muonAPEs")

#process.Tracer = cms.Service("Tracer")

# Output definition
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

#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string('patTuple.root'),
#                               # save only events passing the full path
#                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                               # save PAT Layer 1 output; you need a '*' to                               # unpack the list of commands 'patEventContent'                      
#                               outputCommands = cms.untracked.vstring('drop *'),
#                               )

# Path and EndPath definitions
process.TFileService = cms.Service('TFileService', fileName=cms.string('zp2mu_histos_ON_pT1000_200um_xAPE_0um.root'))

process.p = cms.Path(process.RawToDigi* process.reconstruction* process.STAfromSegs * process.GLBfromSegs * process.TEVfromSegs * process.PickysegsInFits * process.GLBsegsInFits)
#process.outpath = cms.EndPath(process.out)

# rename output file
#process.out.fileName = cms.untracked.string('rcandidates.root')

#process.out.outputCommands += (['keep *_*_*_*'])


# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.postLS1Customs
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1 

#call to customisation function customisePostLS1 imported from SLHCUpgradeSimulations.Configuration.postLS1Customs
process = customisePostLS1(process)

# Automatic addition of the customisation function from SimGeneral.MixingModule.fullMixCustomize_cff
from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn 

#call to customisation function setCrossingFrameOn imported from SimGeneral.MixingModule.fullMixCustomize_cff
process = setCrossingFrameOn(process)
# End of customisation functions
