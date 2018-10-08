#include "ComputeILeakCorrections.C"
#include "../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.C"

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

double GetAvg(TGraph* g)
{
  double avg=0;
  double x, y;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	avg+=y;
  }
  avg/=g->GetN();
  
  return avg;
}

void DrawTemperature(std::string subdet="TIB", std::string run="20171030_run305862", int detid=369121381, 
                    std::string bad_periods=""){

  // Read files with voltage infos
  TGraph* gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}

  TGraph* gT_DCU = new TGraph();
  TGraph* gcur_DCU = ReadDCUCurrentFromGB(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root",
  run.c_str()), detid, bad_periods, false, gT_DCU);
  
  if(!gT_DCU->GetN()) {std::cout<<" No DCU points. Exit."<<std::endl; return;}

  double Steps_max = GetMaximum(gsteps);
  double DCU_max = GetMaximum(gcur_DCU);
  double scale_DCU = 0.9*Steps_max/DCU_max;
  Scale( gcur_DCU, scale_DCU);
  cout<<"DCU current scale: "<<scale_DCU<<endl;
  TH1F* hT = gT_DCU->GetHistogram();
  gT_DCU->Print();
  double Tymax = hT->GetMaximum();
  double Tymin = hT->GetMinimum();
  double T_max = GetMaximum(gT_DCU);
  double scale_T = 0.75*Steps_max/T_max;
  if(Tymin<0) scale_T = 0.75*Steps_max/(T_max-Tymin);
  if(Tymin<0) Scale( gT_DCU, scale_T, Tymin);
  else Scale( gT_DCU, scale_T);
  gT_DCU->SetMarkerStyle(20);
  gT_DCU->SetMarkerColor(kOrange+1);
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
  
  cout<<" "<<Steps_max<<" "<<T_max<<" "<<Tymin<<" "<<scale_T<<endl;
  cout<<Tymin<<" "<<Tymax<<" "<<scale_T<<" "<<h->GetMinimum()<<" "<<h->GetMaximum()<<endl;
  cout<<Tymin<<" "<<-1*h->GetMaximum()/scale_T<<endl;
  TGaxis *axis;
  if(Tymin>0) axis = new TGaxis(h->GetXaxis()->GetXmax(), h->GetMinimum(), h->GetXaxis()->GetXmax(),
  h->GetMaximum(), 0, h->GetMaximum()/scale_T, 510,"+L");
  else axis = new TGaxis(h->GetXaxis()->GetXmax(), h->GetMinimum(), h->GetXaxis()->GetXmax(),
  h->GetMaximum(), Tymin, Tymin+h->GetMaximum()/scale_T, 510,"+L");
  axis->SetLineColor(kOrange+1);
  axis->SetLabelColor(kOrange+1);
  axis->SetTitle("temperature [^{o}C]");
  axis->SetTextColor(kOrange+1);
  axis->Draw();
   
  // Draw ratio
  /*TCanvas* c2 = new TCanvas();
  TGraph* gratio = ComputeRatio(gcur_DCU, gT_DCU);
  gratio->SetMarkerStyle(20);
  gratio->Draw("APL");*/
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
void TemperatureStudy(){

  //DrawTemperature("TIB", "20180418_run314574", 369121381);
  //DrawTemperature("TIB", "20180801_run320674", 369121613);
  ShowTemperaturePerLayer("TIB", "20180923_run323370", "Steps/bad_periods_20180923_run323370.txt" );
}
