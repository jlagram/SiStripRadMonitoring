#ifndef Fit_h
#define Fit_h

//Contains functions to get TGraphs, Leakage corrections, perform curve smoothing, etc.
#include "../CommonTools/CurvesFunctions.h"
#include "../CommonTools/tdrstyle.C"

#include "TROOT.h"
#include "TRint.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TGraphSmooth.h"
#include <sstream>
#include <sys/stat.h> // to be able to check file existence

//---------------------------------------------
 // #    # ###### #      #####  ###### #####     ###### #    # #    #  ####  ##### #  ####  #    #  ####
 // #    # #      #      #    # #      #    #    #      #    # ##   # #    #   #   # #    # ##   # #
 // ###### #####  #      #    # #####  #    #    #####  #    # # #  # #        #   # #    # # #  #  ####
 // #    # #      #      #####  #      #####     #      #    # #  # # #        #   # #    # #  # #      #
 // #    # #      #      #      #      #   #     #      #    # #   ## #    #   #   # #    # #   ## #    #
 // #    # ###### ###### #      ###### #    #    #       ####  #    #  ####    #   #  ####  #    #  ####
//---------------------------------------------

//Evaluate crossing point of 2 lines (for linear fit method)
double getCrossingPoint ( TF1* function1, TF1* function2)
{
	double x_crossingpoint;
	double oao_function1, cd_function1, oao_function2, cd_function2;
	oao_function1 = function1->GetParameter(0); // ordonnée à l'origine
	cd_function1 = function1->GetParameter(1); // coefficient directeur
	oao_function2 = function2->GetParameter(0);// ordonnée à l'origine
	cd_function2 = function2->GetParameter(1);// coefficient directeur

	// at the crossing point, we have cd_function1*x_crossingpoint + oao_function1 = cd_function2*x_crossingpoint + oao_function2
	// it is the same as having x_crossingpoint = (oao_function2 - oao_function1)/(cd_function1 - cd_function2 )

	x_crossingpoint = (oao_function2 - oao_function1)/(cd_function1 - cd_function2 );

	return x_crossingpoint;
}


void Create_Plot_Directories()
{
	mkdir("plots",0777);
	mkdir("plots/CW",0777);
	mkdir("plots/signal",0777);
	mkdir("plots/CW/kink",0777);
	mkdir("plots/CW/line",0777);
	mkdir("plots/signal/kink",0777);
	mkdir("plots/signal/line",0777);
	mkdir("plots/CW/kink/simu",0777);
	mkdir("plots/CW/line/simu",0777);
	mkdir("plots/signal/kink/simu",0777);
	mkdir("plots/signal/line/simu",0777);
	mkdir("plots/signal/compareCurve",0777);
	mkdir("plots/CW/compareCurve",0777);
	mkdir("plots/CW/relative",0777);
	mkdir("plots/signal/relative",0777);
	mkdir("plots/superimpose_CW_signal",0777);
}




























//---------------------------------------------
// ######## #### ########          ###    ##        ######    #######
// ##        ##     ##            ## ##   ##       ##    ##  ##     ##
// ##        ##     ##           ##   ##  ##       ##        ##     ##
// ######    ##     ##          ##     ## ##       ##   #### ##     ##
// ##        ##     ##          ######### ##       ##    ##  ##     ##
// ##        ##     ##          ##     ## ##       ##    ##  ##     ##
// ##       ####    ##          ##     ## ########  ######    #######
//---------------------------------------------


/**
 * Fit the curves to extract Vfd : currently 2 different methods are implemented ('lines' & 'kink')
 * @param  g             [TGraph of observable vs voltage]
 * @param  debug         [verbosity for debugging]
 * @param  filter_twice  [if true, Hanning smoothing is applied twice]
 * @param  use_curvature [if true, return value from kink method ; else from 'lines' method]
 * @param  showplots     [useless]
 * @param  verbose       [verbosity of function]
 * @param  observable    [signal or CW]
 * @param  draw_plot     [to avoid drawing in FitAll.exe (only interested in Vfd values)]
 * @return               [Vfd value extracted]
 */
