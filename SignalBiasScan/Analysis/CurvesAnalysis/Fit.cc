#include "Fit.h"

//-------------

//-- CHOOSE HERE THE TIB MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTIBSmallScan(string dirname, string date, string run, string type)
{
	vector<ULong64_t> v_modids;

	v_modids.push_back(369125866);

/*
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
*/

  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	FitOneCurve(dirname, "TIB", run, v_modids[i_modid], type, 1, date, 0);
  }
}

//-- CHOOSE HERE THE TOB MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTOBSmallScan(string dirname, string date, string run, string type)
{
	vector<ULong64_t> v_modids;

	v_modids.push_back(4362815122);

/*
//-------- FULL LIST
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
*/

  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	FitOneCurve(dirname, "TOB", run, v_modids[i_modid], type, 1, date, 0);
  }
}

//-- CHOOSE HERE THE TEC MODULE(S) FOR WHICH YOU WANT TO PLOT THE SCAN CURVE
void FitTECSmallScan(string dirname, string date, string run, string type)
{
	vector<ULong64_t> v_modids;

	v_modids.push_back(4701483241);


/*
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
*/
  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	FitOneCurve(dirname, "TEC", run, v_modids[i_modid], type, 1, date, 0);
  }
}


int main()
{
  Modified_tdr_style();
  gStyle->SetOptFit(0);
  
  
//--- Choose the observable
  //string type = "Signal";
  string type = "ClusterWidth";

//--- Choose the subdetector (modules are selected above)
  string subdet = "TIB";
  //string subdet = "TOB";
  //string subdet = "TEC";


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
  //runs.push_back("271056");	dates.push_back("20160423");
  //runs.push_back("274969");	dates.push_back("20160612");
  //runs.push_back("276437");	dates.push_back("20160706");
  //runs.push_back("278167");	dates.push_back("20160803");
  //runs.push_back("280385");	dates.push_back("20160909");
  //runs.push_back("285371");	dates.push_back("20161116");
  
  //2017
  //runs.push_back("295324");	dates.push_back("20170527"); //Full
  //runs.push_back("298996");	dates.push_back("20170714");
  //runs.push_back("302131");	dates.push_back("20170831");
  runs.push_back("303824");	dates.push_back("20170924");
  //runs.push_back("305862");	dates.push_back("20171030");




  string dirname = "../"+type+"Analysis/Code/Outputs";

  for(int irun = 0; irun < runs.size(); irun++)
  {
  	if(subdet == "TIB") {FitTIBSmallScan(dirname, dates[irun], runs[irun], type);}
  	else if(subdet == "TOB") {FitTOBSmallScan(dirname, dates[irun], runs[irun], type);}
  	else if(subdet == "TEC") {FitTECSmallScan(dirname, dates[irun], runs[irun], type);}
  	else {cout<<" Wrong subdet !"<<endl; return 0;}
  }

  
  //system("eog Fit_line.png &");
  //system("eog Fit_curv.png &");
}

