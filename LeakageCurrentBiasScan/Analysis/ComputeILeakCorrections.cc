#include "Code/Steps.h"
#include "Code/Voltage.h"
#include "Code/DCUCurrent.h"
#include "Code/PSCurrent.h"
#include "Code/FitFunctions.h"

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TLine.h"
#include<vector>
#include<sstream>
#include <iomanip>
#include "TString.h"

//Convert double into a TString
TString Convert_Number_To_TString(double number)
{
	stringstream ss;
	ss << std::setprecision(11)<< number;
	TString ts = ss.str();
	return ts;
}


double GetMaximum(TGraph* g)
{
  double max=-999;
  double x, y;
  for(unsigned int ipt=0; ipt<g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	if(y>max) max=y;
  }
  
  return max;
}

void Scale(TGraph *& g, double scale)
{
  double x, y;
  for(unsigned int ipt=0; ipt<g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	g->SetPoint(ipt, x, y*scale);
  }

  return;  
}


void GetConditions(TGraph *&gsteps, TGraph *&gcur_DCU, TGraph *&gcur_PS, TGraph *&gvolt, 
                   char* subdet, char* run, int detid=369121605, char* bad_periods="")
{
  // Read files with voltage infos
  //gsteps = ReadSteps(Form("../../SignalBiasScan/Analysis/VoltageSteps/Steps/Steps_%s.txt", run),false);
  gsteps = ReadSteps(Form("./Steps/Steps_%s.txt", run),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}
  gvolt = 0;//ReadVoltage(Form("Data/ConditionBrowser_%s.root", run));
  //if(!gvolt) std::cout<<" ConditionBrowser file does not exist, but it is not a problem."<<std::endl;
 
  
  int nmodforchannel=1;
  // Read files with current infos
  gcur_DCU = ReadDCUCurrentRoot(Form("Data/DCU_I_%s_%s.root", subdet, run), detid, bad_periods);
  gcur_PS = ReadPSCurrentRoot(Form("Data/PS_I_%s_%s.root", subdet, run), detid, nmodforchannel, bad_periods, false); // last argument for prints
  
  
  if(!gcur_PS) {std::cout<<" No PS current info. Exit."<<std::endl; return;}
  //gcur_DCU = 0; // force to not use DCU
  
  // If no DCU info, will user only PS info
  float scale=1.;
  double time=0;
  double current=0;
  if(!gcur_DCU)
  { 
    std::cout<<" No DCU current info, will try to use PS current instead."<<std::endl;  
    gcur_DCU = (TGraph*) gcur_PS->Clone();
    if(!gcur_DCU) {std::cout<<" No PS info neither. Exit."<<std::endl; return;}
    
    scale = 1./nmodforchannel; // Approximation of an equal sharing among sensors and over Vbias

    // fractions computed on 20120506_run193541
    const int N_TIB_L1_2012=17;
    int detids_TIB_L1_2012[N_TIB_L1_2012]={369121381,369121382,369121385,369121386,369121389,369121390,
	                         369121605,369121606,369121609,369121610,369121613,369121614,
							 369125861,369125862,369125866,369125869,369125870};  
    float dcu_fractions_TIB_L1_2012[N_TIB_L1_2012]={0.346, 0.329, 0.324, 0.339, 0.345, 0.329,
                                      0.345, 0.339, 0.339, 0.312, 0.320, 0.345,
								      0.400, 0.307, 0.217, 0.627, 0.476};
									  
    // fractions computed on 20120928_run203832
    const int N_TOB=6;
    int detids_TOB[N_TOB]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528};
    float dcu_fractions_TOB[N_TOB]={0.296, 0.320, 0.327, 0.336, 0.365, 0.334};
   

    
	if(!strcmp(subdet,"TIB"))
	for(int idet=0; idet<N_TIB_L1_2012; idet++)
	  if(detid==detids_TIB_L1_2012[idet]) scale = dcu_fractions_TIB_L1_2012[idet];
	if(!strcmp(subdet,"TOB"))
	for(int idet=0; idet<N_TOB; idet++)
	  if(detid==detids_TOB[idet]) scale = dcu_fractions_TOB[idet];

	cout<<"dcu/ps fraction = "<<scale<<endl;
	
	//cout<<"gcur_DCU->GetN() = "<<gcur_DCU->GetN()<<endl;

    for(int ic=0; ic<gcur_DCU->GetN(); ic++)
    {
      gcur_DCU->GetPoint(ic, time, current);
      gcur_DCU->SetPoint(ic, time, current*scale);
    }
  }
  
}

