import os
import subprocess
import ROOT
from ROOT import TFile, TTree, TH1F, TCanvas, TH2F, gStyle, TGraph, TLine, TLatex
import numpy as np

def fill_runs_lumi(filename, lumi_dict):
  f = open(filename)
  for line in f:
    words = line.split()
    if len(words)>1:
      run=words[0]
      lumi=words[1]
      lumi_dict[run] = float(lumi)
	
def fill_runs_lumi_Run2(filename, lumi_dict):
  f = open(filename)
  for line in f:
    words = line.split()
    if len(words)>1:
      run=words[0]
      lumi=words[1]
      lumi_dict[run] = float(lumi) + 29.46
	
# return a dictionnary of tree files for the different runs
def list_tree_files(files_regexp):
  files={}
  command='ls '+files_regexp
  p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
  output, err = p.communicate()
  for line in output.splitlines(False):
    #print line
    words = line.split(" ")
    if len(words)>0 :
      #print words[0]
	  file=words[0]
	  run=file.split('_')[3].replace('run', '').replace('.root', '')
	  #print run
	  if run not in files:
	    files[run]=[]
	  files[run].append(file)
  return files

# return a dictionnary with median DCU/PS ratios over runs
def get_median_ratios(filename):
  medians={}
  f = open(filename)
  for line in f:
    words = line.split(" ")
    if len(words)>1 :
      detid=int(words[0])
      median=float(words[1])
      medians[detid]=median
  return medians

def print_diff_with_ref(file):
  
  medians = get_median_ratios('DCUOverPSRatio_Avg/DCUOverPSRatio_Avg_RunIandII.txt')

  tfile = TFile(file)
  ttree = tfile.Get('ratio')
  histo = TH1F("diff", "DCU/PS ratio difference", 40, 0, .4)
  histodiffrel = TH1F("diffrel", "DCU/PS ratio relative difference", 100, 0, 1.)
  for entry in ttree:
    if entry.NPTS > 0 and entry.DETID in medians.keys():
      ratio_ref = medians[entry.DETID]
      diff = abs(ratio_ref - entry.I_RATIO)
      diff_rel = abs(ratio_ref - entry.I_RATIO)/ratio_ref
      histo.Fill(diff)
      histodiffrel.Fill(diff_rel)

  histo.SetBinContent(40, histo.GetBinContent(41))
  histo.SetBinContent(41,0)
  histodiffrel.SetBinContent(40, histodiffrel.GetBinContent(41))
  histodiffrel.SetBinContent(41,0)
  print 'd<5%:', histo.Integral(1,5)/histo.GetEntries(), '5<d<10%:', histo.Integral(6,10)/histo.GetEntries(), 'd>10%:', histo.Integral(11,40)/histo.GetEntries()
  print 'd<5%:', histodiffrel.Integral(1,5)/histodiffrel.GetEntries(), '5<d<10%:', histodiffrel.Integral(6,10)/histodiffrel.GetEntries(), 'd>10%:', histodiffrel.Integral(11,40)/histodiffrel.GetEntries() 

