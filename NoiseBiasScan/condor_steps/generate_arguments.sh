#!/bin/sh


# $1 : scan log file
if [ "$1" != "" ]
  then
    echo "Using log file $1"
  else
    echo " Need a scan log file as input. exit."
    exit
fi

filelist="list.txt"
if [ "$2" != "" ]
  then
    filelist=$2
    echo "Using file list $filelist"
fi


previous_istep=0
while read istep timestamp run event step nevents
do
 if [ $istep -eq $previous_istep ]; then
   echo computeNoiseFromRaw_cfg.py x509up_u16737 SiStripCommissioningSource_$step.root ${2}_step$step.txt EventRange=$previous_run:$previous_event-$run:$event
 fi 
 previous_istep=$istep
 previous_run=$run
 previous_event=$event
done < $1