double FitCurve(TGraphErrors* g, int debug, bool filter_twice, bool use_curvature, bool verbose=true, TString observable = "", bool draw_plot=true)
{
	if(!g) {cout<<BOLD(FRED("Input graph is null -- Abort"))<<endl; return -1; }

//--------------
 //  ####  #####  ##### #  ####  #    #  ####
 // #    # #    #   #   # #    # ##   # #
 // #    # #    #   #   # #    # # #  #  ####
 // #    # #####    #   # #    # #  # #      #
 // #    # #        #   # #    # #   ## #    #
 //  ####  #        #   #  ####  #    #  ####
//--------------
	debug = 0; //Can force here the verbosity for debugging

	bool SGSmooth=false; //if True, SavitzkyGolaySmoother is applied on curve


//---------------------------------------------
 //  ####  #    #  ####   ####  ##### #    # # #    #  ####
 // #      ##  ## #    # #    #   #   #    # # ##   # #    #
 //  ####  # ## # #    # #    #   #   ###### # # #  # #
 //      # #    # #    # #    #   #   #    # # #  # # #  ###
 // #    # #    # #    # #    #   #   #    # # #   ## #    #
 //  ####  #    #  ####   ####    #   #    # # #    #  ####
//---------------------------------------------

//Smoothing of the TGraph
//Tried many possibilities (e.g. TGraphSmoother ROOT class) --> In the end, found more effective to perform only minor smoothing, to preserve information

	TGraphErrors *g_nosmooth = new TGraphErrors(g->GetN(), g->GetX(), g->GetY() , g->GetEX(), g->GetEY()); //Store "original" TGraph before it is smoothed with MedianFillter ; can be necessary not to apply any smoothing (e.g. in cases were Vfd ~ 0, not to loose information)

	g = MedianFilter(g); //Basic filter using the median of 3 points to smooth some stat. fluctuations (def. in CurvesFunctions.h)

	//Needed to do this to avoid weird bug of top line fit -- TBC : check if still necessary
	Double_t* err_Y = g->GetEY();
	for(int i=0; i<g->GetN(); i++)
	{
		g->SetPointError(i, 0, err_Y[i]);
	}


	bool do_supersmoothing = false;
	bool do_approx = false;

	//Tmp TGraph used to apply smoothing to original TGraph
	TGraphErrors *g_tmp = new TGraphErrors(g->GetN(), g->GetX(), g->GetY() , g->GetEX(), g->GetEY());
	g_tmp = MedianFilter(g_tmp);

	//Use points from smoothed graph & error bars from original graph -- Used for 'lines' method
	TGraph* g_fit = new TGraphErrors(g_tmp->GetN(), g_tmp->GetX(), g_tmp->GetY() , g->GetEX(), g->GetEY() );






//--------------
// # #    # # ##### #   ##   #      # ###### ######
// # ##   # #   #   #  #  #  #      #     #  #
// # # #  # #   #   # #    # #      #    #   #####
// # #  # # #   #   # ###### #      #   #    #
// # #   ## #   #   # #    # #      #  #     #
// # #    # #   #   # #    # ###### # ###### ######
//--------------

	//Variables used throughout the function (to store coordinates, etc.)
	double y_tmp = 0, x_tmp = 0; //tmp coordinates
	double ymax=0; //Store maximum y-value of the TGraph
	double ymin=999; //Store minimum y-value of the TGraph
	for(int ipt=0; ipt<g->GetN(); ipt++)
	{
		g->GetPoint(ipt, x_tmp, y_tmp);

		// cout<<"ipt = "<<ipt<<" / x = "<<x_tmp<<" / y  = "<<y_tmp<<endl;

		if(y_tmp>ymax) {ymax=y_tmp;}
		if(y_tmp<ymin) {ymin=y_tmp;}
	}

	double ymax_nosmooth=0; //Store maximum y-value of the non-smoothed TGraph (for protections, see below)
	for(int ipt=0; ipt<g_nosmooth->GetN(); ipt++)
	{
		g_nosmooth->GetPoint(ipt, x_tmp, y_tmp);

		// cout<<"ipt = "<<ipt<<" / x = "<<x_tmp<<" / y  = "<<y_tmp<<endl;

		if(y_tmp>ymax_nosmooth) {ymax_nosmooth=y_tmp;}
	}

	int status = 0; //Store the fit status








//---------------------------------------------
// ##       #### ##    ## ########    ##     ## ######## ######## ##     ##  #######  ########
// ##        ##  ###   ## ##          ###   ### ##          ##    ##     ## ##     ## ##     ##
// ##        ##  ####  ## ##          #### #### ##          ##    ##     ## ##     ## ##     ##
// ##        ##  ## ## ## ######      ## ### ## ######      ##    ######### ##     ## ##     ##
// ##        ##  ##  #### ##          ##     ## ##          ##    ##     ## ##     ## ##     ##
// ##        ##  ##   ### ##          ##     ## ##          ##    ##     ## ##     ## ##     ##
// ######## #### ##    ## ########    ##     ## ########    ##    ##     ##  #######  ########
//---------------------------------------------

if(verbose) cout<<"* Linear Fits Method :";

//-----------------------------------------
//--- Method 1 : Fitting the curves with 2 lines (two regimes : increase & plateau)
//--- Currently used method, updated to take into account many different curve scenarii
//--- NB : imperfect method relying on hypothesis ! In reality, there are not 2 linear regimes, but a convolution of multiple effects
//--- This method is relying on many protections & special cases to account for "not ideal" curves !
//--- As stated in comments below, the current algorithm is done in a few steps : first a "first approximation" of the bottom/top lines, then we again try to move the boundaries of both line and keep the configuration which minimizes chi-2
//--- Therefore this algorithm is not the most robust/logical/..., but is rather an ad-hoc way to extract the Vfd, based on the observation of *MANY* different shapes of curves, leading to an algorithm which is able to get a reasonable estimate of Vfd in most cases (even if the extracted value must be biased because it does not take any theoretical effect into account)
//-----------------------------------------


  TF1* f3_high = new TF1("fphigh", "pol1", 20, 360); //Fitted line 1 - plateau regime
  f3_high->SetLineColor(kBlue);

  TF1* f3_low = new TF1("fplow", "pol1", 20, 360); //Fitted line 2 - increasing regime
  f3_low->SetLineColor(kGreen);

  double chi2_high=0, chi2_low=0; //Store the chi-2 scores of the fit, minimized by algorithm

  double chi2_threshold = 0; //Arbitrary chi-2 threshold, needed to determine whether a configuration is 'good enough' or not












//---------------------------------------------
//    ###    ########  ########  ########   #######  ##     ##           ##       #### ##    ## ########  ######
//   ## ##   ##     ## ##     ## ##     ## ##     ##  ##   ##            ##        ##  ###   ## ##       ##    ##
//  ##   ##  ##     ## ##     ## ##     ## ##     ##   ## ##             ##        ##  ####  ## ##       ##
// ##     ## ########  ########  ########  ##     ##    ###              ##        ##  ## ## ## ######    ######
// ######### ##        ##        ##   ##   ##     ##   ## ##             ##        ##  ##  #### ##             ##
// ##     ## ##        ##        ##    ##  ##     ##  ##   ##  ###       ##        ##  ##   ### ##       ##    ##
// ##     ## ##        ##        ##     ##  #######  ##     ## ###       ######## #### ##    ## ########  ######
//---------------------------------------------

//--- We start by getting some "first approximation" of what the bottom and top lines will look like (i.e. which points are included in each one)
//--- Afterwards, we will refine the lines by running a second algorithm
//--- In both case, the idea is to minimize the sum of the chi-2 scores, while satisfying some protections/requirements




//---------------------------------------------
 // #####  ####  #####
 //   #   #    # #    #
 //   #   #    # #    #
 //   #   #    # #####
 //   #   #    # #
 //   #    ####  #
//---------------------------------------------

//Here we have a first attempt at attributing points to the "top line" -- Afterwards, it is further optimized

	status = 0;
	chi2_threshold = 0.0001; //Arbitrary threshold

	//-- Points indices
	int npt_high_end = g_fit->GetN()-1; //Store index of last point attributed to "top line"
	int npt_high_start = g_fit->GetN()-5; //Store index of first point attributed to "top line" -- Start with 4 points (arbitrary)

	//Store x values of start/end points
	double vdep_high_start = 0;
	double vdep_high_end = g_fit->GetPoint(npt_high_end, vdep_high_end, y_tmp);


	//From "right to left", try to include as many points as possible in the "top line" -- Stop when chi-2 too high, or if x<100 V
	for(int i=npt_high_start; i>0; i--)
	{
		//Get coordinates of selected point
		g_fit->GetPoint(i, vdep_high_start, y_tmp);

		//Fit the selected points
		f3_high->SetRange(vdep_high_start, vdep_high_end);
		status = g_fit->Fit("fphigh", "rqnEX0"); //quiet, use range, don't draw
		chi2_high = f3_high->GetChisquare()/f3_high->GetNDF();

		//cout<<"f3_high->GetChisquare() = "<<f3_high->GetChisquare()<<endl;
		//cout<<"f3_high->GetNDF() = "<<f3_high->GetNDF()<<endl;
		if(debug==1) cout<<endl<<"Top Fit : "<<i<<", chi2 "<<chi2_high;

		//If chi2 too high, retry fit after moving ending point to the left
		if(chi2_high > chi2_threshold || vdep_high_start < 100)
		{
			//Tmp point coordinates
			g_fit->GetPoint(npt_high_end-1, vdep_high_end, y_tmp);

			//Perform fit on updated range
			f3_high->SetRange(vdep_high_start, vdep_high_end);
			status = g_fit->Fit("fphigh", "rqn");
			chi2_high = f3_high->GetChisquare()/f3_high->GetNDF();

			//If chi2 still too high or ending point >= 4th point (from right) --> Stop there & go back to last point!
			if(chi2_high > chi2_threshold || npt_high_end < g_fit->GetN()-3) {npt_high_start = i+1; break;}
			else {npt_high_start = i; npt_high_end--; if(verbose) cout<< " --> Move ending point ";} //Else move points
		}
		else {npt_high_start = i;} //Move 'top line' starting point to the left
	}

	//Set voltage range of 'top line' & fit it
	g_fit->GetPoint(npt_high_end, vdep_high_end, y_tmp);
	g_fit->GetPoint(npt_high_start, vdep_high_start, y_tmp);

	f3_high->SetRange(vdep_high_start, vdep_high_end); //Perform final top line fit : [starting point;350] V
	g_fit->Fit("fphigh", "rqn");

	//Protection against 'top line' using too many points //-- not needed anymore ?
	// if(npt_high_start > g_fit->GetN()-3)  {cout<<FRED("Top line fit covers too many points !")<<endl; return -1;}





//---------------------------------------------
 // #####   ####  ##### #####  ####  #    #
 // #    # #    #   #     #   #    # ##  ##
 // #####  #    #   #     #   #    # # ## #
 // #    # #    #   #     #   #    # #    #
 // #    # #    #   #     #   #    # #    #
 // #####   ####    #     #    ####  #    #
//---------------------------------------------

//Here we have a first attempt at attributing points to the "bottom line" -- Afterwards, it is further optimized

	status = 0;
	chi2_threshold = 0.0001; //Arbitrary threshold

	//-- Points indices
	int npt_low_start  = 0; //Store index of first point attributed to 'bottom fit'
	int npt_low_end  = 4; //Store index of last point of 'bottom fit' -- Start with 5 points (arbitrary)

	//Store x values of start/end points
	double vdep_low_end  = 0; double vdep_low_start  = 0;
	g->GetPoint(g->GetN()-1, vdep_low_end, y_tmp);


	//PROTECTION -- If the curve is only decreasing (not 2 distinct regimes), consider that Vfd is too low to be extracted, and use first point
	bool only_drecreasing_slope = true; double y_ref=999;
	for(int i=0; i<10; i++) //Check 10 first points for any increase in Y-axis
	{
		g->GetPoint(i, x_tmp, y_tmp);
		// if(debug==1) cout<<"y "<<y_tmp<<", y_ref "<<y_ref<<endl;
		if(y_tmp>y_ref) {only_drecreasing_slope = false; break;}
		y_ref=y_tmp;
	}


  //PROTECTION -- remove first points from 'bottom fit' if they are const (else, fit wrong slope)
	double y_variation = 0;
	bool first_point_already_moved = false;
	for(int i=6; i>=1; i--)
	{
		g->GetPoint(i, vdep_low_end, y_tmp);
		y_variation = y_tmp;
		g->GetPoint(0, vdep_low_end, y_tmp);
		y_variation-= y_tmp;

		if( (y_tmp > 20 && y_variation < 2) || y_variation < 0.1) //Trick to separate signal & CW curves
		{
			if(debug==1) {cout<<endl<<endl<<" - First  points ~ constant --> Start at point "<<i+1<<" !"<<endl; cout<<"(y"<<i<<" - y0 = "<<y_variation<<")"<<endl;}
			npt_low_start = i+1;
			npt_low_end = i+4;
			if(i > 0) {first_point_already_moved = true;} //Don't move first point again (loose too many points)
			break;
		}
	}



	//Set initial voltage range fot 'bottom fit'
	vdep_low_end  = 0; g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);



	//Extend the 'bottom line' from left to right, i.e. try to add points "at the end"
	//Perform fit for each point range, and select point when stopping condition is realized
	//NB : while(chi2_low < 0.02 && vdep_low < 350 && npt_low < g->GetN() - 4) //Stopping conditions from michael
	for(int i=npt_low_end; i<npt_high_start+3; i++)
	{
		//Get coordinates of tmp point
		g_fit->GetPoint(i, vdep_low_end, y_tmp);

		//Perform fit
		f3_low->SetRange(vdep_low_start, vdep_low_end);
		status = g_fit->Fit("fplow", "rqn");
		chi2_low = f3_low->GetChisquare()/f3_low->GetNDF();

		if(debug==1) cout<<endl<<"Bottom fit : "<<i<<"  chi2 "<<chi2_low;

		//If chi2 too high, retry fit after moving starting point to the right
		if(chi2_low > chi2_threshold)
		{
			g_fit->GetPoint(npt_low_start+1, vdep_low_start, y_tmp);
			f3_low->SetRange(vdep_low_start, vdep_low_end);
			status = g_fit->Fit("fplow", "rqn");
			chi2_low = f3_low->GetChisquare()/f3_low->GetNDF();

			//If chi2 still high or starting point = 4th point (from left) --> stop there
			if(chi2_low > chi2_threshold || npt_low_start > 3)
			{
				npt_low_end = i-1; //Re-decrement --> go back to last point
				break;
			}
			else {npt_low_start++; if(debug==1) cout<< " --> Move starting point ";}
		}

		if(i==npt_high_start+3 && verbose) cout<<endl<<"End of loop !"<<endl;
		npt_low_end = i; //Re-decrement --> get desired point //if loop end reached
	}

	//Set voltage range for 'bottom line' & fit it
	g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);
	g_fit->GetPoint(npt_low_end, vdep_low_end, y_tmp);
	f3_low->SetRange(vdep_low_start, vdep_low_end); //Perform final top line fit : [arbitrary start;ending point] V
	g_fit->Fit("fplow", "rqn");















