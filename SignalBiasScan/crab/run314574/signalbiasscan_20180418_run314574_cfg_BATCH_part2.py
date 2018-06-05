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

list = ['/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/26E63573-7644-E811-96DC-02163E017FCA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/28882B67-7F44-E811-87CB-FA163EB5923B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/2A569ACB-8144-E811-94DB-FA163E2E929A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/2AFFA3C8-8644-E811-A137-FA163E8561FE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/2CC9BE61-8944-E811-B5E7-FA163EB750C4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/2E6998AC-8644-E811-A0DD-FA163E3FF064.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/2E81D864-7644-E811-BCEA-FA163EE4B838.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3002F3D3-8244-E811-885A-FA163E246552.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3040DFD1-8244-E811-A201-FA163E3637B6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/343878CC-7A44-E811-8741-FA163E361A4E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/344948CF-8244-E811-B717-FA163E23298B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/34B6812C-7F44-E811-8B5B-FA163ECEF674.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/34E5E8AC-8644-E811-9ED7-FA163E1B3BA5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/368593CC-A644-E811-ABB9-02163E00BD01.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/36A867D9-7A44-E811-9DDD-FA163EEEA2CF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/36E57C79-9744-E811-9089-02163E019FA5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3897A5D9-8244-E811-BF24-FA163ED54054.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/38E40CD5-8244-E811-8132-FA163ED844E3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3A3BC9DD-8244-E811-9B69-FA163E81D8C9.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3A4057F3-7A44-E811-8705-02163E00B6DB.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3A49ABAD-8644-E811-995E-FA163E0B5F3C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3AC6A4DB-8244-E811-8545-FA163E94348E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3AF15D23-7F44-E811-BB1D-FA163E5F0085.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3C2C2BF7-8244-E811-B4E1-FA163EB31FD0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3C5492AA-8644-E811-8F4F-FA163EF3E48B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3C68966C-7F44-E811-9CDF-FA163EA0A2DC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3CAB1286-8844-E811-B910-FA163EB1421F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/3EAEECFC-8244-E811-96D9-FA163E42EFA6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/40606D66-7644-E811-852E-FA163EE58090.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/406D649D-8144-E811-A0E9-02163E00ADA6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/40BBA826-7F44-E811-9D6B-FA163EF047E6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/40E098D9-7A44-E811-8515-FA163E4E7054.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/42315430-7F44-E811-9A8A-FA163EE8C7E8.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/42724646-8644-E811-9A72-FA163ECEF674.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/428678D1-8244-E811-AA62-FA163E81D8C9.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/444C4904-8A44-E811-B8C2-FA163E255430.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/4604B0AA-8644-E811-B9B1-FA163E550C54.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/464310D1-7A44-E811-A30C-02163E017F17.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/480401CF-8244-E811-85B6-FA163E75AD21.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/4AC8CB2D-7F44-E811-B89B-FA163E198CD6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/4CA5FCE4-7B44-E811-8B7E-FA163ECAF4E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/4EB6757A-9144-E811-9719-02163E016539.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/5040A8A0-7F44-E811-84F6-FA163E6E5A75.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/509C39C7-7A44-E811-A916-FA163E62E0F4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/50E5A053-7F44-E811-8271-FA163E54F4FF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/522CF12D-7F44-E811-BC6E-FA163E3E7D02.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/525E6FAD-8644-E811-A223-FA163E2149D4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/528F5BCE-7A44-E811-B87B-FA163E29485B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/549E34B0-8644-E811-BCDB-FA163E8D6D0D.root']

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


