import FWCore.ParameterSet.Config as cms

process = cms.Process("Tree")

process.load("FWCore.MessageService.MessageLogger_cfi")

 # Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = '101X_dataRun2_Express_v7'

process.load('Configuration.StandardSequences.Services_cff')

#Geometry and field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")

#process.load('RecoVertex.BeamSpotProducer.BeamSpot_cff')
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
#process.TrackRefitter.src = 'ALCARECOSiStripCalMinBias'


# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/data/Run2018D/EGamma/RECO/21Aug2018-v1/100000/FEC9A5FC-55A5-E811-8DA8-A0369FE2C21E.root'))
    
#this file doesn't contain event from run 317683    

#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("clustersTree.root") )

process.TrackRefitter.NavigationSchool = cms.string("")


###########
# Loading "hltHighLevel_cfi" to select based on HLT bits
# now we can configure it
process.load('HLTrigger/HLTfilters/hltHighLevel_cfi')
process.hltHighLevel.andOr = True # True = OR, False = AND


process.EGammaPaths = process.hltHighLevel.clone()
process.EGammaPaths.HLTPaths = cms.vstring( 
	'HLT_DiEle27_WPTightCaloOnly_L1DoubleEG_v4',
    'HLT_DiSC30_18_EIso_AND_HE_Mass70_v13',
    'HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_NoPixelVeto_Mass55_v13',
    'HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_PixelVeto_Mass55_v15',
    'HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_NoPixelVeto_Mass55_v2',
    'HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_NoPixelVeto_v2',
    'HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90_v13',
    'HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95_v13',
    'HLT_DoubleEle25_CaloIdL_MW_v4',
    'HLT_DoubleEle27_CaloIdL_MW_v4',
    'HLT_DoubleEle33_CaloIdL_MW_v17',
    'HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v20',
    'HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v20',
    'HLT_DoublePhoton33_CaloIdL_v6',
    'HLT_DoublePhoton70_v6',
    'HLT_DoublePhoton85_v14',
    'HLT_ECALHT800_v10',
    'HLT_Ele115_CaloIdVT_GsfTrkIdT_v14',
    'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v18',
    'HLT_Ele135_CaloIdVT_GsfTrkIdT_v7',
    'HLT_Ele145_CaloIdVT_GsfTrkIdT_v8',
    'HLT_Ele15_CaloIdL_TrackIdL_IsoVL_PFJet30_v3',
    'HLT_Ele15_Ele8_CaloIdL_TrackIdL_IsoVL_v3',
    'HLT_Ele15_IsoVVVL_PFHT450_CaloBTagDeepCSV_4p5_v8',
    'HLT_Ele15_IsoVVVL_PFHT450_PFMET50_v16',
    'HLT_Ele15_IsoVVVL_PFHT450_v16',
    'HLT_Ele15_IsoVVVL_PFHT600_v20',
    'HLT_Ele15_WPLoose_Gsf_v3',
    'HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v9',
    'HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v16',
    'HLT_Ele17_WPLoose_Gsf_v3',
    'HLT_Ele200_CaloIdVT_GsfTrkIdT_v8',
    'HLT_Ele20_WPLoose_Gsf_v6',
    'HLT_Ele20_WPTight_Gsf_v6',
    'HLT_Ele20_eta2p1_WPLoose_Gsf_v6',
    'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v18',
    'HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v18',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v19',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v19',
    'HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTauHPS30_eta2p1_CrossL1_v1',
    'HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTauHPS30_eta2p1_TightID_CrossL1_v1',
    'HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTauHPS30_eta2p1_CrossL1_v1',
    'HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTauHPS30_eta2p1_TightID_CrossL1_v1',
    'HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTauHPS30_eta2p1_CrossL1_v1',
    'HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTauHPS30_eta2p1_TightID_CrossL1_v1',
    'HLT_Ele250_CaloIdVT_GsfTrkIdT_v13',
    'HLT_Ele27_Ele37_CaloIdL_MW_v4',
    'HLT_Ele27_WPTight_Gsf_v16',
    'HLT_Ele28_HighEta_SC20_Mass55_v13',
    'HLT_Ele28_WPTight_Gsf_v1',
    'HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v13',
    'HLT_Ele300_CaloIdVT_GsfTrkIdT_v13',
    'HLT_Ele30_WPTight_Gsf_v1',
    'HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v13',
    'HLT_Ele32_WPTight_Gsf_L1DoubleEG_v9',
    'HLT_Ele32_WPTight_Gsf_v15',
    'HLT_Ele35_WPTight_Gsf_L1EGMT_v5',
    'HLT_Ele35_WPTight_Gsf_v9',
    'HLT_Ele38_WPTight_Gsf_v9',
    'HLT_Ele40_WPTight_Gsf_v9',
    'HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v18',
    'HLT_Ele50_IsoVVVL_PFHT450_v16',
    'HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v16',
    'HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v18',
    'HLT_Photon100EBHE10_v2',
    'HLT_Photon100EB_TightID_TightIso_v2',
    'HLT_Photon100EEHE10_v2',
    'HLT_Photon100EE_TightID_TightIso_v2',
    'HLT_Photon110EB_TightID_TightIso_v2',
    'HLT_Photon120EB_TightID_TightIso_v2',
    'HLT_Photon120_R9Id90_HE10_IsoM_v14',
    'HLT_Photon120_v13',
    'HLT_Photon150_v6',
    'HLT_Photon165_R9Id90_HE10_IsoM_v15',
    'HLT_Photon175_v14',
    'HLT_Photon200_v13',
    'HLT_Photon20_HoverELoose_v10',
    'HLT_Photon20_v2',
    'HLT_Photon300_NoHE_v12',
    'HLT_Photon30_HoverELoose_v10',
    'HLT_Photon33_v5',
    'HLT_Photon50_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3_PFMET50_v5',
    'HLT_Photon50_R9Id90_HE10_IsoM_v14',
    'HLT_Photon50_v13',
    'HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350MinPFJet15_v11',
    'HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_v5',
    'HLT_Photon60_R9Id90_CaloIdL_IsoL_v5',
    'HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_CaloMJJ300_PFJetsMJJ400DEta3_v5',
    'HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_CaloMJJ400_PFJetsMJJ600DEta3_v5',
    'HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3_v5',
    'HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ600DEta3_v5',
    'HLT_Photon75_R9Id90_HE10_IsoM_v14',
    'HLT_Photon75_v13',
    #'HLT_Photon90_CaloIdL_PFHT700_v16',
    'HLT_Photon90_R9Id90_HE10_IsoM_v14',
    'HLT_Photon90_v13',
    'HLT_TriplePhoton_20_20_20_CaloIdLV2_R9IdVL_v3',
    'HLT_TriplePhoton_20_20_20_CaloIdLV2_v3',
    'HLT_TriplePhoton_30_30_10_CaloIdLV2_R9IdVL_v4',
    'HLT_TriplePhoton_30_30_10_CaloIdLV2_v4',
    'HLT_TriplePhoton_35_35_5_CaloIdLV2_R9IdVL_v4' )

