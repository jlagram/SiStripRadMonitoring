from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'HVscan_20170527_run295324_v2_ZeroBias2'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20170527_run295324_cfg.py'

config.Data.inputDataset = '/ZeroBias2/Run2017A-SiStripCalMinBias-PromptReco-v1/ALCARECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10 #Change to optimize file size
config.Data.lumiMask = 'DecoHVScan_20170527_run295324_JSON.txt'
config.Data.runRange = '295324-295339' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295324_v2'
config.Data.publication = False
#config.Data.outputDatasetTag = ''


#Else, runs where the data are located (may be busy)
config.Data.ignoreLocality = True

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