TGraph* GetVoltageDrop(TGraph* gsteps, TGraph* gcur_DCU, TGraph* gcur_PS, bool current_ratio=false)
{
  
  // if current_ratio is true, return ratio of leakage current DCU / PS
  
  TGraphErrors* gshift = new TGraphErrors();
  TGraphErrors* gratio = new TGraphErrors();
  
  double time=0;
  double current=0;
  double voltage=0;
  double previous_voltage=300;
  int time_cur=0;
  int time_volt=0;
  int prev_time_volt=0;
  
  double current_PS=0;
  double previous_current_PS=0;
  int time_cur_PS=0;
  int prev_time_cur_PS=0;
  
  double cur_PS=0;
  double cur_PS_err=0;
  double volt=0;
  double volt_err=0;
  double shift=0;
  double shift_err=0;
  
  if(gcur_DCU->GetN() == 0) cout<<"gcur_DCU->GetN() = "<<gcur_DCU->GetN()<<endl;
  
  
  // Loop on DCU measurements
  int ipt=0;
  for(int ic=0; ic<gcur_DCU->GetN(); ic++)
  {
    gcur_DCU->GetPoint(ic, time, current);
    time_cur=int(time);
    if(time_cur==0) continue;
    // cout<<time_cur<<" "<<current<<"uA"<<endl;
    //cout<<__LINE__<<endl;
    
    // Look for current in PS at time_cur
    for(int icps=0; icps<gcur_PS->GetN(); icps++)
    {
      gcur_PS->GetPoint(icps, time, current_PS);
      time_cur_PS=int(time);
      
      // Get PS current
      if(time_cur>=prev_time_cur_PS && time_cur<time_cur_PS)
      {
        // cout<<prev_time_cur_PS<<" "<<previous_current_PS<<"uA - "<<time_cur_PS<<" "<<current_PS<<"uA"<<endl;
        cur_PS=(previous_current_PS+current_PS)/2.;
        if(!prev_time_cur_PS) cur_PS=0; // time_cur before first PS meas. Set to 0 to not use it.
        cur_PS_err=fabs(current_PS-cur_PS);
        if(cur_PS_err<2) cur_PS_err=2;
      }
      
      previous_current_PS=current_PS;
      prev_time_cur_PS=time_cur_PS;
    }
    
	//Remove strange values of DCU in TOB
	if(current>cur_PS*1.5) continue;
    
    
    // Look for voltage step corresponding at time_cur
    previous_voltage=300;
    prev_time_volt=0;
    for(int iv=0; iv<gsteps->GetN(); iv++)
    {
      gsteps->GetPoint(iv, time, voltage);
      time_volt=int(time);
      // cout<<"  "<<time_volt<<" "<<voltage<<"V"<<endl;
      
      // Compute voltage shift
      if(time_cur>=prev_time_volt && time_cur<time_volt)
      {
        //cout<<prev_time_volt<<" "<<previous_voltage<<"V - "<<time_volt<<" "<<voltage<<"V"<<endl;
        volt=(previous_voltage+voltage)/2.;
        volt_err=fabs(voltage-volt);
        if(volt_err<1) volt_err=1;

        /*if(cur_PS_err<2.5 && fabs(current/cur_PS-0.333)>0.05) 
        {
          std::cout<<" Currents : DCU "<<current<<" PS "<<cur_PS<<"  detid "<<detid<<" "<<voltage<<"V"<<std::endl;  
        }*/
		
        if(cur_PS && prev_time_volt!=0) //FIXME
                
        // remove points not in a voltage step
		if(volt_err<=10) //1
		{
          shift= current*13.8e-3+cur_PS*1.e-3;
          shift_err= 2*13.8e-3+cur_PS_err*1.e-3;
          gshift->SetPoint(ipt, volt, shift);
          gshift->SetPointError(ipt, volt_err, shift_err);
		  gratio->SetPoint(ipt, volt, current/cur_PS);
		  gratio->SetPointError(ipt, volt_err, 
		    current/cur_PS*sqrt(pow(cur_PS_err/cur_PS,2) + pow(2/current,2)));
          ipt++;
		}
      }
      
      previous_voltage=voltage;
      prev_time_volt=time_volt;
    }
  } // End of loop on DCU measurements
  
  TH1F* h = gshift->GetHistogram();
  h->GetXaxis()->SetTitle("V_{bias} [V]");
  h->GetYaxis()->SetTitle("V_{drop} [V]");
  
  if(current_ratio) return gratio;
  else return gshift;
  
}


