#include "ComputeILeakCorrections.C"
#include "../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.C"
#include "../../SignalBiasScan/Analysis/CommonTools/ModFluence.h"
#include "TProfile.h"

map< int, TGraph*> map_DCU_T;

TGraph* ComputeRatio(TGraph* g1, TGraph* g2){

  TGraph* gratio = new TGraph();
  double x1, y1, x2, y2;
  for(unsigned int ipt=0; ipt<(unsigned) g1->GetN(); ipt++)
  {
    g1->GetPoint(ipt, x1, y1);
	if(ipt>=g2->GetN()) break;
    g2->GetPoint(ipt, x2, y2);
	if(x1==x2) gratio->SetPoint(ipt, x1, y1/y2);
  }

  return gratio;  
}

void Scale(TGraph *& g, double scale, double min)
{
  double x, y;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	g->SetPoint(ipt, x, (y-min)*scale);
  }

  return;  
}

void Scale(TGraph *& g, double scale, double min, double min_orig)
{
  double x, y;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	g->SetPoint(ipt, x, (y-min)*scale+min_orig);
  }

  return;  
}

double GetAvg(TGraph* g, int timestamp_min=0, int timestamp_max=0)
{
  double avg=0;
  double x, y;
  int npt=0;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	// keep only points at low or high voltage
	if(timestamp_min && timestamp_max){
	 if(x>timestamp_min and x<timestamp_max) {avg+=y; npt++;}
	}
	else {avg+=y; npt++;}
  }
  if(npt) avg/=npt;
  
  if(!npt) return -99;
  else return avg;
}

void DrawTemperature(std::string subdet="TIB", std::string run="20171030_run305862", int detid=369121381, 
                    std::string bad_periods="", bool print=false){

  // Read files with voltage infos
  TGraph* gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}

  TGraph* gT_DCU = new TGraph();
  TGraph* gcur_DCU = ReadDCUCurrentFromGB(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root",
  run.c_str()), detid, bad_periods, false, gT_DCU);
  
  if(!gT_DCU->GetN()) {std::cout<<" No DCU points. Exit."<<std::endl; return;}

  
  double x, y;
  for(int i=0; i< gsteps->GetN(); i++){
    gsteps->GetPoint(i, x, y);
	cout<<(int) x<<" "<<y<<"V"<<endl;
  }
  for(int i=0; i< gT_DCU->GetN(); i++){
    gT_DCU->GetPoint(i, x, y);
	cout<<(int) x<<" "<<y<<"oC"<<endl;
  }
  cout<<"Avg: "<<GetAvg(gT_DCU)<<endl;
  cout<<"Avg: "<<GetAvg(gT_DCU, 1524010715, 1524013160 )<<endl;
  
  
  double Steps_max = GetMaximum(gsteps);
  double DCU_max = GetMaximum(gcur_DCU);
  double scale_DCU = 0.9*Steps_max/DCU_max;
  Scale( gcur_DCU, scale_DCU);
  cout<<"DCU current scale: "<<scale_DCU<<endl;
  TH1F* hT = gT_DCU->GetHistogram();
  //gT_DCU->Print();
  double Tymax = hT->GetMaximum();
  double Tymin = hT->GetMinimum();
  double T_max = GetMaximum(gT_DCU);
  //double scale_T = 0.75*Steps_max/T_max;
  //if(Tymin<0)
  double scale_T = 0.75*Steps_max/(T_max-Tymin);
  //if(Tymin<0) 
   Scale( gT_DCU, scale_T, Tymin);
  //else Scale( gT_DCU, scale_T);
  gT_DCU->SetMarkerStyle(20);
  gT_DCU->SetMarkerColor(kOrange-3);
  //gT_DCU->Print();

  TH1F* h = gsteps->GetHistogram();
  h->SetTitle(Form("DetID %i", detid));
  h->GetYaxis()->SetTitle("[V or #muA]");
  h->GetXaxis()->SetTitle("time");

 // Draw conditions for monitoring
  TCanvas* c1 = new TCanvas();
  gsteps->Draw("APL");
  gcur_DCU->Draw("P");
  gT_DCU->Draw("P");
  
  //double x, y;
  gsteps->GetPoint(0, x, y);
  TLatex *t1 = new TLatex(x, Steps_max, Form("I_{leak} x%.2f", scale_DCU));
  t1->SetTextColor(4);
  t1->SetTextSize(0.04);
  t1->Draw("same");
 
  
  //cout<<" "<<Steps_max<<" "<<T_max<<" "<<Tymin<<" "<<scale_T<<endl;
  //cout<<Tymin<<" "<<Tymax<<" "<<scale_T<<" "<<h->GetMinimum()<<" "<<h->GetMaximum()<<endl;
  //cout<<Tymin<<" "<<Tymin+h->GetMaximum()/scale_T<<endl;
  TGaxis *axis;
  //if(Tymin>0) axis = new TGaxis(h->GetXaxis()->GetXmax(), h->GetMinimum(), h->GetXaxis()->GetXmax(),
  //h->GetMaximum(), 0, h->GetMaximum()/scale_T, 510,"+L");
  //else
   axis = new TGaxis(h->GetXaxis()->GetXmax(), h->GetMinimum(), h->GetXaxis()->GetXmax(),
  h->GetMaximum(), Tymin, Tymin+h->GetMaximum()/scale_T, 510,"+L");
  axis->SetLineColor(kOrange-3);
  axis->SetLabelColor(kOrange-3);
  axis->SetTitle("temperature [^{o}C]");
  axis->SetTextColor(kOrange-3);
  axis->Draw();
  
  if(print) c1->Print(Form("Temp_%s_%s_%i.pdf", subdet.c_str(), run.c_str(), detid));
   
  // Draw ratio
  /*TCanvas* c2 = new TCanvas();
  TGraph* gratio = ComputeRatio(gcur_DCU, gT_DCU);
  gratio->SetMarkerStyle(20);
  gratio->Draw("APL");*/
}

