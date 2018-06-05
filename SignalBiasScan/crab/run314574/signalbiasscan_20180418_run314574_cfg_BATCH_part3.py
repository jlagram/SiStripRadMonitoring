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

list = ['/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/54EBDF23-7F44-E811-8425-02163E01A07C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/56485AEA-7A44-E811-AFF5-02163E014CA9.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/568C172B-7F44-E811-AA06-FA163E2DF0C3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/569568DA-7A44-E811-8A0F-FA163E3EE95B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/56C0E1C7-7A44-E811-8FFB-FA163E74B19E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/56E2A131-7F44-E811-B151-FA163E1B044C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/580D01CD-7A44-E811-8A58-FA163EDF5523.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/58284E79-7F44-E811-B699-FA163E9A36E0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/58842DCA-7A44-E811-8730-FA163E85F2D7.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5C7946CC-7A44-E811-B95A-FA163E396B84.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5CC8AA04-8A44-E811-B81C-FA163E46FDBC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5CDD36CF-8244-E811-8D02-FA163E75AD21.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5EA25BD4-8244-E811-B7B8-FA163E071249.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5EDB5770-8944-E811-8616-FA163E8BA02A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/601BCAD7-8144-E811-88FB-FA163EF9D853.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/62BCE9F8-8944-E811-A843-FA163EF8ED87.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/62F8C8D0-8244-E811-BE4E-FA163E2C529F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/643E7ED8-8244-E811-A335-FA163EA2F461.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/64936DCE-7A44-E811-9D13-FA163E9666C9.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/662FBA6F-7644-E811-8CBB-FA163E2DF0C3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/663260D1-7A44-E811-A75A-FA163E5CBB1F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/664BE1E1-8E44-E811-95DE-FA163E2C529F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/666252D9-8244-E811-B04A-02163E019F44.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6693607D-8644-E811-8FB0-FA163E546B17.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/66B0E9F3-8244-E811-AE99-FA163E3637B6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/66E6D5AF-8644-E811-B62D-FA163E4BE105.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6816B163-7F44-E811-B744-02163E01A024.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/68309A11-8344-E811-A238-02163E00C1E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6C11EEDB-7A44-E811-96A5-FA163E51B015.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6C26902F-7F44-E811-BF5C-FA163E4842F0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6C43BE48-7F44-E811-B791-FA163E1F44DD.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6E460F7E-7644-E811-8963-FA163EC04576.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/6E9725A7-8644-E811-9A2C-FA163EF84031.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/708D6F58-7F44-E811-A51E-FA163EA12A19.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/70C9D110-8344-E811-B0CD-02163E00C1E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/70E35DAD-8644-E811-9AD2-02163E019FCC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/72DF8AAF-8644-E811-A960-FA163ECEF674.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/72F406C9-9544-E811-A7DC-FA163ED2F018.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7423BAFA-7A44-E811-B41B-FA163E04D7E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/74983ADA-8244-E811-BECC-02163E017F04.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/763DDED0-8244-E811-9415-FA163E46FDBC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7699D4E8-9A44-E811-AE15-FA163E1A95B7.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/76AEA1D6-7A44-E811-A52A-FA163E4B44F5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/76B63208-8344-E811-AAE5-FA163EFB71BE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/76BA963F-7F44-E811-8652-FA163E125808.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/76E192CB-7A44-E811-8A3C-FA163EA6A454.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/780CF266-7644-E811-9853-FA163E2B710A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/781B1932-7F44-E811-955C-FA163E9CFF9B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7835D5AD-8C44-E811-8908-FA163E4173E6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/787FF6D3-8244-E811-8F4B-FA163E89304D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/78889DD8-8244-E811-B874-02163E017F04.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7A2DC561-7644-E811-92C7-FA163E17588A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7A9C7A39-7F44-E811-9E10-FA163E08A9EB.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7AD25235-7F44-E811-A75C-FA163E4F1FDC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7C4E36AE-8644-E811-AF8A-FA163E3ECB15.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7CF45D6A-7F44-E811-A0F1-FA163E0A8DBA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7E0C9A1A-8344-E811-BEE8-02163E00C1E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/7E930EEB-8944-E811-BB28-FA163E55C616.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/802967AD-8644-E811-90C0-FA163E735BE1.root']

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