//----------------------------------------------------------------------------

void DrawConditions(char* subdet, char* run, int detid=369121381, 
                    char* bad_periods="Steps/bad_periods_20120928_run203832.txt")
{
    cout<<" DetID "<<detid<<endl;

    // Get currents and voltage
    TGraph* gsteps;
    TGraph* gcur_DCU;
    TGraph* gcur_PS;
    TGraph* gvolt;
    GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, subdet, run, detid, bad_periods);
	
	double Steps_max = GetMaximum(gsteps);
	double PS_max = GetMaximum(gcur_PS);
	double scale_PS = 0.9*Steps_max/PS_max;
	Scale( gcur_PS, scale_PS);
	double DCU_max = GetMaximum(gcur_DCU);
	double scale_DCU = 0.45*Steps_max/DCU_max;
	Scale( gcur_DCU, scale_DCU);
    
    // Draw conditions for monitoring
    TCanvas* c1 = new TCanvas();
    gsteps->Draw("APL");
    if(gvolt) gvolt->Draw("P");
    gcur_DCU->Draw("P");
    gcur_PS->Draw("PL");
    TH1F* h = gsteps->GetHistogram();
    h->SetTitle(Form("DetID %i", detid));
    h->GetYaxis()->SetTitle("[V or #muA]");
    h->GetXaxis()->SetTitle("time");
	
	TLegend *leg = new TLegend(0.70, 0.80, 0.98, 0.98);
	leg->AddEntry(gsteps, "Voltage", "l");
	leg->AddEntry(gcur_PS, Form("Leak. cur. from PS (x%.2f)", scale_PS), "pl");
	leg->AddEntry(gcur_DCU, Form("Leak. cur. from DCU (x%.2f)", scale_DCU), "p");
	leg->Draw();
   
   double x, y;
   for(int ipt=0; ipt<gsteps->GetN(); ipt++)
   {
     gsteps->GetPoint(ipt, x, y);
     TLine *l = new TLine(x, 0, x, 360);
     l->SetLineStyle(3);
     //l->Draw();
   } 
}

double DrawDCUOverPSRatio(char* subdet, char* run, int detid=369121381, char* bad_periods="", bool print=false)
{
    cout<<" DetID "<<detid<<endl;

    // Get currents and voltage
    TGraph* gsteps;
    TGraph* gcur_DCU;
    TGraph* gcur_PS;
    TGraph* gvolt;
    GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, subdet, run, detid, bad_periods);
	
    TGraph* gratio = GetVoltageDrop(gsteps, gcur_DCU, gcur_PS, true);
	
	double mean_ratio = 0.;
	gratio->Fit("pol0");
	mean_ratio = gratio->GetFunction("pol0")->GetParameter(0);

    // Draw conditions for monitoring
    TCanvas* c1 = new TCanvas();
	TH1F* h = gratio->GetHistogram();
    h->SetTitle(Form("DetID %i", detid));
	h->GetYaxis()->SetTitle("I_{leak} ratio DCU/PS");
	h->GetYaxis()->SetTitleOffset(1.2);
	h->GetXaxis()->SetTitle("Voltage [V]");
	gratio->Draw("AP");
	if(print) c1->Print(Form("ILeakDCUOverPS_detid_%i.eps", detid));
	c1->Modified();
	c1->Update();

	
	return mean_ratio;
}

//----------------------------------------------------------------------------

