#include <iostream>
#include <time.h>
#include <map>
#include <utility>
#include <vector>

#include <TString.h>
#include <TROOT.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TRint.h>
#include <exception>
#include <TTree.h>
#include "TSystem.h"

#include "Fit.h"



void FitAllCurves(string DirName, string SubDet, string date, string Run, string type, bool use_curvature=true, bool smallScan_modules_only=true)
{
  gROOT->SetBatch(kTRUE); //doesn't work ? Else execute interactively with option -b (batch mode)

  if(type!="ClusterWidth" && type!="Signal")
  {cout<<"Error in FitAllCurves() : curve type "<<type<<" is not allowed."<<endl; return;}

  int err_type=0;
  const int step = 55;

  string FileToOpen = DirName+SubDet+"_output_DecoMode_"+Run+".root";
  TString Output = "DECO_";
  if(!smallScan_modules_only) Output+= "allModules_";
  Output+= type+"_"+SubDet;
  if(use_curvature) Output+="_kink"; //Add suffix for method
  else Output+="_line";
  Output+=+"_"+Run+".root";

  if(!Check_File_Existence(FileToOpen)) {cout<<BOLD(FRED("File not found : "))<<FileToOpen<<endl; return;}

  std::cout << FGRN("Opening input file ") <<FileToOpen<< std::endl;

  TFile* myFile = TFile::Open(FileToOpen.c_str());
  TTree* tr = (TTree*)myFile->FindObjectAny("T");
  //std::cout << "tree opened" << std::endl;

  //Int_t detid; // Int_t for old files
  ULong64_t detid; // Int_t for old files
  Double_t volt;
  Int_t id;
  Double_t evolt;
  Double_t mpv;
  Double_t empv;
  ULong64_t tempdetid;
  //Double_t chi2overndf;

  tr->SetBranchAddress("DetID",&detid);
  tr->SetBranchAddress("Voltage",&volt);
  tr->SetBranchAddress("Index",&id);
  tr->SetBranchAddress("errVoltage",&evolt);
  if(type=="Signal")
  {
    tr->SetBranchAddress("MPV",&mpv);
    tr->SetBranchAddress("errMPV",&empv);
  }
  else if(type=="ClusterWidth")
  {
    tr->SetBranchAddress("Mean",&mpv);
    tr->SetBranchAddress("errMean",&empv);
  }

  std::cout << FGRN("Opening output file ") << Output << std::endl;

  TFile* output = new TFile(Output,"recreate");
  TTree* tout = new TTree("tout","SignalSummary");

  ULong64_t odetid;
  int olayer;
  double odepvolt;
  double oerrdepvolt;
  double oplateau;
  double ofitchisquare;
  int ofitstatus;
  double olastpty;
  double ochi2;

  tout->Branch("DETID",&odetid,"DETID/l");
  tout->Branch("LAYER",&olayer,"LAYER/I");
  tout->Branch("ERRDEPVOLT",&oerrdepvolt,"ERRDEPVOLT/D");
  tout->Branch("DEPVOLT",&odepvolt,"DEPVOLT/D");
  tout->Branch("PLATEAU",&oplateau,"PLATEAU/D");
  tout->Branch("FITCHI2",&ofitchisquare,"FITCHI2/D");
  tout->Branch("FITSTATUS",&ofitstatus,"FITSTATUS/I");
  tout->Branch("LASTPOINTS",&olastpty,"LASTPOINTS/D");
  tout->Branch("CHI2",&ochi2,"CHI2/D");

  UInt_t nentries = tr->GetEntries();
  Int_t nfit = 0;
  Int_t nbadfit = 0;
  double lastpty = 0;


  double Vdep;
  double errVdep;

  typedef std::map< ULong64_t, std::vector<double> > mapping;
  // std::vector< double > tempdepvolt;
  mapping DetID_Vdep;

  double avolt[step];
  double aevolt[step];
  double ampv[step];
  double aempv[step];
  UInt_t k=0;

  Int_t layer = 0;
  float mean, rms;
  TH1F* hrms = new TH1F("hrms", "yrms", 50, 0, 10);

  //std::cout <<"nentries = "<< nentries << std::endl;

  if(!nentries) {cout<<BOLD(FRED("WARNING : NO ENTRY FOUND !!"))<<endl<<endl;}

  //string leakcurfilename="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/LeakCurCorr_"+SubDet+Run+".root";
  string leakcurfilename="../../../LeakageCurrentBiasScan/Analysis/LeakCurCorr_files/LeakCurCorr_"+SubDet+"_"+date+"_run"+Run+".root";

  TFile* leakcurfile = 0;
  if(Check_File_Existence(leakcurfilename) ) leakcurfile = new TFile(leakcurfilename.c_str(), "read");
  else {cout<<BOLD(FRED("No leakage current correction file : "))<<leakcurfilename<<endl<<"-----------------------------------------------"<<endl; }


  // Loop over entries to create curves
  //  for(Int_t i = 0; i < nentries; i++){
  for(UInt_t i = 0; i <nentries; i++)
  {
   	if(i%1000==0) {cout<<i<<" / "<<nentries<<" entries done"<<endl;}

   	tr->GetEntry(i);

    TString tmp_ts_detid = Convert_Number_To_TString(detid);
    //cout<<"detid = "<<detid<<" / tmp_ts_detid="<<tmp_ts_detid<<endl;


  	if(smallScan_modules_only)
  	{
      if(SubDet == "TOB" && !tmp_ts_detid.Contains("4362815") && !tmp_ts_detid.Contains("4362329") ) {continue;}

      else if(SubDet == "TIB" && !tmp_ts_detid.Contains("36912138") && !tmp_ts_detid.Contains("369121390") && !tmp_ts_detid.Contains("36912160") && !tmp_ts_detid.Contains("36912161") && !tmp_ts_detid.Contains("36912586") && !tmp_ts_detid.Contains("369125870")) {continue;}

      else if(SubDet == "TEC" && !tmp_ts_detid.Contains("470148") ) {continue;}
  	}

    tempdetid = (ULong64_t) detid;

    mapping::iterator iter = DetID_Vdep.find(tempdetid);
    if(iter == DetID_Vdep.end() ) //If detid was not already found in map
    {
      // cout<<"map size 0 = "<<DetID_Vdep.size()<<endl;

      k=0;

      for(UInt_t j = i; j< nentries; j++)
      {
  	    tr->GetEntry(j);

    		if(tempdetid==(ULong64_t) detid)
    		{
    		  if(empv<5)
    		  {
    	    	avolt[k]=volt;
    	    	aevolt[k]=evolt;
    	    	ampv[k]=mpv;
    	    	aempv[k]=empv;
    		    if(err_type==1) {aempv[k]=2.5;} //Default error
    	        //cout<<volt<<" "<<mpv<<endl;
    	    	k++;
    		  }
    		}
	    }//for(UInt_t j = i; j< nentries; j++)

  	  if(k<3) {continue;} // Need enought points to compute 2nd derivative.
  	  if(k) {lastpty = ampv[k-1];}
  	  if(k>2) {lastpty = (ampv[k-1]+ampv[k-2]+ampv[k-3])/3.;}

      TString canvasname = "CW_" + Convert_Number_To_TString(tempdetid);
      TCanvas *c1;
      c1 = new TCanvas(canvasname.Data() );
      c1->cd();
      TGraphErrors * thegraph = new TGraphErrors(k, avolt, ampv, aevolt, aempv);
      string corr_name="_"+SubDet+Run;


      int corrected = 0;
      if(SubDet == "TEC" || SubDet == "TOB") {corrected = CorrectGraphForLeakageCurrent(thegraph, tempdetid/10, leakcurfile);} //Added digit bc of double sensors !
      else {corrected = CorrectGraphForLeakageCurrent(thegraph, tempdetid, leakcurfile);}

      GetYMeanRMS(thegraph, mean, rms);
      //cout<<"rms "<<rms<<endl;
      hrms->Fill(rms);
      // Check if module was in the Vbias scan
      //if(rms<1.) {cerr<<"module "<<tempdetid<<" is skipped, yrms<1. ("<<rms<<")"<<endl; continue;}

      thegraph->SetLineColor(2);
      thegraph->SetMarkerColor(1);
      thegraph->SetMarkerStyle(20);

      layer = GetLayer(tempdetid);

      // std::cout << tempdetid <<" layer "<< layer << std::endl;

  	  int debug = 0;
  	  bool filter_twice = false;
  	  //if(SubDet=="TID" && (Run=="_170000" || Run=="_193928") ) filter_twice=true;

      bool small_rms=false;
      /*
      //FIXME - removed protection
      if(SubDet != "TEC" && ((type=="Signal" && rms<1.) || (type=="ClusterWidth" && rms<0.3)) )
      {
      	small_rms=true;
      	cout<<"small rms !"<<endl; //possibly cst voltage !
      }
      // Check if module was in the Vbias scan
      //if((isGoodCurve(thegraph, type) && !small_rms) || (tempdetid==369121389 && Run=="_258443")) //exception
      */

  	  if( (isGoodCurve(thegraph, type) && !small_rms))
  	  {
  	    Vdep = FitCurve( thegraph, 0, filter_twice , use_curvature, false, "", 0);
      }


      else
      {
        Vdep=-1;
        if(small_rms) cerr<<" small yrms: "<<rms<<endl;
        else {cout<<FRED("isGoodCurve = false");}
        cerr<<" ---> module "<<tempdetid<<" is skipped."<<endl<<endl<<endl;
      }

      std::vector< double > tempdepvolt;
      // tempdepvolt.clear();

      // cout<<"layer = "<<layer<<endl;
      // cout<<__LINE__<<endl;
      tempdepvolt.push_back(layer); //-- possible segfault here ?
      // cout<<__LINE__<<endl;

      tempdepvolt.push_back(Vdep);
      tempdepvolt.push_back(errVdep);
      tempdepvolt.push_back(0);
      tempdepvolt.push_back(0);
      tempdepvolt.push_back(0);
      tempdepvolt.push_back(lastpty);
      tempdepvolt.push_back(0);

      // cout<<__LINE__<<endl;
      // cout<<"DetID_Vdep = "<<&DetID_Vdep<<endl;
      // cout<<"map size 1 = "<<DetID_Vdep.size()<<endl;

      DetID_Vdep.insert(pair<ULong64_t,vector<double> > (tempdetid,tempdepvolt) );
      // DetID_Vdep.insert( std::make_pair(tempdetid,tempdepvolt) );

      delete c1;
      delete thegraph;
    } //end : if(iter == DetID_Vdep.end() )

  }//end : entries loop


  for(mapping::iterator iter = DetID_Vdep.begin(); iter != DetID_Vdep.end(); ++iter)
  {
    olayer = iter->second.at(0);
    odepvolt = iter->second.at(1);
    oerrdepvolt = iter->second.at(2);
    oplateau = iter->second.at(3);
    ofitchisquare = iter->second.at(4);
    ofitstatus = iter->second.at(5);
    olastpty = iter->second.at(6);
    ochi2 = iter->second.at(7);
    odetid = iter->first;

    //cout<<"DETID = "<<odetid<<" -- VDEP = "<<odepvolt<<endl;

	  if(odepvolt==-1) {continue;}

    tout->Fill();
  }

  //std::cout<<nbadfit<<" bad fits over "<<nfit<<std::endl;

  output->cd();
  hrms->Write();
  tout->Write();
  gDirectory->Write();

  delete hrms;
  if(leakcurfile) delete leakcurfile;
  // delete tout;
  delete output;

  //std::cout << "Closing input file " << FileToOpen << std::endl;
  //std::cout << "Closing output file " << Output << std::endl;
}


