#include "ComputeILeakCorrections.C"
#include "GetKink.C"
#include "../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.h"
#include "../../SignalBiasScan/Analysis/CommonTools/ModFluence.h"
// #include "../../SignalBiasScan/Analysis/CurvesAnalysis/DrawKinkVsLumi.cc"
#include "TMathText.h"
#include "TLatex.h"
#include <fstream>
// #include ""

// Correct for the difference of total inelastic cross section when center of mass energy change (because we use a single fluence file for Run1 lumi)
double CorrectForCME(double lumi)
{
  double lumi_7TeV = 0, lumi_8TeV = 0;

  if(lumi < 6.12) {lumi_7TeV = lumi; lumi_8TeV = 0;}
  else if(lumi <= 29.46) {lumi_7TeV = 6.12; lumi_8TeV = lumi - lumi_7TeV;}
  else {cout<<FRED("Error : wrong lumi to be corrected ! Return 0")<<endl; return 0;}

  return lumi_7TeV + lumi_8TeV*74700./73500;

  //return lumi_7TeV + lumi_8TeV*73500./68000; // CMS xsections
  //return lumi_7TeV + lumi_8TeV*74700./73500; // Totem xsections
}

double ComputeFluence(double lumi, ULong64_t detid, TString subdet)
{
	if(subdet != "TIB" && subdet != "TOB" && subdet != "TEC" && subdet != "TID") {cout<<"Wrong argument 'subdet' ! Abort"<<endl; return 0;}

	double lumi_Run1 = 29.46;
	double lumi_Run2 = lumi - lumi_Run1;
	double lumi_Run3 = lumi - lumi_Run2;
	double fluence = -1;

	if(subdet == "TOB" || subdet == "TEC") {detid/= 10;}

	// Fluence files
	ModFluence DetFluenceRun1;
	DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
	ModFluence DetFluenceRun2;
	DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");
	ModFluence DetFluenceRun3;
	DetFluenceRun3.loadFile("../CommonTools/modulesFluence_7000.root");


	double fluenceRun1 = DetFluenceRun1.GetFluence(detid);
	double fluenceRun2 = DetFluenceRun2.GetFluence(detid);
	double fluenceRun3 = DetFluenceRun3.GetFluence(detid);

		//FIXME
	// if(lumi <= lumi_Run1)
	// {
	// 	fluence = CorrectForCME(lumi) * fluenceRun1;
	// }
	// else
	// {
	// 	fluence = CorrectForCME(lumi_Run1) * fluenceRun1 + lumi_Run2 * fluenceRun2;
	// }

	if(lumi <= lumi_Run1)
	{
		fluence = CorrectForCME(lumi) * fluenceRun1;
	}
	else if (lumi <= lumi_Run2 && lumi > lumi_Run1)
	{
		fluence = CorrectForCME(lumi_Run1) * fluenceRun1 + lumi_Run2 * fluenceRun2;
	}
	else{
		fluence = CorrectForCME(lumi_Run1) * fluenceRun1 + lumi_Run2 * fluenceRun2+lumi_Run3 * fluenceRun3;
	}

	// cout<<"CorrectForCME(lumi_Run1) "<<CorrectForCME(lumi_Run1)<<endl;
	// cout<<"fluenceRun1 "<<fluenceRun1<<endl;
	// cout<<"lumi_Run2 "<<lumi_Run2<<endl;
	// cout<<"fluenceRun2 "<<fluenceRun2<<endl;

	return fluence; //Implicit calls to destructors of creates instances
}

double GetLumiFromFluence(double fluence, ULong64_t detid, TString subdet)
{
	if(subdet != "TIB" && subdet != "TOB" && subdet != "TEC" && subdet != "TID") {cout<<"Wrong argument 'subdet' ! Abort"<<endl; return 0;}

	if(subdet == "TOB" || subdet == "TEC") {detid/= 10;}

	// Fluence files
	ModFluence DetFluenceRun1;
	DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
	ModFluence DetFluenceRun2;
	DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");
	ModFluence DetFluenceRun3;
	DetFluenceRun3.loadFile("../CommonTools/modulesFluence_7000.root");
	double fluence_per_lumi_7tev = DetFluenceRun1.GetFluence(detid);
	double fluence_per_lumi_13tev = DetFluenceRun2.GetFluence(detid);
	double fluence_per_lumi_14tev = DetFluenceRun3.GetFluence(detid);

	double lumi_7tev_total = 6.12;
	double lumi_run1_total = 29.46;
	double lumi_8tev_total = lumi_run1_total - lumi_7tev_total;

	double fluence_7tev_total = ComputeFluence(lumi_7tev_total, detid, subdet);
	double fluence_8tev_total = ComputeFluence(lumi_8tev_total, detid, subdet);
	double fluence_run1_total = ComputeFluence(lumi_run1_total, detid, subdet);


	if(fluence <= fluence_7tev_total)
	{
		return fluence / fluence_per_lumi_7tev;
	}
	else if(fluence <= fluence_run1_total)
	{
		return (fluence_7tev_total / fluence_per_lumi_7tev) + ( (fluence - fluence_7tev_total) / (fluence_per_lumi_7tev * (74700./73500) ) );
	}
	else
	{
		return (fluence_7tev_total / fluence_per_lumi_7tev) + (fluence_8tev_total / (fluence_per_lumi_7tev * (74700./73500)) ) + ( (fluence - fluence_run1_total) / fluence_per_lumi_13tev );
	}
}




TGraphErrors* GetIleakVsVbias(TString run, int DID , TGraph* gsteps, TGraph* gcur_DCU, TGraph* gcur_PS)
{
  if(!gsteps || !gcur_DCU || !gcur_PS) {std::cout<<" Missing input. Exiting GetIleakVsVbias()."<<std::endl; return 0;}
  if(!gsteps->GetN() || !gcur_PS->GetN()) {std::cout<<" Empty input. Exiting GetIleakVsVbias()."<<std::endl; return 0;}
  if(gcur_PS->GetN()<8) {std::cout<<" Too few PS currents values. Exiting GetIleakVsVbias()."<<std::endl; return 0;}

  TGraphErrors* gout = new TGraphErrors();

  double time=0;
  double current=0;
  double voltage=0;
  double previous_voltage=300;
  int time_cur=0;
  int time_volt=0;
  int prev_time_volt=0;
  
  double volt=0;
  double volt_err=0;
  
  
  TGraph* gcur = gcur_PS; //choose DCU or PS
  // Loop on DCU measurements
  int ipt=0;
  for(int ic=0; ic<gcur->GetN(); ic++)//loop over the detids
  {
    gcur->GetPoint(ic, time, current);

    time_cur=int(time);
    if(time_cur==0) continue;
       
    // Look for voltage step corresponding at time_cur
    previous_voltage=300;
    prev_time_volt=0;
    for(int iv=0; iv<gsteps->GetN(); iv++)//loop over the steps
      {
        gsteps->GetPoint(iv, time, voltage);
        time_volt=int(time);
        // cout<<" time : "<<time<<" "<<voltage<<"V"<<endl;
      
        // Fill graph Current vs Vbias
        if(time_cur>=prev_time_volt && time_cur<time_volt)
          {
            //cout<<prev_time_volt<<" "<<previous_voltage<<"V - "<<time_volt<<" "<<voltage<<"V"<<endl;
		        volt=(previous_voltage+voltage)/2.;//basically starts at 150 volts for some reasons
            // std::cout<<"volt : "<<volt<<"with previous voltage "<<previous_voltage<<"and voltage : "<<voltage<<std::endl;
            volt_err=fabs(voltage-volt);
            if(volt_err<1) volt_err=1; // the error is either 12.5 volts when using the interpoalted steps or 0 when using the usual steps
          }
	  
        previous_voltage=voltage;// to iterate over all the voltage steps
        prev_time_volt=time_volt;// to iterate over all the voltage steps
      }     
	 
    if(volt!=0)

    //-------------------------------------------------------------//
    // remove points not in a voltage step by using 1, else use 15 . Use 15 for runs of run 1 and 1 for runs of run 2 and 3
    //-------------------------------------------------------------//
     if ( volt_err<=15 && (run == "20180530_run317182" || run == "20180611_run317683" || run == "20220605_run353060" ) 
            && (DID == 436281508 || DID == 436281512 || DID == 436281516 || DID == 436281520 || DID == 436281524 || DID == 436281528 ) ) 
	  {
	    gout->SetPoint(ipt, volt, current);
      gout->SetPointError(ipt, volt_err, sqrt(2*2+0.02*current*0.02*current)); // added a prop. syst.
      ipt++;
	  }

	else if(volt_err<=15 && (( run.Contains("2016") || run.Contains("2015") ||  run.Contains("2012")||  run.Contains("2011")|| run.Contains("2010")) || run.Contains("20170527_run295324"))) //1 //15 + allows to avoid the first point at 155 volt due to the previous loop where (300+10)/2 is done=> error of ~150volts
	  {
	    gout->SetPoint(ipt, volt, current);
      gout->SetPointError(ipt, volt_err, sqrt(2*2+0.02*current*0.02*current)); // added a prop. syst.
      ipt++;
	  }
  else if (volt_err<=1)
	  {
	    gout->SetPoint(ipt, volt, current);
      gout->SetPointError(ipt, volt_err, sqrt(2*2+0.02*current*0.02*current)); // added a prop. syst.
      ipt++;
	  }


	
  } // End of loop on DCU measurements

  TH1F* h = gout->GetHistogram();
  h->GetXaxis()->SetTitle("V_{bias} [V]");
  h->GetYaxis()->SetTitle("I_{leak} [#mu\A]");
  return gout;
  
}

TGraphErrors* AverageIleakVsVbias(TGraphErrors* g, float deriv_min=0.01)
{
  TGraphErrors* gout = new TGraphErrors();
  
  double volt, current, volt_err, curr_err, previous_volt_err;
  double previous_volt=0;
  double mean_current=0;
  double deriv=0;
  double prev_pt_volt=0;
  double prev_pt_curr=0;
  int nmean=0;
  int ip=0;
  std::cout<<"Compute Average IleakVsVbias with derivative: "<<std::endl;
  for(int ipt=0; ipt<g->GetN(); ipt++)
  {
    g->GetPoint(ipt, volt, current);
	  volt_err = g->GetErrorX(ipt);
	  curr_err = g->GetErrorY(ipt);
	 
    if(previous_volt==volt)
	    {
	      mean_current+=current;
	      nmean++;
	    }
	 
	  if( (previous_volt!=0 && previous_volt!=volt) || ipt==g->GetN()-1 ) 
	    // step completed (moved to next one or last point)
	    {
	      if(ipt==g->GetN()-1) previous_volt=volt;
	      mean_current/=nmean;
	      // clean points with no evolution
	      deriv = (prev_pt_curr-mean_current) / (prev_pt_volt - previous_volt);
	      if(previous_volt!=0 && deriv>deriv_min) //0.5
	        {
	          gout->SetPoint(ip, previous_volt, mean_current);
	          gout->SetPointError(ip, previous_volt_err, curr_err/sqrt(nmean));
	          //gout->SetPointError(ip, volt_err, curr_err);
		        cout<<previous_volt<<"V "<<mean_current<<"uA +/- "<<curr_err/sqrt(nmean)<<"with derivatire  y':"<<deriv<<endl;
		        //cout<<previous_volt<<"V "<<mean_current<<"uA +/- "<<curr_err<<" y':"<<deriv<<endl;
		        prev_pt_volt = previous_volt;
		        prev_pt_curr = mean_current;
	          ip++;
	        }
	      mean_current=current;
	      nmean=1;
	     }
	  previous_volt=volt;
	  previous_volt_err=volt_err;
  }

  return gout;
}


TGraphErrors* GetDerivative(TGraph* g)
{
  
  // Initialization
  TGraphErrors* gderivative = new TGraphErrors();
  double deriv=0;
  double volt=0;
  double voltage=0;
  double previous_voltage=0;
  double shift=0;
  double previous_shift=0;
  int ipt=0;
  
  // Loop over points
  for(int ip=0; ip<g->GetN(); ip++)
  {
    previous_voltage = voltage;
    previous_shift = shift;
    g->GetPoint(ip, voltage, shift);
    if(!ip) continue;
    if(ip && voltage==previous_voltage) continue;
    volt=(previous_voltage+voltage)/2.;
    deriv = (shift-previous_shift)/(voltage-previous_voltage);
    //cout<<ipt<<" "<<volt<<" "<<deriv<<endl;
    gderivative->SetPoint(ipt, volt, deriv);
    ipt++;
  }
  
  return gderivative;
  
}


float GetStartFlat(TGraph* g, TH1F* &h, float &deriv_above_thresh, float &thresh_min, float &thresh_max, float &xstart_err)
{
  float xstart = 400;
  float x_above_thresh = 400;
  double voltage, deriv;
  double ymean, yrms;
  // Warning: points not always filled in the same order
  // -> reordering using map
  map< double, double> points;
  std::cout<<"GetStart Flat starting : "<<std::endl;
  for(int ip=0; ip<g->GetN(); ip++)
    {
      g->GetPoint(ip, voltage, deriv);
	    points[voltage]=deriv;
    }
  
  map< double, double>::iterator itPoints = points.end();
  while( itPoints!=points.begin() )
    {
      itPoints--;
      voltage=itPoints->first;
	    deriv=itPoints->second;
	    cout<<"Voltage : "<<voltage<<" with Ileak Deriv "<<deriv<<endl;
      if(voltage>200) h->Fill(deriv);
	    else
	      {
	        ymean = h->GetMean();
	        yrms = h->GetRMS();
	        thresh_min = ymean-3.5*yrms;
	        thresh_max = ymean+3.5*yrms;
	        h->Fill(deriv);
	        deriv_above_thresh = deriv;
	        x_above_thresh = voltage;
	        if(fabs(deriv)>thresh_max)
	          {
	            cout<<" first not flat point: Voltage"<<voltage<<" with Ileak derivative : "<<deriv<<", thresh: "<<thresh_max<<endl;
		          break;
	          }
	        xstart = voltage;
	      }
    }
  
  xstart_err = fabs(x_above_thresh - xstart);
  cout<<" xstart: "<<xstart<<"  err: "<<xstart_err<<endl;
  
  return xstart;
}
//------------------------------------------------------------------------------

