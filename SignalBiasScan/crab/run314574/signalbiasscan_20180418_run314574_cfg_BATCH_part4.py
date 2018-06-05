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

list = ['/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/807926C8-7A44-E811-90D4-FA163E2149D4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/80C9D772-9744-E811-9B7B-FA163E40F14B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8205FDDE-8644-E811-A52D-FA163E28FA06.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/821C4F27-8544-E811-8F75-FA163EED8EBE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8225A7A2-7644-E811-8DFD-FA163E96F630.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/828AC3D1-7A44-E811-8789-FA163E35307D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8295D56C-8644-E811-BEE0-FA163E18CD5B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/845E6DB9-8144-E811-8B38-FA163E7FF690.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/849F252C-7F44-E811-9343-FA163E5D26F2.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/84E47D79-7644-E811-90EA-FA163EB783E7.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/860799DA-7A44-E811-8D97-02163E014C8E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/861E9C39-7F44-E811-A4DE-FA163EA4240B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/86FFF1D4-7A44-E811-A6F9-FA163EA87350.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/882B9563-7F44-E811-A0BD-02163E00BB28.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/88D025A9-8644-E811-BF8A-FA163EEC9D8D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8A0629D9-8944-E811-8EB6-FA163E6FDDFC.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8A6A077E-7F44-E811-8337-FA163E282D47.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8A7179D4-8244-E811-B9DA-FA163E89304D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8A948F2B-7F44-E811-A4D6-FA163E365FB4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8AB63208-8344-E811-B376-FA163EFB71BE.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8AEACA2B-7F44-E811-BE26-FA163E6500B5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8C76AFDD-8244-E811-A47A-02163E01651C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8E3256AC-8644-E811-8DD4-FA163E550C54.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8E91A5D9-8244-E811-B116-FA163ED54054.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/8ED183B2-8644-E811-8713-02163E019FF2.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/929D1FCE-8644-E811-BAB3-FA163E7D17FA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/94082CD4-7A44-E811-A976-FA163EEF6020.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/944853B6-8144-E811-AFE1-FA163EFE4E9E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9455AB2B-7F44-E811-9BB7-FA163EE0B5EF.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/96366CCD-7A44-E811-B190-FA163E2A8777.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/969950DD-7D44-E811-B373-FA163E3F57D2.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/96CE1F32-7F44-E811-81E7-FA163E6D856B.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/96FC98B0-8644-E811-976E-FA163E57A90D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9862AFD4-7F44-E811-992D-FA163EDB988F.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/98A13663-7644-E811-9786-FA163E255430.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/98CC1DD1-8244-E811-90E5-FA163E81E325.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9A08A0C8-7A44-E811-8F11-FA163EBDCDFA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9A5E48DB-8244-E811-972F-FA163ED844E3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9A995FD2-7A44-E811-B70A-FA163E0D4BD4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9AD77D6C-7F44-E811-9FFC-FA163EE06BA5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9ADBDDD2-8244-E811-AE12-FA163E81D8C9.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9AF1C1DB-7A44-E811-B681-FA163EBF9422.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9C79C7BF-7F44-E811-9725-FA163E438CE5.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9CC81CE1-8244-E811-824C-FA163E94348E.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9CC956DB-7A44-E811-8CAD-FA163E2899C3.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9CE5AB2E-7F44-E811-B060-FA163E11C3B8.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9E2A88D3-7A44-E811-82FF-FA163E7D17FA.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/9E4E662C-7F44-E811-A28C-FA163E6F0F4A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A08841CE-7A44-E811-AB94-FA163ED071E1.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A0AFC799-7F44-E811-81F6-FA163E8EF985.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A0CF817B-8C44-E811-A817-02163E01A01A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A0EA1107-8344-E811-9CF9-02163E00C1E4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A0F2BDC7-8644-E811-A441-FA163EB273C1.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A26000B1-8644-E811-AB32-FA163EF5021A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A266DD52-7F44-E811-9712-FA163E2AB66C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A27799D0-7A44-E811-8A98-FA163EB9ECB4.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A2F92761-7F44-E811-B17F-FA163E3F496D.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A4249E62-7644-E811-B52E-FA163ED0C5E8.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A48542E0-7A44-E811-903D-02163E01A006.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A673275C-7F44-E811-AC8F-FA163E600203.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A69E3947-7F44-E811-831D-FA163EB2A47C.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A810BF2F-7F44-E811-A29D-FA163EA73F03.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/A81120DA-9A44-E811-B62D-FA163EE6286A.root',
'/store/data/Commissioning2018/ZeroBias1/ALCARECO/SiStripCalMinBias-PromptReco-v1/000/314/574/00000/AA62065C-8A44-E811-81EC-FA163E856DA3.root']

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