//-------------------------------------------
//  #######  ########  ######## #### ##     ## #### ######## ########       ##       #### ##    ## ########  ######
// ##     ## ##     ##    ##     ##  ###   ###  ##       ##  ##             ##        ##  ###   ## ##       ##    ##
// ##     ## ##     ##    ##     ##  #### ####  ##      ##   ##             ##        ##  ####  ## ##       ##
// ##     ## ########     ##     ##  ## ### ##  ##     ##    ######         ##        ##  ## ## ## ######    ######
// ##     ## ##           ##     ##  ##     ##  ##    ##     ##             ##        ##  ##  #### ##             ##
// ##     ## ##           ##     ##  ##     ##  ##   ##      ##             ##        ##  ##   ### ##       ##    ##
//  #######  ##           ##    #### ##     ## #### ######## ########       ######## #### ##    ## ########  ######
//-------------------------------------------

	if(verbose) cout<<endl<<endl<<"___________________"<<endl;

//--- Now that we have an estimate of what the bottom & top lines ranges are, we try to optimize them further
//--- We try many different possible fit ranges, and try to minimize the chi-2 score
//--- Also : automatize the removal of first/last points of the curves when needed (up to 4 points in both cases)



//-------------------------
// ###
//  #  #    # # ##### #   ##   #      # ###### ######
//  #  ##   # #   #   #  #  #  #      #     #  #
//  #  # #  # #   #   # #    # #      #    #   #####
//  #  #  # # #   #   # ###### #      #   #    #
//  #  #   ## #   #   # #    # #      #  #     #
// ### #    # #   #   # #    # ###### # ###### ######
//-------------------------

	double coeff_low = -99; //Store  slope of bottom line
	double coeff_high = -99; //Store slope of top line

	double best_chi2_low = 1000, best_chi2_high = 1000, chi2_total = 10000; //Store chi-2 scores

	//Initialize voltage range
	g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);
	g_fit->GetPoint(npt_high_end, vdep_high_end, y_tmp);

	int npt_low_start_tmp = npt_low_start, npt_low_end_tmp = npt_low_end, npt_high_start_tmp = npt_high_start, npt_high_end_tmp = npt_high_end; //Store tmp indices

	bool multiple_kinks = false; //Boolean for protections against multiple kinks/bumps

	double slope_before_mv_pt = -99; //At each iteration, store slope of bottom fit before the last point was added/removed (for protections)

	//Store slope of bottom line before its range is modified (for protections)
	g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);
	f3_low->SetRange(vdep_low_start, vdep_low_end);
	status = g_fit->Fit("fplow", "rqn");
	double slope_initial = f3_low->GetParameter(1);

	//Starting point of 'bottom line' -- if it was already moved to the right in previous loop (above), only try configurations where starting point is moved -1/+1 w.r.t. the current point
	int k=0;
	if(first_point_already_moved) {k=npt_low_start-1;}
	else {k=npt_low_start;}





