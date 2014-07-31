import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

# Conditions (Global Tag is used here):
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
# CMSSW523
process.GlobalTag.globaltag = 'GR_P_V42::All'

#Geometry and field
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Geometry.CommonDetUnit.globalTrackingGeometry_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("EmptySource",
)

#name of the output file containing the tree
process.TFileService = cms.Service("TFileService", fileName = cms.string("modulesPositions.root") )

process.demo = cms.EDAnalyzer('ModulePositionWriter'
)


process.p = cms.Path(process.demo)