void DrawTemperature_SmallScans(string subdet="TIB", std::string run="20171030_run305862", std::string
bad_periods="", bool print=false){

  // 2012
  const int N_2012=17;
  int detids_2012[N_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
                           369121613,369121614,369125861,369125862,369125866,369125869,369125870};  
 
  if(subdet=="TIB")
    for( unsigned int idet=0; idet<N_2012; idet++)
	  DrawTemperature(subdet, run, detids_2012[idet], bad_periods, print);
 
}


void DrawTemperatureVsVoltage(std::string subdet="TIB", std::string run="20171030_run305862", int detid=369121381, 
                    std::string bad_periods="", bool print=false){

  // Read files with voltage infos
  TGraph* gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}

  TGraph* gT_DCU = new TGraph();
  TGraph* gcur_DCU = ReadDCUCurrentFromGB(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root",
  run.c_str()), detid, bad_periods, false, gT_DCU);
  
  if(!gT_DCU->GetN()) {std::cout<<" No DCU points. Exit."<<std::endl; return;}

  TGraphErrors *gT = new TGraphErrors();
  TGraphErrors *gcur = new TGraphErrors();

  // Loop on measurements
  int ipt=0;
  double time, temp, current, voltage;
  int time_cur=0;
  int time_volt=0;
  int prev_time_volt=0;
  double previous_voltage=300;
  double volt=0;
  double volt_err=0;
  for(int ic=0; ic<gT_DCU->GetN(); ic++)
  {
    gT_DCU->GetPoint(ic, time, temp);
    gcur_DCU->GetPoint(ic, time, current);
    time_cur=int(time);
    if(time_cur==0) continue;
	
    // Look for voltage step corresponding at time_cur
    previous_voltage=300;
    prev_time_volt=0;
    for(int iv=0; iv<gsteps->GetN(); iv++)
    {
      gsteps->GetPoint(iv, time, voltage);
      time_volt=int(time);
       //cout<<"  "<<time_volt<<" "<<voltage<<"V"<<endl;
      
      // Compute voltage shift and DCU/PS ratio
      if(time_cur>=prev_time_volt && time_cur<time_volt)
      {
        //cout<<prev_time_volt<<" "<<previous_voltage<<"V - "<<time_volt<<" "<<voltage<<"V"<<endl;
        volt=(previous_voltage+voltage)/2.;
        volt_err=fabs(voltage-volt);
        if(volt_err<1) volt_err=1;

        if(temp && prev_time_volt!=0)
        // remove points not in a voltage step
		if(volt_err<=15) //1
		{
          gT->SetPoint(ipt, volt, temp);
          gT->SetPointError(ipt, volt_err, 0.1);
          gcur->SetPoint(ipt, volt, current);
          gcur->SetPointError(ipt, volt_err, 1);
          ipt++;
          cout<<volt<<"V "<<current<<"uA"<<endl;
		}
      }
	
      previous_voltage=voltage;
      prev_time_volt=time_volt;
    }
  }
  
  // Draw conditions for monitoring
  TCanvas* c1 = new TCanvas();
  
  TH1F* hcur = gcur->GetHistogram();
  double DCUmax = hcur->GetMaximum();
  double DCUmin = hcur->GetMinimum();
  double DCU_max = GetMaximum((TGraph*) gcur);
  TH1F* hT = gT->GetHistogram();
  double Tymax = hT->GetMaximum();
  double Tymin = hT->GetMinimum();
  double T_max = GetMaximum(gT);
  gT->Print();
  // Consider minima different from 0
  double scale_T = 0.8*(DCUmax-DCUmin)/(T_max-Tymin);
  Scale( (TGraph *&) gT, scale_T, Tymin, DCUmin);
  gT->SetMarkerStyle(20);
  gT->SetMarkerColor(kOrange-3);


  TH1F* h = gcur->GetHistogram();
  h->SetTitle(Form("DetID %i", detid));
  h->GetXaxis()->SetTitle("V_{bias} [V]");
  h->GetYaxis()->SetTitle("Current [#muA]");
  gcur->SetMarkerStyle(22);
  gcur->SetMarkerColor(4);
  gcur->Draw("APL");
  gT->Draw("PL");
  gT->Print();

  cout<<" "<<DCU_max<<" "<<T_max<<" "<<Tymin<<" "<<scale_T<<endl;
  cout<<Tymin<<" "<<Tymax<<" "<<scale_T<<" "<<h->GetMinimum()<<" "<<h->GetMaximum()<<endl;
  cout<<Tymin<<" "<< Tymin+(DCUmax-DCUmin)/scale_T<<endl;
  TGaxis *axis;
   axis = new TGaxis(h->GetXaxis()->GetXmax(), h->GetMinimum(), h->GetXaxis()->GetXmax(),
  h->GetMaximum(), Tymin, Tymin+(DCUmax-DCUmin)/scale_T, 510,"+L");
  axis->SetLineColor(kOrange-3);
  axis->SetLabelColor(kOrange-3);
  axis->SetTitle("temperature [^{o}C]");
  axis->SetTextColor(kOrange-3);
  axis->Draw();

  if(print) c1->Print(Form("TempVsVoltage_%s_%s_%i.pdf", subdet.c_str(), run.c_str(), detid));
}


