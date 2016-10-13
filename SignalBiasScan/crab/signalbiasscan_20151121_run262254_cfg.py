import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")


 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
#process.GlobalTag.globaltag = '75X_dataRun2_Prompt_ppAt5TeV_v0'
process.GlobalTag.globaltag = '75X_dataRun2_Prompt_v1'

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
    '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ROOTFILES/testRun2015E.root')
    #'/store/data/Run2015E/MinimumBias1/RECO/PromptReco-v1/000/262/253/00000/08F15023-5C95-E511-A27D-02163E014101.root')
    #'/store/data/Run2015E/MinimumBias1/RECO/PromptReco-v1/000/261/395/00000/86AF0C71-318E-E511-89FB-02163E014139.root')
    #'/store/data/Run2015D/DoubleMuon/RECO/16Dec2015-v1/10000/002C811B-80A7-E511-8C4D-0CC47A4D7644.root')
    #'/store/data/Run2015D/SingleMuon/RECO/PromptReco-v4/000/259/682/00000/A0B466B5-5179-E511-A6D2-02163E011A1A.root')
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


