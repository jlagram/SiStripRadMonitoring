#!/bin/sh

# no TID modules in Full Scans
convertSmallScan()
{
 input=$1
 run=$2
 echo "converting $1 to $2 for TIB,TOB,TEC"
 python2.7 PSCurrentToRoot.py -subdet=TIB $input.csv
 mv $input.root PS_I_TIB_$run.root
 python2.7 PSCurrentToRoot.py -subdet=TOB $input.csv
 mv $input.root PS_I_TOB_$run.root
 python2.7 PSCurrentToRoot.py -subdet=TEC $input.csv
 mv $input.root PS_I_TEC_$run.root
}


convertFullScan()
{
 input=$1
 run=$2
 convertSmallScan $input $run
 echo "converting $1 to $2 for TID"
 python2.7 PSCurrentToRoot.py -subdet=TID $input.csv
 mv $input.root PS_I_TID_$run.root
}


#convertFullScan ../FullBiasScan_20120405142300_20120405212600 20120405_run190459
#convertSmallScan SmallBiasScan_20120506150000_20120506163500 20120506_run193541
#convertFullScan ../FullBiasScan_20120510212900_20120510231000 20120510_run193928
#convertSmallScan SmallBiasScan_20120728152000_20120728181500 20120728_run199832
#convertFullScan ../FullBiasScan_20120812234000_20120813023600 20120812_run200786
#convertSmallScan SmallBiasScan_20120928172000_20120928204500 20120928_run203832
#convertSmallScan SmallBiasScan_20121130183000_20121130235959 20121130_run208339
#convertSmallScan SmallBiasScan_20130213091000_20130213103500 20130213_run211797

#convertFullScan ../FullBiasScan_20150603205500_20150604234500 20150603_run246963
#convertSmallScan SmallBiasScan_20150821073500_20150821090500 20150821_run254790
#convertSmallScan SmallBiasScan_20151007083500_20151007094500 20151007_run258443
#convertSmallScan SmallBiasScan_20151121101000_20151121112000 20151121_run262254

#convertFullScan ../FullBiasScan_20160423002700_20160423020500 20160423_run271056
#convertSmallScan SmallBiasScan_20160612090000_20160612101500 20160612_run274969
#convertSmallScan SmallBiasScan_20160706111500_20160706122500 20160706_run276437
#convertSmallScan SmallBiasScan_20160803190000_20160803201500 20160803_run278167
##convert SmallBiasScan_20160909073000_20160909141000
#convertSmallScan SmallBiasScan_20160909103000_20160909121000 20160909_run280385
#convertSmallScan SmallBiasScan_20161116150500_20161116163000 20161116_run285371
#convertSmallScan SmallBiasScan_20171030120700_20171030131100 20171030_run305862

#convertFullScan FullBiasScan_20180418021000_20180418040500 20180418_run314574 
#convertFullScan FullBiasScan_20180419235000_20180420014500 20180419_run314755
#convertSmallScan SmallBiasScan_2018053011220_20180530130200 20180530_run317182
#convertSmallScan SmallBiasScan_20180611163500_20180611174700 20180611_run317683
#convertSmallScan SmallBiasScan_20180801082500_20180801102500 20180801_run320674
#convertFullScan FullBiasScan_20180923130000_20180923151500 20180923_run323370_part1
#convertFullScan FullBiasScan_20180923203500_20180923212200 20180923_run323370_part2

## Noise bias scan
#convertSmallScan SmallBiasScan_20170919184100_20170920093700 noise_20170919_run303272
#convertFullScan FullBiasScan_20180618095500_20180618122500 noise_20180618_run317974_part1 
#convertFullScan FullBiasScan_20180619144500_20180619155500 noise_20180618_run317974_part2 
#convertFullScan FullBiasScan_20180919071200_20180919093200 noise_20180919_run323011
