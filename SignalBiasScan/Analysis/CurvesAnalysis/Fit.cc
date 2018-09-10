#include "Fit.h"

//-------------

//-- CHOOSE HERE THE TIB MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTIBSmallScan(string dirname, string date, string run, string type, bool produce_multiple_plots, bool multiple_runs_selected)
{
	vector<ULong64_t> v_modids;

	if(!produce_multiple_plots || multiple_runs_selected)
	{
		v_modids.push_back(369121385);
	}

	else
	{
		//TIBminus_1_2_2_1
		v_modids.push_back(369121381);
		v_modids.push_back(369121382);
		v_modids.push_back(369121385);
		v_modids.push_back(369121386);

		v_modids.push_back(369121389);
		v_modids.push_back(369121390);
		//TIBminus_1_4_2_5
		v_modids.push_back(369125861);
		v_modids.push_back(369125865);
		v_modids.push_back(369125869);

		//TIBplus_1_6_2_5
		v_modids.push_back(369125862);
		v_modids.push_back(369125866);
		v_modids.push_back(369125870);
	}

	for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
	{
		FitOneCurve(dirname, "TIB", run, v_modids[i_modid], type, 1, date, 0);
		if(produce_multiple_plots || multiple_runs_selected) {system( ("mv Fit_line.png Fit_line_TIB_"+Convert_Number_To_TString(v_modids[i_modid])+"_"+run+".png").Data() );} //For lines plots
		//if(produce_multiple_plots || multiple_runs_selected) {system( ("mv Fit_curv.png Fit_kink_TIB_"+Convert_Number_To_TString(v_modids[i_modid])+"_"+run+".png").Data() );} //For curv. plots
	}
}

//-- CHOOSE HERE THE TOB MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTOBSmallScan(string dirname, string date, string run, string type, bool produce_multiple_plots, bool multiple_runs_selected)
{
	vector<ULong64_t> v_modids;

	if(!produce_multiple_plots || multiple_runs_selected)
	{
		v_modids.push_back(4362329221);
	}

	else
	{
		v_modids.push_back(4362815081);
		v_modids.push_back(4362815082);
		v_modids.push_back(4362815121);
		v_modids.push_back(4362815122);
		v_modids.push_back(4362815161);
		v_modids.push_back(4362815162);
		v_modids.push_back(4362815201);
		v_modids.push_back(4362815202);
		v_modids.push_back(4362815241);
		v_modids.push_back(4362815242);
		v_modids.push_back(4362815281);
		v_modids.push_back(4362815282);

		// TOB + 1.3.1.6 //NEW -- ADDED 04/2018
		v_modids.push_back(4362329011);
		v_modids.push_back(4362329021);
		v_modids.push_back(4362329051);
		v_modids.push_back(4362329061);
		v_modids.push_back(4362329091);
		v_modids.push_back(4362329101);
		v_modids.push_back(4362329131);
		v_modids.push_back(4362329141);
		v_modids.push_back(4362329171);
		v_modids.push_back(4362329181);
		v_modids.push_back(4362329211);
		v_modids.push_back(4362329221);
		v_modids.push_back(4362329012);
		v_modids.push_back(4362329022);
		v_modids.push_back(4362329052);
		v_modids.push_back(4362329062);
		v_modids.push_back(4362329092);
		v_modids.push_back(4362329102);
		v_modids.push_back(4362329132);
		v_modids.push_back(4362329142);
		v_modids.push_back(4362329172);
		v_modids.push_back(4362329182);
		v_modids.push_back(4362329212);
		v_modids.push_back(4362329222);
	}

	for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
	{
		FitOneCurve(dirname, "TOB", run, v_modids[i_modid], type, 1, date, 0);
		if(produce_multiple_plots || multiple_runs_selected) {system( ("mv Fit_line.png Fit_line_TOB_"+Convert_Number_To_TString(v_modids[i_modid])+"_"+run+".png").Data() );}
	}
}

