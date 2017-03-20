import os
import subprocess
import ROOT
from ROOT import TFile, TTree, TH1F, TCanvas, TH2F, gStyle
import numpy as np

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


def print_diff_with_nmod(file):

  tfile = TFile(file)
  ttree = tfile.Get('ratio')
  histo = TH1F("diff", "DCU/PS ratio difference", 40, 0, .4)
  hNmodGuess = TH1F("hNmodGuess", "1 divided by DCU/PS ratio", 40, 0, 8)
  h2NmodGuessVsNmod = TH2F("h2NmodGuessVsNmod", "", 40, 0, 8, 40, 0, 8)
  for entry in ttree:
    #print entry.DETID, entry.NPTS
    print entry.DETID, entry.NMOD
    if entry.NPTS > 0:
      diff = abs(1./entry.NMOD - entry.I_RATIO)
      histo.Fill(diff)
	  #if diff > .2 : print entry.DETID, diff, entry.NMOD, '{:.1f}'.format(1./entry.I_RATIO), entry.NPTS
      if diff > .0 : 
	    hNmodGuess.Fill(1./entry.I_RATIO)
	    h2NmodGuessVsNmod.Fill(entry.NMOD, 1./entry.I_RATIO)
  histo.Print("all")
  histo.SetBinContent(40, histo.GetBinContent(41))
  histo.SetBinContent(41,0)
  print 'd<5%:', histo.Integral(1,5)/histo.GetEntries(), '5<d<10%:', histo.Integral(6,10)/histo.GetEntries(), 'd>10%:', histo.Integral(11,40)/histo.GetEntries()
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



# main

files = list_tree_files('Results/DCUOverPSRatio_TIB_201*.root')
print_evolutions(files)
print_evolution_ratio(files)
#print_diff_with_nmod('DCUOverPSRatio_TIB_20120405_run190459.root')
#print_diff_with_nmod('DCUOverPSRatio_TEC_20120510_run193928.root')
#print_diff_with_nmod('DCUOverPSRatio_TIB_20120812_run200786.root')
#print_diff_with_nmod('DCUOverPSRatio_TIB_20150603_run246963.root')
#print_diff_with_nmod('DCUOverPSRatio_TIB_20160423_run271056.root')
