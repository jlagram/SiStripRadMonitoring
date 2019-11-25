/**
Nicolas Tonon, IPHC
12/2017
*/

#ifndef _helper_h_
#define _helper_h_

/* BASH COLORS */
#define RST   "[0m"
#define KRED  "[31m"
#define KGRN  "[32m"
#define KYEL  "[33m"
#define KBLU  "[34m"
#define KMAG  "[35m"
#define KCYN  "[36m"
#define KWHT  "[37m"
#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST
#define BOLD(x) "[1m" x RST
#define UNDL(x) "[4m" x RST

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include "TTree.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TString.h"
#include "TColor.h"
#include "TProfile.h"
#include "TCut.h"
#include "TLegend.h"
#include "TIterator.h"
#include "TLine.h"
#include "THStack.h"
#include "TString.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TObject.h"
#include "TRandom3.h"
#include "TRandom1.h"
#include "TObjArray.h"
#include "TF1.h"
#include "TLegendEntry.h"
#include "TGaxis.h"
#include "TLeaf.h"
#include "TFractionFitter.h"
#include "TVirtualFitter.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"

#include <iostream>
#include <stdexcept>
#include <memory>
#include <errno.h>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>

#include <cassert> 	//Can be used to terminate program if argument is not true. Ex : assert(test > 0 && "Error message");
#include <sys/stat.h> // to be able to use mkdir

#include "mapFedkeyToDetid.h"
#include "tdrstyle.C"

bool print_LeakFiles_warning = true; //To print error only once in stdout

// ------------------------------
//  ######   #######  ##     ## ##     ##  #######  ##    ##    ######## ##     ## ##    ##  ######
// ##    ## ##     ## ###   ### ###   ### ##     ## ###   ##    ##       ##     ## ###   ## ##    ##
// ##       ##     ## #### #### #### #### ##     ## ####  ##    ##       ##     ## ####  ## ##
// ##       ##     ## ## ### ## ## ### ## ##     ## ## ## ##    ######   ##     ## ## ## ## ##
// ##       ##     ## ##     ## ##     ## ##     ## ##  ####    ##       ##     ## ##  #### ##
// ##    ## ##     ## ##     ## ##     ## ##     ## ##   ###    ##       ##     ## ##   ### ##    ##
//  ######   #######  ##     ## ##     ##  #######  ##    ##    ##        #######  ##    ##  ######
// ------------------------------


//Convert a double into a TString
// precision --> can choose if TString how many digits the TString should display
TString Convert_Number_To_TString(double number, int precision=10)
{
	stringstream ss;
	ss << std::setprecision(precision) << number;
	TString ts = ss.str();
	return ts;
}

//Convert a TString into a double
double Convert_TString_To_Number(TString ts)
{
	double number = 0;
	string s = ts.Data();
	stringstream ss(s);
	ss >> number;
	return number;
}


//Convert a Hexadecimal TString into an unsigned long
unsigned long Convert_Hexa_To_UnsignedLong(TString hex_TString)
{
	return std::strtoul(hex_TString.Data(), 0, 16);
}

//Convert an unsigned long into hexa TString
TString Convert_UnsignedLong_To_Hexa(unsigned long number)
{
	TString ts = "";
	stringstream ss;
	ss << std::hex << number;
	ts = ss.str();
	return ts;
}


//Use stat function (from library sys/stat) to check if a file exists
bool Check_File_Existence(const TString& name)
{
  struct stat buffer;
  return (stat (name.Data(), &buffer) == 0); //true if file exists
}



// initially defined in SignalBiasScan/CommonTools/CurvesFunctions.h
int GetSubdet(ULong64_t modid)
{
  if(modid>999999999) modid/=10; // sensor option for TOB
  // 3->6 : TIB, TID, TOB, TEC
  static const Int_t kSubdetOffset = 25;
  int subdet = (modid>>kSubdetOffset)&0x7;

  return subdet;
}