//-------------------------
//   ##   #       ####   ####  #####  # ##### #    # #    #
//  #  #  #      #    # #    # #    # #   #   #    # ##  ##
// #    # #      #      #    # #    # #   #   ###### # ## #
// ###### #      #  ### #    # #####  #   #   #    # #    #
// #    # #      #    # #    # #   #  #   #   #    # #    #
// #    # ######  ####   ####  #    # #   #   #    # #    #
//-------------------------

//--- This rather long/complicated algorithm aims at testing a lot of configurations (i.e. points of the TGraph are either attributed to bottom line, top line, or left out) & try to find the best configuration relying on the minimization of the chi-2 scores of the linear fits
//--- What makes it long/complicated is the fact that a lot of different protections were implemented in order to try to adress the many different types of curves that are observed in data (low stat., constant regimes, strong slopes, etc.)



	for(k; k<npt_low_start+15; k++) //Try different starting points for bottom line
	{
		if(first_point_already_moved && k > npt_low_start+1) {break;} //If starting point was already moved in previous loop (above), only try 3  configurations for bottom start

		g_fit->GetPoint(k, vdep_low_start, y_tmp); //Get x coordinate of tmp starting point

		for(int l=npt_high_end; l>npt_high_end-3; l--) //Try different ending points for the top line
		{
			g_fit->GetPoint(l, vdep_high_end, y_tmp);
			//if(debug==1) cout<<endl<<"-- High end = "<<l<<endl;

			for(int i=k+3; i<g->GetN()-3; i++) //Try different ending points for 'bottom line'
			{

				for(int j=i; j<g->GetN()-3; j++) //Try different starting points for "top line"
				{
					//Protections
					if(j - i > 3) {continue;} //Too many points left out b/w fits
					if(i-k < 3 || l-j < 3) {continue;} //Ask for at least 4 points used in each fit

					//Set tmp voltage ranges of both lines & fit
					g_fit->GetPoint(i, vdep_low_end, y_tmp);
					f3_low->SetRange(vdep_low_start, vdep_low_end);
					status = g_fit->Fit("fplow", "rqn");
					double chi2_low_fit = f3_low->GetChisquare()/f3_low->GetNDF();

					//Set tmp voltage ranges of both lines & fit
					g_fit->GetPoint(j, vdep_high_start, y_tmp);
					f3_high->SetRange(vdep_high_start, vdep_high_end);
					status = g_fit->Fit("fphigh", "rqn");
					double chi2_high_fit = f3_high->GetChisquare()/f3_high->GetNDF();

					coeff_low = f3_low->GetParameter(1); //slope bottom fit
					coeff_high = f3_high->GetParameter(1); //slope top fit
					//cout<<"f3_low->GetParameter(1) = "<<f3_low->GetParameter(1)<<endl;
					//cout<<"f3_high->GetParameter(1) = "<<f3_high->GetParameter(1)<<endl;
					//if(debug==1) cout<<"chi2_high_fit = "<<chi2_high_fit<<endl;
					//if(debug==1) cout<<"chi2_low_fit = "<<chi2_low_fit<<endl;


					if(coeff_low<0.001) //If coeff_low becomes <=0, it must mean there will be another kink somewhere
					{
						multiple_kinks = true; //Protection against possible "multiple kinks", e.g. meta-maximums
					}

					//Must normalize chi2 sum
					double chi2_total_tmp = (f3_high->GetChisquare()+f3_low->GetChisquare())/(f3_high->GetNDF()+f3_low->GetNDF());



					//If lower chi-2 sum found
					if(chi2_total_tmp < chi2_total)
					{
						//Protection : If suspect that there are multiple kinks/maximums, prevent starting point of bottom line to be moved too far "right" just to extract this kink !
						if(multiple_kinks && k>npt_low_start+2) {continue;}

						//Protection against negative/tiny slopes for bottom fit
						if(coeff_low<0) {continue;} //Forbid negative slope
						else if(y_tmp < 10) //CW
						{
							if(coeff_low < 0.001)
							{
								//cout<<"coeff_low < 0.001 : "<<coeff_low<<" --> Skip !"<<endl;
								continue;
							}
						}
						else //Signal
						{
							if(coeff_low < 0.2)
							{
								//cout<<"coeff_low <  : "<<coeff_low<<" --> Skip !"<<endl;
								continue;
							}
						}






						//-----------------------------
						//  #    # # #####  #####  #      ######    #####   ####  # #    # #####  ####
						//  ##  ## # #    # #    # #      #         #    # #    # # ##   #   #   #
						//  # ## # # #    # #    # #      #####     #    # #    # # # #  #   #    ####
						//  #    # # #    # #    # #      #         #####  #    # # #  # #   #        #
						//  #    # # #    # #    # #      #         #      #    # # #   ##   #   #    #
						//  #    # # #####  #####  ###### ######    #       ####  # #    #   #    ####
						//-----------------------------

						//Associate middle points to one line or the other (depending on chi2 score)


						//--- Can be problematic to allow more than one point being 'left out' (cf. some curves)
						//--- If it is the case for the current configuration being tested, we try to associate the remaining middle points to one line or the other, and see how this affects the chi-2 score
						if(j-i > 1)
						{
							double lowest_chi2_value = 99999; //Store lowest chi-2 score achieved

							int best_m = -1; //Store index of 'middle point' between the bottom and top line

							//Try to move this middle point (i.e. junction point between the 2 lines) to the left and right, depending on how many points are 'left out' in current config.
							for(int m=i; m<j; m++)
						  	{
								//Set voltage range & fit the 2 lines
						  		double v_tmp;
						  		g_fit->GetPoint(m, v_tmp, y_tmp);
						  		f3_low->SetRange(vdep_low_start, v_tmp);
								status = g_fit->Fit("fplow", "rqn");

								g_fit->GetPoint(m+1, v_tmp, y_tmp);
						  		f3_high->SetRange(v_tmp, vdep_high_end);
								status = g_fit->Fit("fphigh", "rqn");

								chi2_total_tmp = (f3_high->GetChisquare()+f3_low->GetChisquare())/(f3_high->GetNDF()+f3_low->GetNDF());


								//If lower chi2 found
								if(chi2_total_tmp  < lowest_chi2_value)
								{
									//PROTECTION -- Make sure that moving the bottom fit starting point, we don't end up with a fit of an (almost) flat part of the TGraph
									//Require some minimal steepness in increasing regime (bottom line) !
									if(k>npt_low_start) //If the starting point of bottom line in current config. being tested is moved w.r.t original one
									{
										double slope_tmp = f3_low->GetParameter(1); //Store slope of bottom line in current config.

										g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);
							 			f3_low->SetRange(vdep_low_start, vdep_low_end);
										status = g_fit->Fit("fplow", "rqn");
										slope_before_mv_pt = f3_low->GetParameter(1); //Store slope of bottom line in previous config. (i.e. same config. as the one being tested, but with starting point of bottom line at its initial position)

										g_fit->GetPoint(k, vdep_low_start, y_tmp); //Then, need to re-store voltage of the current starting point of bottom line, for next iterations !

										// if(debug==1) cout<<"slope_tmp="<<slope_tmp<<endl;
									    // if(debug==1) cout<<"before mv pt = "<<slope_before_mv_pt<<endl;

										// cout<<"slope_initial = "<<slope_initial<<endl;
										// cout<<"current slope = "<<slope_tmp<<endl;

										//-- PROTECTION on bottom line slope -- Require it to be at least 2/3 of the slope *if starting point of bottom line* had not been moved
										//This avoids that we leave out too many points, and end up	with a 'flat' bottom line (i.e. we loose steepness information from first points)
										if(slope_tmp * slope_before_mv_pt < 0 || slope_tmp / slope_before_mv_pt < 0.66) {continue;} //NEW

										if(debug==1) cout<<"slope_tmp / slope_initial "<<slope_tmp / slope_initial<<endl;


										//-- PROTECTION on bottom line slope //TBC -- verify it's relevant
										// ~ same as protection above ; but here we compare current slope to initial slope before *any* point has been moved
										//Require that current slope be at least 20% or initial slope
										if(slope_tmp * slope_initial < 0 || slope_tmp / slope_initial < 0.20) {continue;}

										//FIXME -- NEW -- Protection against negative/tiny slopes for bottom fit
										if(slope_tmp<0) {continue;} //Forbid negative slope
									}


									//PROTECTION -- Make sure that moving the ending point of top line,
									if(l<npt_high_end)
									{
										double slope_tmp = f3_high->GetParameter(1);

										g_fit->GetPoint(npt_high_end, vdep_high_end, y_tmp);
							 			f3_high->SetRange(vdep_high_start, vdep_high_end);
										status = g_fit->Fit("fphigh", "rqn");
										double slope_before_mv_pt = f3_high->GetParameter(1);

										g_fit->GetPoint(l, vdep_high_end, y_tmp); //Need to re-store vdep_high_end for next iterations !

										//-- PROTECTION : ask current slope to be of same sign as slope *if ending point had not been moved*, etc.
										if(slope_tmp*slope_before_mv_pt<0 || (slope_tmp <0 && slope_tmp < slope_before_mv_pt) || (slope_tmp > 0 && slope_tmp > slope_before_mv_pt) )	{continue;}
									}

									//If lower chi2 found and requirements are matched, move 'middle point' accordingly
									lowest_chi2_value = chi2_total_tmp;
									best_m = m;
								} //End 'if lower chi-2' condition

						  	} //End loop on middle/junction point between the 2 lines


							//Now that the 'middle point' has been moved, we can recompute the overall chi-2 and check if it's lower
						  	if(lowest_chi2_value < chi2_total)
						  	{
						  		chi2_total = lowest_chi2_value; //If lower, update the lines' ranges

						  		npt_low_end_tmp = best_m; npt_high_start_tmp = best_m+1;
								npt_low_start_tmp = k; npt_high_end_tmp = l;

								if(debug==1) cout<<"*** Lower chi-2 sum = "<<chi2_total<<" : "<<k<<","<<best_m<<" / "<<best_m+1<<","<<l<<endl;

								continue;
						  	}
						  	else {continue;} //If not best config., abandon this config.

						} //End 'if condition' about multiple middle points



						//PROTECTIONS -- Just like in the loop above, we check that the slopes in the "new best configuration" conserve at least part of the steepness of the original slopes (else it can mean that the points we are not using do not contain enough info)
						if(k>npt_low_start)
						{
							double slope_tmp = coeff_low;

							g_fit->GetPoint(npt_low_start, vdep_low_start, y_tmp);
							f3_low->SetRange(vdep_low_start, vdep_low_end);
							status = g_fit->Fit("fplow", "rqn");
							double slope_before_mv_pt = f3_low->GetParameter(1);

							//Need to re-compute vdep_low_start for next iterations !
							g_fit->GetPoint(k, vdep_low_start, y_tmp);

							if(debug==1) cout<<"slope_tmp="<<slope_tmp<<endl;
							if(debug==1) cout<<"before mv pt = "<<slope_before_mv_pt<<endl;

							//-- PROTECTION on bottom line slope -- Require it to be at least 2/3 of the slope *if starting point of bottom line* had not been moved
							//This avoids that we leave out too many points, and end up	with a 'flat' bottom line (i.e. we loose steepness information from first points)
							if(slope_tmp * slope_before_mv_pt < 0 || slope_tmp / slope_before_mv_pt < 0.66) {continue;} //NEW

							// if(debug==1) cout<<"slope_tmp / slope_initial "<<slope_tmp / slope_initial<<endl;


							//-- PROTECTION on bottom line slope //TBC -- verify it's relevant
							// ~ same as protection above ; but here we compare current slope to initial slope before *any* point has been moved
							//Require that current slope be at least 20% or initial slope
							if(slope_tmp * slope_initial < 0 || slope_tmp / slope_initial < 0.20) {continue;}
						}
						if(l != npt_high_end)
						{
							double slope_tmp = f3_high->GetParameter(1);

							g_fit->GetPoint(npt_high_end, vdep_high_end, y_tmp);
							f3_high->SetRange(vdep_high_start, vdep_high_end);
							status = g_fit->Fit("fphigh", "rqn");
							double slope_before_mv_pt = f3_high->GetParameter(1);

							//Need to re-compute for next iterations !
							g_fit->GetPoint(l, vdep_high_end, y_tmp);

							//-- PROTECTION : ask current slope to be of same sign as slope *if ending point had not been moved*, etc.
							if(slope_tmp*slope_before_mv_pt<0 || (slope_tmp <0 && slope_tmp < slope_before_mv_pt) || (slope_tmp > 0 && slope_tmp > slope_before_mv_pt) )	{continue;}
						}


						//If current configuration is the best one & all requirements are fulfilled, store the new best point indices
						npt_low_end_tmp = i; npt_high_start_tmp = j;
						npt_low_start_tmp = k; npt_high_end_tmp = l;
						chi2_total = chi2_total_tmp;

						if(debug==1) cout<<"* Lower chi-2 sum = "<<chi2_total<<" : "<<k<<","<<i<<" / "<<j<<","<<l<<endl;
					} //End 'if best chi-2 found' condition


				} //End of loop on top line starting point
			}  //End of loop on bottom line ending point
		}	 //End of loop on top line ending point
	} //End of loop on bottom line starting point






