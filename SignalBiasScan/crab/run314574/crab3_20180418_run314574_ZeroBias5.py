from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#Need this to run under CMSSW 10 1 1 for now
config.section_("General")
config.General.instance = 'preprod'

config.General.requestName = 'HVscan_20180418_run314574_ZeroBias5_v2'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20180418_run314574_cfg.py'

config.Data.inputDataset = '/ZeroBias5/Commissioning2018-SiStripCalMinBias-PromptReco-v1/ALCARECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10 #Change to optimize file size
config.Data.lumiMask = 'DecoHVScan_20180418_run314574_JSON.txt'
config.Data.runRange = '314574' 
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias5_v2'
config.Data.publication = False
#config.Data.outputDatasetTag = ''


#Else, runs where the data are located (may be busy)
#config.Data.ignoreLocality = True

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'