// initially defined in SignalBiasScan/CommonTools/CurvesFunctions.h
int GetLayer(ULong64_t modid)
{
  if(modid>999999999) modid/=10; // sensor option for TOB & TEC
  Int_t subdet = GetSubdet(modid);
  
  static const unsigned layerStartBit = 14;
  static const unsigned layerMask = 0x7;
  static const unsigned ringStartBitTID = 9;
  static const unsigned ringMaskTID = 0x3;
  static const unsigned ringStartBitTEC = 5;
  static const unsigned ringMaskTEC = 0x7;
  Int_t layer = 0;
  
  // For TIB and TOB
  if(subdet==3 || subdet==5) layer = ((modid>>layerStartBit) & layerMask);
  // For TID, returns ring
  if(subdet==4) layer = ((modid>>ringStartBitTID) & ringMaskTID);
  // For TEC, returns ring
  if(subdet==6) layer = ((modid>>ringStartBitTEC) & ringMaskTEC);

  return layer;

}



double GetMedian(vector<double> values)
{
	int size = values.size();

	if (size == 0) return 0;
	else
	{
		sort(values.begin(), values.end());
		if (size % 2 == 0)
		{
	 		return (values[size / 2 - 1] + values[size / 2]) / 2;
		}
		else 
		{
			return values[size / 2];
		}
	}
}


//---------------





// ------------------------------
//  ######  ########  ########  ######  #### ######## ####  ######     ######## ##     ## ##    ##  ######
// ##    ## ##     ## ##       ##    ##  ##  ##        ##  ##    ##    ##       ##     ## ###   ## ##    ##
// ##       ##     ## ##       ##        ##  ##        ##  ##          ##       ##     ## ####  ## ##
//  ######  ########  ######   ##        ##  ######    ##  ##          ######   ##     ## ## ## ## ##
//       ## ##        ##       ##        ##  ##        ##  ##          ##       ##     ## ##  #### ##
// ##    ## ##        ##       ##    ##  ##  ##        ##  ##    ##    ##       ##     ## ##   ### ##    ## ###
//  ######  ##        ########  ######  #### ##       ####  ######     ##        #######  ##    ##  ######  ###
// ------------------------------



//  ####  ##### ###### #####     #      #  ####  #####
// #        #   #      #    #    #      # #        #
//  ####    #   #####  #    #    #      #  ####    #
//      #   #   #      #####     #      #      #   #
// #    #   #   #      #         #      # #    #   #
//  ####    #   ###### #         ###### #  ####    #

//Global object, needed by most functions (to know the Vsteps on which to run)
vector<TString> step_list(0); vector<bool> step_isUsed(0); int nsteps_used=0;

/**
 * Fill the global vector needed throughout the codes
 * Run -- load different steps depending on run number
 */