//----------------------
 // ###### #    # ##### #####    ##    ####  #####       #    # ###### #####
 // #       #  #    #   #    #  #  #  #    #   #         #    # #      #    #
 // #####    ##     #   #    # #    # #        #         #    # #####  #    #
 // #        ##     #   #####  ###### #        #         #    # #      #    #
 // #       #  #    #   #   #  #    # #    #   #          #  #  #      #    #
 // ###### #    #   #   #    # #    #  ####    #           ##   #      #####
 //----------------------


	//Update beginning/ending points of both lines
	npt_low_start = npt_low_start_tmp; npt_low_end = npt_low_end_tmp;
	npt_high_start = npt_high_start_tmp; npt_high_end = npt_high_end_tmp;

//-----------------

	//Update accordingly the voltage ranges of both lines & perform fits
	g->GetPoint(npt_low_start, vdep_low_start, y_tmp);
	g->GetPoint(npt_low_end, vdep_low_end, y_tmp);
	f3_low->SetRange(vdep_low_start, vdep_low_end);

	g->GetPoint(npt_high_end, vdep_high_end, y_tmp);
	g->GetPoint(npt_high_start, vdep_high_start, y_tmp);
	f3_high->SetRange(vdep_high_start, vdep_high_end);

	//cout<<"low start = "<<npt_low_start<<endl;
	//cout<<"low end = "<<npt_low_end<<endl;
	//cout<<"high start = "<<npt_high_start<<endl;
	//cout<<"high end = "<<npt_high_end<<endl;
	//cout<<"vdep high start = "<<vdep_high_start<<endl;
	//cout<<"vdep high end = "<<vdep_high_end<<endl;

	g->Fit("fplow", "rqn");
	g->Fit("fphigh", "rqn");


