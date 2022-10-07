#include "FitLeakageCurrent.C"
#include "../../SignalBiasScan/Analysis/CurvesAnalysis/ComputeShiftWithTemperature.C"

map< string, map< int, TGraph*> > map_DCU_currents_perrun;
map< string, map< int, TGraph*> > map_PS_currents_perrun;
map< string, map< int, int> > map_NMOD_perrun;

// load currents for all detids in memory for any run
void LoadConditionsPerRun(map< string, map< int, TGraph*> > &map_dcu_currents_perrun, map< string, map< int, TGraph*> > &map_ps_currents_perrun, 
  map< string, map< int, int> > &map_nmodforchannel_perrun,  std::string subdet="TIB", std::string run="20120506_run193541", std::string bad_periods="")
{
  
  map_dcu_currents_perrun.clear();
  map_ps_currents_perrun.clear();
  map_nmodforchannel_perrun.clear();
  
  map< int, TGraph*> map_dcu_currents;
  map< int, TGraph*> map_ps_currents;
  map< int, int> map_nmodforchannel;
  
  ReadDCUCurrentFromDCUDataForAllDetids(map_dcu_currents, Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root", run.c_str()), subdet.c_str(), bad_periods);
  ReadCurrentRootForAllDetids(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/PS/PS_I_%s_%s.root", subdet.c_str(), run.c_str()), map_ps_currents, map_nmodforchannel, "ps", bad_periods);

  map_dcu_currents_perrun[run] = map_dcu_currents;
  map_ps_currents_perrun[run] = map_ps_currents;
  map_nmodforchannel_perrun[run] = map_nmodforchannel;
}

void GetLoadedConditionsPerRun(TGraph *&gsteps, TGraph *&gcur_DCU, TGraph *&gcur_PS, TGraph *&gvolt, int& nmodforchannel, 
                   std::string run="20120506_run193541", int detid=369121605)
{

  cout<<"Get loaded conditions per run"<<endl;
  // Read files with voltage infos
  gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}
  gvolt = 0;//ReadVoltage(Form("Data/ConditionBrowser_%s.root", run));
  //if(!gvolt) std::cout<<" ConditionBrowser file does not exist, but it is not a problem."<<std::endl;
  
  nmodforchannel=1;
  // Get current infos
  
  if(map_DCU_currents_perrun.find(run)!=map_DCU_currents_perrun.end()){
    if(map_DCU_currents_perrun[run].find(detid) != map_DCU_currents_perrun[run].end())
      gcur_DCU = map_DCU_currents_perrun[run][detid];
    else
      gcur_DCU = new TGraph(); // return an empty graph
  }
  
  
  if(map_PS_currents_perrun.find(run)!=map_PS_currents_perrun.end()){
    if(map_PS_currents_perrun[run].find(detid) != map_PS_currents_perrun[run].end())
    {
      gcur_PS = map_PS_currents_perrun[run][detid];
      nmodforchannel = map_NMOD_perrun[run][detid];
    }
    else
      gcur_PS = new TGraph();
  }
  
  //cout<<"DCU "<<gcur_DCU->GetN()<<endl;
  //cout<<"PS "<<gcur_PS->GetN()<<endl;
  return;
}


TGraphErrors* GetVoltageDropCurve(std::string subdet, int detid, std::string run, double & mean_ratio, int & nmodforchannel)
{

  bool show=false;
  std::string bad_periods="";
  
  // Get currents and voltage
  TGraph* gsteps = 0;
  TGraph* gcur_DCU = 0;
  TGraph* gcur_PS = 0;
  TGraph* gvolt = 0;
  //int nmodforchannel;
  //cout<<"getting conditions"<<endl;
  if(map_PS_currents_perrun.size()>0 && map_PS_currents_perrun.find(run)!=map_PS_currents_perrun.end())  
    GetLoadedConditionsPerRun(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, run, detid);
  else GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, subdet, run, detid, bad_periods);

  if(!gcur_PS || gcur_PS->GetN()==0) {std::cerr<<" No PS currents. Skipping detid."<<std::endl; return 0;}

  TGraph* gcur_DCU_untouched = 0;
  if(gcur_DCU) gcur_DCU_untouched = (TGraph*) gcur_DCU->Clone();
  
  //double mean_ratio;
  //cout<<"applying DCU/PS"<<endl;
  ApplyDCUOverPSRatio(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, mean_ratio, detid);
  //if(mean_ratio==-1) mean_ratio=1./nmodforchannel;

  // Compute voltage drop induced by leakage current
  TCanvas* c2=0;
  if(show) c2 = new TCanvas("c2", "V drop", 400, 0, 700, 500);
  TGraphErrors* gratio;
  TGraphErrors* gvdrop;
  GetVoltageDropAndRatio(gsteps, gcur_DCU, gcur_PS, gvdrop, gratio);
  gvdrop = AverageIleakVsVbias(gvdrop, 0.001);
  gvdrop->SetTitle(Form("DetID %i", detid));
  gvdrop->SetMarkerStyle(20);
  if(show) gvdrop->Draw("AP");
  
  return gvdrop;
}

