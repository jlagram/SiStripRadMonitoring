import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")

 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = '100X_dataRun2_Prompt_v2'
#94X_mc2017_realistic_v12

process.load('Configuration.StandardSequences.Services_cff')

#Geometry and field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")

process.load('RecoVertex.BeamSpotProducer.BeamSpot_cff')
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
process.TrackRefitter.src = 'ALCARECOSiStripCalMinBias'

list = ['/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E8F39EDB-7A44-E811-ADA4-FA163E54F658.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E8F8FEAB-8644-E811-B321-FA163E04B59F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EA2471EF-8E44-E811-91E9-FA163E06E101.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EA6065C2-8644-E811-88E5-FA163E521E8E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EAC83B76-7644-E811-8855-FA163EAB523C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EC5EFCA8-8644-E811-8E25-FA163EFE9C39.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/ECB9C2D1-8244-E811-BC81-FA163ED844E3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EE58B4C4-7A44-E811-94EA-FA163E279E4C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EE69AC57-7F44-E811-B770-02163E019E9A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EE7D0AD6-7A44-E811-9439-FA163ED2DE85.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/EEC2BAB6-8644-E811-9F92-FA163E18A122.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/F29D1CCD-7A44-E811-9FF1-02163E019F57.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/F4749F58-7F44-E811-83EE-FA163EB46049.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/F62F3330-7F44-E811-B542-FA163E70D57B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/F65B9C36-7F44-E811-BCDF-FA163E842886.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/F6E272DF-8244-E811-84BC-02163E01651C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FA23EDCD-7A44-E811-ADDC-FA163E6983E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FA3EF8CF-7A44-E811-AF39-FA163E5F04DA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FA6324CC-8244-E811-AF7D-FA163EDE4F34.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FA7F7BD6-8244-E811-8383-02163E019F44.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FABD3CB5-8644-E811-B793-02163E01A0DD.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FABE2808-8344-E811-916C-FA163EFB71BE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FC0A7156-7B44-E811-B1CC-FA163EC7B1EE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FC250DD7-7A44-E811-9A55-FA163E52F703.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FC7BD9CD-7A44-E811-8716-02163E01A0DD.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FC9952D4-8244-E811-8376-FA163E81E325.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FCAFD63D-7F44-E811-827F-FA163E1E1DBC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FCB094DA-8244-E811-B1F3-FA163ED54054.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/FCC03FD4-8244-E811-9937-FA163E46FDBC.root']

# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( *list ) )
    


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


process.p = cms.Path(process.MeasurementTrackerEvent*process.offlineBeamSpot*process.TrackRefitter*process.demo)
#process.p = cms.Path(process.offlineBeamSpot*process.TrackRefitter*process.demo) 