void Fill_Step_List_Vector(TString run)
{
	 //Vector must be filled only once !
	if(step_list.size() != 0)
	{
		// cout<<FRED("Warning : filling global vector 'step_list' multiple times ! Potentially problematic !")<<endl;
		step_list.resize(0);
	}
	
	else if(run == "331595") // Sept 2019
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("15"); step_isUsed.push_back(true);
		step_list.push_back("20"); step_isUsed.push_back(true);
		step_list.push_back("25"); step_isUsed.push_back(true);
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("35"); step_isUsed.push_back(true);
		step_list.push_back("40"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("70"); step_isUsed.push_back(true);
		step_list.push_back("80"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
	}
	else if(run == "328691") // March 2019
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("15"); step_isUsed.push_back(true);
		step_list.push_back("20"); step_isUsed.push_back(true);
		step_list.push_back("25"); step_isUsed.push_back(true);
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("35"); step_isUsed.push_back(true);
		step_list.push_back("40"); step_isUsed.push_back(true);
		step_list.push_back("45"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("70"); step_isUsed.push_back(true);
		step_list.push_back("80"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
	}
	else if(run == "323011") // Sep 2018
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("15"); step_isUsed.push_back(true);
		step_list.push_back("20"); step_isUsed.push_back(true);
		step_list.push_back("25"); step_isUsed.push_back(true);
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("35"); step_isUsed.push_back(true);
		step_list.push_back("40"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("70"); step_isUsed.push_back(true);
		step_list.push_back("80"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
	}
	else if(run == "318024") // June 2018 part2
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("100"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);

	}
	else if(run == "317974") // June 2018 part1
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("15"); step_isUsed.push_back(true);
		step_list.push_back("20"); step_isUsed.push_back(true);
		step_list.push_back("25"); step_isUsed.push_back(true);
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("35"); step_isUsed.push_back(true);
		step_list.push_back("40"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("70"); step_isUsed.push_back(true);
		step_list.push_back("80"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("100"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("199"); step_isUsed.push_back(false); // outlier

		// part 2
		//step_list.push_back("11"); step_isUsed.push_back(true);
		//step_list.push_back("51"); step_isUsed.push_back(true);
		//step_list.push_back("101"); step_isUsed.push_back(true);
		//step_list.push_back("151"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
	}
	else if(run == "307585")
	{
		step_list.push_back("10"); step_isUsed.push_back(true);
		step_list.push_back("20"); step_isUsed.push_back(true);
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("40"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		//step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("195"); step_isUsed.push_back(true);
		step_list.push_back("210"); step_isUsed.push_back(true);
	}
	else if(run == "303272")
	{
		step_list.push_back("10"); step_isUsed.push_back(false);
		step_list.push_back("20"); step_isUsed.push_back(false);
		step_list.push_back("30"); step_isUsed.push_back(false);
		step_list.push_back("40"); step_isUsed.push_back(false);
		step_list.push_back("50"); step_isUsed.push_back(false);
		step_list.push_back("60"); step_isUsed.push_back(false);
		step_list.push_back("70"); step_isUsed.push_back(false);
		step_list.push_back("80"); step_isUsed.push_back(false);
		step_list.push_back("90"); step_isUsed.push_back(false);
		step_list.push_back("100"); step_isUsed.push_back(false);
		step_list.push_back("110"); step_isUsed.push_back(false);
		step_list.push_back("120"); step_isUsed.push_back(false);
		step_list.push_back("130"); step_isUsed.push_back(false);
		step_list.push_back("140"); step_isUsed.push_back(false);
		step_list.push_back("150"); step_isUsed.push_back(false);
		step_list.push_back("160"); step_isUsed.push_back(false);
		step_list.push_back("170"); step_isUsed.push_back(false);
		step_list.push_back("180"); step_isUsed.push_back(false);
		step_list.push_back("190"); step_isUsed.push_back(false);
		step_list.push_back("200"); step_isUsed.push_back(false);
		step_list.push_back("210"); step_isUsed.push_back(false);
		step_list.push_back("225"); step_isUsed.push_back(false);
		step_list.push_back("240"); step_isUsed.push_back(false);
		step_list.push_back("255"); step_isUsed.push_back(false);
		step_list.push_back("270"); step_isUsed.push_back(false);
		step_list.push_back("285"); step_isUsed.push_back(false);


		step_isUsed[0] = true; //10V
		step_isUsed[1] = true; //20V
		step_isUsed[2] = true; //30V
		step_isUsed[3] = true; //40V
		step_isUsed[4] = true; //50V
		step_isUsed[5] = true; //60V
		step_isUsed[6] = true; //70V
		step_isUsed[7] = true; //80V
		step_isUsed[8] = true; //90V
		step_isUsed[9] = true; //100V
		step_isUsed[10] = true; //110V
		step_isUsed[11] = true; //120V
		step_isUsed[12] = true; //130V
		step_isUsed[13] = true; //140V
		step_isUsed[14] = true; //150V
		step_isUsed[15] = true; //160V
		step_isUsed[16] = true; //170V
		step_isUsed[17] = true; //180V
		step_isUsed[18] = true; //190V
		step_isUsed[19] = false; //200V //-- not present in run303272
		step_isUsed[20] = true; //210V
		step_isUsed[21] = true; //225V
		step_isUsed[22] = true; //240V
		step_isUsed[23] = true; //255V
		step_isUsed[24] = true; //270V
		step_isUsed[25] = true; //285V
	}
	else if(run == "280667")
	{
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("45"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("75"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("195"); step_isUsed.push_back(true);
		step_list.push_back("210"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("240"); step_isUsed.push_back(true);
		step_list.push_back("255"); step_isUsed.push_back(true);
		step_list.push_back("270"); step_isUsed.push_back(true);
		step_list.push_back("285"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
		step_list.push_back("325"); step_isUsed.push_back(true);
		step_list.push_back("350"); step_isUsed.push_back(true);
	}
	else if(run == "203243")
	{
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("45"); step_isUsed.push_back(true);
		step_list.push_back("60"); step_isUsed.push_back(true);
		step_list.push_back("75"); step_isUsed.push_back(true);
		step_list.push_back("90"); step_isUsed.push_back(true);
		step_list.push_back("105"); step_isUsed.push_back(true);
		step_list.push_back("120"); step_isUsed.push_back(true);
		step_list.push_back("135"); step_isUsed.push_back(true);
		step_list.push_back("150"); step_isUsed.push_back(true);
		step_list.push_back("165"); step_isUsed.push_back(true);
		step_list.push_back("180"); step_isUsed.push_back(true);
		step_list.push_back("195"); step_isUsed.push_back(true);
		step_list.push_back("210"); step_isUsed.push_back(true);
		step_list.push_back("225"); step_isUsed.push_back(true);
		step_list.push_back("240"); step_isUsed.push_back(true);
		step_list.push_back("250"); step_isUsed.push_back(true);
		step_list.push_back("260"); step_isUsed.push_back(true);
		step_list.push_back("275"); step_isUsed.push_back(true);
		step_list.push_back("290"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
		step_list.push_back("325"); step_isUsed.push_back(true);
		step_list.push_back("350"); step_isUsed.push_back(true);
	}
	else if(run == "153517")
	{
		step_list.push_back("30"); step_isUsed.push_back(true);
		step_list.push_back("50"); step_isUsed.push_back(true);
		step_list.push_back("65"); step_isUsed.push_back(true);
		step_list.push_back("80"); step_isUsed.push_back(true);
		step_list.push_back("95"); step_isUsed.push_back(true);
		step_list.push_back("110"); step_isUsed.push_back(true);
		step_list.push_back("125"); step_isUsed.push_back(true);
		step_list.push_back("140"); step_isUsed.push_back(true);
		step_list.push_back("155"); step_isUsed.push_back(true);
		step_list.push_back("170"); step_isUsed.push_back(true);
		step_list.push_back("185"); step_isUsed.push_back(true);
		step_list.push_back("200"); step_isUsed.push_back(true);
		step_list.push_back("215"); step_isUsed.push_back(true);
		step_list.push_back("230"); step_isUsed.push_back(true);
		step_list.push_back("245"); step_isUsed.push_back(true);
		step_list.push_back("260"); step_isUsed.push_back(true);
		step_list.push_back("280"); step_isUsed.push_back(true);
		step_list.push_back("300"); step_isUsed.push_back(true);
		step_list.push_back("325"); step_isUsed.push_back(true);
		step_list.push_back("350"); step_isUsed.push_back(true);
	}
	// else {cout<<FRED("Wrong run number ? Abort")<<endl;}

	for(int istep=0; istep<step_isUsed.size(); istep++)
	{
		if(step_isUsed[istep] == true) nsteps_used++;
	}


	return;
}



 // #####  ######   ##   #####      ####  #    # #####      ####  #    # ##### #####  #    # #####
 // #    # #       #  #  #    #    #    # ##  ## #    #    #    # #    #   #   #    # #    #   #
 // #    # #####  #    # #    #    #      # ## # #    #    #    # #    #   #   #    # #    #   #
 // #####  #      ###### #    #    #      #    # #    #    #    # #    #   #   #####  #    #   #
 // #   #  #      #    # #    #    #    # #    # #    #    #    # #    #   #   #      #    #   #
 // #    # ###### #    # #####      ####  #    # #####      ####   ####    #   #       ####    #

/**
 * Executes bash command and returns the output as a Tstring
 * !! PROBLEMS WHEN PIPES INVOLVED (errno=ENOMEM, memory-related issue) --
 */
TString GetStdoutFromCommand(TString cmd_ts)
{
	string output = "";
	FILE* stream=0;
	const int max_buffer = 2000; //Lack of buffer memory causes errors (errno=12)
	char buffer[max_buffer]; //Create buffer
	cmd_ts+= " 2>&1"; //Get both stdout and stderr outputs
	string cmd = cmd_ts.Data();

	stream = popen(cmd.c_str(), "r"); //Open read-only stream, run command

	if(stream)
	{
		while(!feof(stream))
		{
			if(fgets(buffer, max_buffer, stream) != NULL) output.append(buffer); //Get the output
		}

		pclose(stream);
	}
	else //If stream was not opened <-> insufficient buffer memory. Retry with more !
	{
		pclose(stream);

		return "";
	}

	output.erase(std::remove(output.begin(), output.end(), '\n'), output.end()); //Remove the "end of line" character

	TString ts_output(output); //Convert string to TString

	return ts_output;
}


/**
 * Executes bash command and returns the output as a Tstring
 * -- Uses a tmp ofstream file to store command output, reads it and returns it
 */

/*
TString GetStdoutFromCommand(TString cmd)
{
	cmd+= " > cmd_output_tmp.txt 2>&1"; //Get both stdout and stderr outputs, redirect to tmp file

	system(cmd.Data() ); //Execute command

	ifstream file_in("cmd_output_tmp.txt"); //Open file
	string s; TString output;
	while(getline(file_in, s) )
	{
		output+= s + '\n';
	}
	output.Remove(TString::kTrailing, '\n'); //Remove EOL char. at end of TString

	// system("rm cmd_output_tmp.txt"); //Rm tmp file

	return output;
}*/



// ######  ####   ####     #####    ##   ##### #    #
// #      #    # #         #    #  #  #    #   #    #
// #####  #    #  ####     #    # #    #   #   ######
// #      #    #      #    #####  ######   #   #    #
// #      #    # #    #    #      #    #   #   #    #
// ######  ####   ####     #      #    #   #   #    #


/**
 * C++ code to get file path for given run and Vstep (looking at EOS ls output)
 * @param  path_run -- basic path corresponding to the run (in which to look for)
 * @param  run      -- run number
 * @param  step     -- voltage step
 * @return          -- return the path of .root file
 */
TString Find_Path_EOS_File(TString path_run, TString run, TString step)
{
	if(step_list.size() == 0) {Fill_Step_List_Vector(run);} //If vector filling func not already called
	int counter = 0; //Make sure while loops are not infinite
	int nlines = 0;

	TString path_file = path_run;
	TString command = "";
	TString read_output = "";
	path_file.Remove(TString::kTrailing, '/'); //Remove '/' char at end of TString if present
	cout<<FBLU("-- Looking for .root file '"<<step<<"V' in path : "<<path_file<<" --")<<endl;

	//Look recursively in path, as long as .root file not found
	while(!path_file.Contains(".root"))
	{
		cout<<" listing directory: "<<path_file<<endl;
		//Count number of files in dir.
		command = "eos ls " + path_file + " | wc -l";

		 //cout<<"command --> "<<command<<endl;
		 //cout<<"GetStdoutFromCommand(command) --> "<<GetStdoutFromCommand(command)<<endl;
		 //cout<<"Convert_TString_To_Number(GetStdoutFromCommand(command) --> "<<Convert_TString_To_Number(GetStdoutFromCommand(command))<<endl;

		nlines = Convert_TString_To_Number( GetStdoutFromCommand(command) );

		//If there is only 1 file listed
		if(nlines==1)
		{
			command = "eos ls " + path_file;
			read_output = GetStdoutFromCommand(command); //Get command output
			path_file+= "/" + read_output;

			if(read_output.Contains(".root")) {return path_file;} //If it's a .root file, stop there
			else {continue;} //Else look further in subdir arborescence
		}
		else if(nlines==0) {cout<<FRED("Empty dir. -- EOS path of .root file not found !")<<endl; return "";}
		else //If multiple files or dir. listed, look each of them
		{
			cout<<" looking for the file/dir corresponding to the step: "<<step<<endl;
			//Reads content of "path_run" dir. -- Will look for desired Vstep (whether it is present as a subdir. or as a .root file)
			for (int iline = 1; iline <= nlines; iline++)
			{
				command = "eos ls " + path_file + " | sed -n '" + Convert_Number_To_TString(iline) + "p'"; //Look to each filename, one at a time
				read_output = GetStdoutFromCommand(command); //Get command output

				// cout<<"-- ((Checking "<<read_output<<"))"<<endl;

				//If Vstepstring found
				if(read_output.Contains("_" + step) )
				{
					//-- Complicated way of making sure that the "step" pattern is really placed at the end of the path name (not inside the date, ...)
					int step_string_size = ((string) step).size();
					TString ts_tmp = read_output;

					if(ts_tmp.Contains(".root")) //If we are looking at a root file
					{
						if(ts_tmp.Contains("_"+step+".root") || ts_tmp.Contains("_"+step+"_") || ts_tmp.Contains("_"+step+"V") )
						{
							cout<<BOLD(FBLU("FOUND !"))<<endl;
							return path_file + "/" + read_output;
						}

						// ts_tmp.Remove(0, ((string) read_output).size() - ((string) step).size() -1 -5 ); //Keep the "_xxxV.root" part
						// if(ts_tmp == "_" + step + ".root")
						// {
						// 	cout<<BOLD(FBLU("FOUND !"))<<endl;
						// 	return path_file + "/" + read_output;
						// }
					}
					else //If we are looking at a subdir
					{
						ts_tmp.Remove(0, ((string) read_output).size() - ((string) step).size() -1 ); //Just keep the last characters (e.g. "_150")
						// cout<<"ts_tmp "<<ts_tmp<<endl;
						if(ts_tmp == "_" + step)
						{
							cout<<BOLD(FBLU("FOUND !"))<<endl;

							path_file+= "/" + read_output; //If vstep found, add to path
							// cout<<"<<path_file "<<path_file<<endl;
							break;
						}
					}
				}

				if(iline == nlines) {cout<<FRED("EOS path of .root file not found !")<<endl; return "";}
			}
		}

	} //end while
}









/*
 ██████  ██████  ██████  ██████  ███████  ██████ ████████     ██ ██      ███████  █████  ██   ██
██      ██    ██ ██   ██ ██   ██ ██      ██         ██        ██ ██      ██      ██   ██ ██  ██
██      ██    ██ ██████  ██████  █████   ██         ██        ██ ██      █████   ███████ █████
██      ██    ██ ██   ██ ██   ██ ██      ██         ██        ██ ██      ██      ██   ██ ██  ██
 ██████  ██████  ██   ██ ██   ██ ███████  ██████    ██        ██ ███████ ███████ ██   ██ ██   ██
*/



//////////////////////////////////////////////////////////////////////////////////////////////////

// Leackage current corrections
//------------------------------

int CorrectGraphForLeakageCurrent(TGraphErrors* g, double detid, TFile* f)
{
	// Get correction function
	if(!f) {return 0;}

	TString fit_name = "fit_" +Convert_Number_To_TString(detid);
	if(!f ->GetListOfKeys()->Contains(fit_name) && print_LeakFiles_warning) {cout<<"LeakCorr "<<fit_name<<" not found !"<<endl; print_LeakFiles_warning=false; return 0;}
	else if(!f ->GetListOfKeys()->Contains(fit_name) ) {return 0;}
	TF1* func = (TF1*) f->Get(fit_name);

	//WARNING on missing Leakage correction, only if module is in list of monitored detectors
/*	if(!func)
	{
		bool isInList = false;

		ifstream det_file;
		TString ts = Convert_Number_To_TString(detid);
		if(ts.Contains("36912")) {det_file.open("detid_lists/TIB_detit_list.txt");}
		else if(ts.Contains("43628")) {det_file.open("detid_lists/TOB_detit_list.txt");}
		else if(ts.Contains("47014")) {det_file.open("detid_lists/TEC_detit_list.txt");}
		//else {cout<<"Corresponding detector list not found ! Detid = "<<ts<<endl; return 0;}
		//cout<<"detID = "<<detid<<endl;

		string line; double id=0;
		while(getline(det_file,line))
		{
			std::istringstream iss(line);
		    iss >> id;
		    cout<<id<<endl;
		    if(detid == id || (long) (detid/10) == id || detid == (long) (id/10) ) {isInList = true;}
		}

		// if(isInList)
		{cout<<FRED("No leakage current correction for detid : ")<<std::setprecision(11)<<detid<<endl;}
		return 0;
	}*/

	// Correct points
	double x,y;
	for(int i=0; i<g->GetN(); i++)
	{
		g->GetPoint(i, x, y);
		g->SetPoint(i, x-func->Eval(x), y);
	}

	return 1;
}

int CorrectGraphForLeakageCurrent(TGraphErrors* g, double detid, TString output_name)
{
	// Get correction function
	TString filename="LeakCurCorr"+output_name+".root";

	TString filepath="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/wDCUcur/"+filename;

	if (filename.Contains("170000") ) {return 0;} //No leakage correction for this old run, neglected

	if(!Check_File_Existence(filepath) && print_LeakFiles_warning)
	{
		cout<<endl<<BOLD(FRED("No leakage current correction file : "))<<filepath<<endl;
		print_LeakFiles_warning = false;
		return 0;
	}
	else if(!Check_File_Existence(filepath)) {return 0;}


	TFile* f = new TFile(filepath, "read");

	int out = CorrectGraphForLeakageCurrent(g, detid, f);

	f->Close();
	f->Delete();

	return out;
}





#endif
