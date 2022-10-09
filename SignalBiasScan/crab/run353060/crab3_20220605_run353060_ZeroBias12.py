from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'HVscan_20220605_run353060_ZeroBias12_v1'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20220605_run353060_cfg.py'

config.Data.inputDataset = '/ZeroBias12/Run2022A-SiStripCalMinBias-PromptReco-v1/ALCARECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 300 #Change to optimize file size
config.Data.lumiMask = 'DecoHVScan_20220605_run353060_JSON.txt'
config.Data.runRange = '353060' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias12_v1'
config.Data.publication = False
#config.Data.outputDatasetTag = ''


#Else, runs where the data are located (may be busy)
#config.Data.ignoreLocality = True

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