process.SingleMuonPaths = process.hltHighLevel.clone()
process.SingleMuonPaths.HLTPaths = cms.vstring( 
	'HLT_IsoMu20_eta2p1_LooseChargedIsoPFTauHPS27_eta2p1_CrossL1_v4',
    'HLT_IsoMu20_eta2p1_LooseChargedIsoPFTauHPS27_eta2p1_TightID_CrossL1_v1',
    'HLT_IsoMu20_eta2p1_MediumChargedIsoPFTauHPS27_eta2p1_CrossL1_v1',
    'HLT_IsoMu20_eta2p1_MediumChargedIsoPFTauHPS27_eta2p1_TightID_CrossL1_v1',
    'HLT_IsoMu20_eta2p1_TightChargedIsoPFTauHPS27_eta2p1_CrossL1_v1',
    'HLT_IsoMu20_eta2p1_TightChargedIsoPFTauHPS27_eta2p1_TightID_CrossL1_v1',
    'HLT_IsoMu20_v15',
    'HLT_IsoMu24_TwoProngs35_v1',
    'HLT_IsoMu24_eta2p1_v15',
    'HLT_IsoMu24_v13',
    'HLT_IsoMu27_v16',
    'HLT_IsoMu30_v4',
    'HLT_L1SingleMu18_v3',
    'HLT_L1SingleMu25_v2',
    'HLT_L2Mu10_v7',
    'HLT_L2Mu50_v2',
    'HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60_v15',
    'HLT_Mu12_v3',
    'HLT_Mu15_IsoVVVL_PFHT450_CaloBTagDeepCSV_4p5_v8',
    'HLT_Mu15_IsoVVVL_PFHT450_PFMET50_v15',
    'HLT_Mu15_IsoVVVL_PFHT450_v15',
    'HLT_Mu15_IsoVVVL_PFHT600_v19',
    'HLT_Mu15_v3',
    'HLT_Mu20_v12',
    'HLT_Mu27_v13',
    'HLT_Mu3_PFJet40_v16',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu70_PFMHTNoMu70_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight_v2',
    'HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight_v2',
    'HLT_Mu4_TrkIsoVVL_DiPFJet90_40_DEta3p5_MJJ750_HTT300_PFMETNoMu60_v15',
    'HLT_Mu50_IsoVVVL_PFHT450_v15',
    'HLT_Mu50_v13',
    'HLT_Mu55_v3',
    'HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60_v16',
    'HLT_OldMu100_v3',
    'HLT_TkMu100_v2' )

