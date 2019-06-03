#!/bin/python

import sys
import os
import math
from ROOT import TFile, TTree, TObject, TCanvas, TGraphAsymmErrors, TLine, TMath, TH1F
from datetime import datetime


# possibility to filter on a run number
run_to_keep = 0
if len(sys.argv)>1:
  print 'RUN ', sys.argv[1]
  run_to_keep = int(sys.argv[1])


dir='scans/2018Jun/monitor'
stepfilename='noisebias_20180618_10_to_200V.txt' 

#--------------------------------------------------------

def fill_graphs(yvar, xvar):
  graphs = []
  detid_list = detids.keys()
  detid_list_sorted = sorted(detid_list)
  for detid in detid_list_sorted:
	#print detid
	g = TGraphAsymmErrors()
	g.SetName(yvar+'_vs_'+xvar+'_'+str(detid))
	ipt=0
	for job in detids[mod.detid]:
	  #print job, detids[detid][job]
	  if job not in detids[detid]: continue
	  x_err_low=0
	  x_err_up=0
	  y_err_low=0
	  y_err_up=0
	  g.SetPoint(ipt, detids[detid][job][xvar], detids[detid][job][yvar])
	  if xvar=='evt_avg': 
		x_err_low = detids[detid][job]['evt']-detids[detid][job]['evt_min']
		x_err_up = detids[detid][job]['evt_max']-detids[detid][job]['evt']
	  if xvar=='time': 
		x_err_low = detids[detid][job]['time']-detids[detid][job]['time_min']
		x_err_up = detids[detid][job]['time_max']-detids[detid][job]['time']
	  if yvar=='noise': 
		y_err_low = detids[detid][job]['noise']-detids[detid][job]['noise_low']
		y_err_up = detids[detid][job]['noise_up']-detids[detid][job]['noise']
	  if yvar=='ped': 
		y_err_low = detids[detid][job]['ped']-detids[detid][job]['ped_low']
		y_err_up = detids[detid][job]['ped_up']-detids[detid][job]['ped']
	  g.SetPointError(ipt, x_err_low, x_err_up, y_err_low, y_err_up)
	  ipt+=1
	graphs.append(g)
  return graphs


def draw_save_graphs(graphs, yvar, xvar, filename):
  filename.cd()
  newdir = filename.mkdir(yvar+'_vs_'+xvar)
  newdir.cd()
  c = TCanvas()
  c.SetName('canvas')
  l = TLine()
  l.SetLineColor(2)
  for g in graphs:
	#print g.GetName()
	g.SetMarkerStyle(20)
	if xvar=='time': g.GetXaxis().SetTimeDisplay(1)
	g.Draw('AP')
	g.Write()
	h = g.GetHistogram()
	if xvar=='evt': 
	  for evt in step_events:
	    l.DrawLine(evt, h.GetMinimum(), evt, h.GetMaximum())
	if xvar=='time': 
	  for time in step_times:
	    l.DrawLine(time, h.GetMinimum(), time, h.GetMaximum())
	# TIB
	#c.Print('noise_vs_evt.png')
	c.SetName('c_'+g.GetName())
	#print 'c_'+g.GetName()
	c.Write()


#--------------------------------------------------------

# get list of log files
jobs=[]
for root, dirs, files in os.walk(dir):
  for filename in files:
    words = filename.replace('.root', '').split('_')
    if words[0]=="noise" and words[1].isdigit():
      job = words[1]
      jobs.append(job)


# get noise info for each job and each detid
jobs.sort()
detids={}
for job in jobs:
  f = TFile.Open(dir+'/noise_'+job+'.root')
  if f is None: continue
  #print 'opened', f.GetName()
  tree = f.Get('monitor/noisetree')
  if not tree: continue
  #if tree.ClassName() != 'TTree': continue
  for mod in tree:
    if run_to_keep!=0 and mod.run!=run_to_keep: continue
    if mod.detid not in detids.keys(): detids[int(mod.detid)] = {}
    detids[int(mod.detid)][job] = { 'evt': mod.evt_avg, 'evt_min': mod.evt_min, 'evt_max':mod.evt_max,
	 'time': (mod.time_min+mod.time_max)/2., 'time_min': mod.time_min, 'time_max':mod.time_max,
	 'ped':mod.ped, 'ped_low': mod.ped_low, 'ped_up': mod.ped_up,
	 'noise':mod.noise, 'noise_low': mod.noise_low, 'noise_up': mod.noise_up,
	 'ped_64': mod.ped_64, 'noise_64': mod.noise_64}
  

# fill graphs
graphs_noise_vs_evt = fill_graphs('noise', 'evt')
graphs_ped_vs_evt = fill_graphs('ped', 'evt')
graphs_noise64_vs_evt = fill_graphs('noise_64', 'evt')
graphs_ped64_vs_evt = fill_graphs('ped_64', 'evt')
graphs_noise_vs_time = fill_graphs('noise', 'time')
graphs_ped_vs_time = fill_graphs('ped', 'time')


# read steps definition for showing them
step_events=[]
step_times=[]
f = open(stepfilename, 'r')
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
	step_events.append(evt)
	dt = datetime(int(timestamp[0:4]), int(timestamp[4:6]), int(timestamp[6:8]), int(timestamp[8:10]), int(timestamp[10:12]), int(timestamp[-2:]))
	#print dt, int(dt.strftime('%s'))
	step_times.append(int(dt.strftime('%s'))+7200) # unix time
f.close()


# draw & save
fout = TFile('NoiseVsEvents.root', 'recreate')
draw_save_graphs(graphs_noise_vs_evt, 'noise', 'evt', fout)
draw_save_graphs(graphs_ped_vs_evt, 'ped', 'evt', fout)
draw_save_graphs(graphs_noise64_vs_evt, 'noise_64', 'evt', fout)
draw_save_graphs(graphs_ped64_vs_evt, 'ped_64', 'evt', fout)
draw_save_graphs(graphs_noise_vs_time, 'noise', 'time', fout)
draw_save_graphs(graphs_ped_vs_time, 'ped', 'time', fout)
fout.Close()

