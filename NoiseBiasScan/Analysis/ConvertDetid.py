#!/usr/bin/env python
import re
import sys



def setFedId(x):
    return (long(x) & 511) << 10

def setFeUnit(x):
    return (long(x) & 15) << 6

def setFeChannel(x):
    return (long(x) & 15) << 2

def makeFedKey(fedId, feUnit, feCh):
    return setFedId(fedId) | setFeUnit(feUnit) | setFeChannel(feCh)


def filenameF(name):
    suffix='CablingInfo_Run'
    filenameX=suffix+name+'.txt'
    location = '/afs/cern.ch/cms/tracker/sistrcalib/WWW/CondDBMonitoring/pro/CMS_CONDITIONS/DBTagCollection/SiStripFedCabling/SiStripFedCabling_GR10_v1_hlt/CablingLog/'
    return location + filenameX 

def DictionaryCab(filenameC):
    """This function takes a filename as input and looks for it in the URL, then makes a dictionary with DetId as key and pairnumber as key2 or FEDid as key1 and FedCh as key2"""
    FiletxtFEDs = open(filenameC,'r')

    Fd = "FedCrate/FedSlot/FedId/FeUnit/FeChan/FedCh"
    Fc = "FecCrate/FecSlot/FecRing/CcuAddr/CcuChan"
    D = "DcuId/DetId"
    Ll = "LldChan/APV0/APV1"
    pair = "pairNumber/nPairs/nStrips"
    DC = "DCU/MUX/PLL/LLD"
 
    DictionaryCab.CablingInfoDict={}		
    DictionaryCab.CablingInfoDictF={}
    # Creating lists
    FedCrateList = []
    FedSlotList = []
    FedIdList=[]
    FeUnitList=[]
    FeChanList=[]
    FedChList=[]
		
    FecCrateList=[]
    FecSlotList=[]
    FecRingList=[]
    CcuAddrList=[]
    CcuChanList=[]
		
    DcuIdList=[]
    DetIdList=[]
		
    LldChanList=[]
    APV0List=[]
    APV1List=[]
    pairNumberList=[]
    nPairsList=[]
    nStripsList=[]
		
    DCUList=[]
    MUXList=[]
    PLLList=[]
    LLDList=[]


    for line in FiletxtFEDs:
        if Fd in line:
            pattern = re.split('\W+',line)
            FedCrateList.append(pattern[7])
            FedSlotList.append(pattern[8])
            FedIdList.append(pattern[9])
            FeUnitList.append(pattern[10])
            FeChanList.append(pattern[11])
            FedChList.append(pattern[12])
        if Fc in line:
            pattern = re.split('\W+',line)
            FecCrateList.append(pattern[6])
            FecSlotList.append(pattern[7])
            FecRingList.append(pattern[8])
            CcuAddrList.append(pattern[9])
            CcuChanList.append(pattern[10])
        if D in line:
            pattern = re.split('\W+',line)
            DcuIdList.append(str(int(pattern[3],16)))
            DetIdList.append(str(int(pattern[4],16)))
        if Ll in line:
            pattern = re.split('\W+',line)
            LldChanList.append(pattern[4])
            APV0List.append(pattern[5])
            APV1List.append(pattern[6])
        if pair in line:
            pattern = re.split('\W+',line)
            pairNumberList.append(pattern[4])
            nPairsList.append(pattern[5])
            nStripsList.append(pattern[6])
        if DC in line:
            pattern = re.split('\W+',line)
            DCUList.append(pattern[6])
            MUXList.append(pattern[7])
            PLLList.append(pattern[8])
            LLDList.append(pattern[9])
        		
    
    for fedcrate,fedslot,fedid,feunit,fechan,fedch,feccrate,fecslot,fecring,ccuaddr,ccuchan,dcuid,detid,lldchan,apv0,apv1,pairnumber,npairs,nstrips,dcu,mux,pll,lld  in zip(FedCrateList,FedSlotList,FedIdList,FeUnitList,FeChanList,FedChList,FecCrateList,FecSlotList,FecRingList,CcuAddrList,CcuChanList,DcuIdList,DetIdList,LldChanList,APV0List,APV1List,pairNumberList,nPairsList,nStripsList,DCUList,MUXList,PLLList,LLDList):

        if detid in DictionaryCab.CablingInfoDict.keys(): 
            DictionaryCab.CablingInfoDict[detid].update({pairnumber:{"FedCrate": fedcrate,"FedSlot":fedslot,"FedId":fedid,"FeUnit":feunit,"FeChan":fechan,"FedCh":fedch,"FecCrate":feccrate,"FecSlot":fecslot,"FecRing":fecring,"CcuAddr":ccuaddr,"CcuChan":ccuchan,"DcuId":dcuid,"DetId":detid,"pairNumber":pairnumber,"LldChan":lldchan,"APV0":apv0,"APV1":apv1,"nPairs":npairs,"nStrips":nstrips,"DCU":dcu,"MUX":mux,"PLL":pll,"LLD":lld}})
		
        else:
            DictionaryCab.CablingInfoDict.update({detid:{pairnumber:{"FedCrate": fedcrate,"FedSlot":fedslot,"FedId":fedid,"FeUnit":feunit,"FeChan":fechan,"FedCh":fedch,"FecCrate":feccrate,"FecSlot":fecslot,"FecRing":fecring,"CcuAddr":ccuaddr,"CcuChan":ccuchan,"DcuId":dcuid,"DetId":detid,"pairNumber":pairnumber,"LldChan":lldchan,"APV0":apv0,"APV1":apv1,"nPairs":npairs,"nStrips":nstrips,"DCU":dcu,"MUX":mux,"PLL":pll,"LLD":lld}}})
       
      
    for fedcrate, fedslot, fedid, feunit, fechan, fedch, feccrate, fecslot, fecring, ccuaddr, ccuchan, dcuid, detid, lldchan, apv0, apv1, pairnumber, npairs, nstrips, dcu, mux, pll, lld in zip(FedCrateList, FedSlotList, FedIdList, FeUnitList, FeChanList, FedChList, FecCrateList, FecSlotList, FecRingList, CcuAddrList, CcuChanList, DcuIdList, DetIdList, LldChanList, APV0List, APV1List, pairNumberList, nPairsList, nStripsList, DCUList, MUXList, PLLList, LLDList): 
  
        fedkey = makeFedKey(fedid, feunit, fechan)                
        if fedkey in DictionaryCab.CablingInfoDictF.keys(): 
            DictionaryCab.CablingInfoDictF[fedkey].update({fedch:{"FedCrate": fedcrate,"FedSlot":fedslot,"FedId":fedid,"FeUnit":feunit,"FeChan":fechan,"FedCh":fedch,"FecCrate":feccrate,"FecSlot":fecslot,"FecRing":fecring,"CcuAddr":ccuaddr,"CcuChan":ccuchan,"DcuId":dcuid,"DetId":detid,"pairNumber":pairnumber,"LldChan":lldchan,"APV0":apv0,"APV1":apv1,"nPairs":npairs,"nStrips":nstrips,"DCU":dcu,"MUX":mux,"PLL":pll,"LLD":lld}})
               
        else:
            DictionaryCab.CablingInfoDictF.update({fedkey:{fedch:{"FedCrate": fedcrate,"FedSlot":fedslot,"FedId":fedid,"FeUnit":feunit,"FeChan":fechan,"FedCh":fedch,"FecCrate":feccrate,"FecSlot":fecslot,"FecRing":fecring,"CcuAddr":ccuaddr,"CcuChan":ccuchan,"DcuId":dcuid,"DetId":detid,"pairNumber":pairnumber,"LldChan":lldchan,"APV0":apv0,"APV1":apv1,"nPairs":npairs,"nStrips":nstrips,"DCU":dcu,"MUX":mux,"PLL":pll,"LLD":lld}}})
    
    
    return DictionaryCab
	
	