//-----------------------------------
//--- FINALLY, find abscissa of crossing point of the 2 line fits
	double vdep_crossingpoint = getCrossingPoint(f3_low , f3_high);

	//PROTECTION -- If before the algorithm we found that the whole curve is just one ~decreasing regime, we simply select the first point of the TGraph as Vfd
	if(only_drecreasing_slope)
	{
		double x_tmp, y_tmp;
		g->GetPoint(0, x_tmp, y_tmp);

		vdep_crossingpoint = x_tmp;
	}


	//--- PROTECTIONS aggainst low VFD cases (lines algo most likely failed)
	//--- If such case detected, use non-smoothed TGraph instead of MedianFilter-smoothed one (to avoid loosing any info)
	bool is_low_vfd = false;

	if(debug) cout<<endl<<"ymax "<<ymax<<endl<<endl;
	if(debug) cout<<endl<<"ymax_nosmooth "<<ymax_nosmooth<<endl<<endl;

	// for(int ipt=0; ipt<g_nosmooth->GetN(); ipt++)
	// {
	// 	g_nosmooth->GetPoint(ipt, x_tmp, y_tmp);
	//
	// 	cout<<"ipt "<<ipt<<" / x = "<<x_tmp<<" / y_tmp = "<<y_tmp<<endl;
	// }


	//PROTECTION against low Vfd cases -- If one of the first 5 points of TGraph (after potentially having removed first points!) corresponds to a maximum in y-axis, choose it as Vfd
	double x[5], y[5]; //CHANGED -- 5 instead of 4 first points
	for (size_t i = 0; i < 4; i++)
	{
		if(i+5 >= g_nosmooth->GetN()) {break;}

		g_nosmooth->GetPoint(i, x[0], y[0]);
		g_nosmooth->GetPoint(i+1, x[1], y[1]);
		g_nosmooth->GetPoint(i+2, x[2], y[2]);
		g_nosmooth->GetPoint(i+3, x[3], y[3]);
		g_nosmooth->GetPoint(i+4, x[4], y[4]);

		double slope_1, slope_2, slope_3;

		slope_1 = y[1] - y[0];
		slope_2 = y[3] - y[2];
		slope_3 = y[4] - y[3];

		//Check the first 5 points : if they correspond to ymax, consider that plateau is already reached (Vfd ~ 0) ---> Choose point as Vdep
		bool vdep_found = false;
		if(i==0)
		{
			for(int j=0; j<5; j++)
			{
				// if(debug==1) {cout<<"j= "<<j<<" --> y[j] = "<<y[j]<<" /// (y[j] - ymax)/ymax = "<<(y[j] - ymax)/ymax<<endl;}

				if(y[j] == ymax_nosmooth) //max found
				{
					vdep_crossingpoint = x[j];
					if(debug==1) {cout<<FYEL("Special case detected (maximum y within first points) -- Choosing vdep = "<<vdep_crossingpoint<<" !")<<endl;}
					vdep_found=true;
					is_low_vfd=true;
					break;
				}
			}
		}
		if(vdep_found) {break;}

		//Protection against low Vfd cases :
		//- If "inversion of slope" happens within first point, select Vfd among first Points //CHANGED -- removed, seems too strict
		//- Idem if maximum of curve is reached within first points
		if( fabs( (y[3]-y[0])/y[0]) < 0.05 && slope_2 < 0 && slope_3 < 0 && y[2] >= 0.95 * ymax)
		{
			vdep_crossingpoint = x[1];
			if(debug==1) {cout<<FYEL("Special case detected (low Vfd ?) -- Choosing vdep = "<<vdep_crossingpoint<<" !")<<endl;}
			is_low_vfd=true;
			break;
		}
	}

  if(vdep_crossingpoint<0) {vdep_crossingpoint=0;} //Forbid negative Vfd values

  if(verbose) cout<<endl<< "-----> Vdep = " << vdep_crossingpoint << endl;

//----------------