def print_diff_with_nmod(file):

  tfile = TFile(file)
  ttree = tfile.Get('ratio')
  histo = TH1F("diff", "DCU/PS ratio difference", 40, 0, .4)
  histodiffrel = TH1F("diffrel", "DCU/PS ratio relative difference", 100, 0, 1.)
  hNmodGuess = TH1F("hNmodGuess", "1 divided by DCU/PS ratio", 40, 0, 8)
  h2NmodGuessVsNmod = TH2F("h2NmodGuessVsNmod", "", 40, 0, 8, 40, 0, 8)
  for entry in ttree:
    #print entry.DETID, entry.NPTS
    #print entry.DETID, entry.NMOD
    if entry.NPTS > 0:
      diff = abs(1./entry.NMOD - entry.I_RATIO)
      diff_rel = abs(1./entry.NMOD - entry.I_RATIO)*entry.NMOD
      histo.Fill(diff)
      histodiffrel.Fill(diff_rel)
	  #if diff > .2 : print entry.DETID, diff, entry.NMOD, '{:.1f}'.format(1./entry.I_RATIO), entry.NPTS
      if diff > .0 : 
	    hNmodGuess.Fill(1./entry.I_RATIO)
	    h2NmodGuessVsNmod.Fill(entry.NMOD, 1./entry.I_RATIO)
  #histo.Print("all")
  histo.SetBinContent(40, histo.GetBinContent(41))
  histo.SetBinContent(41,0)
  histodiffrel.SetBinContent(40, histodiffrel.GetBinContent(41))
  histodiffrel.SetBinContent(41,0)
  print 'd<5%:', histo.Integral(1,5)/histo.GetEntries(), '5<d<10%:', histo.Integral(6,10)/histo.GetEntries(), 'd>10%:', histo.Integral(11,40)/histo.GetEntries()
  print 'd<5%:', histodiffrel.Integral(1,5)/histodiffrel.GetEntries(), '5<d<10%:', histodiffrel.Integral(6,10)/histodiffrel.GetEntries(), 'd>10%:', histodiffrel.Integral(11,40)/histodiffrel.GetEntries(),  'd>20%:', histodiffrel.Integral(21,40)/histodiffrel.GetEntries()

  c1 = TCanvas()
  ROOT.gStyle.SetOptStat(0)
  histo.SetLineWidth(2)
  histo.Draw()
  c1.Print('LeakCurRationDiff.png')
  hNmodGuess.SetLineWidth(2)
  hNmodGuess.Draw()
  c1.Print('NmodGuess.png')
  h2NmodGuessVsNmod.Draw('colz')
  #c1.SetLogz()
  c1.Print('NmodGuessVsNmod.png')
  

# Print evolution of means
def print_evolutions(files):

  npts=[]
  rms=[]
  I_range=[]
  I_max_diff=[]
  I_max_diff_nmod=[]
  I_ratio=[]
  I_fit_avg_diff=[]
  I_avg_diff_nmod=[]
  
  print ' run     n0   n1  npts     fit       rms  range'

  for run in sorted(files.keys()):
	ndet_empty=0
	npts[:]=[]
	rms[:]=[]
	I_range[:]=[]
	I_max_diff[:]=[]
	I_max_diff_nmod[:]=[]
	I_ratio[:]=[]
	I_fit_avg_diff[:]=[]
	I_avg_diff_nmod[:]=[]

	tfile = TFile(files[run][0])
	ttree = tfile.Get('ratio')
	for entry in ttree:
	  #if entry.DETID==369125866 or entry.DETID==369125870: continue
      #    if entry.DETID/1000==369125: continue
	  if entry.NPTS!=0:
		npts.append( entry.NPTS )
		rms.append( entry.I_RATIO_RMS )
		I_range.append( entry.I_RATIO_MAX - entry.I_RATIO_MIN )
		I_max_diff.append( max(entry.I_RATIO_MAX - entry.I_RATIO, entry.I_RATIO - entry.I_RATIO_MIN))
		I_max_diff_nmod.append( max(entry.I_RATIO_MAX - entry.I_RATIO, entry.I_RATIO - entry.I_RATIO_MIN) - max(abs(entry.I_RATIO_MAX - 1./entry.NMOD), abs(1./entry.NMOD - entry.I_RATIO_MIN)))
		I_fit_avg_diff.append( entry.FIT_RATIO_AVG_DIFF )
		I_avg_diff_nmod.append( entry.FIT_RATIO_AVG_DIFF - entry.NMOD_RATIO_AVG_DIFF)
		I_ratio.append( entry.I_RATIO )
	  else:
	    ndet_empty+=1

	if len(npts)>0:
	  print run, '{0:4d} {1:4d}'.format(ndet_empty, len(npts)), ' {0:4.1f} {1:.3f}+-{2:.3f} {3:.3f} {4:.3f} {5:.3f} {6:6.3f} {7:.3f} {8:6.3f}'.format(np.mean(npts), np.mean(I_ratio), np.std(I_ratio), np.mean(rms), np.mean(I_range), np.mean(I_max_diff), np.mean(I_max_diff_nmod), np.mean(I_fit_avg_diff), np.mean(I_avg_diff_nmod))
	else:
	  print run, '{0:4d} {1:4d}'.format(ndet_empty, len(npts))


