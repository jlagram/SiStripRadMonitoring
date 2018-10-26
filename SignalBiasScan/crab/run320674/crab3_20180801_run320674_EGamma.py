from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()


#config.General.requestName = 'HVscan_20180801_run320674_ZeroBias_noDoubleCounting_v1'
#config.General.requestName = 'HVscan_20180801_run320674_SingleMuon_noDoubleCounting_v1'
config.General.requestName = 'HVscan_20180801_run320674_EGamma_noDoubleCounting_v1'

#config.Data.inputDataset = '/ZeroBias/Run2018D-SiStripCalMinBias-PromptReco-v2/ALCARECO'
#config.Data.inputDataset = '/SingleMuon/Run2018D-21Aug2018-v1/RECO'
config.Data.inputDataset = '/EGamma/Run2018D-21Aug2018-v1/RECO'

#config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_ZeroBias_v1'
#config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_SingleMuon_v1'
config.Data.outLFNDirBase = '/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_EGamma_v1'



config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'signalbiasscan_20180801_run320674_EGamma_cfg.py'


config.Data.inputDBS = 'global'
#config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10 #Change to optimize file size
config.Data.lumiMask = 'DecoSmallHVScan_20180801_run320674_JSON.txt'
config.Data.runRange = '320674' 


config.Data.publication = False
#config.Data.outputDatasetTag = ''


#config.Data.splitting = 'Automatic' #-- TRY THIS ! (to avoid error messsage "didn't yield any job") . --> didn't work
config.Data.splitting = 'LumiBased'


#Else, runs where the data are located (may be busy)
#config.Data.ignoreLocality = True

#config.Site.storageSite = 'T2_FR_IPHC'
config.Site.storageSite = 'T2_CH_CERN'