#############################	

MyFilename=filenameF("195790")
#MyFilename=filenameF("205900")
#MyFilename=filenameF("302081") # for 2017 scan

print "Loading dictionary ..."
ourdictionary=DictionaryCab(MyFilename)
print "Dictionnary loaded"

#Info about a (set of) module(s)
	
#li1 = [369121605, 369125866]
output="test_out.txt"
txtmod=open(output,'w')
dculist=[]
cb=DictionaryCab.CablingInfoDictF
print cb.items()[0:2]

#for i in cb.keys():
#	if i in cb:
#		txtmod.write("\n\n For DetID : %s \n" %i) 
#		for j in cb[i]:
#			cbp=cb[i][j]
#			if  (cbp["DcuId"]) not in dculist:
#				dculist.append(cbp["DcuId"])
#				txtmod.write("\n"+"DcuId"+cbp["DcuId"]+"/DetId"+cbp["DetId"]+"/nPairs"+cbp["nPairs"]+"/nStrips"+cbp["nStrips"]+"/FecCrate"+cbp["FecCrate"]+"/FecSlot"+cbp["FecSlot"]+"/FecRing"+cbp["FecRing"]+"/CcuAddr"+cbp["CcuAddr"]+"/CcuCh"+cbp["CcuChan"])#
#
#			txtmod.write("\n"+"/FedCrate"+cbp["FedCrate"]+"/FedSlot"+cbp["FedSlot"]+"/FedId"+cbp["FedId"]+"/FeUnit"+cbp["FeUnit"]+"/FeChan"+cbp["FeChan"]+"/FedCh"+cbp["FedCh"]+"/LldChan"+cbp["LldChan"]+"/APV0-"+cbp["APV0"]+"/APV1-"+cbp["APV1"])
#

for i in cb.keys():
	for j in cb[i]:
		cbp=cb[i][j]
		#txtmod.write("\n FedKey "+str(i)+" DetId "+cbp["DetId"]+" /FedCrate"+cbp["FedCrate"]+"/FedSlot"+cbp["FedSlot"]+"/FedId"+cbp["FedId"]+"/FeUnit"+cbp["FeUnit"]+"/FeChan"+cbp["FeChan"]+"/FedCh"+cbp["FedCh"]+"/LldChan"+cbp["LldChan"]+"/APV0-"+cbp["APV0"]+"/APV1-"+cbp["APV1"])
		txtmod.write("\n FedKey "+str(i)+" DetId "+cbp["DetId"]+" APV "+cbp["APV0"]+" "+cbp["APV1"]+" /FecCrate"+cbp["FecCrate"]+"/FecSlot"+cbp["FecSlot"]+"/FecRing"+cbp["FecRing"]+"/CcuAddr"+cbp["CcuAddr"]+"/CcuChan"+cbp["CcuChan"]+"/LldChan"+cbp["LldChan"]+"/APV0-"+cbp["APV0"]+"/APV1-"+cbp["APV1"])
  

print("A file named %s  has been created"%output)