# Print evolution of ratio
def print_evolution_ratio(files):

  firstrun = True
  refval = {}
  I_ratio=[]

  for run in sorted(files.keys()):
	I_ratio[:]=[]

	tfile = TFile(files[run][0])
	ttree = tfile.Get('ratio')
	for entry in ttree:
	  #if entry.DETID/1000!=369121 : continue
	  if entry.NPTS!=0 :
	    #if run=='262254' or run =='276437' :  print entry.DETID, ' : ', entry.I_RATIO
	    if firstrun:
		  refval[entry.DETID] = entry.I_RATIO
		  #print entry.DETID, ' : ', entry.I_RATIO
	    else:
		  if entry.DETID in refval.keys():
		    I_ratio.append( abs(entry.I_RATIO - refval[entry.DETID]) )
		    #print entry.DETID, ' : ', entry.I_RATIO, refval[entry.DETID], entry.I_RATIO_MIN, entry.I_RATIO_MAX, entry.I_RATIO_RMS, entry.NMOD_RATIO_AVG_DIFF, entry.FIT_RATIO_AVG_DIFF 
		    #if run=='280385': print entry.DETID, ' : ', entry.I_RATIO, refval[entry.DETID], entry.V_MIN, entry.V_MAX, entry.NPTS
		  else: refval[entry.DETID] = entry.I_RATIO # in case detid was not present in first run
	firstrun = False

	if len(I_ratio)>0: print run, np.mean(I_ratio), np.std(I_ratio)
	else: print run
	#print run, I_ratio


# Print evolution of ratio for a given detid
def print_evolution_ratio(files, detid):

  g = TGraph()
  ipt=0
  I_ratio=[]

  for run in sorted(files.keys()):

	tfile = TFile(files[run][0])
	ttree = tfile.Get('ratio')
	for entry in ttree:
	  if entry.DETID==detid  and run!='314755':
		print run, ' : ', entry.I_RATIO, entry.NPTS, 1./entry.NMOD, ipt, RUN_LUMI[run]
		if entry.NPTS>=5 or (entry.NPTS>=2 and abs(entry.V_MAX-entry.V_MIN)>=80) :
		  I_ratio.append(entry.I_RATIO)
		  g.SetPoint(ipt, RUN_LUMI[run], entry.I_RATIO)
		  ipt+=1
		else:
		  if entry.NPTS>0 or (entry.NPTS==0 and entry.NMOD!=1): # eliminate cases when no PS current found
		    I_ratio.append(1./entry.NMOD)
		    g.SetPoint(ipt, RUN_LUMI[run], 1./entry.NMOD)
		    ipt+=1

  print detid, np.mean(I_ratio), np.std(I_ratio)
  
  c = TCanvas()
  g.SetMarkerStyle(20)
  h = g.GetHistogram()
  h.GetXaxis().SetTitle('int. lumi. (fb^{-1})')
  h.GetYaxis().SetTitle('DCU/PS cur. ratio')
  g.Draw('AP')
  #g.Print('all')
  
  lumi_Run1 = 29.46
  l1 = TLine(lumi_Run1, h.GetMinimum(), lumi_Run1, h.GetMaximum())
  l1.SetLineStyle(2)
  l1.Draw()
  y = h.GetMaximum()*0.8
  temp = TLatex()
  temp.SetTextSize(0.04)
  temp.SetTextColor(4)
  temp.DrawLatex(lumi_Run1-17, y, '+4^{o}C')
  temp.DrawLatex(lumi_Run1+7, y,'-15^{o}C')
  l2 = TLine(lumi_Run1+45.71, h.GetMinimum(), lumi_Run1+45.71, h.GetMaximum())
  l2.SetLineStyle(3)
  l2.Draw()
  eyets = TLatex()
  eyets.SetTextSize(0.03)
  eyets.SetTextAngle(90)
  eyets.DrawLatex(lumi_Run1+42, y*0.7, 'EYETS (2016-17)')
  l3 = TLine(lumi_Run1+91.37, h.GetMinimum(), lumi_Run1+91.37, h.GetMaximum())
  l3.SetLineStyle(2)
  l3.Draw()
  temp.DrawLatex(lumi_Run1+99, y,'-20^{o}C')
  
  c.Print('DCUOverPSRatio_vs_lumi_detid'+str(detid)+'.png')