Double_t fitvdrop(Double_t *x, Double_t *par)
{
  Double_t value, value_th, value_0;
  if (x[0]<par[2])
  {
  	value = par[0] + par[1]*sqrt(x[0]) + par[3]*x[0]*sqrt(x[0]); // x^1/2 + x^3/2
  }
  else
  {
  	value_th = par[0] + par[1]*sqrt(par[2]) + par[3]*par[2]*sqrt(par[2]);
  	value_0 = value_th - par[4]*par[2];
  	value = value_0 + par[4]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  return value;
}


int ComputeCorrection(char* subdet, char* run, double detid, TGraph*& gvdrop, TF1*& fit, char* bad_periods="", bool show=true)
{
  
  // Loop over modules
  cout.precision(10);
  cout<<endl<<" DetID "<<detid<<endl;
  

  // Get currents and voltage
  TGraph* gsteps;
  TGraph* gcur_DCU;
  TGraph* gcur_PS;
  TGraph* gvolt;
  GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, subdet, run, detid, bad_periods);


  // scale graphs for drawing
  double Steps_max = GetMaximum(gsteps);
  double PS_max = GetMaximum(gcur_PS);
  double scale_PS = 0.9*Steps_max/PS_max;
  TGraph* gcur_PS_clone = (TGraph*) gcur_PS->Clone();
  Scale( gcur_PS_clone, scale_PS);
  double DCU_max = GetMaximum(gcur_DCU);
  double scale_DCU = 0.45*Steps_max/DCU_max;
  TGraph* gcur_DCU_clone = (TGraph*) gcur_DCU->Clone();
  Scale( gcur_DCU_clone, scale_DCU);
  

  // Draw conditions for monitoring
  TCanvas* c1;
  if(show) 
  {
    c1 = new TCanvas();
    gsteps->Draw("APL");
    if(gvolt) gvolt->Draw("P");
    gcur_DCU_clone->Draw("P");
    gcur_PS_clone->Draw("PL");
    TH1F* h = gsteps->GetHistogram();
    h->SetTitle(Form("DetID %i", detid));
    h->GetYaxis()->SetTitle("[V or #muA]");
    h->GetXaxis()->SetTitle("time");

    TLegend *leg = new TLegend(0.70, 0.80, 0.98, 0.98);
    leg->AddEntry(gsteps, "Voltage", "l");
    leg->AddEntry(gcur_PS_clone, Form("Leak. cur. from PS (x%.2f)", scale_PS), "pl");
    leg->AddEntry(gcur_DCU_clone, Form("Leak. cur. from DCU (x%.2f)", scale_DCU), "p");
    leg->Draw();
  } 
  
  // Compute voltage drop induced by leakage current
  TCanvas* c2;
  if(show) c2 = new TCanvas("c2", "V drop", 200, 0, 700, 500);
  
  
  gvdrop = GetVoltageDrop(gsteps, gcur_DCU, gcur_PS);
  gvdrop->SetTitle(Form("DetID %i", detid));
  gvdrop->SetMarkerStyle(20);
  if(show) gvdrop->Draw("AP");
  

  int fit_status=-999;
  // Fit voltage drop
  // sqrt(x) function works for spring 2012 runs for modules with high Vdepl (almost no 'plateau')
  // used also for early 2015 runs with reduced leakage current and all Run1 when fitting the curves for all the modules (more robust)
  TF1* fvdrop = new TF1("fvdrop", " [0]*([1]+(1+[2]*x)*sqrt(x)) ", 20, 360);
  fvdrop->SetParameter(0,0.2);
  fvdrop->SetParLimits(0, -5, 10);  
  fvdrop->SetParLimits(1, -10, 100);
  fvdrop->SetParameter(1,-1);  
  
  if(gvdrop->GetN() == 0) cout<<"Void TGraph : N points = "<<gvdrop->GetN()<<endl;
  
  fit_status = gvdrop->Fit("fvdrop");
  // For 2015 bad fits
  if(fvdrop->GetParameter(1)<-9.9){
    fvdrop->SetParameter(0,0.);
    fvdrop->SetParameter(1,50);
    fvdrop->SetParameter(3,0.);
    fit_status = gvdrop->Fit("fvdrop");
  }
  

  // function with curve in 2 parts : x^1/2 + x^3/2 and pol1 for 'plateau'
  // 5 parameters, so need more points
/*  TF1* fvdrop = new TF1("fvdrop", fitfunction, 20, 360, 5);
  fvdrop->SetParameter(2, 150);
  fvdrop->SetParLimits(1, 0.001, 10);
  fvdrop->SetParameter(3, 0.01);
  fit_status = gvdrop->Fit("fvdrop");
/*
  // function with curve in 2 parts : sigmoid and pol1
  // fit needs a lot of points to work
  /*TF1* fvdrop = new TF1("fvdrop", fitfunction2, 20, 360, 5);
  fvdrop->SetParameter(0, 3.);
  fvdrop->SetParameter(2, 150);
  fvdrop->SetParameter(4, 0.03);
  fit_status = gvdrop->Fit("fvdrop");*/

  // For TOB
  
/*  TF1* fvdrop = new TF1("fvdrop", fitvdrop , 30, 360, 5);
  fvdrop->SetParameter(0, -0.2);
  fvdrop->SetParLimits(0, -10, 0);
  fvdrop->SetParameter(1,-1);
  fvdrop->SetParLimits(1, 0, 100);
  fvdrop->SetParameter(2, 200);
  //fvdrop->SetParLimits(2, 175, 225);
  //fvdrop->SetParLimits(3, -1, 10);  
  fit_status = gvdrop->Fit("fvdrop", "R");
  cout << "Kink_fit= " << fvdrop->GetParameter(2) << endl;
  cout << "Chi_Square_fit = " << fvdrop->GetChisquare()/fvdrop->GetNDF() << endl;
*/  
  
  if(show) 
  {
    c1->Modified();
    c1->Update();
    //c1->Print(Form("Ileak-Vbias_%s_%i.pdf", run, detid));
    c2->Modified();
    c2->Update();
    //c2->Print(Form("IleakEffect_%s_%i.pdf", run, detid));
  }
  
  
  fit = (TF1*) gvdrop->GetListOfFunctions()->First();
  
  return fit_status;
  
}

