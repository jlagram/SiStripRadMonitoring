import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")


 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
# /DoubleEG/Run2015C-PromptReco-v1/RECO CMSSW_7_4_8_patch1
# would be better to use 74X_dataRun2_Prompt_v1, but no change for Tk 
process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v0'

process.load('Configuration.StandardSequences.Services_cff')

#process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
#process.load("Geometry.TrackerGeometryBuilder.trackerGeometry_cfi")
#process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cfi")

#Geometry and field
#process.load("Configuration.Geometry.GeometryIdeal_cff")
#process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
#process.load("Geometry.CommonDetUnit.globalTrackingGeometry_cfi")
#process.load("Configuration.StandardSequences.Reconstruction_cff")
#process.load("TrackingTools.RecoGeometry.RecoGeometries_cff")
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")


# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
    '/store/data/Run2015C/DoubleEG/RECO/PromptReco-v1/000/254/790/00000/903DF699-E449-E511-9F33-02163E0143F6.root')
)


#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("clustersTree.root") )

process.TrackRefitter.NavigationSchool = cms.string("")


process.demo = cms.EDAnalyzer('SignalBiasScan',
    trackLabel       = cms.InputTag('generalTracks'),
    tkTraj           = cms.InputTag('TrackRefitter'),
    labelTrajToTrack = cms.InputTag('TrackRefitter'),
    siStripClusters  = cms.InputTag('TrackRefitter'),
    primaryVertexColl= cms.InputTag('offlinePrimaryVertices'),
    fullHitInfo_TIB       = cms.bool(True),
    fullHitInfo_TOB       = cms.bool(True),
    fullHitInfo_TID       = cms.bool(True),
    fullHitInfo_TEC       = cms.bool(True),
    isSmallBiasScan       = cms.untracked.bool(True)
)


process.p = cms.Path(process.TrackRefitter*process.demo)