void ShowVoltageDropForDetID(std::string subdet="TIB", int detid=369125870, bool print=false)
{
  //bool show=false;
  std::string run1="20180418_run314574";
  std::string run2="20180419_run314755";
  double mean_ratio1, mean_ratio2;
  int nmodperchannel1, nmodperchannel2;
  TGraph* gvdrop1 = GetVoltageDropCurve(subdet, detid, run1, mean_ratio1, nmodperchannel1);
  TGraph* gvdrop2 = GetVoltageDropCurve(subdet, detid, run2, mean_ratio2, nmodperchannel2);
  
  if(!gvdrop1 || !gvdrop2)
  { cout<<"WARNING: missing infos for detid "<<detid<<endl; return;}
  
  TCanvas* c1 = new TCanvas();
  gvdrop2->GetHistogram()->SetMinimum(0);
  gvdrop2->SetMarkerColor(4);
  gvdrop2->Draw("APL");
  gvdrop1->Draw("PL");
  if(print) c1->Print(Form("Vdrop_-20-10C_%i.pdf", detid));
  
  return;
}



TGraph* CompareVoltageDropForDetID(std::string subdet="TIB", int detid=369125870, bool show=false )
{
  //bool show=false;
  std::string run1="20180418_run314574";
  std::string run2="20180419_run314755";
  double mean_ratio1, mean_ratio2;
  int nmodperchannel1, nmodperchannel2;
  TGraph* gvdrop1 = GetVoltageDropCurve(subdet, detid, run1, mean_ratio1, nmodperchannel1);
  TGraph* gvdrop2 = GetVoltageDropCurve(subdet, detid, run2, mean_ratio2, nmodperchannel2);
  TGraph* gvdropdiff = new TGraph();
  TGraph* gvdropratio = new TGraph();
  unsigned int ig=0;
  
  if(!gvdrop1 || !gvdrop2)
  { cout<<"WARNING: missing infos for detid "<<detid<<endl; return 0;}
  
  if(mean_ratio1==-1 && mean_ratio2==-1) {
    mean_ratio1=1./nmodperchannel1; mean_ratio2=mean_ratio1;}
  else {
    if(mean_ratio1==-1 && mean_ratio2!=-1) mean_ratio1=mean_ratio2;
    if(mean_ratio1!=-1 && mean_ratio2==-1) mean_ratio2=mean_ratio1;
    if(mean_ratio1!=-1 && mean_ratio2!=-1) mean_ratio2=mean_ratio1;
  }
  
  if(abs((mean_ratio2-mean_ratio1)/mean_ratio2)>0.01) 
  { cout<<"WARNING PS/DCU ratio differs between the 2 runs: "<<mean_ratio1<<" "<<mean_ratio2<<endl; }
  
  std::map< float, float > vdrop1;
  double x, y, errx, erry;
  double x1max=0;
  double x2max=0;
  for(unsigned int ipt=0; ipt<gvdrop1->GetN(); ipt++)
  {
    gvdrop1->GetPoint(ipt, x, y);
    errx = gvdrop1->GetErrorX(ipt);
	//cout<<errx<<" "<<x<<" "<<y<<endl;
    //if(errx==1)
	 vdrop1[x] = y;
	 if(x>x1max) {x1max=x; }
  }
  
  for(unsigned int ipt=0; ipt<gvdrop2->GetN(); ipt++)
  {
    gvdrop2->GetPoint(ipt, x, y);
    errx = gvdrop2->GetErrorX(ipt);
	if(x>x2max) x2max=x;
    //if(errx==1 &&)
	if( vdrop1.find(x)!=vdrop1.end()){ 
      gvdropdiff->SetPoint(ig, x, y - vdrop1[x]);
	  gvdropratio->SetPoint(ig++, x, y / vdrop1[x]);
	  //if(GetLayer(detid)==1) 
	  if(x==300) cout<<"Ratio at 300V: "<<detid<<" "<<y / vdrop1[x]<<endl;
	}
	// consider saturation is reached, that's why no more measurements in -20°C run
	else if(x>x1max){
	  //cout<<"Saturating curves at -20oC"<<endl;
	  gvdropdiff->SetPoint(ig, x, y - vdrop1[x1max]);
	  gvdropratio->SetPoint(ig++, x, y / vdrop1[x1max]);
	  if(x==300) cout<<"Ratio at 300V: "<<detid<<" "<<y / vdrop1[x1max]<<endl;
	}
  }
  
 
  
  cout<<"MAP "<<detid<<" "<<x1max<<endl;
  // In case last points are missing, do extrapolation
  if((x1max<300 || x2max<300) && (x1max>=250 && x2max>=250)) 
    //if(GetLayer(detid)==1) 
	cout<<"Ratio at 300V: "<<detid<<" "<<InterpolX(gvdropratio->GetX(), gvdropratio->GetY(), gvdropratio->GetN() , 300)<<endl;
  if(x1max<250 && x2max>=250)  
	cout<<"Ratio at 300V: "<<detid<<" "<<InterpolX(gvdropratio->GetX(), gvdropratio->GetY(), gvdropratio->GetN() , 300)<<endl;
  //if(x1max<250 || x2max<250) { cout<<"Warning: One correction does not reach 250V for detid: "<<detid<<" "<<x1max<<" "<<x2max<<endl;
  //return 0;}
  if(x2max<250) { cout<<"Warning: correction does not reach 250V for detid: "<<detid<<" "<<x1max<<" "<<x2max<<endl;
  return 0;}
  
  //gvdropdiff->Print("all");
  TCanvas* c1;
  TCanvas* c2;
  gvdropdiff->SetName(Form("Corr_%i", detid));
  if(show){
    c1 = new TCanvas();
	gvdropdiff->SetMarkerStyle(20);
	gvdropdiff->GetYaxis()->SetTitle("V_{drop} difference [V]");
	gvdropdiff->GetXaxis()->SetTitle("V_{bias} [V]");
	gvdropdiff->SetTitle(Form("DetID %i", detid));
    gvdropdiff->Draw("AP");
	c1->Print(Form("Vdrop_diff_-20-10C_%i.pdf", detid));
    c2 = new TCanvas();
	gvdropratio->SetMarkerStyle(20);
	gvdropratio->GetYaxis()->SetTitle("V_{drop} ratio [V]");
	gvdropratio->GetXaxis()->SetTitle("V_{bias} [V]");
	gvdropratio->SetTitle(Form("DetID %i", detid));
    gvdropratio->Draw("AP");
	c2->Print(Form("Vdrop_ratio_-20-10C_%i.pdf", detid));
  }
  
  return gvdropdiff;
}

