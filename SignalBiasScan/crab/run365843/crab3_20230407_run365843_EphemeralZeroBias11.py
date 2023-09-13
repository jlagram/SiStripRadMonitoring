from CRABClient.UserUtilities import config 
# ,getUsernameFromSiteDB
config = config()

config.General.requestName = 'HVscan_20230407_run365843_EphemeralZeroBias11_v1'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20230407_run365843_cfg.py'

config.Data.inputDataset = '/EphemeralZeroBias11/Run2023A-SiStripCalMinBias-20Apr2023_365843_SiStripCal-v1/ALCARECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10 #Change to optimize file size
config.Data.lumiMask = 'DecoHVScan_20230407_run365843_JSON.txt'
config.Data.runRange = '365843' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias11_v1'
config.Data.publication = False
#config.Data.outputDatasetTag = ''


#Else, runs where the data are located (may be busy)
#config.Data.ignoreLocality = True

# config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