std::pair<std::vector<float>,std::vector<float>> Fit(char* subdet, char* run,int LAY, int* detids, const int N, char* bad_periods="", float* Vinit = nullptr)
{
  
  // Histos and output file
  TString RUN = run;
  TFile* fout = new TFile(Form("LeakageCurrent_%s_%s.root", subdet, run),"recreate");
  TH1F* hchi2 = new TH1F("hchi2", "Chi2/NDF", 100, 0, 5);
  TH1F* hparam0 = new TH1F("hparam0", "param0", 100, 0, 50);
  TH1F* hparam1 = new TH1F("hparam1", "param1", 500, -5, 5);
  TH1F* hparam2 = new TH1F("hparam2", "param2", 100, 0, 400);
  TH1F* hparam3 = new TH1F("hparam3", "param3", 500, -5, 5);
  
  TFile* output = new TFile(Form("DECO_Ileak_%s_%s.root", subdet, run),"recreate");
  TTree* tout = new TTree("tout","SignalSummary");
  std::ofstream ofs ("Summary"+RUN+".txt", std::ofstream::out);

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

  Int_t kSubdetOffset = 25;
  Int_t subdet_i = 0;
  unsigned layerStartBit_ = 14;
  unsigned layerMask_ = 0x7;
  
  double lastpty = 0;

  // Loop over modules
  int detid=0;
  int ifit=0;
  std::vector<float> VfdOutput ;
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);
  VfdOutput.push_back(0);

  std::vector<float> DeltaVfdOutput ;
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  DeltaVfdOutput.push_back(0);
  int COUNT[10]= {0};

  std::pair<std::vector<float>,std::vector<float>> VfdData;

  std::vector<float> HotVfdOutput ;
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  HotVfdOutput.push_back(0);
  int HotCOUNT[10]= {0};

  for(int idet=0; idet<N; idet++)
  {
    detid = detids[idet];
    
    

	// Int_t detid_forlayer = detid;
  //   if(subdet=="TOB") detid_forlayer = detid/10;
	// subdet_i = (detid_forlayer>>kSubdetOffset)&0x7;
	// // For TIB and TOB
  //   if(subdet_i==3 || subdet_i==5) olayer = ((detid_forlayer>>layerStartBit_) & layerMask_);
	
    if(subdet=="TIB")  // TIB : subdetid==3
  {
    if(((detid>>14)&0x7)!=LAY) continue; //return 1;
    // else if(((detid>>14)&0x7)==2) return 2;
    // else if(((detid>>14)&0x7)==3) return 3;
    // else if(((detid>>14)&0x7)==4) return 4;
  }  



  else if(subdet=="TOB") // TOB subdetid==5
  {       
    if(((detid>>14)&0x7)!=LAY) continue; //return 5;
    // else if(((detid>>14)&0x7)==2) return 6;
    // else if(((detid>>14)&0x7)==3) return 7;
    // else if(((detid>>14)&0x7)==4) return 8;
    // else if(((detid>>14)&0x7)==5) return 9;
    // else if(((detid>>14)&0x7)==6) return 10;
  }   
cout<<" DetID "<<detid<<endl;
    // Get currents and voltage
    TGraph* gsteps;
    TGraph* gcur_DCU;
    TGraph* gcur_PS;
    TGraph* gvolt;
	  int nmodforchannel;
    GetConditions(gsteps, gcur_DCU, gcur_PS, gvolt, nmodforchannel, subdet, run, detid, bad_periods);
    // Only gsteps and gcur_DCU really needed
  	if(!gcur_DCU && !gcur_PS) {std::cout<<" No DCU , neither PS currents graphs. Exiting."<<std::endl; continue;}
    if(!gcur_DCU && gcur_PS) 
	    {
	      std::cout<<" No DCU current info, will try to use PS current instead."<<std::endl; 
	      gcur_DCU = (TGraph*) gcur_PS->Clone();
	      Scale( gcur_DCU, 1./nmodforchannel);
	    }
    
	// Exclude n last points due to thermal runaway ?
	
	
	double Steps_max = GetMaximum(gsteps);//300Volt
	double PS_max = GetMaximum(gcur_PS);//number of entries i guess
	TGraph* gcur_PS_unscaled = (TGraph*) gcur_PS->Clone();
	double scale_PS = 0.9*Steps_max/PS_max;
	Scale( gcur_PS, scale_PS);
	double DCU_max = GetMaximum(gcur_DCU);
	TGraph* gcur_DCU_unscaled = (TGraph*) gcur_DCU->Clone();
	double scale_DCU = 0.45*Steps_max/DCU_max;
	Scale( gcur_DCU, scale_DCU);
		
    // Draw conditions for monitoring
    TCanvas* c1 = new TCanvas("c1", "Currents");
    gsteps->Draw("APL");
    if(gvolt) gvolt->Draw("P");
    gcur_DCU->Draw("P");
    gcur_PS->Draw("PL");
    // gcur_DCU->Draw("P");
    TH1F* h = gsteps->GetHistogram();
    h->SetTitle(Form("DetID %i", detid));
    h->GetYaxis()->SetTitle("[V or #muA]");
    h->GetXaxis()->SetTitle("time");
    
    TLegend* leg = new TLegend(0.4,0.57,0.6,0.87);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);
    leg->SetTextFont(42);
    leg->SetTextSize(0.02);
    leg->AddEntry(gsteps,"Voltage (Steps)","APL");
    leg->AddEntry(gsteps,"Voltage (volt)","P");
    leg->AddEntry(gcur_DCU,"DCU current","P");
    leg->AddEntry(gcur_PS,"PS Current","PL");
    leg->Draw();
    

    // Get leakage current vs Vbias
    TCanvas* c2 = new TCanvas("c2", "FinalIleakvsVbias", 200, 0, 700, 500);
    TGraphErrors* gIleak = GetIleakVsVbias(RUN,detid, gsteps, gcur_DCU_unscaled, gcur_PS_unscaled);
	  if(!gIleak) {cout<<"Skipping detid"<<endl; continue;}
	  gIleak = AverageIleakVsVbias(gIleak);
    gIleak->SetMarkerStyle(20);
    gIleak->Draw("AP");
	  gIleak->SetTitle(Form("DetID %i", detid));
    TH1F* hleak = gIleak->GetHistogram();

    hleak->GetXaxis()->SetTitle("V_{bias} [V]");
    hleak->GetYaxis()->SetTitle("I_{leak} [#muA]");
    int npt = gIleak->GetN();
	  // c2->Print(Form("IleakVsVbias_raw_%i_%s_detid_%i.pdf",npt, run, detid));
	

	if(npt<3) continue;
	double x,y;
	lastpty = 0;
	gIleak->GetPoint(npt-1,x,y);
	lastpty += y;
	gIleak->GetPoint(npt-2,x,y);
	lastpty += y;
	gIleak->GetPoint(npt-3,x,y);
	lastpty += y;
	lastpty /= 3.;
	double x0,y0;
  gIleak->GetPoint(0,x0,y0);
   
   //------------------------------------------------------------//
    //------------------------------------------------------------//
    //---------------------Leakage current vs Vbiais--------------//
    //------------------------------------------------------------//
  //------------------------------------------------------------//


    double ymin = gIleak->GetYaxis()->GetXmin();
    double ymax = gIleak->GetYaxis()->GetXmax();

//-- COMMON VARIABLES TO THE DIFFERENT FIT FUNCTIONS --/

float fitdown = 10;
float fitup = 350;

 // the most important parameter is the parameter 2 as it sets the Vfd value
 // One has to have an idea of the Vfd for each year ot set the limits on this parameter
 // and help the fitting to converge

//=> ask for the chi2/ndof of the fit to be between 0 and 5
// /red
float f1down[4]  = {50,70,200,140};
float f1start[4] = {100,150,280,200};
float f1up[4]    = {150,170,300,360};

float f1par2down  = f1down[0];
float f1par2start = f1start[0];
float f1par2end   = f1up[0];

//blue 50-150 2018  and above 50-190 2017 // 50-250 -2016
float f2down[2]  = {50,200};
float f2start[2] = {150,280};
float f2up[5]    = {150,190,300,360,220};

float f2par2down  = f2down[0];
float f2par2start = f2start[0];
float f2par2end   = f2up[0];

//brown : 60-150 2018 and above, 60-170 2017 50-250 : 2016 with extra pts
float f3down[4]  = {45,60,85,200};
float f3start[3] = {75,150,280};
float f3up[5]    = {150,170,300,360,220};

float f3par2down  = f3down[0];
float f3par2start = f3start[0];
float f3par2end   = f3up[0];



//green : 2018 and above: 10-150 worked 2017 : 50-190; 50-250 : 2016 with extra pts
float f4down[3]  = {10,50,200};
float f4start[4] = {75,150,220,280};
float f4up[6]    = {150,190,260,200,360,200};

float f4par2down  = f4down[0];
float f4par2start = f4start[0];
float f4par2end   = f4up[0];

//------------------------------------------------------------//
    // function with curve in 2 parts : sigmoid and pol1
    // fit needs a lot of points to work
//------------------------------------------------------------//

