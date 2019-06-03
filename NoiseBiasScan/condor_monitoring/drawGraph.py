#!/bin/python

import sys
import math
from ROOT import TCanvas, TGraphAsymmErrors, TFile, TLine, TMath, TH1F


# possibility to filter on a run number
run_to_keep = 0
if len(sys.argv)>1:
  print 'RUN ', sys.argv[1]
  run_to_keep = int(sys.argv[1])

gr_noise = TGraphAsymmErrors()
ipt=0

f = open('FileListWithEventsAndNoise.txt', 'r')
fout = TFile('NoiseVsEvents.root', 'recreate')

# read noise values
for line in f:
  words = line.split(" ")
  if len(words)>12:
    n_evt = int(words[2])
    run = int(words[4])
    if run_to_keep!=0 and run!=run_to_keep: continue
    evt_range = words[6]
    evt_range = evt_range[1:-1]
    pair = evt_range.split(':')
    evt_min = int(pair[0])
    evt_max = int(pair[1])
    #if words[8].isdigit()==False : continue
    evt_avg = int(float(words[8]))
    noise = float(words[12])
    #if noise==0: continue
    gr_noise.SetPoint(ipt, evt_avg, noise)
	# Set error
    if len(words)>14:
      noise_interv = words[14]
      pair = noise_interv.split('-')
      noise_low = float(pair[0])
      #if n_evt>2 : noise_low = noise*math.sqrt((n_evt-1)/TMath.ChisquareQuantile(0.84,n_evt-1))
      noise_up = float(pair[1])
      #if n_evt>2 : noise_up = noise*math.sqrt((n_evt-1)/TMath.ChisquareQuantile(0.16,n_evt-1))
      gr_noise.SetPointError(ipt, evt_avg-evt_min, evt_max-evt_avg, noise-noise_low, noise_up-noise)
      print run, evt_avg, noise, noise/math.sqrt(n_evt)
    ipt+=1
f.close()


# read steps definition
lines=[]
h = gr_noise.GetHistogram()
f = open('noisebias_20180618_200_to_300V.txt', 'r')
for line in f:
  words = line.split(" ")
  if len(words)>4 and str.isdigit(words[0]):
	print words
	index = words[0]
	timestamp = words[1]
	run = int(words[2])
        if run_to_keep!=0 and run!=run_to_keep: continue
	evt = int(words[3])
	step = int(words[4])
	lines.append( TLine(evt, h.GetMinimum(), evt, h.GetMaximum()))
f.close()


c = TCanvas()
c.SetName('canvas')
gr_noise.SetMarkerStyle(20)
gr_noise.Draw('AP')
for line in lines:
  line.SetLineColor(2)
  line.Draw()
  line.Print()
c.Print('noise_vs_evt.png')
gr_noise.SetName('graph_noise')
fout.cd()
gr_noise.Write()
c.Write()
fout.Close()

