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

list = ['/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AC13EEAB-8644-E811-B0C2-FA163E417E96.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AC1BC32B-7F44-E811-915A-FA163EB1421F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/ACBFF52C-7F44-E811-9DE8-FA163E10B175.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/ACFED6AF-8644-E811-9DBA-FA163E455B73.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AE2A53AB-8644-E811-8385-FA163E63CDB5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AE4B2663-7644-E811-BBFE-FA163E96F2FF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AE777778-7F44-E811-BE16-02163E014EB7.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AED2847B-8C44-E811-AF0C-FA163EAB59EB.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AEE12335-7F44-E811-B8BC-FA163EB63606.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B02EC5DA-7A44-E811-8AB9-FA163E198CD6.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B0567FB0-8644-E811-BA48-FA163ECEF674.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B292A8C8-7A44-E811-BF09-FA163E0972CE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B2B09C5C-7F44-E811-836F-FA163E5E2735.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B4098AB8-7B44-E811-AB21-FA163E50B3BE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B46E474B-7644-E811-BFE6-FA163EAD8BBA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B4D1D929-7F44-E811-8387-02163E017EF3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B64141C8-7A44-E811-9D7E-FA163E250820.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B692508D-8C44-E811-853B-FA163ED0971C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/B6B772A6-8644-E811-BE43-FA163EE81CA0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/BA41FFDD-7A44-E811-934E-FA163E570424.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/BCB74DCC-7A44-E811-AE4C-FA163E836BB3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/BCC5FACF-7A44-E811-8F10-FA163EAD4CB1.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/BE513AC7-BA44-E811-8D34-02163E016529.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C0148BC6-8244-E811-A8F4-FA163EFD9234.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C01BD1DD-8244-E811-8851-02163E019F44.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C07729ED-8244-E811-A634-FA163EB68402.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C0CAF4A7-8644-E811-B9A9-FA163E617795.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C2194DD6-9A44-E811-BF80-FA163E4219A8.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C2865BDC-8244-E811-96D3-FA163EA2F461.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C40B3FE5-8244-E811-BDE2-FA163EA2F461.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C40E7BCE-8244-E811-9007-FA163E2C529F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C4CF476C-7F44-E811-B9E9-FA163E4670DE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C84609D5-7A44-E811-8526-FA163E768310.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C84C72D6-8244-E811-800A-FA163E246552.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C89D7DAD-8644-E811-A088-FA163E6967EB.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/C8F32845-7F44-E811-BA0C-FA163EFE4E9E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CA5928DD-7A44-E811-8371-02163E019F81.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CA96A5D9-8244-E811-AC93-FA163ED54054.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CC099121-7B44-E811-8628-FA163E6421AA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CC12FA0E-8A44-E811-94DB-FA163E669861.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CCDE8BB9-8644-E811-BFB3-FA163EF90EF0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CCE178B0-8644-E811-8A78-FA163E504868.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/CE4D7FFC-7A44-E811-BCDE-02163E0152EE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D03AE65B-7F44-E811-B9D6-FA163E11507C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D0474536-7F44-E811-B218-FA163E95A69B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D050A8D0-8244-E811-AB1C-FA163E81E325.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D066B583-8944-E811-8893-FA163EF1F9AF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D0CB8D28-7F44-E811-B08E-FA163E946B87.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D28EC2A9-8644-E811-BAE0-FA163E4CAB1F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D465790B-9F44-E811-AD8A-FA163E8ADB39.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D4F6D212-8A44-E811-A853-02163E019F71.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D6C626CC-8244-E811-A584-FA163E2C529F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D6DA90CE-7A44-E811-B7E5-FA163E3BD878.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D6E3FCD1-7A44-E811-94A1-FA163EE02984.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D6E94518-7C44-E811-A2D5-FA163E83B28A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/D8D2FF3C-8F44-E811-B20B-FA163E85E011.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/DA1DE663-7644-E811-B00D-FA163E802403.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/DA5E10B1-8644-E811-A147-FA163E5F04DA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/DAE6F2CA-7A44-E811-A9C6-FA163E038311.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/DC8B93DA-8244-E811-90E6-FA163E94348E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/DEC49A73-7F44-E811-9944-FA163EDC4AC3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E03EFF54-8644-E811-A0C6-FA163E946B87.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E0FABBF6-8944-E811-91DA-02163E017FD0.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E20EF1B3-8644-E811-89AD-FA163E399AD5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E216582E-7F44-E811-A1AA-FA163E364471.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E293DAFB-8944-E811-BD71-FA163E726F24.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E29F5DD8-8244-E811-8EEF-02163E017F04.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E2DF40CC-7A44-E811-9222-FA163EA1976B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E421A830-7F44-E811-969A-FA163E24B9CF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E43920CF-8C44-E811-9CB9-FA163EBB47F8.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E6728A37-7F44-E811-A94A-FA163EB8F6B3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E68DE8F9-7A44-E811-8D8C-FA163E934A3C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E6FD60DC-8244-E811-978A-02163E019F44.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/E850A32F-7F44-E811-8CF5-FA163EFD98DC.root']

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


