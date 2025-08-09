/* Title */
=============

README File for determining the full depletion volatge using the leakage current.

=============
/* Context */
=============

-The leakage current information comes from DCU and/or PS measurement.

It can happen that the DCU measurement is not available for many reasons. When this happens,
the PS information is used and the following assumption is made : the amount of current is 
equally divided for the modules associated to the APV.

The leakage current method is implemented to be complementary w.r.t the cluster width and 
cluster charge methods. Saturation/ change of regime is also expected in the leakage current
figures.

=============
/*  Code  */
=============

Main file is FitLeakageCurrent.C (This file is quite a pain, i'm sorry)

 / ----- Main Idea ----- /
        
 There are 3 curves that are being analyzed : ILeak VS Vbias, the associated first and 
 second derivatives. We will try to fit each of these curves with different functions
 available in the ./FitFunctions.h file. There are 10 functions that are used at the 
 moment.
        
 // !!!!!!!!!! PLS, if you want to make changes, add a new run, look at the comments in the macro (especially in the main function a the end of the file) !!!!!!!//
        
 / ---- Compilation ---- /
        
 There are two ways of running the FitLeakageCurrent.C file that corresponds to two ways of running the code.
        
 / ----- Analyzing a scan ---- /
 
Comment all the scans except the one you want to analyze and uncomment the "saveas" lines around lines 2200.
 
$ make
 
$ ./FitLeakageCurrent.exe 
 
This will save the data with the fits so that you can look at the fits and correct for the parameters
The boring part is : parameters of the fits may have to be adjusted for each run and for each subdet (even for each layer of a subdet...)

/ ----- Analyzing all the runs ---- /

 | Comment the getchar() line
 
$ make

$ ./FitLeakageCurrent.exe 

This method will run the main() of the FitLeakageCurrent.C file.
This is used to analyze many runs with many detids since there is no 
display of the fits.
You can use the Plot.. function to register the VFD values for either
small or full scans (there is a boolean :D ). A full scan
is about 1h, so be sure of what you want. A small scan is 
about 1 min.(Considering the current amount of runs being analyzed)
The DeltaVFD means that we are doing the difference between the current
value and the initial value of Vfd.

|----|--------| ----- END ------ /

============

Make grey-ish area plots:

There is only one macro to use: SummaryPlot.C. The parameters to change are at the beginning of the scan (subdet, layer, small or full scan)

-Input for this macro is generated with FitLeakageCurrent.C

The different possibilities are :

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
       
If you want to add other layers, you have to compute the fits for the layer that you want with the FitLeakageCurrent.c macro.



================

Compare leakage current  of a given detid using => CompareLeakageCurrent.C

- Select the runs and the detids that you want (be careful, in smallscans you only have a few detids)

- Up to 10 runs can be compared (limited by the color palette of cms)

- Input for this macro is generated with FitLeakageCurrent.C

================

Compare leakage current  of a given detid * resistance using => CompareLeakageCurrentTimesResistance.C

- Select the runs and the detids that you want (be careful, in smallscans you only have a few detids)

- Up to 10 runs can be compared (limited by the color palette of cms)

- Input to this macro is generate with ComputeIleakCorrections.C

================



The lists of detids for TIB and TOB are available in this directory as TIB_list.txt and TOB_list.txt :D

You now have everything,
Happy Coding !

Paul