//-- CHOOSE HERE THE TEC MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTECSmallScan(string dirname, string date, string run, string type, bool produce_multiple_plots, bool multiple_runs_selected)
{
	vector<ULong64_t> v_modids;

	if(!produce_multiple_plots || multiple_runs_selected)
	{
		v_modids.push_back(4701483361);
	}

	else
	{
	//--- FULL LIST
		//1 sensor (+0)
		v_modids.push_back(4701481960);
		v_modids.push_back(4701482280);
		v_modids.push_back(4701482400);
		v_modids.push_back(4701482000);
		v_modids.push_back(4701482320);
		v_modids.push_back(4701482040);
		v_modids.push_back(4701482360);
		//2 sensors (+1/2)
		v_modids.push_back(4701482651);
		v_modids.push_back(4701482961);
		v_modids.push_back(4701483241);
		v_modids.push_back(4701483361);
		v_modids.push_back(4701482611);
		v_modids.push_back(4701482661);
		v_modids.push_back(4701483001);
		v_modids.push_back(4701483281);
		v_modids.push_back(4701483401);
		v_modids.push_back(4701482621);
		v_modids.push_back(4701482921);
		v_modids.push_back(4701483321);
		//...
		v_modids.push_back(4701482652);
		v_modids.push_back(4701482962);
		v_modids.push_back(4701483242);
		v_modids.push_back(4701483362);
		v_modids.push_back(4701482612);
		v_modids.push_back(4701482662);
		v_modids.push_back(4701483002);
		v_modids.push_back(4701483282);
		v_modids.push_back(4701483402);
		v_modids.push_back(4701482622);
		v_modids.push_back(4701482922);
		v_modids.push_back(4701483322);
	}

  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	FitOneCurve(dirname, "TEC", run, v_modids[i_modid], type, 1, date, 0);
    if(produce_multiple_plots || multiple_runs_selected) {system( ("mv Fit_line.png Fit_line_TEC_"+Convert_Number_To_TString(v_modids[i_modid])+"_"+run+".png").Data() );}
  }
}


int main()
{
  Modified_tdr_style();
  gStyle->SetOptFit(0);

  //If true :
  //- If only 1 scan selected, will produce plots for full detid list
  //- If several scans selected, will produce plots for 1 detid but all the selected scans
  bool produce_multiple_plots = true;

//--- Choose the observable
  //string type = "Signal";
  string type = "ClusterWidth";

//--- Choose the subdetector (modules are selected above)
	vector<string> v_subdet;
	//v_subdet.push_back("TIB");
	v_subdet.push_back("TOB");
	//v_subdet.push_back("TEC");

  vector<string> runs; vector<string> dates;

//--- Choose the scan(s) to plot
//Old (10)
  //runs.push_back("160497");	dates.push_back("20110315");
  //runs.push_back("170000");	dates.push_back("20110715");
  //runs.push_back("190459");	dates.push_back("20120405");
  //runs.push_back("193541");	dates.push_back("20120506");
 // runs.push_back("193928");	dates.push_back("20120510");
  //runs.push_back("199832");	dates.push_back("20120728");
  //runs.push_back("200786");	dates.push_back("20120812");
  //runs.push_back("203832");	dates.push_back("20120928"); //--
  //runs.push_back("208339");	dates.push_back("20121130");
  //runs.push_back("211797");	dates.push_back("20130213");

//2015 (4)
  //runs.push_back("246963");	dates.push_back("20150603");
  //runs.push_back("254790");	dates.push_back("20150821");
  //runs.push_back("258443");	dates.push_back("20151007");
  //runs.push_back("262254");	dates.push_back("20151121");

//2016 (5)
  //runs.push_back("271056");	dates.push_back("20160423"); //Full
  //runs.push_back("274969");	dates.push_back("20160612");
  //runs.push_back("276437");	dates.push_back("20160706");
  //runs.push_back("278167");	dates.push_back("20160803");
  //runs.push_back("280385");	dates.push_back("20160909");
  //runs.push_back("285371");	dates.push_back("20161116");

  //2017
  //runs.push_back("295324");	dates.push_back("20170527"); //Full
  //runs.push_back("295376");	dates.push_back("20170527"); //Full
  //runs.push_back("298996");	dates.push_back("20170714");
  //runs.push_back("302131");	dates.push_back("20170831");
  //runs.push_back("303824");	dates.push_back("20170924"); //Full
  //runs.push_back("305862");	dates.push_back("20171030");

  //2018
  runs.push_back("314574");	dates.push_back("20180418"); //Full
  // runs.push_back("314755");	dates.push_back("20180420"); //Full, -10°
  //runs.push_back("317182");	dates.push_back("20180530"); //alcareco issue
  //runs.push_back("317683");	dates.push_back("20180611"); 
  //runs.push_back("320674");	dates.push_back("20180801"); //alcareco issue ?



//--------------------------------------------
  bool multiple_runs_selected = false;
  if(runs.size() > 1) {multiple_runs_selected = true;}

  string dirname = "../"+type+"Analysis/Code/Outputs";

  for(int irun = 0; irun < runs.size(); irun++)
  {
	  for(int j=0; j<v_subdet.size(); j++)
	  {
		  if(v_subdet[j] == "TIB") {FitTIBSmallScan(dirname, dates[irun], runs[irun], type, produce_multiple_plots, multiple_runs_selected);}
		  else if(v_subdet[j] == "TOB") {FitTOBSmallScan(dirname, dates[irun], runs[irun], type, produce_multiple_plots, multiple_runs_selected);}
		  else if(v_subdet[j] == "TEC") {FitTECSmallScan(dirname, dates[irun], runs[irun], type, produce_multiple_plots, multiple_runs_selected);}
		  else {cout<<" Wrong subdet !"<<endl; return 0;}
	  }
  }


  //system("eog Fit_line.png &");
  //system("eog Fit_curv.png &");
}