void DrawTemperatureVsVoltage_SmallScans(string subdet="TIB", std::string run="20171030_run305862", std::string
bad_periods="", bool print=false){

  // 2012
  const int N_2012=17;
  int detids_2012[N_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
                           369121613,369121614,369125861,369125862,369125866,369125869,369125870};  
 
  if(subdet=="TIB")
    for( unsigned int idet=0; idet<N_2012; idet++)
	  DrawTemperatureVsVoltage(subdet, run, detids_2012[idet], bad_periods, print);
 
}

TH1F* DrawLayerTemperature(std::string subdet="TIB", std::string run="20171030_run305862", int
layer=0, std::string bad_periods=""){

  TH1F* hT = new TH1F("hT", "average temperature", 50, -25, 25);
  
  // Read files with voltage infos
  TGraph* gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return 0;}
  
  // Store in memory DCU infos
  map_DCU_T.clear();
  ReadDCUCurrentFromDCUDataForAllDetids(map_DCU_T,
  Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root", run.c_str()),
  subdet.c_str(), bad_periods, false, true);

  // input file with list of detids
  std::string filename="Data/"+subdet+"_detids_sorted.txt";
  std::string line;
  ifstream fin(filename);
  int idet=0;
  int detid=0;
  bool show=false;
  
  // Loop on detids
  if(fin.is_open())  {
    while( getline ( fin, line) && idet < 6500)
    {
	  if(idet%100==0) cout<<idet<<endl;
      if(fin.eof()) continue;
      std::stringstream ss(line);
      ss >> detid;
	  idet++;
	  
	  // filtering on layer
	  if(layer>0 && GetLayer(detid)!=layer) continue;

      TGraph* gT_DCU;
      if(map_DCU_T.find(detid) != map_DCU_T.end())
        gT_DCU = map_DCU_T[detid];
      else
        gT_DCU = new TGraph(); // return an empty graph

      if(!gT_DCU->GetN()) {std::cout<<" No DCU points."<<std::endl; continue;}
	  
	  hT->Fill(GetAvg(gT_DCU));

    }
    fin.close();
  }
  else {cout<<"File "<<filename<<" not found."<<endl;}

  if(show){
    TCanvas* c1 = new TCanvas();
    hT->Draw();
    getchar();
  }
  return hT;
}

