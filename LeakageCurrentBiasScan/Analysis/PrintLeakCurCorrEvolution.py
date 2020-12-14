import os
import subprocess
from ROOT import TFile, TF1, TCanvas, TGraph, TH1, TLine, TLatex



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
def list_tree_files(files_regexp, useNoiseScan=False, useNoiseScanOnly=False):
  isNoiseScan = False
  files={}
  command='ls '+files_regexp
  p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
  output, err = p.communicate()
  noiserun_list = []
  for line in output.splitlines(False):
    #print line
    words = line.split(" ")
    if len(words)>0 :
      #print words[0]
	  file=words[0]
	  if len(file.split('_'))>3 :
	     if file.split('_')[2] == 'noise' : # noise scan
	        isNoiseScan = True
	        run=file.split('_')[4].replace('run', '').replace('.root', '')
	        if useNoiseScan: noiserun_list.append(run)
	     else :
		    run=file.split('_')[3].replace('run', '').replace('.root', '')
	  #print run
	  if (not isNoiseScan and not useNoiseScanOnly) or (useNoiseScan and isNoiseScan):
	    if run not in files:
	      files[run]=[]
	    files[run].append(file)

  return files


def get_leak_corr_at_300V(filename, detid):
  tfile = TFile(filename)
  func = tfile.Get('fit_'+str(detid))
  if func==None: return -99
  else: return func.Eval(300)


def get_mean_temp(filename, detid):

  f = TFile(filename)
  tree = f.Get('outTree')
  if tree==None: return -99

  previous_modid=0
  mean=0
  n=0
  for entry in tree:
    if entry.Detid==detid:
      mean+=entry.TemperatureSi
      n+=1
  if n!=0: mean/=n
  else: mean=-999
  return mean
  
  

def print_leakcurcorr_evolution(files, detid=369125870, temperature=False, noiseScans=[]):

  # points from signal bias scans
  g = TGraph()
  ipt=0
  # points from noise bias scans
  gn = TGraph()
  iptn=0
  # Loop on runs
  for run in sorted(files.keys()):
	if temperature==True: corr = get_mean_temp(files[run][0], detid)
	else: corr = get_leak_corr_at_300V(files[run][0], detid)
	if run in RUN_LUMI and corr>=0 and run!='314755':
	  #print run, files[run][0], RUN_LUMI[run],corr
	  g.SetPoint(ipt, RUN_LUMI[run], corr)
	  ipt+=1
	if run in RUN_LUMI and corr>=0 and run in noiseScans:
	  gn.SetPoint(iptn, RUN_LUMI[run], corr)
	  iptn+=1
	  
  c = TCanvas()
  g.SetMarkerStyle(20)
  h = g.GetHistogram()
  h.GetXaxis().SetTitle('int. lumi. (fb^{-1})')
  if temperature==True: h.GetYaxis().SetTitle('temperature (^{o}C)')
  else: h.GetYaxis().SetTitle('corr. at 300V (V)')
  g.Draw('AP')
  #g.Print('all')
  
  gn.SetMarkerStyle(20)
  gn.SetMarkerColor(2)
  gn.Draw('P')
  
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
  
  if temperature==True: c.Print('Temperature_vs_lumi_detid'+str(detid)+'.png')
  else: c.Print('LeakCurCorr_vs_lumi_detid'+str(detid)+'.png')


def get_temperature(filename, detid):
  tfile = TFile(filename)
  # outTree Detid Dcutimestamp TemperatureSi Ileak 

#------------------------------------------------------------

RUN_LUMI={}
fill_runs_lumi('runs_lumi_Run1.txt', RUN_LUMI)
fill_runs_lumi_Run2('runs_lumi_Run2.txt', RUN_LUMI)
print RUN_LUMI
files = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/wDCUcur/LeakCurCorr_TIB_*.root', True)
files_noise = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/wDCUcur/LeakCurCorr_TIB_*.root', True, True)
files_temperature = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_*.root', True)
small_scan_detids=[369121381, 369121382, 369121385, 369121386, 369121389, 369121390, 
369125861, 369125862, 369125865, 369125866 , 369125869 , 369125870 ]
files_temperature_noise = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_*.root', True, True)

print files_temperature_noise

#for mod in small_scan_detids:
#  print_leakcurcorr_evolution(files, mod)
  #print_leakcurcorr_evolution(files_temperature, mod, True)
print_leakcurcorr_evolution(files, 369121381, False, files_noise.keys())
print_leakcurcorr_evolution(files_temperature, 369125870, True, files_noise.keys())

#files = list_tree_files('~/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/LeakCurCorr_TIB_*.root')
