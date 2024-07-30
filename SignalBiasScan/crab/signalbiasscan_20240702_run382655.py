import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")

 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = '140X_dataRun3_Prompt_v4"'

process.load('Configuration.StandardSequences.Services_cff')

#Geometry and field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")

process.load('RecoVertex.BeamSpotProducer.BeamSpot_cff')
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
process.TrackRefitter.src = 'ALCARECOSiStripCalMinBias'


# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)

# process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/store/data/Run2024F/ZeroBias/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/382/655/00000/09413ef7-635e-4dee-a4e2-051f6abb5a48.root',
))


#dataset1 =/ZeroBias/Run2023A-SiStripCalMinBias-PromptReco-v1/ALCARECO
#=> /store/data/Run2023A/ZeroBias/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/365/843/00000/8af2a99c-1ac2-487e-ba54-0d6b660780ba.root

#can also be this not sure:
#dataset=/StreamExpress/Run2023A-SiStripCalMinBias-Express-v1/ALCARECO

#dataset=/ZeroBias/Run2023A-SiStripCalZeroBias-PromptReco-v1/ALCARECO
#=> /store/data/Run2023A/ZeroBias/ALCARECO/SiStripCalZeroBias-PromptReco-v1/000/365/843/00000/da3afeba-f9d3-46cd-9169-88a5fc81dd16.root

#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("clustersTree.root") )

process.TrackRefitter.NavigationSchool = cms.string("")

process.demo = cms.EDAnalyzer('SignalBiasScan',
    trackLabel       = cms.InputTag('ALCARECOSiStripCalMinBias'), #generalTracks
    tkTraj           = cms.InputTag('TrackRefitter'),
    labelTrajToTrack = cms.InputTag('TrackRefitter'),
#   siStripClusters  = cms.InputTag('TrackRefitter'),
    primaryVertexColl= cms.InputTag('offlinePrimaryVertices'),
    fullHitInfo_TIB       = cms.bool(False),
    fullHitInfo_TOB       = cms.bool(False),
    fullHitInfo_TID       = cms.bool(False),
    fullHitInfo_TEC       = cms.bool(False),
    isSmallBiasScan       = cms.untracked.bool(False)
)
##Most common set of value for the booleans, Everything is true when smallscan, Evertything is false when Full scan

process.p = cms.Path(process.MeasurementTrackerEvent*process.offlineBeamSpot*process.TrackRefitter*process.demo)


