import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")


 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
# CMSSW523
process.GlobalTag.globaltag = 'GR_P_V42::All'

#Geometry and field
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Geometry.CommonDetUnit.globalTrackingGeometry_cfi")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("TrackingTools.RecoGeometry.RecoGeometries_cff")
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")


# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
    '/store/data/Run2012D/SingleElectron/RECO/PromptReco-v1/000/203/835/02692CE4-760B-E211-B9A1-BCAEC518FF8A.root'
    )
)


#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("clustersTree.root") )

# Loading "hltHighLevel_cfi" to select based on HLT bits
# now we can configure it
process.load('HLTrigger/HLTfilters/hltHighLevel_cfi')
process.hltHighLevel.andOr = True # True = OR, False = AND

process.DoublePhotonPaths = process.hltHighLevel.clone()
process.DoublePhotonPaths.HLTPaths = cms.vstring(
  "HLT_Photon26_CaloId10_Iso50_Photon18_CaloId10_Iso50_Mass60_v5", 
  "HLT_Photon26_CaloId10_Iso50_Photon18_R9Id85_Mass60_v5",
  "HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_R9Id85_OR_CaloId10_Iso50_Mass60_v5",
  "HLT_Photon26_R9Id85_Photon18_CaloId10_Iso50_Mass60_v5",
  "HLT_Photon26_R9Id85_Photon18_R9Id85_Mass60_v3",
  "HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50_v5",
  "HLT_Photon36_CaloId10_Iso50_Photon22_R9Id85_v5",
  "HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v5",
  "HLT_Photon36_R9Id85_Photon22_CaloId10_Iso50_v5",
  "HLT_Photon36_R9Id85_Photon22_R9Id85_v3")

process.SingleElectronPaths = process.hltHighLevel.clone()
process.SingleElectronPaths.HLTPaths = cms.vstring(
  "HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralPFNoPUJet30_BTagIPIter_v6",
  "HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_TriCentralPFNoPUJet30_v5",
  "HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_TriCentralPFNoPUJet50_40_30_v5",
  "HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet30_v1",
  "HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet50_40_30_v1",
  "HLT_Ele27_WP80_CentralPFJet80_v6",
  "HLT_Ele27_WP80_PFMET_MT50_v4",
  "HLT_Ele27_WP80_WCandPt80_v6",
  "HLT_Ele27_WP80_v10",
  "HLT_Ele32_WP80_CentralPFJet35_CentralPFJet25_PFMET20_v1",
  "HLT_Ele32_WP80_CentralPFJet35_CentralPFJet25_v1",
  "HLT_Ele32_WP80_PFJet30_PFJet25_Deta3_CentralPFJet30_v1",
  "HLT_Ele80_CaloIdVT_GsfTrkIdT_v1",
  "HLT_Ele90_CaloIdVT_GsfTrkIdT_v1"
)

process.SingleMuonPaths = process.hltHighLevel.clone()
process.SingleMuonPaths.HLTPaths = cms.vstring(
  "HLT_IsoMu17_eta2p1_CentralPFNoPUJet30_BTagIPIter_v1",
  "HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet30_v1",
  "HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet50_40_30_v1",
  "HLT_IsoMu20_eta2p1_CentralPFJet80_v6",
  "HLT_IsoMu20_eta2p1_WCandPt80_v6",
  "HLT_IsoMu24_CentralPFJet30_CentralPFJet25_PFMET20_v1",
  "HLT_IsoMu24_CentralPFJet30_CentralPFJet25_v1",
  "HLT_IsoMu24_PFJet30_PFJet25_Deta3_CentralPFJet25_v1",
  "HLT_IsoMu24_eta2p1_v13",
  "HLT_IsoMu24_v15",
  "HLT_IsoMu30_eta2p1_v13",
  "HLT_IsoMu30_v9",
  "HLT_IsoMu34_eta2p1_v11",
  "HLT_IsoMu40_eta2p1_v8",
  "HLT_L2Mu70_eta2p1_PFMET55_v1",
  "HLT_L2Mu70_eta2p1_PFMET60_v1",
  "HLT_Mu12_eta2p1_DiCentral_40_20_DiBTagIP3D1stTrack_v3",
  "HLT_Mu15_eta2p1_TriCentral_40_20_20_DiBTagIP3D1stTrack_v3",
  "HLT_Mu40_eta2p1_Track50_dEdx3p6_v3",
  "HLT_Mu40_eta2p1_Track60_dEdx3p7_v3",
  "HLT_Mu40_eta2p1_v9",
  "HLT_Mu40_v12",
  "HLT_Mu50_eta2p1_v6"
)

process.MinBiasPaths = process.hltHighLevel.clone()
process.MinBiasPaths.HLTPaths = cms.vstring(
  "HLT_Physics_v4",
  "HLT_Random_v2",
  "HLT_ZeroBias_v6"
)

process.DoublePhotonPaths_NoDoubleCounting = cms.Sequence( process.DoublePhotonPaths )
process.SingleElectronPaths_NoDoubleCounting = cms.Sequence( process.SingleElectronPaths + ~process.DoublePhotonPaths )
process.SingleMuonPaths_NoDoubleCounting = cms.Sequence( process.SingleMuonPaths + ~process.DoublePhotonPaths + ~process.SingleElectronPaths)
process.MET_NoDoubleCounting = cms.Sequence( ~process.MinBiasPaths )


process.demo = cms.EDAnalyzer('SignalBiasScan',
    trackLabel       = cms.InputTag('generalTracks'),
    tkTraj           = cms.InputTag('TrackRefitter'),
    labelTrajToTrack = cms.InputTag('TrackRefitter'),
    siStripClusters  = cms.InputTag('TrackRefitter'),
    primaryVertexColl= cms.InputTag('offlinePrimaryVertices'),
    fullHitInfo_TIB       = cms.bool(False),
    fullHitInfo_TOB       = cms.bool(False),
    fullHitInfo_TID       = cms.bool(False),
    fullHitInfo_TEC       = cms.bool(False),
    isSmallBiasScan       = cms.untracked.bool(False)
)


process.p = cms.Path(process.MET_NoDoubleCounting + process.TrackRefitter*process.demo)

