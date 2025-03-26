SiStripRadMonitoring
====================
Twiki to look at: https://twiki.cern.ch/twiki/bin/view/CMS/SiStripSignalHVScansCode and https://twiki.cern.ch/twiki/bin/view/CMS/SiStripSignalHVScans
====================

Since the next person will take over my workspace, here are a few things to know.

While the repository should be cloned on lxplus, I wasn't able to make the crab job submition work on el8 and el9 (because CERN removed slc7)

And I also couldn't make it work with cmssw-el7 (If i recall correctly but one could try).

# Solution :

Clone the code on the ui (that are using slc7) and launch the crab jobs here.

One also has to run the SignalAnalysis and ClusterWidthAnalysis on the ui.

Then, you have to swith to lxplus (and so copy the root files from the ui to lxplus) to produce the plots in the CurvesAnalysis directory.

# Why so?

Leakage current corrections are only available on lxplus, and all the root files are stored on lxplus on a common directory, so it is easier to run on lxplus to compare runs together

====================

Note : Becareful, in the ~/SiStripRadMonitoring/SignalBiasScan/Analysis/VoltageSteps/Code/VoltageStepsMaker.C file, 

it seems that when running on the ui, you do not need to take into account a time shift between the elog and the real time, which was always the case on lxplus. => see line ~35 (probably something related to cern 

machines that are always 1h shifted with respect to local time in strasbourg)

Compile with : scramv1 b -j4 (in SiStripRadMonitoring directory)

When running the crab jobs, the output will be stored in : /eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/

!!!!!!!!! REALLY IMPORTANT !!!!!!!!

You need to copy  the clusters Tree in your ui envrionment from eos to be able to run the next code.

Indeed, In SignalAnalysis and ClusterWidthAnalysis, you can run Run_Tree_Compiled that eeds the ClustersTree*.root. This can only be done on the ui.

Even if the code runs on lxplus, the data is not registered so ui is mandadory IN THE SINGULARITY

====================

There is always a Makefile to compile the needed code. You basically just have to add the latest run with the date on the associated luminosity (use brilcalc from twiki)
So you just need to type "make" :D
Still, some macros need to be compiled with .L in root but usually thes emacros are for plotting 

====================

The common direcotory I am mentioning above can be different depending on what you want (I will let you discover that) : 

/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/SignalAnalysis/Code/Outputs

/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/ClusterWidthAnalysis/Code/Outputs

/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/DECO_files_kink_range_fix

====================

In ClusterWidthAnalysis and SignalAnalysis, it's quite straight forward to understand, just update : Run_TreeMaker_compiled.C

You actually "need" to create the file in the Run_TreeMaker_files directory and copy paste the code in the Run_TreeMaker_compiled.C file

so that we can keep track of all the runs analyzed. Then, you can put the outputs in the common directory and switch ti the CurvesAnalysis directory.

PS : you can put the plots on your environment but for some codes, it can be annoying. I will let you explore this ..

====================

For CurvesAnalysis, you have to take care of DrawKinkVsLumi.cc, CompareCurves.cc, Fit.cc and FitAll.cc. (well, you just have to add the latest run and select what type of plot you want, so nothing hard)

You just have to run FitAll.exe (type "make" :D ) first and then the other ones (DrawKinkVsLumi.exe, ...) should run smoothly

====================

Fit.cc / FitAll.cc : Apply the line and kink methods (I actually never modified those files) on the CW and Signal Curves

!! You can select the modules you want to analyze (can be usful to analyze specific power supplies or problematic modules)

I'm sorry, there might be some "saveas(<name>.png)" that are left out uncommented and it might give you a lof of png file in your nevironment

====================

CompareCurves.cc : This macro is important since it's usually the one you use to make plots that are shown to compare CW and Signal curves  between scans. Therefore, you can monitor the changes of the Vfd value just with this macro.

!! You can select the modules you want (ideal when you want to analyze specific modules). You can also slect the runs you want
to compare. 

You have here common directories that is given : 

/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/SignalAnalysis/Code/Outputs

/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/ClusterWidthAnalysis/Code/Outputs

The outputs are stored in : /SignalBiasScan/Analysis/CurvesAnalysis/plots/CW/compareCurve and /SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/signal/compareCurve


====================

 DrawKinkVsLumi.cc : This macro allows to make all the plots : Vfd_Drop_Per_Layer_MultiScans.pdf, Vfd_Drop_Per_Layer_MultiScans_TIB(TOB,TID,TEC).pdf vs integrated luminosity or fluence, Vfd_Drop_Per_Layer_CW.pdf. You can also produce plots that superimpose the kink and line methods, see

/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/superimpose_curves

Usually, these plots are good for a global presentation, with an overview of all the layers


====================

In DrawKinkVsLumi.cc, you may see : the  "simulation2025.root" file. (This one is generated by me :D )

It simulates the full depletion voltage evolution vs Int.Lumi up until the end of Run 3 (and potentially beyond).

!!! The latest predictions were made in January 2025 ..., so it might not be accurate at your point in time, anyway, you may have to redo the predictions if you start after me :D

!!! Full Depletion Voltage Prediction code :  /afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution !!!

=============

Make DeltaVFD or Vfd plots with leakage current and CW, CC varaibles => SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/superimpose_curves/superimpose_results.C macro

Output plots are in : /SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/superimpose_curves

but these plots are easily replicable. The inputs are generated from FitleakageCurrent.C (LeakageCurrent direcotry) and the CompareCurves.cc from the SignalAnalysis directory

The possibilities are  :
    // -- Smallscan --//
    // TIBL1
    // TOBL1
    // TOBL4
    //
    //-- Fullscan --//
    // TIBL1
    // TIBL4
    // TOBL1
    // TOBL4
    //-----------------

================


====================

Global Note 1 : When analyzing runs, make sure to run the code for all sub-detectors (TIB,TOB,TID,TEC) because backward compatibility 
(analyzing a run 2 scan for example) is not really possible anymore...

Global Note 2 : Cw was a better observable before type inversion (so middle of Run 2 for TIB L1) and 
CLuster charge is a better observable after type inversion (at least up until beginning of 2025).

So you might want to show plot using the clutser charge in your case. You will find out by yourself i guess.

Global Note 3 : For the leakage current observable, see /SiStripRadMonitoring/LeakageCurrentBiasScan/README.md

====================
Happy Coding !!

Adieu,

Paul Vaucelle (Ph.D 2022 - 2025)
