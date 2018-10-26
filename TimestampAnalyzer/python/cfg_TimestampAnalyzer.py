import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/data/Run2018B/VRRandom0/RAW/v1/000/318/025/00000/CCF3744D-DE73-E811-85AD-FA163E54D73A.root'
    )
)

process.demo = cms.EDAnalyzer('TimestampAnalyzer'
)


process.p = cms.Path(process.demo)
