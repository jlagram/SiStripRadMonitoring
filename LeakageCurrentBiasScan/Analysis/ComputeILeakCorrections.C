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
#include "TMath.h"

map< int, TGraph*> map_DCU_currents;
map< int, TGraph*> map_PS_currents;
map< int, int> map_NMOD;


double GetMaximum(TGraph* g)
{
  double max=-999;
  double x, y;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	if(y>max) max=y;
  }
  
  return max;
}

void Scale(TGraph *& g, double scale)
{
  double x, y;
  for(unsigned int ipt=0; ipt<(unsigned) g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x, y);
	g->SetPoint(ipt, x, y*scale);
  }

  return;  
}


void GetConditions(TGraph *&gsteps, TGraph *&gcur_DCU, TGraph *&gcur_PS, TGraph *&gvolt, int& nmodforchannel,
                   std::string subdet="TIB_L1", std::string run="20120506_run193541", int detid=369121605,
				   std::string bad_periods="")
{

  // Read files with voltage infos
  gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}
  gvolt = 0;//ReadVoltage(Form("Data/ConditionBrowser_%s.root", run));
  //if(!gvolt) std::cout<<" ConditionBrowser file does not exist, but it is not a problem."<<std::endl;
  
  nmodforchannel=1;
  // Read files with current infos
  //gcur_DCU = ReadDCUCurrentRoot(Form("Data/DCU_I_%s_%s.root", subdet, run), detid, bad_periods);
  //gcur_DCU = ReadDCUCurrentFromGB("~/work/DCU_TIBD_TOB_from_1348837200_to_1348862400.root", detid, bad_periods);
  gcur_DCU = ReadDCUCurrentFromGB(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root", run.c_str()), detid, bad_periods);
  //gcur_PS = ReadPSCurrentRoot(Form("Data/PS_I_%s_%s.root", subdet, run), detid, nmodforchannel, bad_periods, false); // last argument for prints
  gcur_PS = ReadPSCurrentRoot(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/PS/PS_I_%s_%s.root", subdet.c_str(), run.c_str()), detid, nmodforchannel, bad_periods, false); // last argument for prints
  
  return;
}

void ClearConditions()
{
  map_DCU_currents.clear();
  map_PS_currents.clear();
  map_NMOD.clear();
}

// load currents for all detids in memory
void LoadConditions(map< int, TGraph*> &map_dcu_currents, map< int, TGraph*> &map_ps_currents, map< int, int> &map_nmodforchannel, 
                    std::string subdet="TIB", std::string run="20120506_run193541", std::string bad_periods="")
{
  
  map_dcu_currents.clear();
  map_ps_currents.clear();
  map_nmodforchannel.clear();
  //ReadCurrentRootForAllDetids(Form("Data/PS_I_%s_%s.root", subdet, run), map_dcu_currents, map_nmodforchannel, "dcu", bad_periods);
  ReadDCUCurrentFromDCUDataForAllDetids(map_dcu_currents, Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_%s.root", run.c_str()), subdet.c_str(), bad_periods);
  //ReadCurrentRootForAllDetids(Form("Data/PS_I_%s_%s.root", subdet, run), map_ps_currents, map_nmodforchannel, "ps", bad_periods);
  ReadCurrentRootForAllDetids(Form("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/PS/PS_I_%s_%s.root", subdet.c_str(), run.c_str()), map_ps_currents, map_nmodforchannel, "ps", bad_periods);
}

void GetLoadedConditions(TGraph *&gsteps, TGraph *&gcur_DCU, TGraph *&gcur_PS, TGraph *&gvolt, int& nmodforchannel, 
                   std::string run="20120506_run193541", int detid=369121605)
{

  // Read files with voltage infos
  gsteps = ReadSteps(Form("Steps/Steps_%s.txt", run.c_str()),false);
  if(!gsteps) {std::cout<<" No voltage steps info. Exit."<<std::endl; return;}
  gvolt = 0;//ReadVoltage(Form("Data/ConditionBrowser_%s.root", run));
  //if(!gvolt) std::cout<<" ConditionBrowser file does not exist, but it is not a problem."<<std::endl;
  
  nmodforchannel=1;
  // Get current infos
  
  if(map_DCU_currents.find(detid) != map_DCU_currents.end())
    gcur_DCU = map_DCU_currents[detid];
  else
    gcur_DCU = new TGraph(); // return an empty graph
  
  if(map_PS_currents.find(detid) != map_PS_currents.end())
  {
    gcur_PS = map_PS_currents[detid];
    nmodforchannel = map_NMOD[detid];
  }
  else
    gcur_PS = new TGraph();
  
  //cout<<"DCU "<<gcur_DCU->GetN()<<endl;
  //cout<<"PS "<<gcur_PS->GetN()<<endl;
  return;
}



// Get voltage drop and DCU/PS ratio. Done in the same time because both need interpolation
void GetVoltageDropAndRatio(TGraph* gsteps, TGraph* gcur_DCU, TGraph* gcur_PS, TGraphErrors *&gvdrop, TGraphErrors *&gratio)
{
  gvdrop = new TGraphErrors();
  gratio = new TGraphErrors();
  
  if(!gsteps || !gcur_DCU || !gcur_PS) {std::cout<<" Missing input. Exiting GetVoltageDropAndRatio()."<<std::endl; return;}
  if(!gsteps->GetN() || !gcur_DCU->GetN() || !gcur_PS->GetN()) {std::cout<<" Empty input. Exiting GetVoltageDropAndRatio()."<<std::endl; return;}
  //cout<<"steps: "<<gsteps->GetN()<<" dcu: "<<gcur_DCU->GetN()<<" ps: "<<gcur_PS->GetN()<<endl;
  if(gcur_PS->GetN()<8) {std::cout<<" Too few PS currents values. Exiting GetVoltageDropAndRatio()."<<std::endl; return;}
  
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
  double min_err=2;
  double volt=0;
  double volt_err=0;
  double shift=0;
  double shift_err=0;
  
  
  // Loop on DCU measurements
  int ipt=0;
  for(int ic=0; ic<gcur_DCU->GetN(); ic++)
  {
    gcur_DCU->GetPoint(ic, time, current);
    time_cur=int(time);
    if(time_cur==0) continue;
    // cout<<time_cur<<" "<<current<<"uA"<<endl;
    
    // Look for current in PS at time_cur
    for(int icps=0; icps<gcur_PS->GetN(); icps++)
    {
      gcur_PS->GetPoint(icps, time, current_PS);
      time_cur_PS=int(time);
	   //cout<<"PS "<<time_cur_PS<<" "<<current_PS<<"uA"<<endl;
      
      // Get PS current
      if(time_cur>=prev_time_cur_PS && time_cur<time_cur_PS)
      {
         //cout<<prev_time_cur_PS<<" "<<previous_current_PS<<"uA - "<<time_cur_PS<<" "<<current_PS<<"uA"<<endl;
        cur_PS=(previous_current_PS+current_PS)/2.;
        if(!prev_time_cur_PS) cur_PS=0; // time_cur before first PS meas. Set to 0 to not use it.
        cur_PS_err=fabs(current_PS-cur_PS);
		min_err = sqrt(2*2+0.005*cur_PS*0.005*cur_PS); // added a prop. syst. otherwise too small error for large currents
        if(cur_PS_err<min_err) cur_PS_err=min_err;
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
       //cout<<"  "<<time_volt<<" "<<voltage<<"V"<<endl;
      
      // Compute voltage shift and DCU/PS ratio
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
		
        if(cur_PS && prev_time_volt!=0)
        // remove points not in a voltage step
		if(volt_err<=10) //1
		{
          shift= current*13.8e-3+cur_PS*1.e-3;
          shift_err= sqrt(2*2+0.005*cur_PS*0.005*cur_PS)*13.8e-3+cur_PS_err*1.e-3;
		  //cout<<"cur_PS "<<cur_PS<<" "<<" curPS_err "<<cur_PS_err<<" shift "<<shift<<" "<<shift_err<<endl;
          gvdrop->SetPoint(ipt, volt, shift);
          gvdrop->SetPointError(ipt, volt_err, shift_err);
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
  
  TH1F* h = gvdrop->GetHistogram();
  h->GetXaxis()->SetTitle("V_{bias} [V]");
  h->GetYaxis()->SetTitle("V_{drop} [V]");
  
  return;  
}


// Decide to use DCU currents or not. If not update with PS currents * scale factor
void ApplyDCUOverPSRatio(TGraph *gsteps, TGraph *&gcur_DCU, TGraph *gcur_PS, TGraph *gvolt, int nmodforchannel, double &mean_ratio,
                         int detid=369121605, bool force_use_DCU=false)
{

  TGraphErrors* gvdrop;
  TGraphErrors* gratio;
  /*cout<<gcur_DCU<<" "<<gcur_PS<<endl;
  if(!gcur_DCU) cout<<"No dcu"<<endl;
  else cout<<" n dcu "<<gcur_DCU->GetN()<<endl;
  if(!gcur_PS) cout<<"No ps"<<endl;
  else cout<<" n ps "<<gcur_PS->GetN()<<endl;*/
  
  bool use_DCU=true;
  mean_ratio = -1;
  double xfirst,xlast;
  
  if(!gcur_DCU && !gcur_PS) {std::cout<<" No DCU , neither PS currents graphs. Exiting."<<std::endl; return;}
  // In the following take care of cases with no DCU infos but not with no PS currents
  if(!gcur_PS) {std::cout<<" No PS currents graph. Exiting."<<std::endl; return;}

  // If no DCU info, will use only PS info
  if(!gcur_DCU)
  {
	use_DCU=false;
    std::cout<<" No DCU current info, will try to use PS current instead."<<std::endl;
  }
  else
  {
	if(gcur_DCU->GetN()==0)
	{
	  use_DCU=false;
      std::cout<<" No DCU current info, will try to use PS current instead."<<std::endl;
	}
	else
	if(!force_use_DCU)
	{
      // For checking DCU points within scan and range covered
      GetVoltageDropAndRatio(gsteps, gcur_DCU, gcur_PS, gvdrop, gratio);
      
	  if(gratio->GetN()){

		xfirst = TMath::MinElement(gratio->GetN(), gratio->GetX());
		xlast = TMath::MaxElement(gratio->GetN(), gratio->GetX());
		cout<<" nb of useful DCU values: "<<gratio->GetN()<<endl;
		cout<<" range of useful DCU values: "<<fabs(xlast-xfirst)<<endl;

		// if enough DCU values, will use them to compute Vdrop
		// check also how wide is the range covered
		if(gratio->GetN()>=8 && (xfirst<=75 && xlast>=275))
	      use_DCU=true;
		else
		// if too few points available for DCU values within the scan
		// uses PS values and DCU/PS ratio
		if(gratio->GetN()>=5 || (gratio->GetN()>=2 && fabs(xlast-xfirst)>=80))
		{
		  use_DCU=false;
		  gratio->Fit("pol0", "q");
		  mean_ratio = gratio->GetFunction("pol0")->GetParameter(0);
		}
		else
		// if even less DCU values within the scan uses PS values
		use_DCU=false;
	  
	  }
	  else use_DCU=false; 
	
	  // Old measurements
      /*use_DCU=false;
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
		if(detid==detids_TIB_L1_2012[idet]) mean_ratio = dcu_fractions_TIB_L1_2012[idet];

	  if(!strcmp(subdet,"TOB"))
	  for(int idet=0; idet<N_TOB; idet++)
		if(detid==detids_TOB[idet]) mean_ratio = dcu_fractions_TOB[idet];*/

  	}
}
  cout<<" fitted ratio: "<<mean_ratio<<endl;
  
  // If no DCU info, will use only PS info
  float scale=1.;
  double time=0;
  double current=0;
  if(!use_DCU)
  { 
	gcur_DCU = (TGraph*) gcur_PS->Clone();
    if(gcur_DCU->GetN()==0) {std::cout<<" No PS info. Exit."<<std::endl; return;}
    
    scale = 1./nmodforchannel; // Approximation of an equal sharing among sensors and over Vbias
	
	// DCU/PS ratio
	if(mean_ratio>0 && mean_ratio<1) scale=mean_ratio;
	
	// Old measurements
    /*// fractions computed on 20120506_run193541
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

    
	if(!strcmp(subdet,"TIB_L1"))
	for(int idet=0; idet<N_TIB_L1_2012; idet++)
	  if(detid==detids_TIB_L1_2012[idet]) scale = dcu_fractions_TIB_L1_2012[idet];
	if(!strcmp(subdet,"TOB"))
	for(int idet=0; idet<N_TOB; idet++)
	  if(detid==detids_TOB[idet]) scale = dcu_fractions_TOB[idet];
	*/

	cout<<" nb of modules: "<<nmodforchannel<<endl;
	cout<<" dcu/ps fraction: "<<scale<<endl;

    for(int ic=0; ic<gcur_DCU->GetN(); ic++)
    {
      gcur_DCU->GetPoint(ic, time, current);
      gcur_DCU->SetPoint(ic, time, current*scale);
    }
  }
  
}


//----------------------------------------------------------------------------

void DrawConditions(std::string subdet="TIB", std::string run="20171030_run305862", int detid=369121381, 
                    std::string bad_periods="", bool print=false)
{
    cout<<" DetID "<<detid<<endl;

    // Get currents and voltage
    TGraph* gsteps;
    TGraph* gcur_DCU;
    TGraph* gcur_PS;
    TGraph* gvolt;
	int nmodforchannel;
    GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, subdet, run, detid, bad_periods);
	
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
	cout<<"I_PS"<<endl;
	gcur_PS->Print("all");
    gcur_DCU->Draw("P");
	cout<<"I_DCU"<<endl;
	gcur_DCU->Print("all");
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
   
   if(print) c1->Print(Form("Conditions_detid_%i.eps", detid));
}

double DrawDCUOverPSRatio(std::string subdet="TIB", std::string run="20171030_run305862", int detid=369121381, std::string bad_periods="", bool print=false)
{
    cout<<" DetID "<<detid<<endl;

    // Get currents and voltage
    TGraph* gsteps;
    TGraph* gcur_DCU;
    TGraph* gcur_PS;
    TGraph* gvolt;
	int nmodforchannel;
    GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, subdet, run, detid, bad_periods);
		
    TGraphErrors* gvdrop;
	TGraphErrors* gratio;
	GetVoltageDropAndRatio(gsteps, gcur_DCU, gcur_PS, gvdrop, gratio);
	
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


int ComputeCorrection(std::string subdet, std::string run, int detid, TGraphErrors *&gvdrop, TF1 *&fit, std::string bad_periods="", bool show=true)
{
  
  // Loop over modules
  cout<<" DetID "<<detid<<endl;
  
  gvdrop = 0;
  fit = 0;

  // Get currents and voltage
  TGraph* gsteps = 0;
  TGraph* gcur_DCU = 0;
  TGraph* gcur_PS = 0;
  TGraph* gvolt = 0;
  int nmodforchannel;
  //cout<<"getting conditions"<<endl;
  if(map_PS_currents.size()>0)  GetLoadedConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, run, detid);
  else GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, subdet, run, detid, bad_periods);

  if(!gcur_PS || gcur_PS->GetN()==0) {std::cerr<<" No PS currents. Skipping detid."<<std::endl; return -11;}

  TGraph* gcur_DCU_untouched = 0;
  if(gcur_DCU) gcur_DCU_untouched = (TGraph*) gcur_DCU->Clone();
  
  double mean_ratio;
  //cout<<"applying DCU/PS"<<endl;
  ApplyDCUOverPSRatio(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, mean_ratio, detid);

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
  TCanvas* c1=0;
  if(show) 
  {
    c1 = new TCanvas("c1", "Conditions");
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
  
  // Show DCU/PS ratio
  TCanvas* c3=0;
  TGraphErrors* gratio_dcu_untouched;
  if(show && mean_ratio!=-1)
  {
    c3 = new TCanvas("c3", "DCU/PS ratio", 200, 0, 700, 500);
	GetVoltageDropAndRatio(gsteps, gcur_DCU_untouched, gcur_PS, gvdrop, gratio_dcu_untouched);
	gratio_dcu_untouched->Fit("pol0", "q");
	gratio_dcu_untouched->Draw("AP");
  }
   
  // Compute voltage drop induced by leakage current
  TCanvas* c2=0;
  if(show) c2 = new TCanvas("c2", "V drop", 400, 0, 700, 500);
  TGraphErrors* gratio;
  GetVoltageDropAndRatio(gsteps, gcur_DCU, gcur_PS, gvdrop, gratio);
  gvdrop->SetTitle(Form("DetID %i", detid));
  gvdrop->SetMarkerStyle(20);
  if(show) gvdrop->Draw("AP");
  
  

  int fit_status=-999;
  // Fit voltage drop
  // sqrt(x) function works for spring 2012 runs for modules with high Vdepl (almost no 'plateau')
  // used also for early 2015 runs with reduced leakage current and all Run1 when fitting the curves for all the modules (more robust)
  TF1* fvdrop = new TF1("fvdrop", " [0]*([1]+(1+[2]*x)*sqrt(x)) ", 10, 300); //20,360 // limit at 200V for 2018 -10° scan due to thermal runaway
  fvdrop->SetParameter(0,0.2);
  fvdrop->SetParLimits(0, -5, 10);  
  fvdrop->SetParLimits(1, -10, 100);
  fvdrop->SetParameter(1,-1);
  fit_status = gvdrop->Fit("fvdrop");//, "R");
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
  fit_status = gvdrop->Fit("fvdrop");*/

  // function with curve in 2 parts : sigmoid and pol1
  // fit needs a lot of points to work
/*  TF1* fvdrop = new TF1("fvdrop", fitfunction2, 20, 360, 5);
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
    if(c1) {c1->Modified(); c1->Update(); }
    //c1->Print(Form("Ileak-Vbias_%s_%i.pdf", run.c_str(), detid));
    if(c2) { c2->Modified(); c2->Update(); }
    //c2->Print(Form("IleakEffect_%s_%i.pdf", run.c_str(), detid));
    if(c3) { c3->Modified(); c3->Update(); }
  }
  
  fit = (TF1*) gvdrop->GetListOfFunctions()->First();
  if(fit) cout<<"Correction at 300V:  "<<fit->Eval(300)<<" V"<<endl;
  
  //delete c1;
  //delete c2;
  //delete c3;
  
  return fit_status;
  
}

void ComputeCorrections(std::string subdet, std::string run, int* detids, const int N, std::string bad_periods="")
{
  
  // Histos and output file
  TFile* fout = new TFile(Form("LeakCurCorr_%s_%s.root", subdet.c_str(), run.c_str()),"recreate");
  TH1F* hchi2 = new TH1F("hchi2", "Chi2/NDF", 100, 0, 50);
  TGraph* g2param = new TGraph();
  TH1F* hparam0 = new TH1F("hparam0", "param0", 100, -1, 1);
  TH1F* hparam1 = new TH1F("hparam1", "param1", 500, -5, 5);
  TH1F* hparam2 = new TH1F("hparam2", "param2", 100, -10, 10);

  // Loop over modules
  int detid=0;
  int ifit=0;
  int status=-999;
  TGraphErrors *gvdrop;
  TF1 *fit;
  double xfirst,xlast;
  for(int idet=0; idet<N; idet++)
  {

    detid = detids[idet];
	status = ComputeCorrection(subdet, run, detid, gvdrop, fit, bad_periods);
	
	if(status==-11) continue; // no PS currents
	
	if(gvdrop)
	if(gvdrop->GetN()) 
	{
	  xfirst = TMath::MinElement(gvdrop->GetN(), gvdrop->GetX());
	  xlast = TMath::MaxElement(gvdrop->GetN(), gvdrop->GetX());
	  //if(gvdrop->GetN()<8 || !(xfirst<=75 && xlast>=240)) 
	  if(gvdrop->GetN()<8 || !(xfirst<=45 && xlast>=190)) // for 2018 scans
	    {cerr<<"Too few points for computing correction for detid "<<detid<<endl; continue;}
	}
    
    // Store fit result
    if(fit)
    if(fit->GetNDF()>1)
    //if(fit->GetChisquare()/fit->GetNDF() < 50.)
    {
      fout->cd();
      cout<<"Storing fit for detid "<<detid<<endl;
      cout<<"Chi2/ndf: "<<fit->GetChisquare()/fit->GetNDF()<<endl;
      fit->SetName(Form("fit_%i", detid));
      fit->Write();
      gvdrop->SetName(Form("vdrop_%i", detid));
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

void ComputeDCUOverPSRatios(std::string subdet, std::string run, int* detids, const int N, std::string bad_periods="")
{
  
  // Histos and output file
  TFile* fout = new TFile(Form("DCUOverPSRatio_%s_%s.root", subdet.c_str(), run.c_str()),"recreate");
  //TH1F* hratio = new TH1F("hratio", "ratio", 100, 0.2, .7);
  //TH1F* hnpts = new TH1F("hnpts", "npts", 30, 0, 30);
  //TH1F* hrange = new TH1F("hrange", "range", 35, 0, 350);
  
  TTree* tout = new TTree("ratio","DCUOverPSRatio");
  ULong64_t detid;
  double nmod_avg_diff, fit_avg_diff, I_min, I_max, I_rms, I_ratio, V_min, V_max;
  int nmod, npts;
  tout->Branch("DETID",&detid,"DETID/l");
  tout->Branch("NMOD", &nmod, "NMOD/I");
  tout->Branch("NMOD_RATIO_AVG_DIFF", &nmod_avg_diff,"NMOD_RATIO_AVG_DIFF/D");
  tout->Branch("FIT_RATIO_AVG_DIFF", &fit_avg_diff, "FIT_RATIO_AVG_DIFF/D");
  tout->Branch("I_RATIO_MIN",&I_min,"I_RATIO_MIN/D");
  tout->Branch("I_RATIO_MAX",&I_max,"I_RATIO_MAX/D");
  tout->Branch("I_RATIO_RMS",&I_rms,"I_RATIO_RMS/D");
  tout->Branch("I_RATIO",&I_ratio,"I_RATIO/D");
  tout->Branch("V_MIN",&V_min,"V_MIN/D");
  tout->Branch("V_MAX",&V_max,"V_MAX/D");
  tout->Branch("NPTS",&npts,"NTPS/I");
  
  TGraph* gsteps;
  TGraph* gcur_DCU;
  TGraph* gcur_PS;
  TGraph* gvolt;
  TGraphErrors* gvdrop;
  TGraphErrors* gratio;

  // Loop over modules
  //int detid=0;
  int ifit=0;
  double mean_ratio = 0.;
  double x, y;
  for(int idet=0; idet<N; idet++)
  {

    detid = detids[idet];
    
    // Get currents and voltage
    if(map_PS_currents.size()>0)  GetLoadedConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmod, run, detid);
    else GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmod, subdet, run, detid, bad_periods);
	
    // to get explicitely gvdrop and gratio
	GetVoltageDropAndRatio(gsteps, gcur_DCU, gcur_PS, gvdrop, gratio);
	//cout<<" ratio: "<<gratio->GetN()<<endl;
	
	mean_ratio = 0;
	nmod_avg_diff = 0;
	fit_avg_diff = 0;
	I_min = -1;
	I_max = -1;
	I_rms = 0;
	V_min = -1;
	V_max = -1;
	
	if(gratio->GetN()>0){

	  gratio->Fit("pol0", "q");
	  mean_ratio = gratio->GetFunction("pol0")->GetParameter(0);
	  
	  double* x_val = gratio->GetX();
	  V_min = TMath::MinElement(gratio->GetN(), x_val);
	  V_max = TMath::MaxElement(gratio->GetN(), x_val);
	  
	  double* y_val = gratio->GetY();
	  I_min = TMath::MinElement(gratio->GetN(), y_val);
	  I_max = TMath::MaxElement(gratio->GetN(), y_val);
	  I_rms = gratio->GetRMS(2);
	  
	  for(int ipt=0; ipt<gratio->GetN(); ipt++){
	    gratio->GetPoint(ipt, x, y);
		nmod_avg_diff+=TMath::Abs(1./nmod-y);
		fit_avg_diff+=TMath::Abs(mean_ratio-y);
	  }
	  nmod_avg_diff/=gratio->GetN();
	  fit_avg_diff/=gratio->GetN();
	}
	
	//hratio->Fill(mean_ratio);
	//hnpts->Fill(gratio->GetN());
	//hrange->Fill(fabs(xlast-xfirst));    

	I_ratio = mean_ratio;
	npts = gratio->GetN();
    tout->Fill();
    
    //getchar();
      
  } // End of loop over modules
  

  // Write output root file
  fout->cd();
  //hratio->Write();
  //hnpts->Write();
  //hrange->Write();
  tout->Write();
  fout->Close();
  
  ClearConditions();
  
}

void ComputeAllDCUOverPSRatios(std::string subdet, std::string run, std::string filename, std::string bad_periods="")
{
  
  // load currents for all detids
  LoadConditions(map_DCU_currents, map_PS_currents, map_NMOD, subdet, run, bad_periods);

  // input file with list of detids
  std::string line;
  ifstream fin(filename);
  int detid=0;
  int idet=0;
  int detids[6500];

  // Loop on detids
  if(fin.is_open())  {
    while( getline ( fin, line) && idet < 6500)
    {
      if(fin.eof()) continue;
      std::stringstream ss(line);
      ss >> detid;
	  detids[idet]=detid;
	  idet++;
	}
  }
  
  ComputeDCUOverPSRatios(subdet, run, detids, idet, bad_periods);
  ClearConditions();
  return;
}


void ComputeAllCorrections(std::string subdet, std::string run, std::string filename, std::string bad_periods="", float CHI2_TOREJECT=50.)
{
  
  // load currents for all detids
  LoadConditions(map_DCU_currents, map_PS_currents, map_NMOD, subdet, run, bad_periods);

  // Histos and output file
  TFile* fout = new TFile(Form("LeakCurCorr_%s_%s.root", subdet.c_str(), run.c_str()),"recreate");
  TH1F* hchi2 = new TH1F("hchi2", "Chi2/NDF", 100, 0, 50);
  TGraph* g2param = new TGraph();
  TH1F* hparam0 = new TH1F("hparam0", "param0", 100, -1, 1);
  TH1F* hparam1 = new TH1F("hparam1", "param1", 500, -5, 5);
  TH1F* hparam2 = new TH1F("hparam2", "param2", 100, -10, 10);

  //float CHI2_TOREJECT=20.;

  int idet=0;
  int ifit=0;
  int status=0;
  int ngoodfit=0;
  int nbadfit=0;
  int nrejectedfit=0;
  int nrejectedfit_chi2=0;
  int nnoinfo=0;
  int nnotconv=0;
  int nfewcurpts=0;
  TGraphErrors *gvdrop;
  TF1 *fit;

  // input file with list of detids
  std::string line;
  ifstream fin(filename);
  int detid=0;
  bool show=false;
  double xfirst,xlast;

  // storing graphs for bad fits (limit on chi2/ndf)
  float chi2_limit = 5.; //2012: 5. //2015: 3. // 2018: 5., 15. for TIB/TID

  // Loop on detids
  if(fin.is_open())  {
    while( getline ( fin, line) && idet < 6500)
    {
      if(fin.eof()) continue;
      std::stringstream ss(line);
      ss >> detid;
	  idet++;

	  status = ComputeCorrection(subdet, run, detid, gvdrop, fit, bad_periods, show);
	  if(status==-11) nnoinfo++;
	  
	  if(gvdrop)
	  if(gvdrop->GetN())
	  {
		xfirst = TMath::MinElement(gvdrop->GetN(), gvdrop->GetX());
		xlast = TMath::MaxElement(gvdrop->GetN(), gvdrop->GetX());
		//if(gvdrop->GetN()<8 || !(xfirst<=75 && xlast>=240)) 
	    if(gvdrop->GetN()<8 || !(xfirst<=45 && xlast>=190)) // for 2018 scans
		  {cerr<<"Too few points for computing correction for detid "<<detid<<".  "<<gvdrop->GetN()<<" pts  from "<<xfirst<<" to "<<xlast<<" V"<<endl; nfewcurpts++; continue;}
	  }
	  
      // Store fit result
      if(fit)
	  {
    	if(fit->GetNDF()>1)
		{
		  if( fit->GetChisquare()/fit->GetNDF() < CHI2_TOREJECT)
    	  {
    		fout->cd();
    		cout<<"Storing fit for detid "<<detid<<endl;
    		fit->SetName(Form("fit_%i", detid));
    		fit->Write();
			cerr<<detid<<" "<<fit->Eval(300)<<endl;
    		gvdrop->SetName(Form("vdrop_%i", detid));
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
		  else { nrejectedfit_chi2++; nrejectedfit++;}
    	}
		else nrejectedfit++;
	  }
	  else nrejectedfit++;

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
  
  cerr<<"N tot detids: "<<idet<<endl;
  cerr<<"N good fits: "<<ngoodfit<<endl;
  cerr<<"N bad fits: "<<nbadfit<<" (chi2/ndf > "<<chi2_limit<<", not bad enough that the fit is rejected (>"<<int(CHI2_TOREJECT)<<"))"<<endl;
  cerr<<"N rejected fits: "<<nrejectedfit<<" ; "<<nrejectedfit_chi2<<" due to too large chi2 (chi2/ndf > "<<int(CHI2_TOREJECT)<<")"<<endl;
  cerr<<"N fits w/o convergence: "<<nnotconv<<endl;
  cerr<<"N detids w/o currents infos: "<<nnoinfo<<endl;
  cerr<<"N detids w too few currents infos: "<<nfewcurpts<<endl;
  
  ClearConditions();

}

//------------------------------------------------------------------------

void ComputeDCUFractions(int* detids, int N, std::string subdet="TIB_L1", std::string run="20120506_run193541", std::string bad_periods="")
{
  
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

void ComputeILeakDCUFractions()
{
  // TIB 2012
  const int N_TIB_L1_2012=17;
  int detids_TIB_L1_2012[N_TIB_L1_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
                                         369121613,369121614,369125861,369125862,369125866,369125869,369125870};  
  //ComputeDCUFractions(detids_TIB_L1_2012, N_TIB_L1_2012, "TIB_L1", "20120506_run193541", "");
  ComputeDCUFractions(detids_TIB_L1_2012, N_TIB_L1_2012, "TIB_L1", "20120928_run203832", "");

  // 20120506_run193541
  float dcu_fractions_TIB_L1_2012[N_TIB_L1_2012]={0.346, 0.329, 0.324, 0.339, 0.345, 0.329,
                                    0.345, 0.339, 0.339, 0.312, 0.320, 0.345,
								    0.400, 0.307, 0.217, 0.627, 0.476};
  // 20120928_run203832
  
  
  // TOB
  const int N_TOB=6;
  int detids_TOB[N_TOB]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528};
  //ComputeDCUFractions(detids_TOB, N_TOB, "TOB", "20120928_run203832", "");
  
  // 20120928_run203832
  float dcu_fractions_TOB[N_TOB]={0.296, 0.320, 0.327, 0.336, 0.365, 0.334};
  
}

void ComputeILeakCorrections(std::string subdet="TIB_L1", std::string run="20120506_run193541")
{

  // 2011
  const int N_2011=11;
  int detids_2011[N_2011]={369121605,369121606,369121609,369121610,369121613,369121614,369125861,369125862,369125866,369125869,369125870};
  // Not enough data //ComputeCorrections("TIB_L1", "20111012_run178367", detids_2011, N_2011);

  // 2012
  const int N_2012=17;
  int detids_2012[N_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
                           369121613,369121614,369125861,369125862,369125866,369125869,369125870};  
  //ComputeCorrections("TIB_L1", "20120405_run190459", detids_2012, N_2012, "Steps/bad_periods_20120405_run190459.txt");
  //ComputeCorrections("TIB_L1", "20120506_run193541", detids_2012, N_2012);
  // only PS currents
  //ComputeCorrections("TIB_L1", "20120510_run193928", detids_2012, N_2012, "Steps/bad_periods_20120510_run193928.txt");

  // L2 2012
  const int N_L2_2012=6;
  int detids_L2_2012[N_L2_2012]={369136677, 369136678, 369136681, 369136682, 369136685, 369136686};
  //ComputeCorrections("TIB_L2", "20120405_run190459", detids_L2_2012, N_L2_2012);
  // Not enought data //ComputeCorrections("TIB_L2", "20120506_run193541", detids_L2_2012, N_L2_2012);
  
  // August 2012 , only PS currents (L1+L2)
  const int N=4;
  int detids[N]={369125861, 369125862, 369136681, 369136682};
  //ComputeCorrections("TIB_L1", "20120812_run200786", detids, N);

  // From Summer 2012
  const int N_2012_bis=11;
  int detids_2012_bis[N_2012_bis]={369121381,369121382,369121385,369121386,369121389,369121390,369125861,369125862,369125866,369125869,369125870};
  int detids_2012_4PS[4]={369121381,369121382,369125861,369125862};


  /*ComputeCorrections("TIB", "20120506_run193541", detids_2012, N_2012, "");
  ComputeDCUOverPSRatios("TIB", "20120506_run193541", detids_2012, N_2012, "");
  
  ComputeCorrections("TIB", "20120728_run199832", detids_2012, N_2012, "Steps/bad_periods_20120728_run199832.txt");
  ComputeDCUOverPSRatios("TIB", "20120728_run199832", detids_2012, N_2012, "Steps/bad_periods_20120728_run199832.txt");

  //ComputeCorrections("TIB_L1", "20120928_run203832", detids_2012_4PS, 4, "Steps/bad_periods_20120928_run203832.txt");	
  ComputeCorrections("TIB", "20120928_run203832", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20120928_run203832.txt");
  ComputeDCUOverPSRatios("TIB", "20120928_run203832", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20120928_run203832.txt");

  ComputeCorrections("TIB", "20121130_run208339", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20121130_run208339.txt");
  ComputeDCUOverPSRatios("TIB", "20121130_run208339", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20121130_run208339.txt");

  ComputeCorrections("TIB", "20130213_run211797", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20130213_run211797", detids_2012_bis, N_2012_bis, "");
 */
  
  /*TGraphErrors* gv;
  TF1* fv;
  //LoadConditions(map_DCU_currents, map_PS_currents, map_NMOD, "TIB", "20180418_run314574", "");
  ComputeCorrection("TIB", "20180418_run314574", 369158756, gv, fv, "");*/
  
  /*//ComputeCorrections("TIB", "20120405_run190459", detids_2012, N_2012, "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllCorrections("TIB", "20120405_run190459", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllDCUOverPSRatios("TIB", "20120405_run190459", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  //ComputeCorrections("TIB", "20120510_run193928", detids_2012, N_2012, "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllCorrections("TIB", "20120510_run193928", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllDCUOverPSRatios("TIB", "20120510_run193928", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllCorrections("TIB", "20120812_run200786", "Data/TIB_detids_sorted.txt");
  ComputeAllDCUOverPSRatios("TIB", "20120812_run200786", "Data/TIB_detids_sorted.txt");*/

  // 2015

  //ComputeAllCorrections("TIB", "20150603_run246963", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  //ComputeAllDCUOverPSRatios("TIB", "20150603_run246963", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");

  /*ComputeCorrections("TIB", "20150821_run254790", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20150821_run254790", detids_2012_bis, N_2012_bis, "");
  ComputeCorrections("TIB", "20151007_run258443", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20151007_run258443.txt");
  ComputeDCUOverPSRatios("TIB", "20151007_run258443", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20151007_run258443.txt");
  ComputeCorrections("TIB", "20151121_run262254", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20151121_run262254", detids_2012_bis, N_2012_bis, "");
*/

  // 2016
  //ComputeAllCorrections("TIB", "20160423_run271056", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");
  //ComputeAllDCUOverPSRatios("TIB", "20160423_run271056", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  //ComputeDCUOverPSRatios("TIB", "20160423_run271056", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20160423_run271056", detids_2012_bis, N_2012_bis, "");
  /*ComputeDCUOverPSRatios("TIB", "20160612_run274969", detids_2012_bis, N_2012_bis, "");
  ComputeCorrections("TIB", "20160612_run274969", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20160706_run276437", detids_2012_bis, N_2012_bis, "");
  ComputeCorrections("TIB", "20160706_run276437", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20160803_run278167", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20160803_run278167.txt");
  ComputeCorrections("TIB", "20160803_run278167", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20160803_run278167.txt");
  ComputeDCUOverPSRatios("TIB", "20160909_run280385", detids_2012_bis, N_2012_bis, "");
  ComputeCorrections("TIB", "20160909_run280385", detids_2012_bis, N_2012_bis, "");
  ComputeDCUOverPSRatios("TIB", "20161116_run285371", detids_2012_bis, N_2012_bis, "");
  ComputeCorrections("TIB", "20161116_run285371", detids_2012_bis, N_2012_bis, "");
*/
const int N1=1;
int detids1[N1]={436311928};
//ComputeCorrections("TOB", "20180923_run323370", detids1, N1, "");

  // 2017
  //ComputeCorrections("TIB", "20170527_run295324", detids_2012, N_2012, "Steps/bad_periods_20170527_run295324.txt");
  //ComputeDCUOverPSRatios("TIB", "20171030_run305862", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20170527_run295376", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20170714_run298996", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20170714_run298996.txt");
  //ComputeCorrections("TIB", "20171030_run305862", detids_2012_bis, N_2012_bis, "");
  
  //ComputeDCUOverPSRatios("TIB", "", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "", detids_2012_bis, N_2012_bis, "");
  
  // 2018
  //ComputeDCUOverPSRatios("TIB", "20180418_run314574", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20180418_run314574", detids_2012_bis, N_2012_bis, "");
  //ComputeAllDCUOverPSRatios("TIB", "20180418_run314574", "Data/TIB_detids_sorted.txt", "");
  //ComputeAllCorrections("TIB", "20180418_run314574", "Data/TIB_detids_sorted.txt", "");
  
  //ComputeDCUOverPSRatios("TIB", "20180419_run314755", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20180419_run314755", detids_2012_bis, N_2012_bis, "");
  //ComputeAllDCUOverPSRatios("TIB", "20180419_run314755", "Data/TIB_detids_sorted.txt", "");
  //ComputeAllCorrections("TIB", "20180419_run314755", "Data/TIB_detids_sorted.txt", "");

  //ComputeDCUOverPSRatios("TIB", "20180530_run317182", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20180530_run317182", detids_2012_bis, N_2012_bis, "");
  //ComputeDCUOverPSRatios("TIB", "20180611_run317683", detids_2012_bis, N_2012_bis, "");
  //ComputeCorrections("TIB", "20180611_run317683", detids_2012_bis, N_2012_bis, "");
  //ComputeDCUOverPSRatios("TIB", "20180801_run320674", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20180801_run320674.txt");
  //ComputeCorrections("TIB", "20180801_run320674", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20180801_run320674.txt");

  //ComputeDCUOverPSRatios("TIB", "20180923_run323370", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20180923_run323370.txt");
  ComputeCorrections("TIB", "20180923_run323370", detids_2012_bis, N_2012_bis, "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllDCUOverPSRatios("TIB", "20180923_run323370", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllCorrections("TIB", "20180923_run323370", "Data/TIB_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");

  //noise
  //ComputeCorrections("TIB", "noise_20180618_run317974", detids_2012_bis, N_2012_bis, "");
  //ComputeAllDCUOverPSRatios("TIB", "noise_20180618_run317974", "Data/TIB_detids_sorted.txt", "");
  //ComputeAllCorrections("TIB", "noise_20180618_run317974", "Data/TIB_detids_sorted.txt", "");
  //ComputeCorrections("TIB", "noise_20180919_run323011", detids_2012_bis, N_2012_bis, "");
  //ComputeAllDCUOverPSRatios("TIB", "noise_20180919_run323011", "Data/TIB_detids_sorted.txt", "");
  //ComputeAllCorrections("TIB", "noise_20180919_run323011", "Data/TIB_detids_sorted.txt", "");
  

  // TESTS
  const int N_test=10;
  int detids_test[N_test]={369174348, 369170952, 369158584, 369137878, 369157316, 369138229, 369141097, 369175404, 369157284, 369157176};
  //ComputeCorrections("TIB", "20150603_run246963", detids_test, N_test, "Steps/bad_periods_20150603_run246963.txt");  

 
  // TOB
  //-----
  const int N_TOB=6;
  int detids_TOB[N_TOB]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528};
  
  const int N_TOB_2012=18;
  int detids_TOB_2012[N_TOB_2012]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528,
                         436232901, 436232902, 436232905, 436232906, 436232909, 436232910,
						 436232913, 436232914, 436232917, 436232918, 436232921, 436232922};
  //DrawConditions("TOB", "20121130_run208339",436281508 , "Steps/bad_periods_20121130_run208339.txt");

  // 2012
  
/*  ComputeAllDCUOverPSRatios("TOB", "20120405_run190459", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllDCUOverPSRatios("TOB", "20120510_run193928", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllDCUOverPSRatios("TOB", "20120812_run200786", "Data/TOB_detids_sorted.txt");
*/
/*  //ComputeDCUOverPSRatios("TOB", "20120405_run190459", detids_TOB, N_TOB, "Steps/bad_periods_20120405_run190459.txt");
  ComputeDCUOverPSRatios("TOB", "20120506_run193541", detids_TOB, N_TOB, "");
  //ComputeDCUOverPSRatios("TOB", "20120510_run193928", detids_TOB, N_TOB, "Steps/bad_periods_20120510_run193928.txt");
  ComputeDCUOverPSRatios("TOB", "20120728_run199832", detids_TOB, N_TOB, "Steps/bad_periods_20120728_run199832.txt");
  //ComputeDCUOverPSRatios("TOB", "20120812_run200786", detids_TOB, N_TOB);
  ComputeDCUOverPSRatios("TOB", "20120928_run203832", detids_TOB, N_TOB, "Steps/bad_periods_20120928_run203832.txt");
  ComputeDCUOverPSRatios("TOB", "20121130_run208339", detids_TOB, N_TOB, "Steps/bad_periods_20121130_run208339.txt");
  ComputeDCUOverPSRatios("TOB", "20130213_run211797", detids_TOB, N_TOB, "");
*/
    
 /* ComputeAllCorrections("TOB", "20120405_run190459", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllCorrections("TOB", "20120510_run193928", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllCorrections("TOB", "20120812_run200786", "Data/TOB_detids_sorted.txt");
*/
/*  //ComputeCorrections("TOB", "20120405_run190459", detids_TOB, N_TOB, "Steps/bad_periods_20120405_run190459.txt");
  ComputeCorrections("TOB", "20120506_run193541", detids_TOB, N_TOB, "");
  //ComputeCorrections("TOB", "20120510_run193928", detids_TOB, N_TOB, "Steps/bad_periods_20120510_run193928.txt");
  ComputeCorrections("TOB", "20120728_run199832", detids_TOB, N_TOB, "Steps/bad_periods_20120728_run199832.txt");
  //ComputeCorrections("TOB", "20120812_run200786", detids_TOB, N_TOB);
  ComputeCorrections("TOB", "20120928_run203832", detids_TOB, N_TOB, "Steps/bad_periods_20120928_run203832.txt");
  ComputeCorrections("TOB", "20121130_run208339", detids_TOB, N_TOB, "Steps/bad_periods_20121130_run208339.txt");
  ComputeCorrections("TOB", "20130213_run211797", detids_TOB, N_TOB, "");
*/    
  // 2015

  //ComputeAllDCUOverPSRatios("TOB", "20150603_run246963", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  
  /*//ComputeDCUOverPSRatios("TOB", "20150603_run246963", detids_TOB, N_TOB, "Steps/bad_periods_20150603_run246963.txt"); 
  ComputeDCUOverPSRatios("TOB", "20150821_run254790", detids_TOB, N_TOB, "");
  ComputeDCUOverPSRatios("TOB", "20151007_run258443", detids_TOB, N_TOB, "Steps/bad_periods_20151007_run258443.txt");
  ComputeDCUOverPSRatios("TOB", "20151121_run262254", detids_TOB, N_TOB, "");*/

  //ComputeAllCorrections("TOB", "20150603_run246963", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  
  /*ComputeCorrections("TOB", "20150603_run246963", detids_TOB, N_TOB, "Steps/bad_periods_20150603_run246963.txt"); 
  ComputeCorrections("TOB", "20150821_run254790", detids_TOB, N_TOB, "");
  ComputeCorrections("TOB", "20151007_run258443", detids_TOB, N_TOB, "Steps/bad_periods_20151007_run258443.txt");
  ComputeCorrections("TOB", "20151121_run262254", detids_TOB, N_TOB, "");*/


  // 2016
  /*ComputeAllDCUOverPSRatios("TOB", "20160423_run271056", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  //ComputeDCUOverPSRatios("TOB", "20160423_run271056", detids_TOB, N_TOB, "");
  ComputeDCUOverPSRatios("TOB", "20160612_run274969", detids_TOB, N_TOB, "");
  ComputeDCUOverPSRatios("TOB", "20160706_run276437", detids_TOB, N_TOB, "");
  ComputeDCUOverPSRatios("TOB", "20160803_run278167", detids_TOB, N_TOB, "Steps/bad_periods_20160803_run278167.txt");
  ComputeDCUOverPSRatios("TOB", "20160909_run280385", detids_TOB, N_TOB, "");
  ComputeDCUOverPSRatios("TOB", "20161116_run285371", detids_TOB, N_TOB, "");*/

  //ComputeAllCorrections("TOB", "20160423_run271056", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  //ComputeCorrections("TOB", "20160423_run271056", detids_TOB, N_TOB, "");
  /*ComputeCorrections("TOB", "20160612_run274969", detids_TOB, N_TOB, "");
  ComputeCorrections("TOB", "20160706_run276437", detids_TOB, N_TOB, "");
  ComputeCorrections("TOB", "20160803_run278167", detids_TOB, N_TOB, "Steps/bad_periods_20160803_run278167.txt");
  ComputeCorrections("TOB", "20160909_run280385", detids_TOB, N_TOB, "");
  ComputeCorrections("TOB", "20161116_run285371", detids_TOB, N_TOB, "");*/

  // 2017
  //ComputeDCUOverPSRatios("TOB", "20171030_run305862", detids_TOB_2012, N_TOB_2012, "");
  //ComputeCorrections("TOB", "20171030_run305862", detids_TOB_2012, N_TOB_2012, "");
  
  // 2018
  //ComputeDCUOverPSRatios("TOB", "20180418_run314574", detids_TOB_2012, N_TOB_2012, "");
  //ComputeCorrections("TOB", "20180418_run314574", detids_TOB_2012, N_TOB_2012, "");
  //ComputeAllDCUOverPSRatios("TOB", "20180418_run314574", "Data/TOB_detids_sorted.txt", "");
  //ComputeAllCorrections("TOB", "20180418_run314574", "Data/TOB_detids_sorted.txt", "", 20);

  //ComputeDCUOverPSRatios("TOB", "20180419_run314755", detids_TOB_2012, N_TOB_2012, "");
  //ComputeCorrections("TOB", "20180419_run314755", detids_TOB_2012, N_TOB_2012, "");
  //ComputeAllDCUOverPSRatios("TOB", "20180419_run314755", "Data/TOB_detids_sorted.txt", "");
  //ComputeAllCorrections("TOB", "20180419_run314755", "Data/TOB_detids_sorted.txt", "");

  //ComputeDCUOverPSRatios("TOB", "20180530_run317182", detids_TOB_2012, N_TOB_2012, "");
  //ComputeCorrections("TOB", "20180530_run317182", detids_TOB_2012, N_TOB_2012, "");
  //ComputeDCUOverPSRatios("TOB", "20180611_run317683", detids_TOB_2012, N_TOB_2012, "");
  //ComputeCorrections("TOB", "20180611_run317683", detids_TOB_2012, N_TOB_2012, "");
  //ComputeDCUOverPSRatios("TOB", "20180801_run320674", detids_TOB_2012, N_TOB_2012, "Steps/bad_periods_20180801_run320674.txt");
  //ComputeCorrections("TOB", "20180801_run320674", detids_TOB_2012, N_TOB_2012, "Steps/bad_periods_20180801_run320674.txt");

  //ComputeCorrections("TOB", "20180923_run323370", detids_TOB_2012, N_TOB_2012, "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllDCUOverPSRatios("TOB", "20180923_run323370", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllCorrections("TOB", "20180923_run323370", "Data/TOB_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");

  //noise
  //ComputeCorrections("TOB", "noise_20180618_run317974", detids_TOB_2012, N_TOB_2012, "");
  //ComputeAllDCUOverPSRatios("TOB", "noise_20180618_run317974", "Data/TOB_detids_sorted.txt", "");
  //ComputeAllCorrections("TOB", "noise_20180618_run317974", "Data/TOB_detids_sorted.txt", "");
  //ComputeCorrections("TOB", "noise_20180919_run323011", detids_TOB_2012, N_TOB_2012, "");
  //ComputeAllDCUOverPSRatios("TOB", "noise_20180919_run323011", "Data/TOB_detids_sorted.txt", "");
  //ComputeAllCorrections("TOB", "noise_20180919_run323011", "Data/TOB_detids_sorted.txt", "");

  //TGraphErrors* gv;
  //TF1* fv;
  //LoadConditions(map_DCU_currents, map_PS_currents, map_NMOD, "TOB", "20120510_run193928", "Steps/bad_periods_20120510_run193928.txt");
  //ComputeCorrection("TOB", "20120510_run193928", 436281508, gv, fv, "Steps/bad_periods_20120510_run193928.txt");
  //DrawDCUOverPSRatio("TOB", "20120510_run193928", 436281508, "Steps/bad_periods_20120510_run193928.txt");

  // TID
  //-----
  const int N_TID=4;
  int detids_TID[N_TID]={402668829, 402672930, 402677025, 402677041};
  //ComputeCorrections("TID", "20160423_run271056", detids_TID, N_TID, "Steps/bad_periods_20160423_run271056.txt");

/*  ComputeAllDCUOverPSRatios("TID", "20120405_run190459", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllDCUOverPSRatios("TID", "20120510_run193928", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllDCUOverPSRatios("TID", "20120812_run200786", "Data/TID_detids_sorted.txt");
  ComputeAllDCUOverPSRatios("TID", "20150603_run246963", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  ComputeAllDCUOverPSRatios("TID", "20160423_run271056", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  ComputeAllCorrections("TID", "20120405_run190459", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  ComputeAllCorrections("TID", "20120510_run193928", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  ComputeAllCorrections("TID", "20120812_run200786", "Data/TID_detids_sorted.txt");
  ComputeAllCorrections("TID", "20150603_run246963", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  ComputeAllCorrections("TID", "20160423_run271056", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");
*/  

  // 2018
  //ComputeAllDCUOverPSRatios("TID", "20180418_run314574", "Data/TID_detids_sorted.txt", "");
  //ComputeAllCorrections("TID", "20180418_run314574", "Data/TID_detids_sorted.txt", "");
  //ComputeAllDCUOverPSRatios("TID", "20180419_run314755", "Data/TID_detids_sorted.txt", "");
  //ComputeAllCorrections("TID", "20180419_run314755", "Data/TID_detids_sorted.txt", "");
  //ComputeAllDCUOverPSRatios("TID", "20180923_run323370", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllCorrections("TID", "20180923_run323370", "Data/TID_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");

  //ComputeAllDCUOverPSRatios("TID", "noise_20180919_run323011", "Data/TID_detids_sorted.txt", "");
  //ComputeAllCorrections("TID", "noise_20180919_run323011", "Data/TID_detids_sorted.txt", "");

  // TEC
  //------
  const int N_TEC=20;
  int detids_TEC[N_TEC]={470148196, 470148200, 470148204, 470148228, 470148232, 470148236, 470148240, 470148292, 470148296, 470148300, 470148304, 470148261, 470148262, 470148265, 470148266, 470148324, 470148328, 470148332, 470148336, 470148340};
  
   // 2012
  
  //ComputeAllDCUOverPSRatios("TEC", "20120405_run190459", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  //ComputeAllDCUOverPSRatios("TEC", "20120510_run193928", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  //ComputeAllDCUOverPSRatios("TEC", "20120812_run200786", "Data/TEC_detids_sorted.txt");

  /*//ComputeDCUOverPSRatios("TEC", "20120405_run190459", detids_TEC, N_TEC, "Steps/bad_periods_20120405_run190459.txt");
  //ComputeDCUOverPSRatios("TEC", "20120510_run193928", detids_TEC, N_TEC, "Steps/bad_periods_20120510_run193928.txt");
  ComputeDCUOverPSRatios("TEC", "20120728_run199832", detids_TEC, N_TEC, "Steps/bad_periods_20120728_run199832.txt");
  //ComputeDCUOverPSRatios("TEC", "20120812_run200786", detids_TEC, N_TEC);
  ComputeDCUOverPSRatios("TEC", "20120928_run203832", detids_TEC, N_TEC, "Steps/bad_periods_20120928_run203832.txt");
  ComputeDCUOverPSRatios("TEC", "20121130_run208339", detids_TEC, N_TEC, "Steps/bad_periods_20121130_run208339.txt");
  ComputeDCUOverPSRatios("TEC", "20130213_run211797", detids_TEC, N_TEC, "");*/
 
  //ComputeAllCorrections("TEC", "20120405_run190459", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20120405_run190459.txt");
  //ComputeAllCorrections("TEC", "20120510_run193928", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20120510_run193928.txt");
  //ComputeAllCorrections("TEC", "20120812_run200786", "Data/TEC_detids_sorted.txt");

  /*//ComputeCorrections("TEC", "20120405_run190459", detids_TEC, N_TEC, "Steps/bad_periods_20120405_run190459.txt");
  //ComputeCorrections("TEC", "20120510_run193928", detids_TEC, N_TEC, "Steps/bad_periods_20120510_run193928.txt");
  ComputeCorrections("TEC", "20120728_run199832", detids_TEC, N_TEC, "Steps/bad_periods_20120728_run199832.txt");
  //ComputeCorrections("TEC", "20120812_run200786", detids_TEC, N_TEC);
  ComputeCorrections("TEC", "20120928_run203832", detids_TEC, N_TEC, "Steps/bad_periods_20120928_run203832.txt");
  ComputeCorrections("TEC", "20121130_run208339", detids_TEC, N_TEC, "Steps/bad_periods_20121130_run208339.txt");
  ComputeCorrections("TEC", "20130213_run211797", detids_TEC, N_TEC, "");*/
 
 
  // 2015

  //ComputeAllDCUOverPSRatios("TEC", "20150603_run246963", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  
 /* ComputeDCUOverPSRatios("TEC", "20150603_run246963", detids_TEC, N_TEC, "Steps/bad_periods_20150603_run246963.txt"); 
  ComputeDCUOverPSRatios("TEC", "20150821_run254790", detids_TEC, N_TEC, "");
  ComputeDCUOverPSRatios("TEC", "20151007_run258443", detids_TEC, N_TEC, "Steps/bad_periods_20151007_run258443.txt");
  ComputeDCUOverPSRatios("TEC", "20151121_run262254", detids_TEC, N_TEC, "");*/

  //ComputeAllCorrections("TEC", "20150603_run246963", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20150603_run246963.txt");
  
  /*ComputeCorrections("TEC", "20150603_run246963", detids_TEC, N_TEC, "Steps/bad_periods_20150603_run246963.txt"); 
  ComputeCorrections("TEC", "20150821_run254790", detids_TEC, N_TEC, "");
  ComputeCorrections("TEC", "20151007_run258443", detids_TEC, N_TEC, "Steps/bad_periods_20151007_run258443.txt");
  ComputeCorrections("TEC", "20151121_run262254", detids_TEC, N_TEC, "");*/


  // 2016
  //ComputeAllDCUOverPSRatios("TEC", "20160423_run271056", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  /*//ComputeDCUOverPSRatios("TEC", "20160423_run271056", detids_TEC, N_TEC, "");
  ComputeDCUOverPSRatios("TEC", "20160612_run274969", detids_TEC, N_TEC, "");
  ComputeDCUOverPSRatios("TEC", "20160706_run276437", detids_TEC, N_TEC, "");
  ComputeDCUOverPSRatios("TEC", "20160803_run278167", detids_TEC, N_TEC, "Steps/bad_periods_20160803_run278167.txt");
  ComputeDCUOverPSRatios("TEC", "20160909_run280385", detids_TEC, N_TEC, "");
  ComputeDCUOverPSRatios("TEC", "20161116_run285371", detids_TEC, N_TEC, "");*/

  //ComputeAllCorrections("TEC", "20160423_run271056", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20160423_run271056.txt");

  /*//ComputeCorrections("TEC", "20160423_run271056", detids_TEC, N_TEC, "");
  ComputeCorrections("TEC", "20160612_run274969", detids_TEC, N_TEC, "");
  ComputeCorrections("TEC", "20160706_run276437", detids_TEC, N_TEC, "");
  ComputeCorrections("TEC", "20160803_run278167", detids_TEC, N_TEC, "Steps/bad_periods_20160803_run278167.txt");
  ComputeCorrections("TEC", "20160909_run280385", detids_TEC, N_TEC, "");
  ComputeCorrections("TEC", "20161116_run285371", detids_TEC, N_TEC, "");*/

  // 2018
  //ComputeDCUOverPSRatios("TEC", "20180418_run314574", detids_TEC, N_TEC, "");
  //ComputeCorrections("TEC", "20180418_run314574", detids_TEC, N_TEC, "");
  //ComputeAllDCUOverPSRatios("TEC", "20180418_run314574", "Data/TEC_detids_sorted.txt", "");
  //ComputeAllCorrections("TEC", "20180418_run314574", "Data/TEC_detids_sorted.txt", "", 10);

  //ComputeDCUOverPSRatios("TEC", "20180419_run314755", detids_TEC, N_TEC, "");
  //ComputeCorrections("TEC", "20180419_run314755", detids_TEC, N_TEC, "");
  //ComputeAllDCUOverPSRatios("TEC", "20180419_run314755", "Data/TEC_detids_sorted.txt", "");
  //ComputeAllCorrections("TEC", "20180419_run314755", "Data/TEC_detids_sorted.txt", "");
  
  //ComputeDCUOverPSRatios("TEC", "20180530_run317182", detids_TEC, N_TEC, "");
  //ComputeCorrections("TEC", "20180530_run317182", detids_TEC, N_TEC, "");
  //ComputeDCUOverPSRatios("TEC", "20180611_run317683", detids_TEC, N_TEC, "");
  //ComputeCorrections("TEC", "20180611_run317683", detids_TEC, N_TEC, "");
  //ComputeDCUOverPSRatios("TEC", "20180801_run320674", detids_TEC, N_TEC, "Steps/bad_periods_20180801_run320674.txt");
  //ComputeCorrections("TEC", "20180801_run320674", detids_TEC, N_TEC, "Steps/bad_periods_20180801_run320674.txt");

  //ComputeCorrections("TEC", "20180923_run323370", detids_TEC, N_TEC, "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllDCUOverPSRatios("TEC", "20180923_run323370", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");
  //ComputeAllCorrections("TEC", "20180923_run323370", "Data/TEC_detids_sorted.txt", "Steps/bad_periods_20180923_run323370.txt");

  // noise
  //ComputeCorrections("TEC", "noise_20180618_run317974", detids_TEC, N_TEC, "");
  //ComputeAllDCUOverPSRatios("TEC", "noise_20180618_run317974", "Data/TEC_detids_sorted.txt", "");
  //ComputeAllCorrections("TEC", "noise_20180618_run317974", "Data/TEC_detids_sorted.txt", "");
  //ComputeCorrections("TEC", "noise_20180919_run323011", detids_TEC, N_TEC, "");
  //ComputeAllDCUOverPSRatios("TEC", "noise_20180919_run323011", "Data/TEC_detids_sorted.txt", "");
  //ComputeAllCorrections("TEC", "noise_20180919_run323011", "Data/TEC_detids_sorted.txt", "");
}