def print_mean_ratio(files):

  ratios = {}
  hmean = TH1F("median", "median", 100, 0, 1)
  hmedian = TH1F("mean", "mean", 100, 0, 1)
  hrms = TH1F("rms", "rms", 50, 0, 0.1)
  hn = TH1F("n", "n", 40, 0, 40)

  for run in sorted(files.keys()):

	tfile = TFile(files[run][0])
	ttree = tfile.Get('ratio')
	for entry in ttree:
	  #if entry.DETID/1000!=369121 : continue
	  if entry.NPTS>=5 or (entry.NPTS>=2 and abs(entry.V_MAX-entry.V_MIN)>=80):
	    if entry.DETID not in ratios.keys():
		  ratios[entry.DETID] = [entry.I_RATIO]
	    else:
		  ratios[entry.DETID].append(entry.I_RATIO)

  for mod in ratios.keys():
	hmean.Fill(np.mean(ratios[mod]))
	hmedian.Fill(np.median(ratios[mod]))
	hrms.Fill(np.std(ratios[mod]))
	hn.Fill(len(ratios[mod]))
	#if np.std(ratios[mod])>0.04: 
	print mod, np.median(ratios[mod]), np.mean(ratios[mod]), np.std(ratios[mod]), len(ratios[mod])


  c1 = TCanvas()
  #ROOT.gStyle.SetOptStat(0)
  hmean.Draw()
  c1.Print('Ratios_mean.png')
  hmedian.Draw()
  c1.Print('Ratios_median.png')
  hrms.Draw()
  c1.Print('Ratios_rms.png')
  hn.Draw()
  c1.Print('Ratios_n.png')


# main

#files = list_tree_files('Results/DCUOverPSRatio_TIB_201*.root')
files = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_201*.root')
#files2 = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_2016*.root')
#files.update(files2)
#files3 = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_2017*.root')
#files.update(files3)
#files4 = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_2018*.root')
#files.update(files4)
#print_evolutions(files)
#print_evolution_ratio(files)
#print_diff_with_nmod('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20120405_run190459.root')
#print_diff_with_nmod('DCUOverPSRatio_TEC_20120510_run193928.root')
#print_diff_with_nmod('DCUOverPSRatio_TIB_20120812_run200786.root')
#print_diff_with_nmod('DCUOverPSRatio_TIB_20150603_run246963.root')
#print_diff_with_nmod('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20120405_run190459.root')
#print_diff_with_nmod('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20160423_run271056.root')
#print_diff_with_nmod('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20170527_run295376.root')
#print_diff_with_nmod('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20180418_run314574.root')
#print_diff_with_ref('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20120405_run190459.root')
#print_diff_with_ref('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20160423_run271056.root')
#print_diff_with_ref('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20170527_run295376.root')
#print_diff_with_ref('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/DCUOverPSRatios/DCUOverPSRatio_TIB_20180418_run314574.root')


RUN_LUMI={}
fill_runs_lumi('runs_lumi_Run1.txt', RUN_LUMI)
fill_runs_lumi_Run2('runs_lumi_Run2.txt', RUN_LUMI)
#print RUN_LUMI
small_scan_detids=[369121381, 369121382, 369121385, 369121386, 369121389, 369121390, 
369125861, 369125862, 369125865, 369125866 , 369125869 , 369125870 ]
print_evolution_ratio(files, 369125870)
#for mod in small_scan_detids:
#  print_evolution_ratio(files, mod)
# print_mean_ratio(files)
