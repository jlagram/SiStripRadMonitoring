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

options.register ('FileIndex',
                  0, # default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "index of the file in the list")

options.register ('OutputName',
                  'noise.root', # default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "Name of output tree file")

# get and parse the command line arguments
options.parseArguments()

#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string(options.OutputName) )

filelist = FileUtils.loadListFromFile (options.FileList)
readFiles = cms.untracked.vstring( *filelist)
selected_readFiles = cms.untracked.vstring( readFiles[options.FileIndex:options.FileIndex+1] )

process.source = cms.Source("PoolSource",
 fileNames = selected_readFiles
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20) )

process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('RecoLocalTracker.Configuration.RecoLocalTracker_cff')

process.monitor = cms.EDAnalyzer('InputFileNoise',
#    srcZSdigi = cms.InputTag('siStripZeroSuppression','VirginRaw')
srcZSdigi = cms.InputTag('siStripDigis','VirginRaw'))

#process.p = cms.Path(process.siStripDigis*process.siStripZeroSuppression*process.monitor)
process.p = cms.Path(process.siStripDigis*process.monitor)