void ComputeCorrections(char* subdet, char* run, vector<double> detids, char* bad_periods="", bool show=true)
{
  int N = detids.size();
  
  // Histos and output file
  TFile* fout = new TFile(Form("LeakCurCorr_%s_%s.root", subdet, run),"recreate");
  //TFile* fout = new TFile(Form("LeakCurCorr_files/LeakCurCorr_%s_%s.root", subdet, run),"recreate");
  TH1F* hchi2 = new TH1F("hchi2", "Chi2/NDF", 100, 0, 50);
  TGraph* g2param = new TGraph();
  TH1F* hparam0 = new TH1F("hparam0", "param0", 100, -1, 1);
  TH1F* hparam1 = new TH1F("hparam1", "param1", 500, -5, 5);
  TH1F* hparam2 = new TH1F("hparam2", "param2", 100, -10, 10);

  // Loop over modules
  double detid=0;
  int ifit=0;
  TGraph *gvdrop;
  TF1 *fit;
  for(int idet=0; idet<N; idet++)
  {

    detid = detids[idet];
	ComputeCorrection(subdet, run, detid, gvdrop, fit, bad_periods, show);
    
    // Store fit result
    if(fit)
    if(fit->GetNDF()>1)
    if(fit->GetChisquare()/fit->GetNDF() < 50.)
    {
      fout->cd();
      cout<<"Storing fit for detid "<<detid<<endl;
      fit->SetName( ("fit_" + Convert_Number_To_TString(detid)).Data() );
      fit->Write();
      gvdrop->SetName( ("vdrop_" + Convert_Number_To_TString(detid)).Data() );
      gvdrop->Write();
      if(fit->GetNDF()) hchi2->Fill(fit->GetChisquare()/fit->GetNDF());
      g2param->SetPoint(ifit, fit->GetParameter(0), fit->GetParameter(1));
      hparam0->Fill(fit->GetParameter(0));
      hparam1->Fill(fit->GetParameter(1));
      hparam2->Fill(fit->GetParameter(2));
      ifit++;
    }
    
    getchar();
    
//    delete c1;
//    delete c2;
  
  } // End of loop over modules
  

  // Write output root file
  cout<<"Chi2 mean : "<<hchi2->GetMean()<<endl;
  cout<<"Correl param : "<<g2param->GetCorrelationFactor()<<endl;
  cout<<"param0 : "<<hparam0->GetMean()<<" +/- "<<hparam0->GetRMS()<<endl;
  cout<<"param1 : "<<hparam1->GetMean()<<" +/- "<<hparam1->GetRMS()<<endl;
  cout<<"param2 : "<<hparam2->GetMean()<<" +/- "<<hparam2->GetRMS()<<endl;
  fout->cd();
  hchi2->Write();
  g2param->SetName("g2param");
  g2param->Write();
  hparam0->Write();
  hparam1->Write();
  hparam2->Write();
  fout->Close();
  
}

