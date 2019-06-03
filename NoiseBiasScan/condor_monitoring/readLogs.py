#!/bin/python

import sys
import os
import subprocess

dir = "jobs_output"
jobid = 0

if len(sys.argv)>1:
  print 'JOBID ', sys.argv[1]
  jobid = int(sys.argv[1])
else:
  print 'Syntax: readLogs.py JOBID'
  exit

# get list of jobs from logs
proc_list=[]
for root, dirs, files in os.walk(dir):
  for filename in files:
    words = filename.split('.')
    job = int(words[1])
    proc = words[2]
    if words[0]=="output" and job == jobid:
      print filename, proc
      proc_list.append(proc)

# get events & noise info from logs
noise_list={}
for proc in proc_list:
  #print " sed -n '/median noise/p'", dir+'/output.'+str(jobid)+'.'+proc+'.out'
  line = subprocess.check_output(["sed", "-n", "/median_noise/p", dir+"/output."+str(jobid)+"."+proc+".out"])
  noise_list[proc] = line.replace('\n','')

# get input file name from error logs
file_list={}
for proc in proc_list:
  #print " sed -n '/request to open/p'", dir+'/error.'+str(jobid)+'.'+proc+'.err'
  line = subprocess.check_output(["sed", "-n", "/Initiating request to open/p", dir+"/error."+str(jobid)+"."+proc+".err"])
  file_list[proc] = '/'+line.split('//')[-1].replace('\n','')


# write filename and noise info
fout = open('FileListWithEventsAndNoise.txt', 'w')
proc_list.sort()
for proc in proc_list:
  if noise_list[proc]!='':
     fout.write(file_list[proc]+' '+noise_list[proc]+'\n')
fout.close()