void ComputeShiftForAll(std::string subdet="TIB")
{
  std::string run1="20180418_run314574";
  std::string run2="20180419_run314755";
  // load currents for all detids
  LoadConditionsPerRun(map_DCU_currents_perrun, map_PS_currents_perrun, map_NMOD_perrun, subdet, run1, "");
  LoadConditionsPerRun(map_DCU_currents_perrun, map_PS_currents_perrun, map_NMOD_perrun, subdet, run2, "");

  TFile* fout = new TFile(Form("LeakCurCorr_%s_TemperatureShift.root", subdet.c_str()),"recreate");

  // input file with list of detids
  std::string line;
  std::string filename="Data/"+subdet+"_detids_sorted.txt";
  ifstream fin(filename);
  int idet=0;
  int detid=0;
  bool show=false;
  TGraph* gvdropdiff;

  // Loop on detids
  if(fin.is_open())  {
    while( getline ( fin, line) && idet < 6500)//6500
    {
      if(fin.eof()) continue;
      std::stringstream ss(line);
      ss >> detid;
	  idet++;
	  
	  cout<<"Analyzing detid: "<<detid<<endl;

	  gvdropdiff = CompareVoltageDropForDetID(subdet, detid);
	  if(!gvdropdiff) continue;
	  cout<<"Storing detid: "<<detid<<endl;
      fout->cd();
	  gvdropdiff->Write();
  
    } // End of loop over modules
  }

}

void CompareVoltageDrop()
{
 //ShowVoltageDropForDetID("TIB", 369121609, true);
 //ShowVoltageDropForDetID("TIB", 369125862, true);
 //ShowVoltageDropForDetID("TIB", 369173572, true);
 //ShowVoltageDropForDetID("TIB", 369158324, true);
 //ShowVoltageDropForDetID("TOB", 436228278, true);
 //ShowVoltageDropForDetID("TOB", 436316492, true);
 CompareVoltageDropForDetID("TIB", 369121386, true);
 //CompareVoltageDropForDetID("TIB", 369125870, true);
 //CompareVoltageDropForDetID("TIB", 369121609, true);
 //CompareVoltageDropForDetID("TIB", 369125862, true);
 //CompareVoltageDropForDetID("TIB", 369173572, true);
 //CompareVoltageDropForDetID("TIB", 369158324, true);
 //CompareVoltageDropForDetID("TOB", 436228278, true);
 //CompareVoltageDropForDetID("TOB", 436316492, true);
 //ComputeShiftForAll("TIB");
}
