import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")


 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'

process.load('Configuration.StandardSequences.Services_cff')

#Geometry and field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")

process.load("RecoTracker.TrackProducer.TrackRefitters_cff")


# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_8_0_29/ZeroBias2/RECO/80X_dataRun2_Prompt_v6_RelVal_zb22016A-v1/00000/004B2663-3779-E711-B5B8-0025905B860E.root')
)

#/store/data/Run2016A/ZeroBias1/RECO/PromptReco-v2/000/271/056/00000/00693934-A00A-E611-97BE-02163E01391D.root #ZeroBias1
#/store/relval/CMSSW_8_0_29/ZeroBias2/RECO/80X_dataRun2_Prompt_v6_RelVal_zb22016A-v1/00000/004B2663-3779-E711-B5B8-0025905B860E.root #ZeroBias2


#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("clustersTree.root") )

process.TrackRefitter.NavigationSchool = cms.string("")


process.demo = cms.EDAnalyzer('SignalBiasScan',
    trackLabel       = cms.InputTag('generalTracks'),
    tkTraj           = cms.InputTag('TrackRefitter'),
    labelTrajToTrack = cms.InputTag('TrackRefitter'),
#    siStripClusters  = cms.InputTag('TrackRefitter'),
    primaryVertexColl= cms.InputTag('offlinePrimaryVertices'),
    fullHitInfo_TIB       = cms.bool(False),
    fullHitInfo_TOB       = cms.bool(False),
    fullHitInfo_TID       = cms.bool(False),
    fullHitInfo_TEC       = cms.bool(False),
    isSmallBiasScan       = cms.untracked.bool(False)
)


process.p = cms.Path(process.TrackRefitter*process.demo)