//---------------------------------------------
// ##    ## #### ##    ## ##    ##    ##     ## ######## ######## ##     ##  #######  ########
// ##   ##   ##  ###   ## ##   ##     ###   ### ##          ##    ##     ## ##     ## ##     ##
// ##  ##    ##  ####  ## ##  ##      #### #### ##          ##    ##     ## ##     ## ##     ##
// #####     ##  ## ## ## #####       ## ### ## ######      ##    ######### ##     ## ##     ##
// ##  ##    ##  ##  #### ##  ##      ##     ## ##          ##    ##     ## ##     ## ##     ##
// ##   ##   ##  ##   ### ##   ##     ##     ## ##          ##    ##     ## ##     ## ##     ##
// ##    ## #### ##    ## ##    ##    ##     ## ########    ##    ##     ##  #######  ########
//---------------------------------------------

	if(verbose) cout<<endl<<"* Curvature Method : "<<endl;

//--- 'Kink' Method : Locate the kink of the curve after smoothing
//--- We look for a minimum with a parabola in the curvature Plot
//--- Thos method performs pretty well for TIB (better-looking curves) but can be quite unstable
//--- In particular in case of important fluctuations, of multiple minima, etc.
//--- Because it is easier/more adaptable/robust, we choose to rather rely on the 'lines method' (above) -- Therefore this method was not updated with time/new scans
//--- However this method is still performed, to see how the results compare with the other method.



//--- Compute a voltage threshold for Kink finding algo (0.8 or 0.9 * max)
	double ythresh = ymin + (ymax-ymin)*0.7;
	double xthresh=0; float vdep_kink = 0;

	for(int ipt=0; ipt<g->GetN(); ipt++)
	{
		g->GetPoint(ipt, x_tmp, y_tmp);

		if(y_tmp>ythresh)
		{
			ipt--;
			g->GetPoint(ipt, x_tmp, y_tmp);
			xthresh = x_tmp;
			break;
		}
	}
	if(debug==1) cout<<"xmin threshold (y >= 0.7*max) : "<<xthresh<<endl;


	double xthreshmax = 350;
	//xthresh = vdep_crossingpoint-30; xthreshmax = vdep_crossingpoint+30;


//------------------
 //  ####  #    #  ####   ####   ####  ##### #    # # #    #  ####
 // #      ##  ## #    # #    # #    #   #   #    # # ##   # #    #
 //  ####  # ## # #    # #    # #    #   #   ###### # # #  # #
 //      # #    # #    # #    # #    #   #   #    # # #  # # #  ###
 // #    # #    # #    # #    # #    #   #   #    # # #   ## #    #
 //  ####  #    #  ####   ####   ####    #   #    # # #    #  ####
 //------------------

	TGraphErrors* gsmooth = new TGraphErrors(g->GetN(), g->GetX(), g->GetY() , g->GetEX(), g->GetEY());
	if(SGSmooth) gsmooth = SavitzkyGolaySmoother(gsmooth, 5, 0);
	else
	{
	gsmooth = HanningFilter(gsmooth);
	// To check : 2nd hanningFilter better with 3 or 5 points ?
	if(filter_twice) gsmooth = HanningFilter(gsmooth); // For TOB, some scans for TID
	}

	TGraphErrors* gscurv = 0;
	if(SGSmooth) gscurv = GetSavitzkyGolayCurvatureGraph( gsmooth );
	else gscurv = GetCurvatureGraph( gsmooth );

	gscurv->SetMarkerStyle(20);
	TGraph* g3pts = new TGraph();

	//vdep_kink = GetOptimalMinNPts(gscurv, g3pts, xthresh);
	vdep_kink = GetOptimalMinNPts(gscurv, g3pts, xthresh, xthreshmax);

	if(verbose) cout<<"-----> Vdep = "<<vdep_kink<<endl<<endl;




//---------------------------------------------
// #####  #####    ##   #    # # #    #  ####
// #    # #    #  #  #  #    # # ##   # #    #
// #    # #    # #    # #    # # # #  # #
// #    # #####  ###### # ## # # #  # # #  ###
// #    # #   #  #    # ##  ## # #   ## #    #
// #####  #    # #    # #    # # #    #  ####
//---------------------------------------------

// Drawing the curves and fits
	TCanvas *c1;
	TCanvas *c2;
	//--- Curvature Graph
	c2 = new TCanvas;
	c2->SetTopMargin(0.1);

	gscurv->SetTitle("");
	gscurv->Draw("AP");
	//gscurv->GetXaxis()->SetTitle("V_{bias} [V]");
	gscurv->GetXaxis()->SetTitle("Bias voltage [V]");
	gscurv->GetYaxis()->SetTitle("Curvature [a.u.]");
	gscurv->GetXaxis()->SetTitleSize(.04);
	gscurv->GetYaxis()->SetTitleSize(.04);
	gscurv->GetXaxis()->SetTitleOffset(1.18);
	gscurv->GetYaxis()->SetTitleOffset(1.4);

	g3pts->Draw("P");
	g3pts->Fit("pol2", "q");

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(0.58,0.93,cmsText);

	TString extraText   = "Preliminary 2017";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(0.70, 0.932, extraText);

	c2->Modified();
	c2->Update();


	//---- Rather use these parameters, if don't want to display y-axis numbers
	//gscurv->GetYaxis()->SetTitleOffset(1.);
	//gscurv->GetYaxis()->SetLabelSize(.0);



	//--- Lines Method Graph
	c1 = new TCanvas();
	c1->SetTopMargin(0.1);

	g->SetTitle("");
	if(!is_low_vfd) g->Draw("AP"); //"P" = marker, "A" = draw axis, "X" = no errors
	else {g_nosmooth->Draw("AP");}
	//g->SetMarkerColor(15);
	//g->GetXaxis()->SetTitle("V_{bias} [V]");
	g->GetXaxis()->SetTitle("Bias voltage [V]");
	//g->GetYaxis()->SetTitle("ClusterWidth [#strips]");
	if(observable == "Signal") g->GetYaxis()->SetTitle("Cluster charge [a.u.]");
	//else if(observable == "ClusterWidth") g->GetYaxis()->SetTitle("Cluster width [a.u.]");
	else if(observable == "ClusterWidth") g->GetYaxis()->SetTitle("Cluster width [number of strips]");
	g->GetXaxis()->SetTitleSize(.04);
	g->GetYaxis()->SetTitleSize(.04);
	g->GetXaxis()->SetTitleOffset(1.18);
	g->GetYaxis()->SetTitleOffset(1.4);

	//----------------
	// CAPTIONS //
	//----------------

