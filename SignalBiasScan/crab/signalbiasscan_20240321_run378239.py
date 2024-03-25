import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")

 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = '140X_dataRun3_Prompt_v2'

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
    fileNames = cms.untracked.vstring('file:/store/data/Run2023A/EphemeralZeroBias0/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/44c864d7-8c9a-4b5a-b6d4-ea811ce268ea.root',
'file:/store/data/Run2023A/EphemeralZeroBias0/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/ce621ae4-e3b2-4649-a05b-6aacf17b8a30.root',
'file:/store/data/Run2023A/EphemeralZeroBias1/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/96ae4bb8-721a-4716-ad95-e351249e07b1.root',
'file:/store/data/Run2023A/EphemeralZeroBias1/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/a96eb4fd-2c61-4c87-95a0-6774c9397e96.root',
'file:/store/data/Run2023A/EphemeralZeroBias10/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/79c68801-1ebe-48b9-8ec7-90ae2e8345cc.root',
'file:/store/data/Run2023A/EphemeralZeroBias10/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/fb3ff53b-6af6-448d-8db4-94ed74f3894a.root',
'file:/store/data/Run2023A/EphemeralZeroBias11/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/1a40d86d-b4c6-4c1e-a5c5-776e3e036e7a.root',
'file:/store/data/Run2023A/EphemeralZeroBias11/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/db3b7a30-0176-4be2-8478-107842a0b139.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias11/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/1ddaff70-51e3-4642-bfcc-30ea12b5c370.root', 
'file:/store/data/Run2023A/EphemeralZeroBias12/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/11b6c366-a6b2-401e-9443-0b7bf2b8162f.root',
'file:/store/data/Run2023A/EphemeralZeroBias12/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/42534af5-be50-4092-8b2c-d7fd89c38a87.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias12/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/3fb4e786-4b9b-496e-bc9c-6676b982a5ef.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias12/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/c59a2f20-bda7-4737-bf90-2e5d32b5c8b1.root',
'file:/store/data/Run2023A/EphemeralZeroBias13/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/259c26e4-a026-457e-85bb-bb069b057f64.root',
'file:/store/data/Run2023A/EphemeralZeroBias13/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/c79422ec-11d0-47dc-9804-f1b6c2c17607.root',
'file:/store/data/Run2023A/EphemeralZeroBias14/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/4dd4a09f-2b48-41fb-8e8e-5ba668ce4068.root',
'file:/store/data/Run2023A/EphemeralZeroBias14/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/7991e001-db31-4d61-9235-24cbf2762370.root',
'file:/store/data/Run2023A/EphemeralZeroBias15/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/e850c1c8-2207-46ea-a366-af82d2a7d4a6.root',
'file:/store/data/Run2023A/EphemeralZeroBias15/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/f455a80d-8ccb-4600-8ca3-d59df0f743b8.root',
'file:/store/data/Run2023A/EphemeralZeroBias16/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/377f839c-f70c-4183-ad11-50d143da6ca3.root',
'file:/store/data/Run2023A/EphemeralZeroBias16/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/592b48ef-6da4-4735-949e-676b5c900d12.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias16/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/2db88331-b1ec-45ad-b2bd-6030a9866733.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias16/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/dd4741b2-0587-48e0-9cb0-0f3444d6727e.root',
'file:/store/data/Run2023A/EphemeralZeroBias17/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/499d852a-0e70-4d4a-889b-6c891f89b20d.root',
'file:/store/data/Run2023A/EphemeralZeroBias17/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/983496fe-fcbd-40ad-a778-40591cfdc307.root',
'file:/store/data/Run2023A/EphemeralZeroBias18/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/20e00d55-677e-41d0-bb2f-82fa40d74a7b.root',
'file:/store/data/Run2023A/EphemeralZeroBias18/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/621d7e78-249e-40e1-9eb8-ca91ec7bf878.root',
'file:/store/data/Run2023A/EphemeralZeroBias19/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/71d5a84c-00cf-4e44-b69b-662825f953c4.root',
'file:/store/data/Run2023A/EphemeralZeroBias19/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/84589ebf-5fe3-4228-811d-b250e358d99a.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias19/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/678747b8-ea13-4d94-800e-4cd98e3ffa46.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias19/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/ab4b5bb8-85fd-4fe9-a612-f2ea414acdf8.root',
'file:/store/data/Run2023A/EphemeralZeroBias2/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/a24804ed-b3f3-49c8-ac47-eea87a08da74.root',
'file:/store/data/Run2023A/EphemeralZeroBias2/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/c6cce57b-d6bb-41b4-9761-5013d9ccea46.root',
'file:/store/data/Run2023A/EphemeralZeroBias3/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/73a6afe5-625a-4919-9ebe-3ffcad6f9d9d.root',
'file:/store/data/Run2023A/EphemeralZeroBias3/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/dc4ec6c3-97fb-4f34-8882-c4d2feb6d454.root',
'file:/store/data/Run2023A/EphemeralZeroBias4/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/0d1026aa-e894-4e03-88cd-03cd1bffc663.root',
'file:/store/data/Run2023A/EphemeralZeroBias4/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/f354d68b-9e12-4dab-9ead-7cd835ee1cf5.root',
'file:/store/data/Run2023A/EphemeralZeroBias5/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/496b6f40-a080-4bb6-980a-f7dfb6321bec.root',
'file:/store/data/Run2023A/EphemeralZeroBias5/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/8e3324be-ae65-4fd7-a385-063fa7163993.root',
'file:/store/data/Run2023A/EphemeralZeroBias6/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/12c44478-7da5-40a8-96bc-3e95dc880b85.root',
'file:/store/data/Run2023A/EphemeralZeroBias6/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/265490da-6883-4430-a878-5a3501dfbd18.root',
'file:/store/data/Run2023A/EphemeralZeroBias7/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/d7de7920-6bfa-442e-80aa-2dda826bb8ed.root',
'file:/store/data/Run2023A/EphemeralZeroBias7/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/ee222a8e-d0dc-49d2-a984-296f0ce72ee8.root',
'file:/store/data/Run2023A/EphemeralZeroBias8/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/458d6bfc-4fe9-4fea-9c90-8d08b1c2713b.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias8/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/20272ab8-a19b-4ec1-8b4d-d8fa18145b34.root',
# 'file:/store/data/Run2023A/EphemeralZeroBias8/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/30000/86cda80b-9f40-42f5-88bd-f69ffa0a9080.root',
'file:/store/data/Run2023A/EphemeralZeroBias9/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/14ba3180-7aba-4689-8f98-1756d7be9ede.root',
'file:/store/data/Run2023A/EphemeralZeroBias9/ALCARECO/SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/2540000/36dfd56d-3378-4f67-9951-5f41b01eb507.root'))


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


