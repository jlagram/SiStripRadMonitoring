from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'HVscan_20160423_run271056_ZeroBias6_v1'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20160423_run271056_cfg.py'

config.Data.inputDataset = '/ZeroBias6/CMSSW_8_0_29-80X_dataRun2_Prompt_v6_RelVal_zb62016A-v1/RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.lumiMask = 'DecoHVScan_20160423_run271056_JSON.txt'
config.Data.runRange = '271056' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20160423_run271056_ZeroBias6_v1'
config.Data.publication = False
#config.Data.outputDatasetTag = ''

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'

#Datasets : 
#/ZeroBias1/Run2016A-PromptReco-v2/RECO
#/ZeroBias8/CMSSW_8_0_29-80X_dataRun2_Prompt_v6_RelVal_zb82016A-v1/RECO
#...
