
#Script to run TreeMaker code via batch

# $1 output path
if [ "$1" != "" ]
  then RUN=$1
  
  else 
  	echo "Syntax : bsub -q8nh script_runTreeMake_batch.sh [RUN NR] [PROXY NAME]"
 	exit
fi

##############################

# CMSSW directory -- no '/' at the end
CMSSWDIR=/afs/cern.ch/work/n/ntonon/public/CMSSW_9_2_10/src

#starting directory
echo "You are working here : "
pwd
echo "Your CMSSW directory is :"
echo $CMSSWDIR
echo

cd $CMSSWDIR
eval `scramv1 runtime -sh`
cd -

# set proxy to read files on the grid
if [ "$2" != "" ] ; then
    echo " setting proxy: $2"
    export X509_USER_PROXY=/afs/cern.ch/user/n/ntonon/$2
fi
#############################

#PAY ATTENTION TO PATH !
cd /afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_9_2_10/src/SiStripRadMonitoring/SignalBiasScan/Analysis/SignalAnalysis/Code

echo "Will execute script from : "
pwd

make

./Run_TreeMaker_compiled

OUTPUTNAME="TIB_output_DecoMode_"$RUN".root"
mv TIB_output_DecoMode.root Outputs/$OUTPUTNAME

OUTPUTNAME="TOB_output_DecoMode_"$RUN".root"
mv TOB_output_DecoMode.root Outputs/$OUTPUTNAME

OUTPUTNAME="TEC_output_DecoMode_"$RUN".root"
mv TEC_output_DecoMode.root Outputs/$OUTPUTNAME

OUTPUTNAME="TID_output_DecoMode_"$RUN".root"
mv TID_output_DecoMode.root Outputs/$OUTPUTNAME