// -- using https://twiki.cern.ch/twiki/pub/CMS/Internal/FigGuidelines


	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(0.58,0.93,cmsText);

	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(0.70, 0.932, extraText);







	TLine *l = new TLine(vdep_kink, ymin, vdep_kink, ymax+0.1);
	l->SetLineStyle(3);
	//l->Draw(); //Draw vertical line at vdep by kink method

	// draw low threshold
	TLine *lthresh = new TLine(xthresh, ymin, xthresh, ymax+0.1);
	lthresh->SetLineStyle(2);
	//lthresh->Draw();
	g->GetPoint(0, x_tmp, y_tmp);
	TGraph* gthresh = new TGraph(4);
	gthresh->SetPoint(0, x_tmp, ymax+0.1);
	gthresh->SetPoint(1, xthresh, ymax+0.1);
	gthresh->SetPoint(2, xthresh, ymin);
	gthresh->SetPoint(3, x_tmp, ymin);
	gthresh->SetFillStyle(3354);
	//gthresh->Draw("f");


	//--- draw 2 line fit and crossing point position
	f3_high->Draw("same");  // blue line
	f3_low->Draw("same");  // green line
	TLine *lcross = new TLine(vdep_crossingpoint, ymin, vdep_crossingpoint, ymax+0.1);
	lcross->SetLineStyle(2);
	lcross->Draw(); ////Draw vertical line at vdep by fit method

	//Sigmoid Fit
	//status = g->Fit("ftest", "rqn");
	//ftest->SetLineColor(2);
	//ftest->Draw("same");

	c1->Modified();
	c1->Update();

	//Save canvas in temporary files (erased each time --> need to abort root if want to keep particules plots)
	if(draw_plot) //For FitAll.exe, don't want to save plots!
	{
		c1->SaveAs("Fit_line.png");
		c2->SaveAs("Fit_curv.png");
	}

	//getchar(); //Waits for user to press enter to continue
	c1->Close();
	c2->Close();





//-----------------------------
//                                            	 ##
// #####  ###### #      ###### ##### ######     #  #      #####  ###### ##### #    # #####  #    #
// #    # #      #      #        #   #           ##       #    # #        #   #    # #    # ##   #
// #    # #####  #      #####    #   #####      ###       #    # #####    #   #    # #    # # #  #
// #    # #      #      #        #   #         #   # #    #####  #        #   #    # #####  #  # #
// #    # #      #      #        #   #         #    #     #   #  #        #   #    # #   #  #   ##
// #####  ###### ###### ######   #   ######     ###  #    #    # ######   #    ####  #    # #    #
//-----------------------------

	delete g_tmp; delete gsmooth; delete g_fit; delete g3pts; delete gthresh; delete g_nosmooth;
	delete l; delete lcross;
	delete f3_high; delete f3_low;
	delete c1; delete c2;


	//Return Vfd extracted either with 'lines method' or 'kink method'
	if(use_curvature) {return vdep_kink;}
	else {return vdep_crossingpoint;}
}































//---------------------------------------------
//    ###    ########  ########  ##       ##    ##       ######## #### ########
//   ## ##   ##     ## ##     ## ##        ##  ##        ##        ##     ##
//  ##   ##  ##     ## ##     ## ##         ####         ##        ##     ##
// ##     ## ########  ########  ##          ##          ######    ##     ##
// ######### ##        ##        ##          ##          ##        ##     ##
// ##     ## ##        ##        ##          ##          ##        ##     ##
// ##     ## ##        ##        ########    ##          ##       ####    ##
//---------------------------------------------

/**
 * Apply fit method on one curve/scan
 * @param dirname    [path of data file]
 * @param subdet     [subdetector : TIB, TOB, TEC]
 * @param run        [run number]
 * @param modid      [ID number of detector module]
 * @param type       [signal or cluster width observable]
 * @param debug      [verbosity]
 * @param date       [date of run]
 */
void FitOneCurve(string dirname, string subdet, string run, ULong64_t modid, string type, int debug, string date, bool use_curvature)
{
	if(type!="ClusterWidth" && type!="Signal")
	{cout<<"Error in FitOneCurve() : curve type "<<type<<" is not allowed."<<endl; return;}

	TGraphErrors* g=0;
	if(type=="ClusterWidth") g = GetClusterWidthGraph(dirname, subdet, run, modid, 0);
	else if(type=="Signal") g = GetGraph(dirname, subdet, run, modid, 0);
	if(!g) {cout<<"g is null !"<<endl; return;}

	// isGoodCurve(g, type);
	if(!isGoodCurve(g, type)) {cout<<"Bad curve !"<<endl; return;}

	// Correct for voltage drop due to leakage current
	string corr_name="_" + subdet + "_" + date + "_run" + run;

	int modid_tmp = modid; if(subdet!="TIB") {modid_tmp = modid/10;}
	int corrected = CorrectGraphForLeakageCurrent(g, modid_tmp, corr_name);
	if(!corrected) {cout<<"Leakage correction not found !"<<endl;}

	//g->SetTitle(Form("DetID %i", (int) modid));
	TString g_name = "DetID "+ Convert_Number_To_TString(modid);
	g->SetTitle(g_name.Data());
	g->SetName("g");

	cout<<endl<<"Run = "<<run<<endl;
	cout<<"DetID "<<modid<<endl; cout<<"------"<<endl;

	TString tmp_type = type;

	FitCurve(g, debug, false, use_curvature, false, tmp_type);
}


















//---------------------------------------
//  #######  ########   ######   #######  ##       ######## ######## ########
// ##     ## ##     ## ##    ## ##     ## ##       ##          ##    ##
// ##     ## ##     ## ##       ##     ## ##       ##          ##    ##
// ##     ## ########   ######  ##     ## ##       ######      ##    ######
// ##     ## ##     ##       ## ##     ## ##       ##          ##    ##
// ##     ## ##     ## ##    ## ##     ## ##       ##          ##    ##
//  #######  ########   ######   #######  ######## ########    ##    ########
//---------------------------------------

/*
//Perform fit with sigmoid function
Double_t fitsigmo(Double_t *x, Double_t *par)
{
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0]/(1+exp(-1*par[1]*(x[0]+par[3])));//+ par[4]*x[0]; // sigmoid
  }
  else
  {
    value_th = par[0]/(1+exp(-1*par[1]*(par[2]+par[3])));
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }

  return value;
}

//Perform fit with polynom
Double_t fitpol(Double_t *x, Double_t *par)
{
  Double_t value, value_th, value_0;
  if (x[0]<par[4]) {
	value = par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0];
  }
  else
  {
    value_th = par[0] + par[1]*par[4] + par[2]*par[4]*par[4] + par[3]*par[4]*par[4]*par[4];
    value_0 = value_th - par[5]*par[4];
    value = value_0 + par[5]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}

  return value;
}

Double_t GetKinkByFit(Double_t *x, Double_t *par)
{
  Double_t value, value_th, value_0;
  if (x[0]<par[2])
  {
        value = par[0] + par[1]*x[0] + par[3]*x[0]*x[0]; // pol2
        //value = par[0] + par[1]*x[0];
  }
  else
  {
        //value_th = par[0] + par[1]*par[2]
        value_th = par[0] + par[1]*par[2] + par[3]*par[2]*par[2] ;
        value_0 = value_th - par[4]*par[2];
        value = value_0 + par[4]*x[0]; // pol1
  }

  return value;
}
*/

#endif

