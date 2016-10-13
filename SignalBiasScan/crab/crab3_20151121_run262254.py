from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'HVscan_20151121_run262254_v2'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20151121_run262254_cfg.py'

config.Data.inputDataset = '/MinimumBias1/Run2015E-PromptReco-v1/RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 25
config.Data.lumiMask = 'DecoSmallHVScan_20151121_run262254_JSON.txt'
config.Data.runRange = '262254' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20151121_run262254_v1_3'
config.Data.publication = False
#config.Data.outputDatasetTag = ''

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
