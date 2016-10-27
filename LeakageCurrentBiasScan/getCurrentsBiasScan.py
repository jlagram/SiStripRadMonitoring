## This is a little script to query the power supply data from the DCS cond DB for bias voltage scans
## author: Christian.Barth@cern.ch
## 2016 @ CERN 

import cx_Oracle
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as dates

def str2bool(v):
	return v.lower() in ("yes", "true", "t", "1")

smallScan = raw_input("Is it a small scan? (yes/no) ") 
issmall = str2bool(smallScan)
startTimeRaw = raw_input("Enter start of the scan in the format provided by the script: ")
endTimeRaw = raw_input("Enter end of the scan in the format provided by the script: ")

startTime = str(startTimeRaw)
endTime = str(endTimeRaw)

### Opening connection
connection = cx_Oracle.connect('cms_trk_r/1A3C5E7G:FIN@cms_omds_adg')
cursor = connection.cursor()
cursor.arraysize=50

## query for full scan		      
query="""
	with cables as (
	  select distinct substr(lal.alias,INSTR(lal.alias,  '/', -1, 2)+1)  cable, id dpid, cd from
	    (select max(since) as cd, alias from  cms_trk_dcs_pvss_cond.aliases group by alias ) md, cms_trk_dcs_pvss_cond.aliases lal
	  join cms_trk_dcs_pvss_cond.dp_name2id on dpe_name=concat(dpname,'.')
	  where md.alias=lal.alias and lal.since=cd
	  and (lal.alias like '%TIB%' or lal.alias like '%TOB%' or lal.alias like '%TEC%')
	)
	select cable, change_date,actual_Imon from cms_trk_dcs_pvss_cond.fwcaenchannel caen
	join cables on (caen.dpid=cables.dpid)
        where change_date >= (to_timestamp(:the_start_time,'YYYYMMDDHH24MISS') - interval '03' minute)
        and change_date < to_timestamp(:the_end_time,'YYYYMMDDHH24MISS')
        and actual_imon is not null
	order by cable, change_date
	"""

## query for small bias scan
query2="""
	with cables as (
	  select distinct substr(lal.alias,INSTR(lal.alias,  '/', -1, 2)+1)  cable, id dpid, cd from
	    (select max(since) as cd, alias from  cms_trk_dcs_pvss_cond.aliases group by alias ) md, cms_trk_dcs_pvss_cond.aliases lal
	  join cms_trk_dcs_pvss_cond.dp_name2id on dpe_name=concat(dpname,'.')
	  where md.alias=lal.alias and lal.since=cd
	  and (lal.alias like '%TIBplus_1_6_2_5_%' or lal.alias like '%TOBplus_4_#_3_8%' or lal.alias like '%TECminus_3_7_1_1_2%' or lal.alias like '%TECminus_3_7_1_1_3%' or lal.alias like '%TIBminus_1_2_2_1%')
	)
	select cable, change_date,actual_Imon from cms_trk_dcs_pvss_cond.fwcaenchannel caen
	join cables on (caen.dpid=cables.dpid)
        where change_date >= (to_timestamp(:the_start_time,'YYYYMMDDHH24MISS') - interval '03' minute)
        and change_date < to_timestamp(:the_end_time,'YYYYMMDDHH24MISS')
        and actual_imon is not null
	order by cable, change_date
	"""
	
print startTime		      
print endTime		    
print "Small Scan:"
print issmall  
if issmall:
	cursor.execute(query2, {"the_start_time" : startTime, "the_end_time" : endTime})
else: 
	cursor.execute(query, {"the_start_time" : startTime, "the_end_time" : endTime})
row = cursor.fetchall()		      
if issmall:
	fileCurrents = "SmallBiasScan_" + str(startTime) + "_" + str(endTime) + ".csv"
else:
	fileCurrents = "FullBiasScan_" + str(startTime) + "_" + str(endTime) + ".csv"
fcur = open(fileCurrents, "w+")
	
		      
for i in xrange(len(row)):			      
	fcur.write(str(row[i][0]) + "," + str(row[i][1]) + "," + str(row[i][2])+ "\n")
		      
fcur.close()
connection.close()

