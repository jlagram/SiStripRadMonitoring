#! /usr/bin/env python
# -*- coding: utf-8 -*-

import csv
from ROOT import TTree, TFile, AddressOf, gROOT
import numpy as n
import time
import argparse

#filein = 'Data/Current_12_08_2012.csv'

parser = argparse.ArgumentParser(description='Script to put leakage currents values of a full bias scan in a root file. It uses a separate file mapping the PSU channels to the detids: Data/PSUtoDETID_wADD.csv .')
parser.add_argument('input', nargs='+', help='input file in csv format')
parser.add_argument('-subdet', nargs='?', help='option to parse only infos from a subdet (TIB,TID...)')
parser.add_argument('-T', nargs='?', const='T', help='run only until it finds 10 selected entries')
args = parser.parse_args()

# class to store option when argparse package is not available
#class arg_struc:
#    def __init__(self):
#        self.input = ['Data/Current_03-04_07_2015.csv']
#        self.subdet = 1
#        self.T = 1

#args = arg_struc()


filein = args.input[0]
fileout = filein.replace('.csv', '.root')

if args.subdet:
   print 'subdet: ', args.subdet

##############################

# Create a struct
gROOT.ProcessLine(\
  "struct Entry{\
    Int_t DETID;\
    Long64_t TIME;\
    Float_t CURRENT;\
    Int_t NMODFORCHANNEL;\
    Char_t PS[50];\
   };")


from ROOT import Entry
entry = Entry()

# dictionnary to link PS channels and detids
cr = csv.reader(open("Data/PSUtoDETID_wADD.csv","rb"))
# PS_CHANNEL, DETIDS

PS_DETIDS = {}
for row in cr:
    PS_DETIDS[row[0]] = row[1]


# create output file
fout = TFile(fileout, 'recreate')

# create output tree
tree = TTree('ps', 'ps')
#tree.Branch('data', entry, 'DETID/I:TIME/L:CURRENT/F:NMODFORCHANNEL/I')
tree.Branch('PS', AddressOf(entry, 'PS'), 'PS/C')
tree.Branch('DETID', AddressOf(entry, 'DETID'), 'DETID/I') 
tree.Branch('TIME', AddressOf(entry, 'TIME'), 'TIME/L')
tree.Branch('CURRENT', AddressOf(entry, 'CURRENT'), 'CURRENT/F')
tree.Branch('NMODFORCHANNEL', AddressOf(entry, 'NMODFORCHANNEL'), 'NMODFORCHANNEL/I')

# Read input file
currents = csv.DictReader(open(filein,"rb"))
# "PGNAME","DATETIME","PS_ILEAK"
nrow=1
for row in currents:
    if nrow%10000==0: print nrow," entries"
    str_ps = row['PGNAME']
    str_time = row['DATETIME']
    current = float(row['PS_ILEAK'])
    #print row

    # apply subdet filter
    if args.subdet:
        if str_ps.count(args.subdet)==0 : continue
    # check if will process the entry
    if not PS_DETIDS.has_key(str_ps):
        if current < 10:
            if "channel000" not in str_ps: 
              print str_ps, "not found in dictionnary. Skip it.   Ileak < 10"
              # it is normal that channel000 is not in dictionnary
            continue
        else:
            # check if it corresponds to cases with channel = 0 in the MAP.csv file
            str_ps_00 = str_ps.replace('003', '000')
            str_ps_00 = str_ps_00.replace('002', '000')
            if PS_DETIDS.has_key(str_ps_00):
                print 'use ', str_ps_00, 'instead of ', str_ps
                str_ps = str_ps_00
            else:
                print str_ps, " NOT FOUND "
                continue
    nrow+=1
    if args.T:
        if nrow >=10 : break
    
    # adapt time format
    entry.TIME=0
    str_time_parts = str_time.split()
    nparts=len(str_time_parts)
    if nparts < 2: print "ERROR : wrong time format"
    if str_time_parts[nparts-1]=='AM' or str_time_parts[nparts-1]=='PM' :
        t = time.strptime(str_time, "%d-%b-%y %I.%M.%S.%f000 %p")
    else:
        try:	
            t = time.strptime(str_time, "%d-%b-%y %H.%M.%S.%f000")
        except ValueError:
			# try an other format
			try:
			    t = time.strptime(str_time, "%Y-%m-%d %H:%M:%S.%f000")
			except ValueError:
				# try still an other format
				t = time.strptime(str_time, "%Y-%m-%d %H:%M:%S")
    # local time in s
	time_in_sec = time.mktime(t)
	# time in UTC
	t_utc = time.gmtime(time_in_sec)
    entry.TIME = time_in_sec #int(time.mktime(t_utc))
    #print 'time shift: ', int(time.mktime(t_utc)), t_utc
    
    # fill one entry per detid
    entry.PS = str_ps
    entry.CURRENT = current
    modules = PS_DETIDS[str_ps].split(',')
    entry.NMODFORCHANNEL = len(modules)
    for mod in modules:
        entry.DETID=int(mod)
        tree.Fill()

tree.Write()
fout.Close()

