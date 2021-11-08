# Thorben Quast
# 29 June 2021
# example run command: cmsRun trackster_ntupling.py dataFile=step3.root outputFile=ticlSimTrackstersNtuple.root

import os
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


options = VarParsing.VarParsing('standard')
options.register('dataFile',
                 'step3.root',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 'Data file containing the processed data incl. tracksters and clusters.')

options.register('outputFile',
                 'output.root',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 'Output file where the ntuple is stored to.')

options.parseArguments()
print(options)


process = cms.Process('Ntupling')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')

####################################
# Reduces the frequency of event count couts
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 2
####################################


process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(-1),
    output=cms.optional.untracked.allowed(cms.int32, cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
                                'file:%s' % options.dataFile)
                            )

print(options.outputFile)

#writing out regular TICL EM tracksters
process.trackstersNtupler_EM = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlTrackstersEM'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersEM.root'))

process.trackstersNtupler_HAD = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlTrackstersHAD'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersHAD.root'))

process.trackstersNtupler_Trk = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlTrackstersTrk'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersTrk.root')) 

process.trackstersNtupler_TrkEM = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlTrackstersTrkEM'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersTrkEM.root'))

process.trackstersNtupler_Merge = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlTrackstersMerge'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersMerge.root'))

process.trackstersNtupler_Sim = cms.EDAnalyzer('TracksterNtupler',
  	tracksters = cms.InputTag('ticlSimTracksters'),
  	layerClusters=cms.InputTag('hgcalLayerClusters'),
  	outfilePath = cms.untracked.string('ntuples/ticlTrackstersSim.root'))


process.p = cms.Path(process.trackstersNtupler_EM * 
		     process.trackstersNtupler_HAD * 
		     process.trackstersNtupler_Trk * 
	  	     process.trackstersNtupler_TrkEM *
		     process.trackstersNtupler_Merge * 
		     process.trackstersNtupler_Sim)