void ComputeAllCorrections(char* subdet, char* run, bool fullscan, char* bad_periods="")
{
  std::string string_subdet(subdet);

  TString filename = "Data/detid_lists/"+string_subdet+"_detid_list";
  if(fullscan) filename+= "_full";
  filename+= ".txt";
  
  // Histos and output file
  TFile* fout = new TFile(Form("LeakCurCorr_%s_%s.root", subdet, run),"recreate");
  TH1F* hchi2 = new TH1F("hchi2", "Chi2/NDF", 100, 0, 50);
  TGraph* g2param = new TGraph();
  TH1F* hparam0 = new TH1F("hparam0", "param0", 100, -1, 1);
  TH1F* hparam1 = new TH1F("hparam1", "param1", 500, -5, 5);
  TH1F* hparam2 = new TH1F("hparam2", "param2", 100, -10, 10);

  int idet=0;
  int ifit=0;
  int status=0;
  int ngoodfit=0;
  int nbadfit=0;
  int nnotconv=0;
  TGraph *gvdrop;
  TF1 *fit;

  // input file with list of detids
  std::string line;
  ifstream fin(filename.Data() );
  double detid=0; //can't use int, since INT_MAX=2147483647, not enough
  bool show=false;

  // storing graphs for bad fits (limit on chi2/ndf)
  float chi2_limit = 3.; //2015: 3.

  // Loop on detids
  if(fin.is_open())  
  {
    while( getline ( fin, line) && idet < 6000)
    {
      if(fin.eof()) continue;
      std::istringstream iss(line);
      
      iss >> detid;
            
	  idet++;
	  
	  //cout<<endl<<endl<<"DETID == "<<detid<<endl<<endl;

	  status = ComputeCorrection(subdet, run, detid, gvdrop, fit, bad_periods, show);

      // Store fit result
      if(fit)
      if(fit->GetNDF()>1)
      //if(fit->GetChisquare()/fit->GetNDF() < 50.)
      {
    	fout->cd();
    	cout<<"Storing fit for detid "<<detid<<endl;
    	fit->SetName( ("fit_" + Convert_Number_To_TString(detid)).Data() );
    	fit->Write();
		cerr<<std::setprecision(10)<< detid<<" "<<fit->Eval(300)<<endl;
    	gvdrop->SetName( ("vdrop_" + Convert_Number_To_TString(detid)).Data() );
    	//gvdrop->Write();
    	if(fit->GetChisquare()/fit->GetNDF() > chi2_limit) gvdrop->Write();
        if(fit->GetChisquare()/fit->GetNDF() > chi2_limit) nbadfit++;
        else ngoodfit++;
        if(status==4) nnotconv++;
    	if(fit->GetNDF()) hchi2->Fill(fit->GetChisquare()/fit->GetNDF());
    	g2param->SetPoint(ifit, fit->GetParameter(0), fit->GetParameter(1));
    	hparam0->Fill(fit->GetParameter(0));
    	hparam1->Fill(fit->GetParameter(1));
    	hparam2->Fill(fit->GetParameter(2));
    	ifit++;
      }

      if(show) getchar();


    }
    fin.close();
  }
  else {cout<<"File "<<filename<<" not found."<<endl;}


  // Write output root file
  cerr<<"Chi2 mean : "<<hchi2->GetMean()<<endl;
  cerr<<"Correl param : "<<g2param->GetCorrelationFactor()<<endl;
  cerr<<"param0 : "<<hparam0->GetMean()<<" +/- "<<hparam0->GetRMS()<<endl;
  cerr<<"param1 : "<<hparam1->GetMean()<<" +/- "<<hparam1->GetRMS()<<endl;
  cerr<<"param2 : "<<hparam2->GetMean()<<" +/- "<<hparam2->GetRMS()<<endl;
  fout->cd();
  hchi2->Write();
  g2param->SetName("g2param");
  g2param->Write();
  hparam0->Write();
  hparam1->Write();
  hparam2->Write();
  fout->Close();
  
  cerr<<"N good fits: "<<ngoodfit<<endl;
  cerr<<"N bad fits: "<<nbadfit<<" (chi2/ndf > "<<chi2_limit<<", not bad enough that the fit is rejected (>50.))"<<endl;
  cerr<<"N fits w/o convergence: "<<nnotconv<<endl;

}

//------------------------------------------------------------------------

void ComputeDCUFractions(vector<double> detids, char* subdet="TIB", char* run="20120506_run193541", char* bad_periods="")
{
  int N = detids.size();
  
  // Loop over modules
  int detid=0;
  int ifit=0;
  for(int idet=0; idet<N; idet++)
  {
    detid = detids[idet];
    cout<<" DetID "<<detid<<endl;
	
    DrawDCUOverPSRatio(subdet, run, detid, bad_periods);
	getchar();
  }
  
}


int main()
{

//NB : detids lists in Data/detid_lists/
//NB : leakage info files must be stored in Data/

	bool fullscan = true; //If small scan, compute corrections only for dew detids !

//--- FUNCTION CALLS
	//ComputeAllCorrections("TIB", "20120921_run203243", fullscan);
	ComputeAllCorrections("TOB", "20120921_run203243", fullscan);
	//ComputeAllCorrections("TEC", "20170919_run303272", fullscan);


	return 0;
}