if (subdet=="TIB")
  {
    if (RUN.Contains("2023") || RUN.Contains("2022") ||  RUN.Contains("2021")  ) 
        {
          f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[0] ;
          f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[0] ;
          f3par2down = f3down[0] ; f3par2start= f3start[0]; f3par2end=f3up[0] ;
          f4par2down = f4down[0] ; f4par2start= f4start[0]; f4par2end=f4up[0] ;
        
        } 
    if (RUN.Contains("2018") )
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[0] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[0] ;
        f3par2down = f3down[1] ; f3par2start= f3start[0]; f3par2end=f3up[0] ;
        f4par2down = f4down[0] ; f4par2start= f4start[0]; f4par2end=f4up[0] ;
      }
    if (RUN.Contains("2017") ) 
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[1] ;
        f3par2down = f3down[1] ; f3par2start= f3start[0]; f3par2end=f3up[1] ;
        f4par2down = f4down[1] ; f4par2start= f4start[0]; f4par2end=f4up[1] ;
      }

      
    //Care 2016 needs extra pts but with larger errors
        // + it is recommended to use the red and brown fit functions <=> sigmoid + pol1 or pol2
      // blue is ok
    if (RUN.Contains("2016") ) 
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[2] ;
        f3par2down = f3down[2] ; f3par2start= f3start[1]; f3par2end=f3up[1] ;
        f4par2down = f4down[1] ; f4par2start= f4start[0]; f4par2end=f4up[3] ;

      } 
      //Care 2015 needs extra pts but with larger errors
      // + it is recommended to use the red and brown fit functions <=> sigmoid + pol1 or pol2
      // green is wrong , blue is rarely good
      if (RUN.Contains("2015") ) 
      {
        f1par2down = f1down[0] ; f1par2start= f1start[1]; f1par2end=f1up[2] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[2] ;
        f3par2down = f3down[1] ; f3par2start= f3start[1]; f3par2end=f3up[2] ;
        f4par2down = f4down[1] ; f4par2start= f4start[2]; f4par2end=f4up[2] ;
      } 

      if (RUN.Contains("2012") ) 
      {
        f1par2down = f1down[2] ; f1par2start= f1start[2]; f1par2end=f1up[3] ;
        f2par2down = f2down[1] ; f2par2start= f2start[1]; f2par2end=f2up[3] ;
        f3par2down = f3down[3] ; f3par2start= f3start[2]; f3par2end=f3up[3] ;
        f4par2down = f4down[2] ; f4par2start= f4start[3]; f4par2end=f4up[4] ;
        fitdown = 50;
        fitup = 360;
      } 
  }

  if (subdet=="TOB")
  {
    if (RUN.Contains("2023") || RUN.Contains("2022") ) 
        {
          f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
          f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[4] ;
          f3par2down = f3down[0] ; f3par2start= f3start[0]; f3par2end=f3up[4] ;
          f4par2down = f4down[1] ; f4par2start= f4start[0]; f4par2end=f4up[1] ;
        
        } 
    if ( RUN.Contains("2021")  ) 
        {
          f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
          f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[4] ;
          f3par2down = f3down[1] ; f3par2start= f3start[1]; f3par2end=f3up[4] ;
          f4par2down = f4down[1] ; f4par2start= f4start[1]; f4par2end=f4up[3] ;
        } 
    if (RUN.Contains("2018") )
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[0] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[0] ;
        f3par2down = f3down[1] ; f3par2start= f3start[0]; f3par2end=f3up[0] ;
        f4par2down = 50 ;        f4par2start= 100; f4par2end=120 ;
      }
    if (RUN.Contains("2017") ) 
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[1] ;
        f3par2down = f3down[1] ; f3par2start= f3start[0]; f3par2end=f3up[1] ;
        f4par2down = f4down[1] ; f4par2start= f4start[0]; f4par2end=f4up[1] ;
      }

      
    //Care 2016 needs extra pts but with larger errors
        // + it is recommended to use the red and brown fit functions <=> sigmoid + pol1 or pol2
      // blue is ok
    if (RUN.Contains("2016") ) 
      {
        f1par2down = f1down[0] ; f1par2start= f1start[0]; f1par2end=f1up[1] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[4] ;
        f3par2down = f3down[2] ; f3par2start= f3start[1]; f3par2end=f3up[1] ;
        f4par2down = f4down[1] ; f4par2start= f4start[0]; f4par2end=f4up[3] ;

      } 

      //Care 2015 needs extra pts but with larger errors
      // + it is recommended to use the red and brown fit functions <=> sigmoid + pol1 or pol2
      // green is wrong , blue is rarely good
      if (RUN.Contains("2015") ) 
      {
        f1par2down = f1down[3] ; f1par2start= f1start[3]; f1par2end=f1up[2] ;
        f2par2down = f2down[0] ; f2par2start= f2start[0]; f2par2end=f2up[2] ;
        f3par2down = f3down[2] ; f3par2start= f3start[1]; f3par2end=f3up[2] ;
        f4par2down = f4down[1] ; f4par2start= f4start[2]; f4par2end=f4up[2] ;
      } 

      if (RUN.Contains("2012") ) 
      {
        f1par2down = f1down[2] ; f1par2start= f1start[2]; f1par2end=f1up[3] ;
        f2par2down = f2down[1] ; f2par2start= f2start[1]; f2par2end=f2up[3] ;
        f3par2down = f3down[3] ; f3par2start= f3start[2]; f3par2end=f3up[3] ;
        f4par2down = f4down[2] ; f4par2start= f4start[3]; f4par2end=f4up[4] ;
        fitdown = 50;
        fitup = 360;
      } 
  }
    
    TF1* fvdrop = new TF1("fvdrop", fitfunction2, fitdown, fitup, 5);//xmin, xmax, nbrof parameter
     fvdrop->SetParameter(0, 200.);
     fvdrop->SetParameter(1, 0.02);
     fvdrop->SetParameter(2, f1par2start);
	   fvdrop->SetParLimits(2, f1par2down, f1par2end);//70-150 for above 2017
     fvdrop->SetParameter(4, 0.1);
     fvdrop->SetLineColor(2);//red

     std::cout<<"Starting Fit of Leakage current vs Vbias with sigmoid and pol1 (red)"<<std::endl;

     int status = gIleak->Fit("fvdrop", "R");
	  if(status==4) {fvdrop->SetParameter(2, 100); status = gIleak->Fit("fvdrop", "R");}

    std::cout<<"End of  Fit of Leakage current vs Vbias with sigmoid and pol1 (red)"<<std::endl;

	  cout<<"Fit status: "<<status;
	  if(fvdrop->GetNDF()) cout<<" chi2/ndf: "<<fvdrop->GetChisquare()/fvdrop->GetNDF();
	  cout<<endl;
	 
    ymin = gIleak->GetYaxis()->GetXmin();
    ymax = gIleak->GetYaxis()->GetXmax();

	  fvdrop->Draw("same");
    TLine* lvdrop = new TLine(fvdrop->GetParameter(2), ymin, fvdrop->GetParameter(2), ymax);
    lvdrop->SetLineStyle(2);
    lvdrop->SetLineColor(2);//red
    lvdrop->Draw();
    // std::cout<<"help 1"<<std::endl;
    float detidDetection = 60.;
    if(fvdrop->GetNDF())
      {
        // std::cout<<"help 2"<<std::endl;
        if (fvdrop->GetChisquare()/fvdrop->GetNDF()<5  && fvdrop->GetChisquare()/fvdrop->GetNDF()>0.)
          {
            bool skip = false;
            if (COUNT[0]!=0)
              {
                if (abs(VfdOutput[0]/COUNT[0]-fvdrop->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[0]+=fvdrop->GetParameter(2);
                    HotCOUNT[0]++;
                    // skip = true; //activate if you want to remove modules
                    ofs << "Run : "<<RUN<<" for fit sigmoid+pol1  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[0]/HotCOUNT[0]<<" and cold Mean Vfd : "<<VfdOutput[0]/COUNT[0]<<std::endl;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[0]+=fvdrop->GetParameter(2);
                
                DeltaVfdOutput[0]+= (fvdrop->GetParameter(2)-Vinit[idet]);
                COUNT[0]++;
              }
          }
      }

    
    //------------------------------------------------------------//
    // function with curve in 2 parts : atan and pol1
    //------------------------------------------------------------//

    TF1* fvdrop3 = new TF1("fvdrop3", fitfunction3,fitdown, fitup, 4);
    fvdrop3->SetParameter(0, 1.);
    fvdrop3->SetParameter(1, 200.);
    fvdrop3->SetParameter(2, f2par2start);
	  fvdrop3->SetParLimits(2, f2par2down, f2par2end);//50-150 2018  and above 50-190 2017 50-250 -2016
    fvdrop3->SetParameter(3, 1.);
	  fvdrop3->SetLineColor(4);//blue

    std::cout<<"Starting Fit of Leakage current vs Vbias with atan and pol1 (blue)"<<std::endl;
    status = gIleak->Fit("fvdrop3", "Rsame")  ;

	  if(status==4) {fvdrop3->SetParameter(2, 100); status = gIleak->Fit("fvdrop3", "Rsame");}

    std::cout<<"End of  Fit of Leakage current vs Vbias with atan and pol1 (blue)"<<std::endl;
	  cout<<"Fit status: "<<status;

	  if(fvdrop3->GetNDF()) cout<<" chi2/ndf: "<<fvdrop3->GetChisquare()/fvdrop3->GetNDF();
	  cout<<endl;

    TLine* lvdrop3 = new TLine(fvdrop3->GetParameter(2), ymin, fvdrop3->GetParameter(2), ymax);
    lvdrop3->SetLineStyle(2);
    lvdrop3->SetLineColor(4);//blue
    lvdrop3->Draw();
    float chi2up = 5;
    if (subdet=="TOB" && (RUN.Contains("2021") || RUN.Contains("2022") || RUN.Contains("2023") || RUN.Contains("2024")))
      {
        chi2up = 30;
      }
    if(fvdrop3->GetNDF())
      {
        if (fvdrop3->GetChisquare()/fvdrop3->GetNDF()<chi2up && fvdrop3->GetChisquare()/fvdrop3->GetNDF()>0.0)
          {
            bool skip = false;
            if (COUNT[1]!=0)
              {
                if (abs(VfdOutput[1]/COUNT[1]-fvdrop3->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[1]+=fvdrop3->GetParameter(2);
                    HotCOUNT[1]++;
                    ofs << "Run : "<<RUN<<" for fit atan+pol1  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[1]/HotCOUNT[1]<<" and cold Mean Vfd : "<<VfdOutput[1]/COUNT[1]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[1]+=fvdrop3->GetParameter(2);
                DeltaVfdOutput[1]+= (fvdrop->GetParameter(2)-Vinit[idet]);
                COUNT[1]++;
              }
          }
      }
    //------------------------------------------------------------//
    // function with curve in 2 parts : sigmoid and pol2
    // fit needs a lot of points to work
    //------------------------------------------------------------//

    TF1* fvdrop6 = new TF1("fvdrop6", fitfunction6, fitdown, fitup, 6);//xmin, xmax, nbrof parameter
     fvdrop6->SetParameter(0, 200.);
     fvdrop6->SetParameter(1, 0.02);
     fvdrop6->SetParameter(2, f3par2start);//75
	   fvdrop6->SetParLimits(2, f3par2down, f3par2end);//60-150 2018 and above, 60-170 2017 50-250 : 2016 with extra pts
    //  fvdrop6->SetParameter(4, 0.1);
     fvdrop6->SetLineColor(44);// brown-ish color
     std::cout<<"Starting Fit of Leakage current vs Vbias with sigmoid and pol2 (brown-ish color)"<<std::endl;
     /*int */status = gIleak->Fit("fvdrop6", "R");
     
	  if(status==4) {fvdrop6->SetParameter(2, 50); status = gIleak->Fit("fvdrop6", "R");}
    std::cout<<"End of  Fit of Leakage current vs Vbias with sigmoid and pol2 (brown-ish color)"<<std::endl;

	  cout<<"Fit status: "<<status;
	  if(fvdrop6->GetNDF()) cout<<" chi2/ndf: "<<fvdrop6->GetChisquare()/fvdrop6->GetNDF();
	  cout<<endl;
	 
    ymin = gIleak->GetYaxis()->GetXmin();
    ymax = gIleak->GetYaxis()->GetXmax();

	  fvdrop6->Draw("same");
    TLine* lvdrop6 = new TLine(fvdrop6->GetParameter(2), ymin, fvdrop6->GetParameter(2), ymax);
    lvdrop6->SetLineStyle(2);
    lvdrop6->SetLineColor(44);//
    lvdrop6->Draw();

    if(fvdrop6->GetNDF())
      {
        if (fvdrop6->GetChisquare()/fvdrop6->GetNDF()<5 && fvdrop6->GetChisquare()/fvdrop6->GetNDF()>0.)
          {
            bool skip = false;
            if (COUNT[2]!=0)
              {
                if (abs(VfdOutput[2]/COUNT[2]-fvdrop6->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[2]+=fvdrop6->GetParameter(2);
                    HotCOUNT[2]++;
                    ofs << "Run : "<<RUN<<" for fit sigmoid+pol2  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[2]/HotCOUNT[2]<<" and cold Mean Vfd : "<<VfdOutput[2]/COUNT[2]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[2]+=fvdrop6->GetParameter(2);
                DeltaVfdOutput[2]+= (fvdrop->GetParameter(2)-Vinit[idet]);
                COUNT[2]++;
              }
          }
      }
    //------------------------------------------------------------//
        // function with curve in 2 parts : atan and pol2
    //------------------------------------------------------------//


    TF1* fvdrop4 = new TF1("fvdrop4", fitfunction4, fitdown, fitup, 5);
    fvdrop4->SetParameter(0, 1.);
    fvdrop4->SetParameter(1, 200.);
    fvdrop4->SetParameter(2, f4par2start);//150
	  fvdrop4->SetParLimits(2, f4par2down,f4par2end);//2018 and above: 10-150 worked 2017 : 50-190; 50-250 : 2016 with extra pts
    fvdrop4->SetParameter(3, 1.);
    fvdrop4->SetParameter(4, 1.);
    fvdrop4->SetParameter(5, 0.1);
	  fvdrop4->SetLineColor(3);//green
    fvdrop4->SetLineWidth(2);//

    std::cout<<"Starting Fit of Leakage current vs Vbias with atan and pol2 (green)"<<std::endl;

    status = gIleak->Fit("fvdrop4", "Rsame")  ;
	  if(status==4) {fvdrop4->SetParameter(2, 100); status = gIleak->Fit("fvdrop4", "Rsame");}

    std::cout<<"End of  Fit of Leakage current vs Vbias with atan and pol2 (green)"<<std::endl;
	  cout<<"Fit status: "<<status;
	  if(fvdrop4->GetNDF()) cout<<" chi2/ndf: "<<fvdrop4->GetChisquare()/fvdrop4->GetNDF();
	  cout<<endl;
     fvdrop4->Draw("same");
    TLine* lvdrop4 = new TLine(fvdrop4->GetParameter(2), ymin, fvdrop4->GetParameter(2), ymax);
    lvdrop4->SetLineStyle(2);
    lvdrop4->SetLineColor(3);//green
    lvdrop4->Draw();
    chi2up = 5;
    if (subdet=="TOB" && (RUN.Contains("2021") || RUN.Contains("2022") || RUN.Contains("2023") || RUN.Contains("2024")))
      {
        chi2up = 30;
      }
    if(fvdrop4->GetNDF())
      {
        if (fvdrop4->GetChisquare()/fvdrop4->GetNDF()<chi2up && fvdrop4->GetChisquare()/fvdrop4->GetNDF()>0.)
          {
            bool skip = false;
            if (COUNT[3]!=0)
              {
                if (abs(VfdOutput[3]/COUNT[3]-fvdrop4->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[3]+=fvdrop4->GetParameter(2);
                    HotCOUNT[3]++;
                    ofs << "Run : "<<RUN<<" for fit atan+pol2  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[3]/HotCOUNT[3]<<" and cold Mean Vfd : "<<VfdOutput[3]/COUNT[3]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[3]+=fvdrop4->GetParameter(2);
                DeltaVfdOutput[3]+= (fvdrop->GetParameter(2)-Vinit[idet]);
                COUNT[3]++;
              }
          }
      }

    // test Paul --//
    //------------------------------------------------------------//
        // General Function
    //------------------------------------------------------------//


    TF1* fvdrop10 = new TF1("fvdrop10", fitfunctionGeneral, fitdown, fitup,6);
    fvdrop10->SetParameter(0, 500);
    fvdrop10->SetParLimits(0, 10,1500);
    fvdrop10->SetParameter(1, 10);
    fvdrop10->SetParLimits(1, 1,100);
    fvdrop10->SetParameter(2, -3);//150
	  fvdrop10->SetParLimits(2, -5,-0.001);//2018 and above: 10-150 worked 2017 : 50-190; 50-250 : 2016 with extra pts
    fvdrop10->SetParameter(3, 0.008);
    fvdrop10->SetParLimits(3, 0.002,0.05);///plateau length
    fvdrop10->SetParameter(4, 20);
    fvdrop10->SetParLimits(4, 5,30);
    fvdrop10->SetParameter(5, 0.02);
    fvdrop10->SetParLimits(5, 0.001,0.05);
	  fvdrop10->SetLineColor(1);//green
    fvdrop10->SetLineWidth(2);//

    //     fvdrop10->SetParameter(0, 150);
    // fvdrop10->SetParLimits(0, 10,500);
    // fvdrop10->SetParameter(1, 10);
    // fvdrop10->SetParLimits(1, 1,100);
    // fvdrop10->SetParameter(2, -3);//150
	  // fvdrop10->SetParLimits(2, -5,-0.1);//2018 and above: 10-150 worked 2017 : 50-190; 50-250 : 2016 with extra pts
    // fvdrop10->SetParameter(3, 0.008);
    // fvdrop10->SetParLimits(3, 0.002,0.05);///plateau length
    // fvdrop10->SetParameter(4, 22.5);
    // fvdrop10->SetParLimits(4, 10,30);
	  // fvdrop10->SetLineColor(1);//green
    // fvdrop10->SetLineWidth(5);//

//   Double_t fitfunctionGeneral(Double_t *x, Double_t *par){
//   Double_t value;

//   value = par[0]x[0]*atan(par[2]*x[0])+exp(par[3]*x[0]-par[4])+ln(1+par[1]*x[0]*x[0]);
//   return value;
// }

    std::cout<<"Starting Fit of Leakage current vs Vbias with general function"<<std::endl;
    status = gIleak->Fit("fvdrop10", "Rsame")  ;


	  if(fvdrop10->GetNDF()) cout<<" chi2/ndf: "<<fvdrop10->GetChisquare()/fvdrop10->GetNDF();
	  cout<<endl;

    fvdrop10->Draw("same");

    float p = 0.005;
    if (subdet=="TIB")
      {
        if (RUN.Contains("2016"))
          {
            p = 0.0007;
          }
      }
    if (subdet=="TOB")
      {
        p = 0.001;
        if (LAY == 4)
          {
            if (RUN.Contains("2016"))
              {
                p = 0.001;
              }
            if (RUN.Contains("2012"))
              {
                p = 0.01;
              }
            if (RUN.Contains("2023"))
              {
                p = 0.0005;
              }
          }
        if (LAY == 1)
          {
            if (RUN.Contains("2018"))
              {
                p = 0.0009;//-
              }
            if (RUN.Contains("2021") || RUN.Contains("2022"))
              {
                p = 0.00025;
              }
            if (RUN.Contains("2023"))
              {
                p = 0.0002;
              }
            if (RUN.Contains("run368669") || RUN == "20230907_run373060")
              {
                p = 0.0002;
              }


          }

      }
    float Vfdgen = fvdrop10->GetParameter(0)*3.14*p/(4*fvdrop10->GetParameter(5));// this parameter 5 can be very small
    // therefore the Vfd values can "diverge".This is why you may see in the code a lower of 10-4 to this parameter. If you let it
    // free, it can go to 10e-12 making Vfd hard to retrieve (it's already the case).
    // The free parameter is p.
        std::cout<<" VfdGen : "<<Vfdgen<<std::endl;
    std::cout<<"End of  Fit of Leakage current vs Vbias with general function "<<std::endl;
	  cout<<"Fit status: "<<status;
        TLine* lvdrop10 = new TLine(Vfdgen, ymin, Vfdgen, ymax);
    lvdrop10->SetLineStyle(2);
    lvdrop10->SetLineColor(1);//black
    // lvdrop10->SetLineColor(1);//
    lvdrop10->Draw();
    chi2up = 10;
    if ( RUN == "20230609_run368669"){chi2up = 15;}
    if ( RUN == "20230907_run373060"){chi2up = 20;}
    
    if(fvdrop10->GetNDF())
      {
        if (fvdrop10->GetChisquare()/fvdrop10->GetNDF()<chi2up && fvdrop10->GetChisquare()/fvdrop10->GetNDF()>0. && Vfdgen > 0 && Vfdgen < 400)
          {
            bool skip = false;
            if (COUNT[7]!=0)
              {
                if (abs(VfdOutput[7]/COUNT[7]-Vfdgen)>detidDetection)
                  {
                    HotVfdOutput[7]+=Vfdgen;
                    HotCOUNT[7]++;
                    ofs << "Run : "<<RUN<<" for fit GenFunc  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[7]/HotCOUNT[7]<<" and cold Mean Vfd : "<<VfdOutput[7]/COUNT[7]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[7]+=Vfdgen;
                DeltaVfdOutput[7]+= (Vfdgen-Vinit[idet]);
                COUNT[7]++;
              }
          }
      }




    //------------------------------------------------------------//
    //--------------------end of -Leakage current vs Vbiais-------//
    //------------------------------------------------------------//


  //------------------------------------------------------------//
  // --------2D Derivative part ---------------------//
  //------------------------------------------------------------//
	// Look at derivative as second part of the curve is a line
    
    TCanvas* cd = new TCanvas("cd", "PointsDerivative", 300, 0, 700, 500);
    cd->cd();
    // Compute derivative of current vs voltage curve
    //TGraphErrors* gmed = MedianFilter( gIleak );
	  //gmed = HanningFilter(gmed);
	  TGraphErrors* gderivative = GetDerivative( gIleak );
	  //gderivative = HanningFilter( gderivative );
    gderivative->SetMarkerStyle(20);
    gderivative->Draw("AP");
    TH1F* hdaxis = gderivative->GetHistogram();
    hdaxis->GetXaxis()->SetTitle("V_{bias} [V]");
    hdaxis->GetYaxis()->SetTitle("derivative");
	
	// start to fit above 200V
	TF1* fflat = new TF1("fflat", "[0]", 10, 350);
	fflat->SetParameter(0, 0.); 
	fflat->SetRange(200, 350);
	status = gderivative->Fit("fflat", "R");
	cout<<"Fit status: "<<status;
	if(fflat->GetNDF()) cout<<" chi2/ndf: "<<fflat->GetChisquare()/fflat->GetNDF()<<endl;
	float cst = fflat->GetParameter(0);
	cout<<"First parameter : constant part of the derivative =>  pol0: "<<cst<<endl;
	TH1F* hd = new TH1F("hd", "derivative", 250, -4*cst, 6*cst);
	float deriv_above, thresh_min, thresh_max, xstart_err;
	float xstart = GetStartFlat(gderivative, hd, deriv_above, thresh_min, thresh_max, xstart_err);

    // cout<<" xstart: "<<xstart<<endl<<endl;

    double ymin_deriv = gderivative->GetYaxis()->GetXmin();
    double ymax_deriv = gderivative->GetYaxis()->GetXmax();
	  TLine* lstart = new TLine(xstart, ymin_deriv, xstart, ymax_deriv*0.8);
    lstart->SetLineStyle(3);
    lstart->SetLineColor(kBlack);
    lstart->Draw("same");

  float Vfd = 0;
  float fitderivdown[2] = {10,50};
  float fitderivup[3]   = {300,350,280};
  float fderivdown[3]  = {130,190,230};
  float fderivstart[5] = {100,150,250,300,200};
  float fderivup[5]    = {150,170,300,360,220};

  float fderiv1par2down  = fderivdown[0];
  float fderiv1par2start = fderivstart[0];
  float fderiv1par2end   = fderivup[0];

  float fderiv2par2down  = fderivdown[0];
  float fderiv2par2start = fderivstart[0];
  float fderiv2par2end   = fderivup[0];

  float fderiv3par2down  = fderivdown[0];
  float fderiv3par2start = fderivstart[0];
  float fderiv3par2end   = fderivup[0];

if (subdet=="TIB")
  {
    if (RUN.Contains("2016") ) //ok
      {
          fderiv1par2down = fderivdown[0] ; fderiv1par2start= fderivstart[2]; fderiv1par2end=fderivup[2] ;
          fderiv2par2down = fderivdown[0] ; fderiv2par2start= fderivstart[2]; fderiv2par2end=fderivup[2] ;
      } 
      if (RUN.Contains("2015") ) //ok
      {
          fderiv1par2down = fderivdown[0] ; fderiv1par2start= fderivstart[4]; fderiv1par2end=fderivup[2] ;
          fderiv2par2down = fderivdown[0] ; fderiv2par2start= fderivstart[2]; fderiv2par2end=fderivup[2] ;
      } 
      if (RUN.Contains("2013") ) //ok
      {
        fderiv1par2down = fderivdown[2] ; fderiv1par2start= fderivstart[3]; fderiv1par2end=fderivup[3] ;
        fderiv2par2down = fderivdown[2] ; fderiv2par2start= fderivstart[3]; fderiv2par2end=fderivup[3] ;
      } 
      if (RUN.Contains("2012") ) //ok
      {
        fderiv1par2down = fderivdown[2] ; fderiv1par2start= fderivstart[3]; fderiv1par2end=fderivup[3] ;
        fderiv2par2down = fderivdown[2] ; fderiv2par2start= fderivstart[3]; fderiv2par2end=fderivup[3] ;
      } 
  }
if (subdet=="TOB")
  {
    if (RUN.Contains("2016") ) 
      {
        fderiv1par2down = fderivdown[0] ; fderiv1par2start= fderivstart[0]; fderiv1par2end=fderivup[1] ;
        fderiv2par2down = fderivdown[0] ; fderiv2par2start= fderivstart[0]; fderiv2par2end=fderivup[2] ;
      } 
      if (RUN.Contains("2015") ) 
      {
        fderiv1par2down = fderivdown[1] ; fderiv1par2start= fderivstart[4]; fderiv1par2end=fderivup[4] ;
        fderiv2par2down = fderivdown[0] ; fderiv2par2start= fderivstart[0]; fderiv2par2end=fderivup[2] ;
      } 
      if (RUN.Contains("2013") ) 
      {
        fderiv1par2down = fderivdown[2] ; fderiv1par2start= fderivstart[3]; fderiv1par2end=fderivup[3] ;
        fderiv2par2down = fderivdown[2] ; fderiv2par2start= fderivstart[3]; fderiv2par2end=fderivup[3] ;
      }
      if (RUN.Contains("2012") ) 
      {
        fderiv1par2down = fderivdown[2] ; fderiv1par2start= fderivstart[2]; fderiv1par2end=fderivup[3] ;
        fderiv2par2down = fderivdown[1] ; fderiv2par2start= fderivstart[1]; fderiv2par2end=fderivup[3] ;
      } 
  }
    //------------------------------------------------------------//
    //------------------------------------------------------------//

    if (  RUN.Contains("2015") || RUN.Contains("2016"))
      {
          TF1* fvdropderiv = new TF1("fvdropderiv", fitfunctionderiv2, fitderivdown[0], fitderivup[2], 4);
        fvdropderiv->SetParameter(0, 4.);
        fvdropderiv->SetParLimits(0, 0.1, 15);
        fvdropderiv->SetParameter(1, -0.01);
        fvdropderiv->SetParameter(2, fderiv1par2start);
        fvdropderiv->SetParLimits(2, fderiv1par2down, fderiv1par2end);
        fvdropderiv->SetParameter(3, 0.);
        fvdropderiv->SetParLimits(3, -0.1, 0.1);
        // fvdropderiv->SetParameter(4, 1.);

        fvdropderiv->SetLineColor(44);
        fvdropderiv->SetLineWidth(2);
        std::cout<<"Starting Fit of deriv with exp"<<std::endl;
        status = gderivative->Fit("fvdropderiv", "Rsame")  ;
        if(status==4) {fvdropderiv->SetParameter(2, 200); status = gIleak->Fit("fvdropderiv", "Rsame");}
        std::cout<<"End of  Fit of deriv with exp"<<std::endl;
        cout<<"Fit status: "<<status;
        if(fvdropderiv->GetNDF()) cout<<" chi2/ndf: "<<fvdropderiv->GetChisquare()/fvdropderiv->GetNDF();
        cout<<endl;
        Vfd = fvdropderiv->GetParameter(2);

        if(fvdropderiv->GetNDF())
        {
          if (fvdropderiv->GetChisquare()/fvdropderiv->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[4]!=0)
              {
                if (abs(VfdOutput[4]/COUNT[4]-Vfd)>detidDetection)
                  {
                    HotVfdOutput[4]+=Vfd;
                    HotCOUNT[4]++;
                    ofs << "Run : "<<RUN<<" for fit deriv exp  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[4]/HotCOUNT[4]<<" and cold Mean Vfd : "<<VfdOutput[4]/COUNT[4]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[4]+=Vfd;
                DeltaVfdOutput[4]+= (Vfd-Vinit[idet]);
                COUNT[4]++;
              }
          }
        }

    
      TLine* lvdropderiv = new TLine(fvdropderiv->GetParameter(2), ymin_deriv, fvdropderiv->GetParameter(2), ymax_deriv);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");



            c2->cd();
            lvdropderiv = new TLine(fvdropderiv->GetParameter(2), ymin, fvdropderiv->GetParameter(2), ymax);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);//green
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");
    }

        //------------------------------------------------------------//
    //------------------------------------------------------------//
    if (RUN.Contains("2012") || RUN.Contains("2013") )
      {
        TF1* fvdropderiv = new TF1("fvdropderiv", fitfunctionderiv2, fitderivdown[1], fitderivup[1]+10, 5);
      fvdropderiv->SetParameter(0, 4.);
      fvdropderiv->SetParLimits(0, 0.1, 15);
      fvdropderiv->SetParameter(1, -0.01);
      fvdropderiv->SetParameter(2, fderiv2par2start);
	    fvdropderiv->SetParLimits(2,fderiv2par2down, fderiv2par2end);
        fvdropderiv->SetParameter(3, 1.);
      fvdropderiv->SetParameter(4, 1.);

	    fvdropderiv->SetLineColor(44);//green
      fvdropderiv->SetLineWidth(2);
      std::cout<<"Starting Fit of deriv with exp"<<std::endl;
      status = gderivative->Fit("fvdropderiv", "Rsame")  ;
	   if(status==4) {fvdropderiv->SetParameter(2, 100); status = gIleak->Fit("fvdropderiv", "Rsame");}
     std::cout<<"End of  Fit of deriv with exp"<<std::endl;
	    cout<<"Fit status: "<<status;
	    if(fvdropderiv->GetNDF()) cout<<" chi2/ndf: "<<fvdropderiv->GetChisquare()/fvdropderiv->GetNDF();
	    cout<<endl;
      Vfd = fvdropderiv->GetParameter(2);

      if(fvdropderiv->GetNDF())
      {
        if (fvdropderiv->GetChisquare()/fvdropderiv->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[4]!=0)
              {
                if (abs(VfdOutput[4]/COUNT[4]-Vfd)>detidDetection)
                  {
                    HotVfdOutput[4]+=Vfd;
                    HotCOUNT[4]++;
                    ofs << "Run : "<<RUN<<" for fit deriv exp  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[4]/HotCOUNT[4]<<" and cold Mean Vfd : "<<VfdOutput[4]/COUNT[4]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[4]+=Vfd;
                DeltaVfdOutput[4]+= (Vfd-Vinit[idet]);
                COUNT[4]++;
              }
          }
      }

    
      TLine* lvdropderiv = new TLine(fvdropderiv->GetParameter(2), ymin_deriv, fvdropderiv->GetParameter(2), ymax_deriv);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");

      c2->cd();
      lvdropderiv = new TLine(fvdropderiv->GetParameter(2), ymin, fvdropderiv->GetParameter(2), ymax);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);//green
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");


    }
      //------------------------------------------------------------//
        //------------------------------------------------------------//
else if (RUN.Contains("2023")|| RUN.Contains("2022") || RUN.Contains("2021")|| RUN.Contains("2018") || RUN.Contains("2017"))
  {
    //Initiliazed with TIB values
    float stddevdown = 5;
    float stddevstart = 20;
    float stddevup = 100;

    if (subdet == "TOB")
      {
        stddevdown = 5;
        stddevstart = 30;
        stddevup = 150;
      }
            TF1* fvdropderiv = new TF1("fvdropderiv", fitfunctionderiv3,fitderivdown[0], fitderivup[1], 4);
      fvdropderiv->SetParameter(0, 0.);//ordonné à l'origine
      fvdropderiv->SetParLimits(0, 0, 10);
      fvdropderiv->SetParameter(1, 75);//coef for the exp
      fvdropderiv->SetParLimits(1, 20, 100);
      fvdropderiv->SetParameter(2, 0);//mean
	    fvdropderiv->SetParLimits(2, 0, 20);
      fvdropderiv->SetParameter(3, stddevstart);//std dev
      fvdropderiv->SetParLimits(3, stddevdown, stddevup);

	    fvdropderiv->SetLineColor(44);//green
      fvdropderiv->SetLineWidth(2);//green
      std::cout<<"Starting Fit of deriv with exp"<<std::endl;
      status = gderivative->Fit("fvdropderiv", "Rsame")  ;
	   if(status==4) {fvdropderiv->SetParameter(2, 100); status = gIleak->Fit("fvdropderiv", "Rsame");}
     std::cout<<"End of  Fit of deriv with exp"<<std::endl;
	    cout<<"Fit status: "<<status;
	    if(fvdropderiv->GetNDF()) cout<<" chi2/ndf: "<<fvdropderiv->GetChisquare()/fvdropderiv->GetNDF();
	    cout<<endl;
      Vfd = fvdropderiv->GetParameter(2)+3*fvdropderiv->GetParameter(3);
      fvdropderiv->Draw("same");
      TLine* lvdropderiv = new TLine(Vfd, ymin_deriv, Vfd, ymax_deriv);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);//brown
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");

      if(fvdropderiv->GetNDF())
      {
        if (fvdropderiv->GetChisquare()/fvdropderiv->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[4]!=0)
              {
                if (abs(VfdOutput[4]/COUNT[4]-Vfd)>detidDetection)
                  {
                    HotVfdOutput[4]+=Vfd;
                    HotCOUNT[4]++;
                    ofs << "Run : "<<RUN<<" for fit deriv exp  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[4]/HotCOUNT[4]<<" and cold Mean Vfd : "<<VfdOutput[4]/COUNT[4]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[4]+=Vfd;
                DeltaVfdOutput[4]+= (Vfd-Vinit[idet]);
                COUNT[4]++;
              }
          }
      }

       //-------------------------------//
      // ------three linear regimes ---//
      //-------------------------------//
    //Initiliazed with TIB values
    float par2down = 17;
    float par2start = 25;
    float par2up = 30;

    float par3down = 40;
    float par3start = 70;
    float par3up = 90;

    if (subdet == "TOB")
      {
        par2down = 15;
        par2start = 25;
        par2up = 35;

        par3down = 50;
        par3start = 70;
        par3up = 150;
      }

      TF1* TriL = new TF1("TriL", fitfunctionderiv4, fitderivdown[0], fitderivup[0], 6);
      TriL->SetParameter(0, 20.);//ordonnée à l'origine
      TriL->SetParLimits(0, 15, 100);
      TriL->SetParameter(1, -100);//
      TriL->SetParLimits(1, -1000, 1000);
      TriL->SetParameter(2, par2start);//
	    TriL->SetParLimits(2, par2down, par2up);
      TriL->SetParameter(3, par3start);//
	    TriL->SetParLimits(3, par3down, par3up);
      TriL->SetParameter(4, -10);//
	    TriL->SetParLimits(4, -100, 0);
      TriL->SetParameter(5, 0);//
	    TriL->SetParLimits(5, -10, 10);

	    TriL->SetLineColor(2);//red
      TriL->SetLineWidth(2);
      gderivative->Fit("TriL", "Rsame")  ;

      TriL->Draw("same");
      std::cout<<"TriL "<< TriL->GetChisquare()/TriL->GetNDF()  <<std::endl;

      if(TriL->GetNDF())
      {
        if (TriL->GetChisquare()/TriL->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[5]!=0)
              {
                if (abs(VfdOutput[5]/COUNT[5]-TriL->GetParameter(3))>detidDetection)
                  {
                    HotVfdOutput[5]+=TriL->GetParameter(3);
                    HotCOUNT[5]++;
                    ofs << "Run : "<<RUN<<" for fit deriv TriL  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[5]/HotCOUNT[5]<<" and cold Mean Vfd : "<<VfdOutput[5]/COUNT[5]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[5]+=TriL->GetParameter(3);
                DeltaVfdOutput[5]+= (TriL->GetParameter(3)-Vinit[idet]);
                COUNT[5]++;
              }
          }
      }
            // c2->cd();
      lvdropderiv = new TLine(TriL->GetParameter(3), ymin,TriL->GetParameter(3), ymax);
      lvdropderiv->SetLineStyle(2);
      lvdropderiv->SetLineColor(44);
      lvdropderiv->SetLineWidth(2);
      lvdropderiv->Draw("same");
  }

  //test- Paul-
        TF1* fvdropderivGen = new TF1("fvdropderivGen", fitfunctionderivGeneral,fitderivdown[0], fitderivup[1], 5);
      fvdropderivGen->SetParameter(0, 2.);//ordonné à l'origine
      fvdropderivGen->SetParLimits(0, 0.1, 10);
      fvdropderivGen->SetParameter(1, 0.015);
      fvdropderivGen->SetParLimits(1, 0.001, 0.05);
      fvdropderivGen->SetParameter(2, 10);//u that is the shift of the function due to Vfd ~ 2-3*u
	    fvdropderivGen->SetParLimits(2, 5,20);
      fvdropderivGen->SetParameter(3, 0.02);
      fvdropderivGen->SetParLimits(3, 0.01, 0.03);
      fvdropderivGen->SetParameter(4, 5);
      fvdropderivGen->SetParLimits(4, 1, 20);//coef for the exp

// Double_t fitfunctionderivGeneral(Double_t *x, Double_t *par){
//   Double_t value;

//   value = par[0]/sqrt(par[1])*atan(sqrt(par[1])*(x[0]-par[2]))+exp(par[3]*x[0]-par[4]);//the exponential can be divided by c
//   return value;
// }
	    fvdropderivGen->SetLineColor(9);//green
      fvdropderivGen->SetLineWidth(2);//green
      std::cout<<"Starting Fit of deriv withGen"<<std::endl;
      status = gderivative->Fit("fvdropderivGen", "Rsame")  ;
      std::cout<<"End of  Fit of deriv with Gen"<<std::endl;
	    cout<<"Fit status: "<<status;

	    if(fvdropderivGen->GetNDF()) cout<<" chi2/ndf: "<<fvdropderivGen->GetChisquare()/fvdropderivGen->GetNDF();
	    cout<<endl;
      float k = 0.90;
      Vfd = tan(k*3.14/2)/sqrt(fvdropderivGen->GetParameter(1))+fvdropderivGen->GetParameter(2);
      fvdropderivGen->Draw("same");

      if(fvdropderivGen->GetNDF())
      {
        if (fvdropderivGen->GetChisquare()/fvdropderivGen->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[8]!=0)
              {
                if (abs(VfdOutput[8]/COUNT[8]-Vfd)>detidDetection)
                  {
                    HotVfdOutput[8]+=Vfd;
                    HotCOUNT[8]++;
                    ofs << "Run : "<<RUN<<" for fit deriv Gen  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[8]/HotCOUNT[8]<<" and cold Mean Vfd : "<<VfdOutput[8]/COUNT[8]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip && status != 4)
              {
                VfdOutput[8]+=Vfd;
                DeltaVfdOutput[8]+= (Vfd-Vinit[idet]);
                COUNT[8]++;
              }
          }
      }
          
      TLine* lvdropderivGen = new TLine(Vfd, ymin_deriv,Vfd, ymax_deriv);
      lvdropderivGen->SetLineStyle(2);
      lvdropderivGen->SetLineColor(9);
      lvdropderivGen->SetLineWidth(2);
      lvdropderivGen->Draw("same");

      // c2->cd();
      // lvdropderiv = new TLine(fvdropderiv->GetParameter(2), ymin, fvdropderiv->GetParameter(2), ymax);
      // lvdropderiv->SetLineStyle(2);
      // lvdropderiv->SetLineColor(44);//green
      // lvdropderiv->SetLineWidth(2);
      // lvdropderiv->Draw("same");

  //



	//---------------------------//
  std::cout<<"Vfd from derivative : "<<Vfd<<std::endl;


//--------------------------------------------//
//-----------------Curvature-------------------//
//--------------------------------------------//
   
	  //TF1* fit = (TF1*) gIleak->GetListOfFunctions()->First();
	  TF1* fit = (TF1*) gROOT->GetFunction("fvdrop");
    //TF1* fit = (TF1*) gderivative->GetListOfFunctions()->First();
    //TF1* fit = 0;

    c2->cd();
    // lstart->DrawLine(xstart, ymin, xstart, ymax);
  	c2->Modified();
	  c2->Update();
    // c2->Print(Form("IleakVsVbias_%i_%s_detid_%i.pdf",npt, run, detid));

    // Find Kink
	
    TCanvas* c3 = new TCanvas("c3", "", 400, 0, 700, 500);
    TGraphErrors* gmedian = MedianFilter( gIleak );
    int nfilt=1;
    /*while (!IsMonoton(gmedian) && nfilt<4) {
      gmedian = MedianFilter( gmedian );
      nfilt++;
    }*/
	  //cout<<nfilt<<" median filter applied"<<endl;
    gmedian = HanningFilter(gmedian);
    //gmedian = SavitzkyGolaySmoother(gmedian, 5, 0);
	  gIleak->SetTitle(Form("Detid %i", detid));

	// Compute a voltage threshold for Kink finding algo
	// 30% of ymax
	gIleak->GetPoint(0,x,ymin);
	// cout<<"ymin: "<<ymin<<endl;
	double ythresh = ymin + (ymax-ymin)*0.3;
	double xthresh=0;
	for(int ipt=0; ipt<gmedian->GetN(); ipt++)
	  {
      gmedian->GetPoint(ipt, x, y);
	    if(y>ythresh)
	      {
		      ipt--;
		      gmedian->GetPoint(ipt, x, y);
		      xthresh = x;
		      ipt = gmedian->GetN();
	      }
	  }
	// cout<<"Voltage threshold (x) : "<<xthresh<<endl;

  gIleak->Draw("AP");
	

    TGraphErrors* gscurv = GetCurvatureGraph( gmedian );//GetDerivative(gmedian);
	  //gscurv = GetDerivative(gscurv);
    //gscurv = HanningFilter(gscurv);
	  gscurv->SetMarkerStyle(20);
	  TGraph* g3pts = new TGraph();
	  double *chi2 = new double;
	  double *ndf = new double;
	  double *n_rms = new double;
	  double *err = new double;
    float xopt = GetOptimalMinNPts(gscurv, g3pts, xthresh, 0,0, chi2, ndf, n_rms, err);//issue with the second 0

    TCanvas* c4 = new TCanvas("c4", "Curvature", 500, 0, 700, 500);
	  gscurv->Draw("AP");
	  g3pts->Draw("P");
	  g3pts->Fit("pol2", "q");
	
     ymin = gscurv->GetYaxis()->GetXmin();
     ymax = gscurv->GetYaxis()->GetXmax();

    TLine* lopt = new TLine(xopt, ymin, xopt, ymax);
    lopt->SetLineStyle(1);
    lopt->SetLineColor(1);//black
    lopt->Draw();
    cout<<endl<<"Kink : "<<xopt<<" V"<<endl;
	
    //------------------------------------------------------------//
    //------------------------------------------//
    //------------------------------------------------------------//
      
      // function with gaussian
    // these parameters are first defined for early run2
    // no second derivative for 2012
    float down = 5;
    float up = 360;
    float curvmax = 0.01;
    float curvmin = 0.0001;
    float mean = 200;
    float meanup = 300;
    float meandown = 40;
    if (subdet=="TIB")
      {

                                                                            
        if (  RUN.Contains("2023") ) {up = 200; down = 20;curvmax = 0.008; mean = 100;
                                      meandown = 50; meanup= 150;}
        if (  RUN.Contains("2022") ) {up = 200; down = 20;curvmax = 0.03; curvmin = 0.01;mean = 100;
                                      meandown = 50; meanup= 150;}
        if ( RUN.Contains("2021")  ) {up = 160; down = 20;curvmax = 0.01; mean = 100;
                                      meandown = 50; meanup= 150;}    

        if (RUN.Contains("2017") || RUN.Contains("2018") ) {up = 160; down = 20;curvmax = 0.01; mean = 100;
                                                            meandown = 50; meanup= 120;} 
      }
    if (subdet=="TOB")
      {                                                       
        if (  RUN.Contains("2023") ) {up = 250; down = 20;curvmax = 0.008; mean = 100;
                                      meandown = 50; meanup= 200;}
        if (  RUN.Contains("2022") ) {up = 200; down = 20;curvmax = 0.03; curvmin = 0.01;mean = 100;
                                      meandown = 50; meanup= 150;}
        if ( RUN.Contains("2021")  ) {up = 160; down = 20;curvmax = 0.01; mean = 100;
                                      meandown = 50; meanup= 150;}    
        if (RUN.Contains("2017") || RUN.Contains("2018") ) {up = 160; down = 20;curvmax = 0.015; mean = 100;
                                                           meandown = 50; meanup= 100;}  
        if (RUN.Contains("2016")  ) {up = 160; down = 20;curvmax = 0.05; mean = 100;
                                                           meandown = 50; meanup= 120;}                                
        if (RUN.Contains("2015")  ) {up = 200; down = 20;curvmax = 0.05; mean = 100;
                                                           meandown = 50; meanup= 150;}
        if (RUN == "20181115_run326776"){up = 200; down = 20;curvmax = 0.05; mean = 500;
                                                           meandown = 30; meanup= 80;}
      
      }

    TF1* fcurv = new TF1("fcurv", fitfunctioncurv, down, up, 4);
    fcurv->SetParameter(0, 0);//abscisse at v = 0
    fcurv->SetParameter(1, -0.005); //coef for the exp
    fcurv->SetParLimits(1, -curvmax, -curvmin);
    fcurv->SetParameter(2, mean);//mean
    fcurv->SetParLimits(2, meandown, meanup);
    fcurv->SetParameter(3, 20);//std dev
    fcurv->SetParLimits(3, 10, 100);
// }
	    fcurv->SetLineColor(2);//green
      fcurv->SetLineWidth(2);//green
    status = gscurv->Fit("fcurv", "Rsame")  ;
	  if(status==4) {fcurv->SetParameter(2, 100); status = gscurv->Fit("fcurv", "Rsame");}
	  if(fcurv->GetNDF()) cout<<" chi2/ndf: "<<fcurv->GetChisquare()/fcurv->GetNDF();
	  cout<<endl;
      fcurv->Draw("same");//green
    // float CurvVfd = fcurv->GetParameter(2);
    // if (  RUN.Contains("2022") ){CurvVfd = fcurv->GetMinimumX()}
      if(fcurv->GetNDF())
      {
        if (fcurv->GetChisquare()/fcurv->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[6]!=0)
              {
                if (abs(VfdOutput[6]/COUNT[6]-fcurv->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[6]+=fcurv->GetParameter(2);
                    HotCOUNT[6]++;
                    ofs << "Run : "<<RUN<<" for fit 2deriv gaussian  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[6]/HotCOUNT[6]<<" and cold Mean Vfd : "<<VfdOutput[6]/COUNT[6]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[6]+=fcurv->GetParameter(2);
                DeltaVfdOutput[6]+= (fcurv->GetParameter(2)-Vinit[idet]);
                COUNT[6]++;
              }
          }
      }

    float ymincurv =  gscurv->GetYaxis()->GetXmin();
    float ymaxcurv =  gscurv->GetYaxis()->GetXmax();
    TLine* lcurv = new TLine(fcurv->GetParameter(2), ymincurv, fcurv->GetParameter(2), ymaxcurv);
    lcurv->SetLineStyle(2);
    lcurv->SetLineColor(2);//green
    lcurv->SetLineWidth(2);
    lcurv->Draw("same");
    float VfdfromCurve = fcurv->GetParameter(2);
    if (  RUN.Contains("2022") ){VfdfromCurve = fcurv->GetMinimum();}
    std::cout<<"Vfd from curve : "<<fcurv->GetParameter(2)<<std::endl;

//test-paul----------


    TF1* fcurvGen = new TF1("fcurvGen", fitfunctioncurvGeneral, down, up, 5);
    fcurvGen->SetParameter(0, 0.005);//gaussian maximum
    fcurvGen->SetParLimits(0,curvmin,curvmax );//gaussian maximum
    fcurvGen->SetParameter(1, 0.005); //width of the gaussian0
    fcurvGen->SetParLimits(1, 0.001,0.1);
    fcurvGen->SetParameter(2, mean);//mean that should e the vfd
    fcurvGen->SetParLimits(2, meandown, meanup);
    fcurvGen->SetParameter(3, 0.05);//exponential growth coefficient
    fcurvGen->SetParLimits(3, 0, 0.1);
    fcurvGen->SetParameter(4, 7);//plateau coefficient
    fcurvGen->SetParLimits(4, 5, 10);
// Double_t fitfunctioncurvGeneral(Double_t *x, Double_t *par){
//   Double_t value;
//     value = -par[0]/(1+par[1]*(x[0]-par[2])*(x[0]-par[2])) +exp(par[3]*x[0]-par[4]); // gaus
//   //  \frac{-a}{1+b\left(x-u\right)^{2}}+\exp\left(cx-d\right)
//   return value;
// }
    status = gscurv->Fit("fcurvGen", "Rsame")  ;
	  if(fcurvGen->GetNDF()) cout<<" chi2/ndf: "<<fcurvGen->GetChisquare()/fcurvGen->GetNDF();
	  cout<<endl;
    	fcurvGen->SetLineColor(44);//green
      fcurvGen->SetLineWidth(2);//green
      fcurvGen->Draw("same");//green
    // float CurvVfd = fcurv->GetParameter(2);
    // if (  RUN.Contains("2022") ){CurvVfd = fcurv->GetMinimumX()}
      if(fcurvGen->GetNDF())
      {
        if (fcurvGen->GetChisquare()/fcurvGen->GetNDF()<5)
          {
            bool skip = false;
            if (COUNT[9]!=0)
              {
                if (abs(VfdOutput[9]/COUNT[9]-fcurvGen->GetParameter(2))>detidDetection)
                  {
                    HotVfdOutput[9]+=fcurvGen->GetParameter(2);
                    HotCOUNT[9]++;
                    ofs << "Run : "<<RUN<<" for fit 2deriv Gen  with Hot detid : "<<detid<<std::endl;
                    ofs << "Hot Mean Vfd "<<HotVfdOutput[9]/HotCOUNT[9]<<" and cold Mean Vfd : "<<VfdOutput[9]/COUNT[9]<<std::endl;
                    // skip = true;
                  }
              }
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            if (!skip)
              {
                VfdOutput[9]+=fcurvGen->GetParameter(2);
                DeltaVfdOutput[9]+= (fcurvGen->GetParameter(2)-Vinit[idet]);
                COUNT[9]++;
              }
          }
      }


    TLine* lcurvGen = new TLine(fcurvGen->GetParameter(2),  ymincurv, fcurvGen->GetParameter(2),ymaxcurv);
    lcurvGen->SetLineStyle(2);
    lcurvGen->SetLineColor(11);//green
    lcurvGen->SetLineWidth(2);
    lcurvGen->Draw("same");
    VfdfromCurve = fcurvGen->GetParameter(2);
    std::cout<<"Vfd from curveGen : "<<fcurvGen->GetParameter(2)<<std::endl;

    //-----------------


  //------------------------------------------//

	  c3->cd();
	  fvdrop->Draw("same");
    fvdrop3->Draw("same");
	  TLine* lopt2 = new TLine(xopt, gIleak->GetYaxis()->GetXmin(),
	                          xopt, gIleak->GetYaxis()->GetXmax());
    //lopt2->Draw();
    lvdrop->Draw();
    // lvdrop3->Draw();

  	c3->Modified();
	  c3->Update();
    // c4->Print(Form("IleakCurvatureHisto_%i_%s_detid_%i.pdf",npt, run, detid));



    // c3->Print(Form("IleakVsVbias_%s_detid_%i.pdf", run, detid));
    //c4->Print(Form("IleakVsVbias_curv_%s_detid_%i.pdf", run, detid));
	
	//-----------------------------------//
	
	// STORING RESULTS

	odetid = detid;

	Int_t detid_forlayer = detid;
    if(subdet=="TOB") detid_forlayer = detid/10;
	subdet_i = (detid_forlayer>>kSubdetOffset)&0x7;
	// For TIB and TOB
    if(subdet_i==3 || subdet_i==5) olayer = ((detid_forlayer>>layerStartBit_) & layerMask_);
	
  //   if(subdetid==3)  // TIB
  // {
  //   if(((detid>>14)&0x7)==1) return 1;
  //   else if(((detid>>14)&0x7)==2) return 2;
  //   else if(((detid>>14)&0x7)==3) return 3;
  //   else if(((detid>>14)&0x7)==4) return 4;
  // }    
  // else if(subdetid==5) // TOB
  // {       
  //   if(((detid>>14)&0x7)==1) return 5;
  //   else if(((detid>>14)&0x7)==2) return 6;
  //   else if(((detid>>14)&0x7)==3) return 7;
  //   else if(((detid>>14)&0x7)==4) return 8;
  //   else if(((detid>>14)&0x7)==5) return 9;
  //   else if(((detid>>14)&0x7)==6) return 10;
  // }   

    // Store fit result
    if(fit)
    {
	    if(fit->GetNDF()>1)
        {
          if(fit->GetChisquare()/fit->GetNDF() < 50.)//5
            {
              fout->cd();
              cout<<"Vdepl: "<<fit->GetParameter(2)<<" V"<<endl;
              cout<<"Storing fit for detid "<<detid<<endl;
              fit->SetName(Form("fit_%i", detid));
              fit->Write();
              gIleak->SetName(Form("Ileak%i", detid));
              gIleak->Write();
              if(fit->GetNDF()) hchi2->Fill(fit->GetChisquare()/fit->GetNDF());
              hparam0->Fill(fit->GetParameter(0));
              hparam1->Fill(fit->GetParameter(1));
              hparam2->Fill(fit->GetParameter(2));
              hparam3->Fill(fit->GetParameter(3));
              ifit++;
		  		  
		          output->cd();
		  
    	        //odepvolt = fit->GetParameter(2);
		          //oerrdepvolt = fit->GetParError(2);
    	        odepvolt = xstart;
		          oerrdepvolt = xstart_err;

    	        oplateau = 0;
    	        ofitchisquare = fit->GetChisquare()/fit->GetNDF();
    	        ofitstatus = status;
    	        olastpty = lastpty;
    	        ochi2 = 0;
		          tout->Fill();
            }
        }
    }
	  else
	    {
	      output->cd();
        odepvolt = xopt;
	      if(err) oerrdepvolt = *err;
	      else oerrdepvolt = 0;
        oplateau = 0;
        ofitchisquare = *chi2;
        ofitstatus = *ndf;
        olastpty = lastpty;
        ochi2 = *n_rms;
	      tout->Fill();
	    }
    std::cout<<"Run : "<<RUN<<std::endl;
	
    c1->Modified();
    c1->Update();
    //c1->Print(Form("Ileak-Vbias_%s_%i.pdf", run, detid));
    c2->Modified();
    c2->Update();
    //c2->Print(Form("IleakEffect_%s_%i.pdf", run, detid));
    cd->Modified();
    cd->Update();
    // cd2->Modified();
    // cd2->Update();
    c3->Modified();
    c3->Update();
    c4->Modified();
    c4->Update();
    // getchar();//to desactivate when running on all the modules and all the runs
    
 
    delete fvdrop;
    delete fvdrop3;
	  delete hd;
	  delete gderivative;
    delete gmedian;
    delete gscurv;
    delete g3pts;

    delete c1;
    delete c2;
    delete cd;
    // delete cd2;
    delete c3;
    delete c4;
      std::cout<<" Vinit[idet] : "<<Vinit[idet]<<std::endl;
  } // End of loop over modules

  
  for (unsigned int i = 0 ; i < VfdOutput.size() ; i++)
    {
      if (VfdOutput[i]==0) continue;
      else VfdOutput[i]=VfdOutput[i]/COUNT[i];
    }

  for (unsigned int i = 0 ; i < DeltaVfdOutput.size() ; i++)
    {
      if (DeltaVfdOutput[i]==0) continue;
      else DeltaVfdOutput[i]=DeltaVfdOutput[i]/COUNT[i];
    }


  for (unsigned int i = 0 ; i < HotVfdOutput.size() ; i++)
    {
      if (HotVfdOutput[i]==0) continue;
      else HotVfdOutput[i]=HotVfdOutput[i]/HotCOUNT[i];
    }

    ofs <<"//----------------------------//"<<std::endl;
    ofs.close();
    VfdData = std::make_pair(VfdOutput,DeltaVfdOutput);



  
  // Write output root file
  cout<<"Chi2/ndf mean : "<<hchi2->GetMean()<<endl;
  cout<<"param0 : "<<hparam0->GetMean()<<" +/- "<<hparam0->GetRMS()<<endl;
  cout<<"param1 : "<<hparam1->GetMean()<<" +/- "<<hparam1->GetRMS()<<endl;
  cout<<"param2 : "<<hparam2->GetMean()<<" +/- "<<hparam2->GetRMS()<<endl;
  cout<<"param3 : "<<hparam3->GetMean()<<" +/- "<<hparam3->GetRMS()<<endl;
  fout->cd();
  hchi2->Write();
  hparam0->Write();
  hparam1->Write();
  hparam2->Write();
  hparam3->Write();
  fout->Close();
  
  output->cd();
  tout->Write();
  //gDirectory->Write();
  output->Close();
  return VfdData;//returns the mean value for each fit methods for a given run
}

//------------------------------------------------------------------------------

std::pair<std::vector<std::vector<float>>,std::vector<std::vector<float>>> FitLeakageCurrent(char* subdet="TIB",  char* run="20230907_run373060", int LAY=1 , bool Small = true)
{

  // Many points needed, works well only for august 2012 scan


  int ID;
  float vinit;
  const int N_test=1;
  std::string line;
  int cnt = 0;
  const int NFit = 10;
  std::vector<std::vector<float>> VfdFits;
  std::vector<std::vector<float>> DeltaVfdFits;
  std::pair<std::vector<std::vector<float>>,std::vector<std::vector<float>>> VfdPair;

  std::pair<std::vector<float>,std::vector<float>> Fit0;
  std::pair<std::vector<float>,std::vector<float>> Fit1;
  std::pair<std::vector<float>,std::vector<float>> Fit2;
  std::pair<std::vector<float>,std::vector<float>> Fit3;
  std::pair<std::vector<float>,std::vector<float>> Fit4;
  std::pair<std::vector<float>,std::vector<float>> Fit5;
  std::pair<std::vector<float>,std::vector<float>> Fit6;
  std::pair<std::vector<float>,std::vector<float>> Fit7;
  std::pair<std::vector<float>,std::vector<float>> Fit8;
  std::pair<std::vector<float>,std::vector<float>> Fit9;
  std::pair<std::vector<float>,std::vector<float>> Fit10;
  std::pair<std::vector<float>,std::vector<float>> Fit11;
  std::pair<std::vector<float>,std::vector<float>> Fit12;
  std::pair<std::vector<float>,std::vector<float>> Fit13;

  // 2012
  if (Small)
    {
      if (subdet == "TIB")
        {
          const int N_2012=18;
          int detids_2012[N_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
          369121613,369121614,369125861,369125862,369125865,369125866,369125869,369125870}; 
          float  Vinit[N_2012] = {220,230,270,260,270,270,280,270,270,260,231.71,245.52,280,280,279.71,270,280,270};

          Fit0 = Fit("TIB", "20120506_run193541", LAY , detids_2012, N_2012, "",Vinit);
          Fit1 = Fit("TIB", "20151121_run262254", LAY , detids_2012, N_2012, "",Vinit);
          Fit2 = Fit("TIB", "20160706_run276437", LAY , detids_2012, N_2012, "",Vinit);
          Fit3 = Fit("TIB", "20160909_run280385", LAY , detids_2012, N_2012, "",Vinit);
          Fit4 = Fit("TIB", "20171030_run305862", LAY , detids_2012, N_2012, "",Vinit);
          Fit5 = Fit("TIB", "20180530_run317182", LAY , detids_2012, N_2012, "",Vinit);
          Fit6 = Fit("TIB", "20180611_run317683", LAY , detids_2012, N_2012, "",Vinit);
          Fit7 = Fit("TIB", "20181018_run324841", LAY , detids_2012, N_2012, "",Vinit);
          Fit8 = Fit("TIB", "20181115_run326776", LAY , detids_2012, N_2012, "",Vinit);
          Fit9 = Fit("TIB", "20211029_run346395", LAY , detids_2012, N_2012, "",Vinit);
          Fit10 = Fit("TIB", "20220605_run353060", LAY , detids_2012, N_2012, "",Vinit);
          Fit11 = Fit("TIB", "20230407_run365843", LAY , detids_2012, N_2012, "",Vinit);
          Fit12 = Fit("TIB", "20230609_run368669", LAY , detids_2012, N_2012, "",Vinit);
          Fit13 = Fit("TIB", "20230907_run373060", LAY , detids_2012, N_2012, "",Vinit);

            VfdFits.push_back(Fit0.first);
            VfdFits.push_back(Fit1.first);
            VfdFits.push_back(Fit2.first);
            VfdFits.push_back(Fit3.first);
            VfdFits.push_back(Fit4.first);
            VfdFits.push_back(Fit5.first);
            VfdFits.push_back(Fit6.first);
            VfdFits.push_back(Fit7.first);
            VfdFits.push_back(Fit8.first);
            VfdFits.push_back(Fit9.first);
            VfdFits.push_back(Fit10.first);//Full
            VfdFits.push_back(Fit11.first);//FUll
            VfdFits.push_back(Fit12.first);
            VfdFits.push_back(Fit13.first);


            DeltaVfdFits.push_back(Fit0.second);
            DeltaVfdFits.push_back(Fit1.second);
            DeltaVfdFits.push_back(Fit2.second);
            DeltaVfdFits.push_back(Fit3.second);
            DeltaVfdFits.push_back(Fit4.second);
            DeltaVfdFits.push_back(Fit5.second);
            DeltaVfdFits.push_back(Fit6.second);
            DeltaVfdFits.push_back(Fit7.second);
            DeltaVfdFits.push_back(Fit8.second);
            DeltaVfdFits.push_back(Fit9.second);
            DeltaVfdFits.push_back(Fit10.second);//Full
            DeltaVfdFits.push_back(Fit11.second);//FUll
            DeltaVfdFits.push_back(Fit12.second);
            DeltaVfdFits.push_back(Fit13.second);

            for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
              {
                for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
                  {
                    std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
                  }
              }
            VfdPair = make_pair(VfdFits,DeltaVfdFits);
            return VfdPair;
        }
      	     
      if (subdet == "TOB")
        {


          const int N_TOB_2012=18;//18
          int detids_TOB_2012[N_TOB_2012]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528,
                                            436232901, 436232902, 436232905, 436232906, 436232909, 436232910,
                                            436232913, 436232914, 436232917, 436232918, 436232921, 436232922};
          float  Vinit[N_TOB_2012] = {216.5,210,244.5,217.565,230,230.5,
                                      190,190,180,200,170,195,
                                      190,171,190,190,185,155};


          Fit0 = Fit("TOB", "20120506_run193541", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit1 = Fit("TOB", "20151121_run262254", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit2 = Fit("TOB", "20160706_run276437", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit3 = Fit("TOB", "20160909_run280385", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit4 = Fit("TOB", "20171030_run305862", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit5 = Fit("TOB", "20180530_run317182", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit6 = Fit("TOB", "20180611_run317683", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit7 = Fit("TOB", "20181018_run324841", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit8 = Fit("TOB", "20181115_run326776", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit9 = Fit("TOB", "20211029_run346395", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit10 = Fit("TOB", "20220605_run353060", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit11 = Fit("TOB", "20230407_run365843", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit12 = Fit("TOB", "20230609_run368669", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);
          Fit13 = Fit("TOB", "20230907_run373060", LAY , detids_TOB_2012, N_TOB_2012, "",Vinit);

            VfdFits.push_back(Fit0.first);
            VfdFits.push_back(Fit1.first);
            VfdFits.push_back(Fit2.first);
            VfdFits.push_back(Fit3.first);
            VfdFits.push_back(Fit4.first);
            VfdFits.push_back(Fit5.first);
            VfdFits.push_back(Fit6.first);
            VfdFits.push_back(Fit7.first);
            VfdFits.push_back(Fit8.first);
            VfdFits.push_back(Fit9.first);
            VfdFits.push_back(Fit10.first);//Full
            VfdFits.push_back(Fit11.first);//FUll
            VfdFits.push_back(Fit12.first);
            VfdFits.push_back(Fit13.first);

            DeltaVfdFits.push_back(Fit0.second);
            DeltaVfdFits.push_back(Fit1.second);
            DeltaVfdFits.push_back(Fit2.second);
            DeltaVfdFits.push_back(Fit3.second);
            DeltaVfdFits.push_back(Fit4.second);
            DeltaVfdFits.push_back(Fit5.second);
            DeltaVfdFits.push_back(Fit6.second);
            DeltaVfdFits.push_back(Fit7.second);
            DeltaVfdFits.push_back(Fit8.second);
            DeltaVfdFits.push_back(Fit9.second);
            DeltaVfdFits.push_back(Fit10.second);//Full
            DeltaVfdFits.push_back(Fit11.second);//FUll
            DeltaVfdFits.push_back(Fit12.second);
            DeltaVfdFits.push_back(Fit13.second);


            //     // Missign leakae current information
            //       // VfdFits.push_back(Fit("TOB", "20231025_run375658", LAY , detids_2012, N_TOB_2012, ""));

            for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
              {
                for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
                  {
                    std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
                  }
              }
            VfdPair = make_pair(VfdFits,DeltaVfdFits);
            return VfdPair;
        }

    }
 
  else
    {
      if (subdet == "TIB")
        {
            const int N_Full = 2720;
            int Full_detid[N_Full]= {0};
            float Full_Vinit[N_Full]= {0};
            //--------------Full Scans -----------------//
            std::ifstream input( "TIB_list.txt" );
            while (std::getline(input, line) )
              {
                
                std::istringstream iss(line);
                if (!(iss >> ID >> vinit)) { break; } // error
                if (369175436 < ID ){break;} 
                Full_detid[cnt]=ID;
                Full_Vinit[cnt]= vinit;
                cnt+=1;

              }
          
            Fit0 = Fit("TIB", "20120510_run193928", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit1 = Fit("TIB", "20160423_run271056", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit2 = Fit("TIB", "20170527_run295376", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit3 = Fit("TIB", "20170924_run303824", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit4 = Fit("TIB", "20180418_run314574", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit5 = Fit("TIB", "20180923_run323370", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit6 = Fit("TIB", "20220605_run353060", LAY, Full_detid, N_Full, "",Full_Vinit);
            Fit7 = Fit("TIB", "20230407_run365843", LAY, Full_detid, N_Full, "",Full_Vinit);

            VfdFits.push_back(Fit0.first);
            VfdFits.push_back(Fit1.first);
            VfdFits.push_back(Fit2.first);
            VfdFits.push_back(Fit3.first);
            VfdFits.push_back(Fit4.first);
            VfdFits.push_back(Fit5.first);
            VfdFits.push_back(Fit6.first);
            VfdFits.push_back(Fit7.first);

            DeltaVfdFits.push_back(Fit0.second);
            DeltaVfdFits.push_back(Fit1.second);
            DeltaVfdFits.push_back(Fit2.second);
            DeltaVfdFits.push_back(Fit3.second);
            DeltaVfdFits.push_back(Fit4.second);
            DeltaVfdFits.push_back(Fit5.second);
            DeltaVfdFits.push_back(Fit6.second);
            DeltaVfdFits.push_back(Fit7.second);

            for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
              {
                for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
                  {
                    std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
                  }
              }
            VfdPair = make_pair(VfdFits,DeltaVfdFits);
            return VfdPair;
            //------------------------------------------------//
            // Missign leakae current information
            // VfdFits.push_back(Fit("TIB", "20231025_run375658", detids_2012, N_2012, ""));
        }
      if (subdet == "TOB")
        {
          std::ifstream input( "TOB_list.txt" );//TOB_list
          const int N_FullTOB_2012=5057;//
          int detids_FullTOB_2012[N_FullTOB_2012]={0};
          float Full_Vinit[N_FullTOB_2012]= {0};
            //--------------Full Scans -----------------//
            bool SKIP = false;
          while (std::getline(input, line) )
              {
                std::istringstream iss(line);
                if (!(iss >> ID >> vinit)) { break; } // error
                if (436228133 > ID || ID > 436316504){SKIP = true;} 
                if (!SKIP)
                  {
                    detids_FullTOB_2012[cnt]=ID;
                    Full_Vinit[cnt]= vinit;
                    cnt+=1;
                  }
 
              }

            Fit0 = Fit("TOB", "20120510_run193928", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit1 = Fit("TOB", "20160423_run271056", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit2 = Fit("TOB", "20170527_run295376", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit3 = Fit("TOB", "20170924_run303824", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit4 = Fit("TOB", "20180418_run314574", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit5 = Fit("TOB", "20180923_run323370", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit6 = Fit("TOB", "20220605_run353060", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);
            Fit7 = Fit("TOB", "20230407_run365843", LAY, detids_FullTOB_2012, N_FullTOB_2012, "",Full_Vinit);

            VfdFits.push_back(Fit0.first);
            VfdFits.push_back(Fit1.first);
            VfdFits.push_back(Fit2.first);
            VfdFits.push_back(Fit3.first);
            VfdFits.push_back(Fit4.first);
            VfdFits.push_back(Fit5.first);
            VfdFits.push_back(Fit6.first);
            VfdFits.push_back(Fit7.first);

            DeltaVfdFits.push_back(Fit0.second);
            DeltaVfdFits.push_back(Fit1.second);
            DeltaVfdFits.push_back(Fit2.second);
            DeltaVfdFits.push_back(Fit3.second);
            DeltaVfdFits.push_back(Fit4.second);
            DeltaVfdFits.push_back(Fit5.second);
            DeltaVfdFits.push_back(Fit6.second);
            DeltaVfdFits.push_back(Fit7.second);

          for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
            {
              for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
                {
                  // FitsVfd[j]+=RunVfd[i][j];
                  std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
                }
            }
          VfdPair = make_pair(VfdFits,DeltaVfdFits);
          return VfdPair;
        }  
    }
  
  



  // Fit("TIB", "20120405_run190459", detids_2012, N_2012); // lack stats
  // Fit("TIB", "20120506_run193541", detids_2012, N_2012);// PS current  not in DB
  
  const int N_1PS=6;
  int detids_1PS[N_1PS]={369121605,369121606,369121609,369121610,369121613,369121614};  
  //Fit("TIB_L1", "20120928_run203832", detids_1PS, N_1PS, "Steps/bad_periods_20120928_run203832.txt");
  // Fit("TIB_L1", "20120928_run203832", detids_2012, N_2012, "Steps/bad_periods_20120928_run203832.txt");

//---------run to analyse------------//

// // Fit("TIB", "20181118_run326883", detids_2012, N_2012, "");//there is an outlier


//--------end of -run to analyse------------//

////////////////////////////////////////////
//////////////////////////////////////////

//Noise bias scan- ------------//
  // Fit("TIB", "noise_20160914_run280667", detids_test, N_test, ""); // use points between steps
  // Fit("TIB", "noise_20160914_run280667", detids_test, N_test, "");
  // Fit("TIB", "noise_20170919_run303272", detids_test, N_test, "Steps/bad_periods_noise_20170919_run303272.txt"); // with smoothing
  // Fit("TIB", "noise_20171205_run307585", detids_test, N_test, "");
  // Fit("TIB", "noise_20180618_run317974", detids_test, N_test, "");
  // Fit("TIB", "noise_20180919_run323011", detids_test, N_test, "");
  // Fit("TIB", "noise_20190321_run328691", detids_test, N_test, "");
  // Fit("TIB", "noise_20190920_run331595", detids_test, N_test, "");

  //--------------------------------------//
  const int N_hot=2;
  int detids_hot[N_hot]={369124397, 369124414/*, 369124422, 369124662, 369124666, 369124694, 369124710, 369124726,
  369124774, 369125870*/};


  //---------------------------------//
  //---------------------------------//
  //               TOB               //
  //---------------------------------//
  //---------------------------------//
  const int N_TOB=6;
  int detids_TOB[N_TOB]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528};
  


  // const int N_TOB_2012_R3=12;//18
  // int detids_TOB_2012[N_TOB_2012]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528,
  //                        436232901, 436232902, 436232905, 436232906, 436232909, 436232910,
	// 					 436232913, 436232914, 436232917, 436232918, 436232921, 436232922};




  // Fit("TOB", "20180418_run314574", detids_TOB_2012, N_TOB_2012, "");

  // TID
  //-----
  const int N_TID=4;
  int detids_TID[N_TID]={402668829, 402672930, 402677025, 402677041};
  // Fit("TID", "20180418_run314574", detids_TID, N_TID, "");
  const int N_TID_m4=8;
  // TIDminus_4_2_1_X
  int detids_TID_m4[N_TID_m4]={402666257, 402666258, 402666269, 402666270, 402666125, 402666126, 402666137, 402666138};
  // TIDminus_4_2_2_X
  //int detids_TID_m4[N_TID_m4]={402666777, 402666778, 402666789, 402666790, 402666629, 402666630, 402666633, 402666634};
  // Fit("TID", "20230407_run365843", detids_TID_m4, N_TID_m4, "");
  //Fit("TID", "20230407_run365843", detids_TID_m4, N_TID_m4, "Steps/bad_periods_20230407_run365843.txt");
// Fit("TID", "20230407_run365843", detids_TID, N_TID, "");
  // TEC
  //------
  const int N_TEC=5;//20
  int detids_TEC[N_TEC]={470148196, 470148200, 470148204, 470148228, 470148232/*, 470148236, 470148240, 470148292, 470148296, 470148300, 470148304, 470148261, 470148262, 470148265, 470148266, 470148324, 470148328, 470148332, 470148336, 470148340*/};

  // Fit("TEC", "20180418_run314574", detids_TEC, N_TEC, "");
  //Fit("TEC", "20180418_run314574", detids_TEC, N_TEC, "");

//   Fit("TEC", "20181118_run326883", detids_TEC, N_TEC, "");
// Fit("TEC", "20181115_run326776", detids_TEC, N_TEC, "");
// Fit("TEC", "20181018_run324841", detids_TEC, N_TEC, "");
// Fit("TEC", "20180611_run317683", detids_TEC, N_TEC, "");
// Fit("TEC", "20180530_run317182", detids_TEC, N_TEC, "");
// Fit("TIB", "20181115_run326776", detids_test, N_test, "");

// Fit("TEC", "20230407_run365843", detids_TEC, N_TEC, "");

}

int PlotMeanVfdPerRunwPerFit (std::vector<std::vector<float>> VFD, std::vector<float> LUMI,TString subdet , TString LAY, bool SMALL) 
  {
    if (VFD.size()==0)// number of runs
      {
        std::cout<<"Number of Runs to be analyzed is 0. Please look a the FitLeakageCurrent function"<<std::endl;
        return 0;
      }
    TCanvas* c1 = new TCanvas("c1","c1", 1000, 800);
    const unsigned int Npts = VFD.size();// number of runs
    const int Nfit = 10;
    std::vector<std::vector<float>> FitX ;
    std::vector<float> fitx;
    for (unsigned int i = 0; i < Nfit ; i++)
      {
        for (unsigned int j = 0 ; j < Npts; j++)
          {
            fitx.push_back(VFD[j][i]);
            // FitX.push_back({VFD[0][0],VFD[1][0],VFD[2][0],VFD[3][0]});
          }
          FitX.push_back(fitx);
          fitx.clear();
      }

    float *graphy[Nfit];
    float *graphx  = LUMI.data();
    TGraph *gX[Nfit];

    for (unsigned int i = 0 ; i < Nfit ; i++)
      {
        graphy[i] = FitX[i].data();
        gX[i] = new TGraph(Npts,graphx,graphy[i]);
        gX[i]->SetLineWidth(2);
        gX[i]->SetMarkerColor(4);
        gX[i]->SetMarkerSize(1.5);
        gX[i]->SetMarkerStyle(21);
      }
    gX[7]->SetLineStyle(2);
    gX[7]->SetLineColor(1);

    gX[0]->SetLineStyle(1);
    gX[0]->SetLineColor(1);
    gX[2]->SetLineStyle(5);
    gX[2]->SetLineColor(1);

    gX[1]->SetLineStyle(9);
    gX[1]->SetLineColor(4);
    gX[3]->SetLineStyle(10);
    gX[3]->SetLineColor(4);

    gX[4]->SetLineStyle(5);
    gX[4]->SetLineColor(8);
    gX[5]->SetLineStyle(1);
    gX[5]->SetLineColor(8);
    gX[8]->SetLineStyle(2);
    gX[8]->SetLineColor(8);

    gX[6]->SetLineStyle(1);
    gX[6]->SetLineColor(46);
    gX[9]->SetLineStyle(2);
    gX[9]->SetLineColor(46);

    for (unsigned int i = 0 ; i < Nfit ; i++)
      {
        if ( i == 0) {gX[i]->Draw("AL");}
        else {gX[i]->Draw("SAME");}
      }
    gX[0]->SetTitle("Full Depletion Voltage Average");
    gX[0]->GetXaxis()->SetTitle("Int. Lumi [fb^{-1}]");
    gX[0]->GetYaxis()->SetTitle("Full Depletion Voltage [V]");
    gX[0]->GetXaxis()->SetRangeUser(0,280);
    gX[0]->SetMaximum(350);
    gX[0]->SetMinimum(0);

    TLegend *leg = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg->SetTextFont(42);
    leg->SetHeader("Fitting Methods");
    leg->AddEntry(gX[7], "General", "l");
    leg->AddEntry(gX[0], "sigmoid + pol1", "l");
    leg->AddEntry(gX[2], "sigmoid + pol2", "l");
    leg->AddEntry(gX[1], "atan + pol1", "l");
    leg->AddEntry(gX[3], "atan + pol2", "l");

    leg->AddEntry(gX[8], "1st Order Deriv : General", "l");
    leg->AddEntry(gX[4], "1st Order Deriv : exp or gaussian", "l");
    leg->AddEntry(gX[5], "1st Order Deriv : Trilinear", "l");

    leg->AddEntry(gX[9], "2nd Order Deriv.:  General", "l");
    leg->AddEntry(gX[6], "2nd Order Deriv.:  Gaussian", "l");


    leg->Draw("SAME");

        TLatex *t = new TLatex(5,355,"CMS");
        t->SetTextFont(61);
        t->SetTextAlign(11);
        float fac = c1->GetTopMargin();
        t->SetTextSize(fac*0.5);
        t->Draw();

      //  TLatex *t2 = new TLatex();
      //  t2->SetTextFont(52);
      //  t2->SetTextAlign(11);
      //  t2->SetTextSize(0.5);
      //  t2->DrawLatex(18,310," L_{int} = 260 fb^{-1}");

        TLatex *t3 = new TLatex(37.5,355,"Preliminary");
        t3->SetTextFont(52);
        t3->SetTextAlign(11);
        t3->SetTextSize(fac*0.3);
        t3->Draw();
        TString text = "Tracker Inner Barrel Layer L"+LAY;
        float dx = 174;
        if (subdet=="TOB") {text = "Tracker Outer Barrel L"+LAY;dx = 195;}

        TLatex *t4 = new TLatex(dx,355,text);
        t4->SetTextFont(61);
        t4->SetTextAlign(11);
        t4->SetTextSize(fac*0.3);
        t4->Draw();

    TLine* lvdrop = new TLine(195, 0, 195, 260);
    lvdrop->SetLineStyle(6);
    lvdrop->SetLineColor(1);
    lvdrop->Draw("SAME");

    TLine* lvdrop2 = new TLine(30, 0, 30, 350);
    lvdrop2->SetLineStyle(6);
    lvdrop2->SetLineColor(1);
    lvdrop2->Draw("SAME");


    TGraph* g_simu = 0;

  //cout<<"--- Superimpose simulation : Fluence --> Lumi"<<endl;

  int choice_simu =5 ; // 5 is the most up-to-date choise (Paul development).
  //  It comes from the  /afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/ envrionment
  //where predictions where made for differnet scenarios of lumi temperature and dismantling time.
  // Predictions have been made for run3 or Run 3 and dismantling
  TString dirname="/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/DECO_files_kink_range_fix";
  bool superimpose_simu = true;
  TString FILE = "";
  if(superimpose_simu)
  {
      if(subdet != "TIB" && choice_simu == 0) {cout<<"Can only plot this simulation for TIB ! Abort"<<endl; return 0;}
      TString simufile_name = "";
      if(choice_simu == 0) {simufile_name = dirname+"/simulation/VdepGraphs_TIB_2017.root";}
      else if(choice_simu == 1) {simufile_name = dirname+"/simulation/Simu_JLA_v1.root";}
      else if(choice_simu == 2) {simufile_name = dirname+"/simulation/Simu_JLA_v2.root";}
      else if(choice_simu == 3) {simufile_name = dirname+"/simulation/Graph_new_nico_thesis.root";}
      else if(choice_simu == 4) {simufile_name = dirname+"/simulation/VdepGraphs_2022.root";}
      else if(choice_simu == 5) 
        {
          if (subdet == "TIB")
            {
              simufile_name = "/afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/lumigr_TIB_L"+LAY+".root";
              FILE = "lumigr_TIB_L"+LAY;
            }
          if (subdet == "TOB")
            {
              simufile_name = "/afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/lumigr_TOB_L"+LAY+".root";
              FILE = "lumigr_TOB_L"+LAY;
            }

        }

    TFile* file = new TFile(simufile_name, "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << file << std::endl;
        return 0;
    }

      TGraph* histogram = nullptr;
      histogram = dynamic_cast<TGraph*>(file->Get(FILE));//lumigr_TIB_L1.png

    if (!histogram) {
        std::cerr << "Error: Histogram not found in the file." << std::endl;
        file->Close();
        return 0;
    }
    std::cout<<"Obtain Simugraph "<<std::endl;
    histogram->Draw("SAME");
    histogram->SetLineColor(kRed);
    leg->AddEntry(histogram, "Prediction", "L");
        
  }

    TString save_text = "TIB_L"+LAY;
    if (subdet=="TOB") {save_text = "TOB_L"+LAY;}
    if (SMALL){save_text = save_text+"SMALLSCAN";}
    else {save_text = save_text+"FULLSCAN";}
    c1->SaveAs(save_text+".root");
    c1->SaveAs(save_text+".pdf");
       
    delete c1;
    return 1;
  }


int PlotDeltaMeanVfdPerRunPerFit (std::vector<std::vector<float>> VFD, std::vector<float> LUMI,TString subdet , TString LAY, bool SMALL) 
  {
    if (VFD.size()==0)// number of runs
      {
        std::cout<<"Number of Runs to be analyzed is 0. Please look a the FitLeakageCurrent function"<<std::endl;
        return 0;
      }
    TCanvas* c1 = new TCanvas("c1","c1", 1000, 800);
    const unsigned int Npts = VFD.size();// number of runs
    const int Nfit = 10;
    std::vector<std::vector<float>> FitX ;
    std::vector<float> fitx;
    for (unsigned int i = 0; i < Nfit ; i++)
      {
        for (unsigned int j = 0 ; j < Npts; j++)
          {
            fitx.push_back(VFD[j][i]);
            // FitX.push_back({VFD[0][0],VFD[1][0],VFD[2][0],VFD[3][0]});
          }
          FitX.push_back(fitx);
          fitx.clear();
      }

    float *graphy[Nfit];
    float *graphx  = LUMI.data();
    TGraph *gX[Nfit];

    for (unsigned int i = 0 ; i < Nfit ; i++)
      {
        graphy[i] = FitX[i].data();
        gX[i] = new TGraph(Npts,graphx,graphy[i]);
        gX[i]->SetLineWidth(2);
        gX[i]->SetMarkerColor(4);
        gX[i]->SetMarkerSize(1.5);
        gX[i]->SetMarkerStyle(21);
      }
    gX[7]->SetLineStyle(2);
    gX[7]->SetLineColor(1);

    gX[0]->SetLineStyle(1);
    gX[0]->SetLineColor(1);
    gX[2]->SetLineStyle(5);
    gX[2]->SetLineColor(1);

    gX[1]->SetLineStyle(9);
    gX[1]->SetLineColor(4);
    gX[3]->SetLineStyle(10);
    gX[3]->SetLineColor(4);

    gX[4]->SetLineStyle(5);
    gX[4]->SetLineColor(8);
    gX[5]->SetLineStyle(1);
    gX[5]->SetLineColor(8);
    gX[8]->SetLineStyle(2);
    gX[8]->SetLineColor(8);

    gX[6]->SetLineStyle(1);
    gX[6]->SetLineColor(46);
    gX[9]->SetLineStyle(2);
    gX[9]->SetLineColor(46);

    for (unsigned int i = 0 ; i < Nfit ; i++)
      {
        if ( i == 0) {gX[i]->Draw("AL");}
        else {gX[i]->Draw("SAME");}
      }
    gX[0]->SetTitle("Delta Full Depletion Voltage Average");
    gX[0]->GetXaxis()->SetTitle("Int. Lumi [fb^{-1}]");
    gX[0]->GetYaxis()->SetTitle("Delta Full Depletion Voltage [V]");
    gX[0]->GetXaxis()->SetRangeUser(0,280);
    gX[0]->SetMaximum(200);
    gX[0]->SetMinimum(-250);

    TLegend *leg = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg->SetTextFont(42);
    leg->SetHeader("Fitting Methods");
    leg->AddEntry(gX[7], "General", "l");
    leg->AddEntry(gX[0], "sigmoid + pol1", "l");
    leg->AddEntry(gX[2], "sigmoid + pol2", "l");
    leg->AddEntry(gX[1], "atan + pol1", "l");
    leg->AddEntry(gX[3], "atan + pol2", "l");

    leg->AddEntry(gX[8], "1st Order Deriv : General", "l");
    leg->AddEntry(gX[4], "1st Order Deriv : exp or gaussian", "l");
    leg->AddEntry(gX[5], "1st Order Deriv : Trilinear", "l");

    leg->AddEntry(gX[9], "2nd Order Deriv.:  General", "l");
    leg->AddEntry(gX[6], "2nd Order Deriv.:  Gaussian", "l");


    leg->Draw("SAME");

        TLatex *t = new TLatex(5,205,"CMS");
        t->SetTextFont(61);
        t->SetTextAlign(11);
        float fac = c1->GetTopMargin();
        t->SetTextSize(fac*0.5);
        t->Draw();

      //  TLatex *t2 = new TLatex();
      //  t2->SetTextFont(52);
      //  t2->SetTextAlign(11);
      //  t2->SetTextSize(0.5);
      //  t2->DrawLatex(18,310," L_{int} = 260 fb^{-1}");

        TLatex *t3 = new TLatex(37.5,205,"Preliminary");
        t3->SetTextFont(52);
        t3->SetTextAlign(11);
        t3->SetTextSize(fac*0.3);
        t3->Draw();
        TString text = "Tracker Inner Barrel Layer L"+LAY;
        float dx = 174;
        if (subdet=="TOB") {text = "Tracker Outer Barrel L"+LAY;dx = 174;}

        TLatex *t4 = new TLatex(dx,205,text);
        t4->SetTextFont(61);
        t4->SetTextAlign(11);
        t4->SetTextSize(fac*0.3);
        t4->Draw();

    TLine* lvdrop = new TLine(195, -250, 195, 60);
    lvdrop->SetLineStyle(6);
    lvdrop->SetLineColor(1);
    lvdrop->Draw("SAME");

    TLine* lvdrop2 = new TLine(30, -250, 30, 200);
    lvdrop2->SetLineStyle(6);
    lvdrop2->SetLineColor(1);
    lvdrop2->Draw("SAME");


    TGraph* g_simu = 0;

  //cout<<"--- Superimpose simulation : Fluence --> Lumi"<<endl;

  int choice_simu =5 ; // 5 is the most up-to-date choise (Paul development).
  //  It comes from the  /afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/ envrionment
  //where predictions where made for differnet scenarios of lumi temperature and dismantling time.
  // Predictions have been made for run3 or Run 3 and dismantling
  TString dirname="/eos/user/j/jlagram/SiStripRadMonitoring/ntonon/DECO_files_kink_range_fix";
  bool superimpose_simu = false;
  TString FILE = "";
  if(superimpose_simu)
  {
      if(subdet != "TIB" && choice_simu == 0) {cout<<"Can only plot this simulation for TIB ! Abort"<<endl; return 0;}
      TString simufile_name = "";
      if(choice_simu == 0) {simufile_name = dirname+"/simulation/VdepGraphs_TIB_2017.root";}
      else if(choice_simu == 1) {simufile_name = dirname+"/simulation/Simu_JLA_v1.root";}
      else if(choice_simu == 2) {simufile_name = dirname+"/simulation/Simu_JLA_v2.root";}
      else if(choice_simu == 3) {simufile_name = dirname+"/simulation/Graph_new_nico_thesis.root";}
      else if(choice_simu == 4) {simufile_name = dirname+"/simulation/VdepGraphs_2022.root";}
      else if(choice_simu == 5) 
        {
          if (subdet == "TIB")
            {
              simufile_name = "/afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/lumigr_TIB_L"+LAY+".root";
              FILE = "lumigr_TIB_L"+LAY;
            }
          if (subdet == "TOB")
            {
              simufile_name = "/afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/lumigr_TOB_L"+LAY+".root";
              FILE = "lumigr_TOB_L"+LAY;
            }

        }

    TFile* file = new TFile(simufile_name, "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << file << std::endl;
        return 0;
    }

      TGraph* histogram = nullptr;
      histogram = dynamic_cast<TGraph*>(file->Get(FILE));//lumigr_TIB_L1.png

    if (!histogram) {
        std::cerr << "Error: Histogram not found in the file." << std::endl;
        file->Close();
        return 0;
    }
    std::cout<<"Obtain Simugraph "<<std::endl;
    histogram->Draw("SAME");
    histogram->SetLineColor(kRed);
    leg->AddEntry(histogram, "Prediction", "L");
        
  }

    TString save_text = "DeltaTIB_L"+LAY;
    if (subdet=="TOB") {save_text = "DeltaTOB_L"+LAY;}
    if (SMALL){save_text = save_text+"SMALLSCAN";}
    else {save_text = save_text+"FULLSCAN";}
    c1->SaveAs(save_text+".root");
    c1->SaveAs(save_text+".pdf");
       
    delete c1;
    return 1;
  }


int main()
  {
    std::vector<std::vector<float>> MeanVfdTOB;
    std::vector<std::vector<float>> MeanVfdTIB;
    std::vector<float> Lumi;
    std::pair<std::vector<std::vector<float>>,std::vector<std::vector<float>>> DATATIB;
    std::pair<std::vector<std::vector<float>>,std::vector<std::vector<float>>> DATATOB;
    bool SmallScan = true;

    if (SmallScan)
      {
        // // // Run1
           Lumi.push_back(7);//193541
            // Lumi.push_back(15);//199832
            // Lumi.push_back(21);//203832
            // Lumi.push_back(28);//208339

        // // // Run2
            Lumi.push_back(29);//     20151121_run262254
            Lumi.push_back(44);// 20160706_run276437
            // Lumi.push_back(54);// 20160803_run278167
            Lumi.push_back(65);// 20160909_run280385
            // Lumi.push_back(76);// 20170527_run295324
            Lumi.push_back(121);// 20171030_run305862 : first good run for tob

            Lumi.push_back(144);// 20180530_run317182
            Lumi.push_back(151);// 20180611_run317683
            Lumi.push_back(192);// 20181018_run324841 // TOBnot many data points, add inter step pounts
            Lumi.push_back(194);// 20181115_run326776

        // Run3
            Lumi.push_back(194);//run :20211029_run346395 // TOBnot many data points, add inter step pounts : first modules avant 436232901, nbre de points veto, error bars sometimes are bigger
            Lumi.push_back(195);//run :20220605_run353060 : Full // TOBnot many data points, add inter step pounts, parameter gaussian deriv szconde and courbe verte func
            Lumi.push_back(235);//run :20230407_run365843: Full
            Lumi.push_back(252);//run :run368669 
            Lumi.push_back(266);//run :20230907_run373060
      }

else  
  {
    // Full scans -----------//

    ////// Run 1
        // Lumi.push_back(0);//run :20110315_run160497
        Lumi.push_back(7.4);//run :20110315_run160497
    // // // // Run2
        Lumi.push_back(33);//run :20160423_run271056
        Lumi.push_back(74);//run :20170527_run295376
        Lumi.push_back(100);//run : 20170924_run303824
        Lumi.push_back(126);//run 20180418_run314574
        Lumi.push_back(181);//run 20180923_run323370
    // // // Run3
        Lumi.push_back(195);//run :20220605_run353060 : Full
        Lumi.push_back(235);//run :20230407_run365843: Full
  }

    // //--------------------------------------------------------//
    // // Vfd
    // //--------------------------------------------------------//
    DATATOB = FitLeakageCurrent("TOB","",1,SmallScan);
    MeanVfdTOB = DATATOB.first;
    PlotMeanVfdPerRunwPerFit(MeanVfdTOB,Lumi,"TOB","1",SmallScan);
    //--------------------------------------------------------//
    // Delta Vfd-Vinit mean
    //--------------------------------------------------------//
    MeanVfdTOB = DATATOB.second;
    PlotDeltaMeanVfdPerRunPerFit(MeanVfdTOB,Lumi,"TOB","1",SmallScan);
    //     //--------------------------------------------------------//
    // // Vfd
    // //--------------------------------------------------------//
    // DATATOB = FitLeakageCurrent("TOB","",4,SmallScan);
    // MeanVfdTOB = DATATOB.first;
    // PlotMeanVfdPerRunwPerFit(MeanVfdTOB,Lumi,"TOB","4",SmallScan);
    // //--------------------------------------------------------//
    // // Delta Vfd-Vinit mean
    // //--------------------------------------------------------//
    // MeanVfdTOB = DATATOB.second;
    // PlotDeltaMeanVfdPerRunPerFit(MeanVfdTOB,Lumi,"TOB","4",SmallScan);

    //--------------------------------------------------------//
    // Vfd
    //--------------------------------------------------------//
    // DATATOB = FitLeakageCurrent("TOB","",5,SmallScan);
    // MeanVfdTOB = DATATOB.first;
    // PlotMeanVfdPerRunwPerFit(MeanVfdTOB,Lumi,"TOB","5",SmallScan);
    // //--------------------------------------------------------//
    // // Delta Vfd-Vinit mean
    // //--------------------------------------------------------//
    // MeanVfdTOB = DATATOB.second;
    // PlotDeltaMeanVfdPerRunPerFit(MeanVfdTOB,Lumi,"TOB","5",SmallScan);
    
    // //--------------------------------------------------------//
    // // Vfd
    // //--------------------------------------------------------//
    // DATATOB = FitLeakageCurrent("TOB","",6,SmallScan);
    // MeanVfdTOB = DATATOB.first;
    // PlotMeanVfdPerRunwPerFit(MeanVfdTOB,Lumi,"TOB","6",SmallScan);
    // //--------------------------------------------------------//
    // // Delta Vfd-Vinit mean
    // //--------------------------------------------------------//
    // MeanVfdTOB = DATATOB.second;
    // PlotDeltaMeanVfdPerRunPerFit(MeanVfdTOB,Lumi,"TOB","6",SmallScan);

//-----------------------------------------------------------------------//

    //--------------------------------------------------------//
    // Vfd
    //--------------------------------------------------------//
    DATATIB = FitLeakageCurrent("TIB","",1,SmallScan);
    MeanVfdTIB = DATATIB.first;
    PlotMeanVfdPerRunwPerFit(MeanVfdTIB,Lumi,"TIB","1",SmallScan);
    //--------------------------------------------------------//
    // Delta Vfd-Vinit mean
    //--------------------------------------------------------//
    MeanVfdTIB = DATATIB.second;
    PlotDeltaMeanVfdPerRunPerFit(MeanVfdTIB,Lumi,"TIB","1",SmallScan);

//         //--------------------------------------------------------//
//     // Vfd
//     //--------------------------------------------------------//
//     DATATIB = FitLeakageCurrent("TIB","",4,SmallScan);
//     MeanVfdTIB = DATATIB.first;
//     PlotMeanVfdPerRunwPerFit(MeanVfdTIB,Lumi,"TIB","4",SmallScan);
//     //--------------------------------------------------------//
//     // Delta Vfd-Vinit mean
//     //--------------------------------------------------------//
//     MeanVfdTIB = DATATIB.second;
//     PlotDeltaMeanVfdPerRunPerFit(MeanVfdTIB,Lumi,"TIB","4",SmallScan);
// //-----------------------------------------------------------------------//



    return 0;
  }
