import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Source")

#process.load("DQM.SiStripCommon.MessageLogger_cfi")

process.load("DQM.SiStripCommon.DaqMonitorROOTBackEnd_cfi")

process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data')


process.SiStripDetInfoFileReader = cms.Service("SiStripDetInfoFileReader")

process.sistripconn = cms.ESProducer("SiStripConnectivity")


# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

options.register ('FileList',
                  'March2019_VRRandom0_files.txt', # default value
                  VarParsing.VarParsing.multiplicity.singleton, 
                  VarParsing.VarParsing.varType.string,
                  "FileList in DAS format")

options.register ('EventRange',
                  '', # default value
                  VarParsing.VarParsing.multiplicity.singleton,                                   VarParsing.VarParsing.varType.string,
                  "Event range of step: 'run1:event1-run2-event2")


# get and parse the command line arguments
options.parseArguments()
print 'EVENT RANGE: ', options.EventRange

filelist = FileUtils.loadListFromFile (options.FileList)
readFiles = cms.untracked.vstring( *filelist)

process.source = cms.Source("PoolSource",
 fileNames = readFiles,
 eventsToProcess = cms.untracked.VEventRange(options.EventRange)
)
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50000) )


process.load("EventFilter.SiStripRawToDigi.FedChannelDigis_cfi")
process.FedChannelDigis.LegacyUnpacker    = cms.bool(False)
process.FedChannelDigis.UnpackBadChannels = cms.bool(True)
process.FedChannelDigis.DoAPVEmulatorCheck = cms.bool(False)

process.load("DQM.SiStripCommissioningSources.CommissioningHistos_cfi")
process.CommissioningHistos.CommissioningTask = 'PEDESTALS'  # <-- run type taken from event data, but can be overriden

process.p = cms.Path(process.FedChannelDigis*process.CommissioningHistos)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('digis.root')
)
#process.outpath = cms.EndPath(process.out)