process.ZeroBiasPaths = process.hltHighLevel.clone()
process.ZeroBiasPaths.HLTPaths = cms.vstring( 
	'HLT_Random_v3',
    'HLT_ZeroBias_Alignment_v1',
    'HLT_ZeroBias_FirstBXAfterTrain_v3',
    'HLT_ZeroBias_FirstCollisionAfterAbortGap_v5',
    'HLT_ZeroBias_FirstCollisionInTrain_v4',
    'HLT_ZeroBias_IsolatedBunches_v5',
    'HLT_ZeroBias_LastCollisionInTrain_v3',
    'HLT_ZeroBias_v6' )

process.EGammaPaths_NoDoubleCounting = cms.Sequence( process.EGammaPaths )
process.SingleMuonPaths_NoDoubleCounting = cms.Sequence( process.SingleMuonPaths + ~process.EGammaPaths )
process.ZeroBiasPaths_NoDoubleCounting = cms.Sequence( process.ZeroBiasPaths + ~process.EGammaPaths + ~process.SingleMuonPaths)

##########



process.demo = cms.EDAnalyzer('SignalBiasScan',
    trackLabel       = cms.InputTag('generalTracks'),
    tkTraj           = cms.InputTag('TrackRefitter'),
    labelTrajToTrack = cms.InputTag('TrackRefitter'),
    primaryVertexColl= cms.InputTag('offlinePrimaryVertices'),
    fullHitInfo_TIB       = cms.bool(True),
    fullHitInfo_TOB       = cms.bool(True),
    fullHitInfo_TID       = cms.bool(True),
    fullHitInfo_TEC       = cms.bool(True),
    isSmallBiasScan       = cms.untracked.bool(True)
)


#process.p = cms.Path(process.MeasurementTrackerEvent*process.offlineBeamSpot*process.TrackRefitter*process.demo)
#process.p = cms.Path(process.MET_NoDoubleCounting + process.TrackRefitter*process.demo)

process.p = cms.Path(process.EGammaPaths_NoDoubleCounting + process.TrackRefitter*process.demo)