// root -l -b -q FitAll.C+
int main()
{
  vector<string> v_analysis;
  // v_analysis.push_back("Signal");
  v_analysis.push_back("ClusterWidth");

  vector<string> v_subdet;
  //v_subdet.push_back("TIB");
  v_subdet.push_back("TOB");
  v_subdet.push_back("TEC");
  v_subdet.push_back("TID");

  bool use_curvature = false; //true-->kink ; false-->lines

  bool smallScan_modules_only = false; //Set to true if not interested in Full Scan entries (e.g. for Vfd evol. plots -- Will save LOT of time)


  vector<string> runs; vector<string> dates; /*
  runs.push_back("160497");	dates.push_back("20110315");


//Old (10)
  runs.push_back("160497");	dates.push_back("20110315");
  runs.push_back("170000");	dates.push_back("20110715");
  runs.push_back("190459");	dates.push_back("20120405");
  runs.push_back("193541");	dates.push_back("20120506");
  runs.push_back("193928");	dates.push_back("20120510");
  runs.push_back("199832");	dates.push_back("20120728");
  runs.push_back("200786");	dates.push_back("20120812");
  runs.push_back("203832");	dates.push_back("20120928");
  runs.push_back("208339");	dates.push_back("20121130");
  runs.push_back("211797");	dates.push_back("20130213");

//2015 (4)
  runs.push_back("246963"); dates.push_back("20150603");
  runs.push_back("254790");	dates.push_back("20150821");
  runs.push_back("258443"); dates.push_back("20151007");
  runs.push_back("262254");	dates.push_back("20151121");*/

//2016 (5)
  runs.push_back("271056");	dates.push_back("20160423");
  //runs.push_back("274969");	dates.push_back("20160612");
  //runs.push_back("276437");	dates.push_back("20160706");
  //runs.push_back("278167");	dates.push_back("20160803");
  //runs.push_back("280385");	dates.push_back("20160909");
  //runs.push_back("285371");	dates.push_back("20161116");

//2017
  // runs.push_back("295324");	dates.push_back("20170527"); //Full
  // runs.push_back("295376");	dates.push_back("20170527"); //Full
  //runs.push_back("298996");	dates.push_back("20170714");
  // runs.push_back("302131");	dates.push_back("20170831");
  // runs.push_back("303824");	dates.push_back("20170924"); //Full
  // runs.push_back("305862");	dates.push_back("20171030");

//2018
	// runs.push_back("314574");	dates.push_back("20180418"); //-- FULL (-20°)
	//runs.push_back("314755");	dates.push_back("20180420"); //-- FULL (-10°)
	// runs.push_back("317182");	dates.push_back("20180530");
	// runs.push_back("317683");	dates.push_back("20180611");



  for(int i=0; i<v_analysis.size(); i++)
  {
  	for(int j=0; j<v_subdet.size(); j++)
  	{
 	  for(int irun = 0; irun < runs.size(); irun++)
 	  {
 	  	if(v_subdet[j]=="TOB" && v_analysis[i]=="Signal" && runs[irun]=="203832") {continue;}
 	  	else if(v_subdet[j]=="TEC" && v_analysis[i]=="Signal" && runs[irun]=="160497") {continue;}

 	    string dirname = "../"+v_analysis[i]+"Analysis/Code/Outputs/";
  	    FitAllCurves(dirname, v_subdet[j], dates[irun], runs[irun], v_analysis[i], use_curvature, smallScan_modules_only);
	  }
	}
  }

}