void ShowTemperaturePerLayer(std::string subdet="TIB", std::string run="20180923_run323370", std::string bad_periods=""){
  TCanvas* c1 = new TCanvas();
  std::vector< TH1F* > htemp;
  for(int i=1; i<5; i++){
    htemp.push_back(DrawLayerTemperature(subdet, run, i, bad_periods));
	htemp[i-1]->SetLineColor(i);
	if(i==1) htemp[0]->Draw();
	else htemp[i-1]->Draw("same");
  }
  for(int i=1; i<5; i++){
	cout<<"Layer "<<i<<": "<<htemp[i-1]->GetMean()<<endl;
  }
    
}


void PrintTemperatureVsPosition(std::string subdet="TIB", std::string run="20171030_run305862", int
layer=0, std::string bad_periods="", int timestamp_min=0, int timestamp_max=0){

  TProfile* hTvsZ = new TProfile("hTvsZ", "average temperature", 35, -70, 70);
  
  // Read files with voltage infos
  TGraph* gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return 0;}
  
  // Store in memory DCU infos
  map_DCU_T.clear();
  ReadDCUCurrentFromDCUDataForAllDetids(map_DCU_T,
  Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root", run.c_str()),
  subdet.c_str(), bad_periods, false, true);

  // input file with list of detids
  std::string filename="Data/"+subdet+"_detids_sorted.txt";
  std::string line;
  ifstream fin(filename);
  int idet=0;
  int detid=0;
  bool show=true;
  float temp_avg=0;
  
  ModFluence ModInfo;
  ModInfo.loadFile("../../SignalBiasScan/Analysis/CommonTools/modulesFluence_7000.root");
  
  // Loop on detids
  if(fin.is_open())  {
    while( getline ( fin, line) && idet < 6500)
    {
	  if(idet%100==0) cout<<idet<<endl;
      if(fin.eof()) continue;
      std::stringstream ss(line);
      ss >> detid;
	  idet++;
	  
	  // filtering on layer
	  if(layer>0 && GetLayer(detid)!=layer) continue;

      TGraph* gT_DCU;
      if(map_DCU_T.find(detid) != map_DCU_T.end())
        gT_DCU = map_DCU_T[detid];
      else
        gT_DCU = new TGraph(); // return an empty graph

      if(!gT_DCU->GetN()) {std::cout<<" No DCU points."<<std::endl; continue;}
	  
	  // keep only low voltage steps
	  if(timestamp_min && timestamp_max) temp_avg = GetAvg(gT_DCU, timestamp_min, timestamp_max);
	  else temp_avg = GetAvg(gT_DCU);
	  hTvsZ->Fill(ModInfo.GetZ(detid), temp_avg);
	  if(temp_avg!=-99) std::cerr<<detid<<" "<<temp_avg<<std::endl;

    }
    fin.close();
  }
  else {cout<<"File "<<filename<<" not found."<<endl;}

  if(show){
    TCanvas* c1 = new TCanvas();
	hTvsZ->GetXaxis()->SetTitle("Z");;
	hTvsZ->GetYaxis()->SetTitle("temperature");
    hTvsZ->Draw();
	c1->Print(Form("TempVsZ_%s_layer%i.pdf", subdet.c_str(), layer));
    getchar();
  }
  
  return;
}

void TemperatureStudy(){

  //sDrawTemperature("TIB", "20180418_run314574", 369121381, "", true);
  //DrawTemperature_SmallScans("TIB", "20180418_run314574", "", true);
  //DrawTemperatureVsVoltage("TIB", "20180418_run314574", 369125866, "", true);
  //DrawTemperatureVsVoltage("TIB", "20180418_run314574", 369121870, "", true);
  
  //DrawTemperatureVsVoltage("TIB", "20180418_run314574", 369125870, "", true);
  //DrawTemperatureVsVoltage("TIB", "20180611_run317683", 369121385, "", true);
  //DrawTemperatureVsVoltage("TIB", "20180923_run323370", 369121385, "", true);
  DrawTemperatureVsVoltage("TIB", "noise_20190321_run328691", 369121381, "", true);


  //DrawTemperatureVsVoltage_SmallScans("TIB", "20180418_run314574", "", true);
  //DrawTemperature("TIB", "20180801_run320674", 369121613);
  //DrawTemperature("TIB", "noise_20190321_run328691", 369121381);
  //ShowTemperaturePerLayer("TIB", "20180923_run323370", "Steps/bad_periods_20180923_run323370.txt" );
  //PrintTemperatureVsPosition("TIB", "20180418_run314574", 1, "", 1524010715, 1524013160); // 10-70V
  //PrintTemperatureVsPosition("TIB", "20180418_run314574", 0, "", 1524015416, 1524016664); // 200-300V
}
