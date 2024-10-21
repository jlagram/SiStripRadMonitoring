from CRABClient.UserUtilities import config 
# ,getUsernameFromSiteDB
config = config()

config.General.requestName = 'HVscan_20240702_run382655_v1'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20240702_run382655_cfg.py'

config.Data.inputDataset = '/ZeroBias/Run2024F-SiStripCalMinBias-PromptReco-v1/ALCARECO'
config.Data.inputBlocks = ['/ZeroBias/Run2024F-SiStripCalMinBias-PromptReco-v1/ALCARECO#9917ddf9-bdc5-4409-882d-3e3c582975c0']
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10 #Change to optimize file size
config.Data.lumiMask = 'DecoSmallHVScan_20240702_run382655_JSON.txt'
config.Data.runRange = '382655' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVscan_20240702_run382655_v1'
config.Data.publication = False
#config.Data.outputDatasetTag = ''


#Else, runs where the data are located (may be busy)
#config.Data.ignoreLocality = True

# config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
