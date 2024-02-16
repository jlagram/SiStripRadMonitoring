#include "ComputeILeakCorrections.C"
#include "GetKink.C"
#include "../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.h"
#include "TMathText.h"
#include "TLatex.h"
TGraphErrors* GetIleakVsVbias(TString run,TGraph* gsteps, TGraph* gcur_DCU, TGraph* gcur_PS)
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
	if(volt_err<=15 && (( run.Contains("2016") || run.Contains("2015") ||  run.Contains("2012")||  run.Contains("2011")|| run.Contains("2010")) || run.Contains("20170527_run295324"))) //1 //15 + allows to avoid the first point at 155 volt due to the previous loop where (300+10)/2 is done=> error of ~150volts
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


// getting largest values to avoid not saturated currents during a step
/*TGraphErrors* SaturatedIleakVsVbias(TGraphErrors* g, float deriv_min=0.01)
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
		 cout<<previous_volt<<"V "<<mean_current<<"uA +/- "<<curr_err/sqrt(nmean)<<" y':"<<deriv<<endl;
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
}*/

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

std::vector<float> Fit(char* subdet, char* run, int* detids, const int N, char* bad_periods="")
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
  int COUNT[10]= {0}; 

  for(int idet=0; idet<N; idet++)
  {
    detid = detids[idet];
    
    

	// Int_t detid_forlayer = detid;
  //   if(subdet=="TOB") detid_forlayer = detid/10;
	// subdet_i = (detid_forlayer>>kSubdetOffset)&0x7;
	// // For TIB and TOB
  //   if(subdet_i==3 || subdet_i==5) olayer = ((detid_forlayer>>layerStartBit_) & layerMask_);
	
    if(subdet=="TOB")  // TIB : subdetid==3
  {
    if(((detid>>14)&0x7)!=5) continue; //return 1;
    // else if(((detid>>14)&0x7)==2) return 2;
    // else if(((detid>>14)&0x7)==3) return 3;
    // else if(((detid>>14)&0x7)==4) return 4;
  }    
  // else if(subdet=="TOB") // TOB subdetid==5
  // {       
  //   if(((detid>>14)&0x7)!=1) continue; //return 5;
  //   // else if(((detid>>14)&0x7)==2) return 6;
  //   // else if(((detid>>14)&0x7)==3) return 7;
  //   // else if(((detid>>14)&0x7)==4) return 8;
  //   // else if(((detid>>14)&0x7)==5) return 9;
  //   // else if(((detid>>14)&0x7)==6) return 10;
  // }   
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
    TGraphErrors* gIleak = GetIleakVsVbias(RUN, gsteps, gcur_DCU_unscaled, gcur_PS_unscaled);
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
    if(fvdrop->GetNDF())
      {
        // std::cout<<"help 2"<<std::endl;
        if (fvdrop->GetChisquare()/fvdrop->GetNDF()<5  && fvdrop->GetChisquare()/fvdrop->GetNDF()>0.05)
          {
            // VfdOutput[0]+=fvdrop->GetParameter(2);
            VfdOutput[0]+=fvdrop->GetParameter(2);
            COUNT[0]++;

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

    if(fvdrop3->GetNDF())
      {
        if (fvdrop3->GetChisquare()/fvdrop3->GetNDF()<5 && fvdrop3->GetChisquare()/fvdrop3->GetNDF()>0.05)
          {
            // VfdOutput[1]+=fvdrop3->GetParameter(2);
            VfdOutput[1]+=fvdrop3->GetParameter(2);
            // VfdOutput.push_back(fvdrop3->GetParameter(2));
            COUNT[1]++;
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
        if (fvdrop6->GetChisquare()/fvdrop6->GetNDF()<5 && fvdrop6->GetChisquare()/fvdrop6->GetNDF()>0.05)
          {
            // VfdOutput[2]+=fvdrop6->GetParameter(2);
            VfdOutput[2]+=fvdrop6->GetParameter(2);
            // VfdOutput.push_back(fvdrop6->GetParameter(2));
            COUNT[2]++;
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

    if(fvdrop4->GetNDF())
      {
        if (fvdrop4->GetChisquare()/fvdrop4->GetNDF()<5 && fvdrop4->GetChisquare()/fvdrop4->GetNDF()>0.05)
          {
            // VfdOutput[3]+=fvdrop4->GetParameter(2);
            VfdOutput[3]+=fvdrop4->GetParameter(2);
            // VfdOutput.push_back(fvdrop4->GetParameter(2));
            COUNT[3]++;
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
    fvdrop10->SetParameter(4, 22.5);
    fvdrop10->SetParLimits(4, 10,30);
    fvdrop10->SetParameter(5, 0.02);
    fvdrop10->SetParLimits(5, 0,0.05);
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
    std::cout<<"End of  Fit of Leakage current vs Vbias with general function "<<std::endl;
	  cout<<"Fit status: "<<status;

	  if(fvdrop10->GetNDF()) cout<<" chi2/ndf: "<<fvdrop10->GetChisquare()/fvdrop10->GetNDF();
	  cout<<endl;

    fvdrop10->Draw("same");

    float p = 0.005;
    float Vfdgen = fvdrop10->GetParameter(0)*3.14*p/(4*fvdrop10->GetParameter(5));
        TLine* lvdrop10 = new TLine(Vfdgen, ymin, Vfdgen, ymax);
    lvdrop10->SetLineStyle(2);
    lvdrop10->SetLineColor(1);//black
    // lvdrop10->SetLineColor(1);//
    lvdrop10->Draw();
    if(fvdrop10->GetNDF())
      {
        if (fvdrop10->GetChisquare()/fvdrop10->GetNDF()<5 && fvdrop10->GetChisquare()/fvdrop10->GetNDF()>0.05 && Vfdgen > 0 && Vfdgen < 400)
          {
            // VfdOutput[3]+=fvdrop4->GetParameter(2);
            VfdOutput[7]+=Vfdgen;
            // VfdOutput.push_back(fvdrop4->GetParameter(2));
            COUNT[7]++;
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
              // VfdOutput[4]+=fvdropderiv->GetParameter(2);
              VfdOutput[4]+=fvdropderiv->GetParameter(2);
              // VfdOutput.push_back(fvdropderiv->GetParameter(2));
              COUNT[4]++;
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
            // VfdOutput[4]+=fvdropderiv->GetParameter(2);
            VfdOutput[4]+=fvdropderiv->GetParameter(2);
            // VfdOutput.push_back(fvdropderiv->GetParameter(2));
            COUNT[4]++;
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
            // VfdOutput[4]+=fvdropderiv->GetParameter(2);
            VfdOutput[4]+=Vfd;
            // VfdOutput.push_back(Vfd);
            COUNT[4]++;
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
            // VfdOutput[5]+=TriL->GetParameter(3);
            VfdOutput[5]+=TriL->GetParameter(3);
            // VfdOutput.push_back(TriL->GetParameter(3));
            COUNT[5]++;
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
      std::cout<<"Starting Fit of deriv with exp"<<std::endl;
      status = gderivative->Fit("fvdropderivGen", "Rsame")  ;
     std::cout<<"End of  Fit of deriv with exp"<<std::endl;
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
            // VfdOutput[4]+=fvdropderiv->GetParameter(2);
            VfdOutput[8]+=Vfd;
            // VfdOutput.push_back(Vfd);
            COUNT[8]++;
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


                                                                            
        if (  RUN.Contains("2023") ) {up = 200; down = 20;curvmax = 0.008; mean = 100;
                                      meandown = 50; meanup= 200;}
        if (  RUN.Contains("2022") ) {up = 200; down = 20;curvmax = 0.03; curvmin = 0.01;mean = 100;
                                      meandown = 50; meanup= 150;}
        if ( RUN.Contains("2021")  ) {up = 160; down = 20;curvmax = 0.01; mean = 100;
                                      meandown = 50; meanup= 150;}    
        if (RUN.Contains("2017") || RUN.Contains("2018") ) {up = 160; down = 20;curvmax = 0.015; mean = 100;
                                                           meandown = 50; meanup= 150;}  
        if (RUN.Contains("2016")  ) {up = 160; down = 20;curvmax = 0.05; mean = 100;
                                                           meandown = 50; meanup= 150;}                                
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
            // VfdOutput[6]+=fcurv->GetParameter(2);
            VfdOutput[6]+=fcurv->GetParameter(2);
            // VfdOutput.push_back(fcurv->GetParameter(2));
            COUNT[6]++;
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
            // VfdOutput[6]+=fcurv->GetParameter(2);
            VfdOutput[9]+=fcurvGen->GetParameter(2);
            // VfdOutput.push_back(fcurv->GetParameter(2));
            COUNT[9]++;
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
    //  getchar();//to desactivate when running on all the modules and all the runs
    
 
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
    
  } // End of loop over modules


  for (unsigned int i = 0 ; i < VfdOutput.size() ; i++)
    {
      if (VfdOutput[i]==0) continue;
      else VfdOutput[i]=VfdOutput[i]/COUNT[i];
    }
  return VfdOutput;//returns the mean value for each fit methods for a given run


  
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
}

//------------------------------------------------------------------------------

std::vector<std::vector<float>> FitLeakageCurrent(char* subdet="TIB", char* run="20230907_run373060")
{

  // Many points needed, works well only for august 2012 scan
  
  // 2012
  const int N_2012=18;
  int detids_2012[N_2012]={369121381,369121382,369121385,369121386,369121389,369121390,369121605,369121606,369121609,369121610,
  369121613,369121614,369125861,369125862,369125865,369125866,369125869,369125870};  
  const int N_Full = 2601;
  int Full_detid[N_Full]= {369120277,369120278
    ,369120281
    ,369120282
    ,369120285
    ,369120286
    ,369120293
    ,369120294
    ,369120297
    ,369120298
    ,369120301
    ,369120302
    ,369120309
    ,369120310
    ,369120313
    ,369120314
    ,369120317
    ,369120318
    ,369120325
    ,369120326
    ,369120329
    ,369120330
    ,369120333
    ,369120334
    ,369120341
    ,369120342
    ,369120345
    ,369120346
    ,369120349
    ,369120350
    ,369120357
    ,369120358
    ,369120361
    ,369120362
    ,369120365
    ,369120366
    ,369120373
    ,369120374
    ,369120377
    ,369120378
    ,369120381
    ,369120382
    ,369120389
    ,369120390
    ,369120393
    ,369120394
    ,369120397
    ,369120398
    ,369120405
    ,369120406
    ,369120409
    ,369120410
    ,369120413
    ,369120414
    ,369120421
    ,369120422
    ,369120425
    ,369120426
    ,369120429
    ,369120430
    ,369120437
    ,369120438
    ,369120441
    ,369120442
    ,369120445
    ,369120446
    ,369120453
    ,369120454
    ,369120457
    ,369120458
    ,369120461
    ,369120462
    ,369120469
    ,369120470
    ,369120473
    ,369120474
    ,369120477
    ,369120478
    ,369120485
    ,369120486
    ,369120489
    ,369120490
    ,369120493
    ,369120494
    ,369120501
    ,369120502
    ,369120505
    ,369120506
    ,369120509
    ,369120510
    ,369120517
    ,369120518
    ,369120521
    ,369120522
    ,369120525
    ,369120526
    ,369120533
    ,369120534
    ,369120537
    ,369120538
    ,369120541
    ,369120542
    ,369120629
    ,369120630
    ,369120633
    ,369120634
    ,369120637
    ,369120638
    ,369120645
    ,369120646
    ,369120649
    ,369120650
    ,369120653
    ,369120654
    ,369120661
    ,369120662
    ,369120665
    ,369120666
    ,369120669
    ,369120670
    ,369120677
    ,369120678
    ,369120681
    ,369120682
    ,369120685
    ,369120686
    ,369121301
    ,369121302
    ,369121305
    ,369121306
    ,369121309
    ,369121310
    ,369121317
    ,369121318
    ,369121321
    ,369121322
    ,369121325
    ,369121326
    ,369121333
    ,369121334
    ,369121337
    ,369121338
    ,369121341
    ,369121342
    ,369121349
    ,369121350
    ,369121353
    ,369121354
    ,369121357
    ,369121358
    ,369121365
    ,369121366
    ,369121369
    ,369121370
    ,369121373
    ,369121374
    ,369121381
    ,369121382
    ,369121385
    ,369121386
    ,369121389
    ,369121390
    ,369121397
    ,369121398
    ,369121401
    ,369121402
    ,369121405
    ,369121406
    ,369121413
    ,369121414
    ,369121417
    ,369121418
    ,369121421
    ,369121422
    ,369121429
    ,369121430
    ,369121433
    ,369121434
    ,369121437
    ,369121438
    ,369121445
    ,369121446
    ,369121449
    ,369121450
    ,369121453
    ,369121454
    ,369121461
    ,369121462
    ,369121465
    ,369121466
    ,369121469
    ,369121470
    ,369121477
    ,369121478
    ,369121481
    ,369121482
    ,369121485
    ,369121486
    ,369121493
    ,369121494
    ,369121497
    ,369121498
    ,369121501
    ,369121502
    ,369121509
    ,369121510
    ,369121513
    ,369121514
    ,369121517
    ,369121518
    ,369121525
    ,369121526
    ,369121529
    ,369121530
    ,369121533
    ,369121534
    ,369121541
    ,369121542
    ,369121545
    ,369121546
    ,369121549
    ,369121550
    ,369121557
    ,369121558
    ,369121561
    ,369121562
    ,369121565
    ,369121566
    ,369121573
    ,369121574
    ,369121577
    ,369121578
    ,369121581
    ,369121582
    ,369121589
    ,369121590
    ,369121593
    ,369121594
    ,369121597
    ,369121598
    ,369121605
    ,369121606
    ,369121609
    ,369121610
    ,369121613
    ,369121614
    ,369121621
    ,369121622
    ,369121625
    ,369121626
    ,369121629
    ,369121630
    ,369121637
    ,369121638
    ,369121641
    ,369121642
    ,369121645
    ,369121646
    ,369121653
    ,369121654
    ,369121657
    ,369121658
    ,369121661
    ,369121662
    ,369121669
    ,369121670
    ,369121673
    ,369121674
    ,369121677
    ,369121678
    ,369121685
    ,369121686
    ,369121689
    ,369121690
    ,369121693
    ,369121694
    ,369121701
    ,369121702
    ,369121705
    ,369121706
    ,369121709
    ,369121710
    ,369121717
    ,369121718
    ,369121721
    ,369121722
    ,369121725
    ,369121726
    ,369121733
    ,369121734
    ,369121737
    ,369121738
    ,369121741
    ,369121742
    ,369121749
    ,369121750
    ,369121753
    ,369121754
    ,369121757
    ,369121758
    ,369121765
    ,369121766
    ,369121769
    ,369121770
    ,369121773
    ,369121774
    ,369124373
    ,369124374
    ,369124377
    ,369124378
    ,369124381
    ,369124382
    ,369124389
    ,369124390
    ,369124393
    ,369124394
    ,369124397
    ,369124398
    ,369124405
    ,369124406
    ,369124409
    ,369124410
    ,369124413
    ,369124414
    ,369124421
    ,369124422
    ,369124425
    ,369124426
    ,369124429
    ,369124430
    ,369124437
    ,369124438
    ,369124441
    ,369124442
    ,369124445
    ,369124446
    ,369124453
    ,369124454
    ,369124457
    ,369124458
    ,369124461
    ,369124462
    ,369124469
    ,369124470
    ,369124473
    ,369124474
    ,369124477
    ,369124478
    ,369124485
    ,369124486
    ,369124489
    ,369124490
    ,369124493
    ,369124494
    ,369124501
    ,369124502
    ,369124505
    ,369124506
    ,369124509
    ,369124510
    ,369124517
    ,369124518
    ,369124521
    ,369124522
    ,369124525
    ,369124526
    ,369124533
    ,369124534
    ,369124537
    ,369124538
    ,369124541
    ,369124542
    ,369124549
    ,369124550
    ,369124553
    ,369124554
    ,369124557
    ,369124558
    ,369124565
    ,369124566
    ,369124569
    ,369124570
    ,369124573
    ,369124574
    ,369124581
    ,369124582
    ,369124585
    ,369124586
    ,369124589
    ,369124590
    ,369124597
    ,369124598
    ,369124601
    ,369124602
    ,369124605
    ,369124606
    ,369124613
    ,369124614
    ,369124617
    ,369124618
    ,369124621
    ,369124622
    ,369124629
    ,369124630
    ,369124633
    ,369124634
    ,369124637
    ,369124638
    ,369124645
    ,369124646
    ,369124649
    ,369124650
    ,369124653
    ,369124654
    ,369124661
    ,369124662
    ,369124665
    ,369124666
    ,369124669
    ,369124670
    ,369124677
    ,369124678
    ,369124681
    ,369124682
    ,369124685
    ,369124686
    ,369124693
    ,369124694
    ,369124697
    ,369124698
    ,369124701
    ,369124702
    ,369124709
    ,369124710
    ,369124713
    ,369124714
    ,369124717
    ,369124718
    ,369124725
    ,369124726
    ,369124729
    ,369124730
    ,369124733
    ,369124734
    ,369124741
    ,369124742
    ,369124745
    ,369124746
    ,369124749
    ,369124750
    ,369124757
    ,369124758
    ,369124761
    ,369124762
    ,369124765
    ,369124766
    ,369124773
    ,369124774
    ,369124777
    ,369124778
    ,369124781
    ,369124782
    ,369125397
    ,369125401
    ,369125405
    ,369125413
    ,369125414
    ,369125417
    ,369125418
    ,369125421
    ,369125422
    ,369125429
    ,369125430
    ,369125433
    ,369125434
    ,369125437
    ,369125438
    ,369125445
    ,369125446
    ,369125449
    ,369125450
    ,369125453
    ,369125454
    ,369125461
    ,369125462
    ,369125465
    ,369125466
    ,369125469
    ,369125470
    ,369125477
    ,369125478
    ,369125481
    ,369125482
    ,369125485
    ,369125486
    ,369125493
    ,369125494
    ,369125497
    ,369125498
    ,369125501
    ,369125502
    ,369125509
    ,369125510
    ,369125513
    ,369125514
    ,369125517
    ,369125518
    ,369125525
    ,369125526
    ,369125529
    ,369125530
    ,369125533
    ,369125534
    ,369125541
    ,369125542
    ,369125545
    ,369125546
    ,369125549
    ,369125550
    ,369125557
    ,369125558
    ,369125561
    ,369125562
    ,369125565
    ,369125566
    ,369125573
    ,369125574
    ,369125577
    ,369125578
    ,369125581
    ,369125582
    ,369125589
    ,369125590
    ,369125593
    ,369125594
    ,369125597
    ,369125598
    ,369125605
    ,369125606
    ,369125609
    ,369125610
    ,369125613
    ,369125614
    ,369125621
    ,369125622
    ,369125625
    ,369125626
    ,369125629
    ,369125630
    ,369125637
    ,369125638
    ,369125641
    ,369125642
    ,369125645
    ,369125653
    ,369125654
    ,369125657
    ,369125658
    ,369125661
    ,369125662
    ,369125669
    ,369125670
    ,369125673
    ,369125674
    ,369125677
    ,369125678
    ,369125685
    ,369125686
    ,369125689
    ,369125690
    ,369125693
    ,369125694
    ,369125701
    ,369125702
    ,369125705
    ,369125706
    ,369125709
    ,369125710
    ,369125717
    ,369125718
    ,369125721
    ,369125722
    ,369125725
    ,369125726
    ,369125733
    ,369125734
    ,369125737
    ,369125738
    ,369125741
    ,369125742
    ,369125749
    ,369125750
    ,369125753
    ,369125754
    ,369125757
    ,369125758
    ,369125765
    ,369125766
    ,369125769
    ,369125770
    ,369125773
    ,369125774
    ,369125781
    ,369125782
    ,369125785
    ,369125786
    ,369125789
    ,369125790
    ,369125797
    ,369125798
    ,369125801
    ,369125802
    ,369125805
    ,369125806
    ,369125813
    ,369125814
    ,369125817
    ,369125818
    ,369125821
    ,369125822
    ,369125829
    ,369125830
    ,369125833
    ,369125834
    ,369125837
    ,369125838
    ,369125845
    ,369125846
    ,369125849
    ,369125850
    ,369125853
    ,369125854
    ,369125861
    ,369125862
    ,369125865
    ,369125866
    ,369125869
    ,369125870
    ,369136661
    ,369136662
    ,369136665
    ,369136666
    ,369136669
    ,369136670
    ,369136677
    ,369136678
    ,369136681
    ,369136682
    ,369136685
    ,369136686
    ,369136693
    ,369136694
    ,369136697
    ,369136698
    ,369136701
    ,369136702
    ,369136709
    ,369136710
    ,369136713
    ,369136714
    ,369136717
    ,369136718
    ,369136725
    ,369136726
    ,369136729
    ,369136730
    ,369136733
    ,369136734
    ,369136741
    ,369136742
    ,369136745
    ,369136746
    ,369136749
    ,369136750
    ,369136757
    ,369136758
    ,369136761
    ,369136762
    ,369136765
    ,369136766
    ,369136773
    ,369136774
    ,369136777
    ,369136778
    ,369136781
    ,369136782
    ,369136789
    ,369136790
    ,369136793
    ,369136794
    ,369136797
    ,369136798
    ,369136805
    ,369136806
    ,369136809
    ,369136810
    ,369136813
    ,369136814
    ,369136821
    ,369136822
    ,369136825
    ,369136826
    ,369136829
    ,369136830
    ,369136837
    ,369136838
    ,369136841
    ,369136842
    ,369136845
    ,369136846
    ,369136853
    ,369136854
    ,369136857
    ,369136858
    ,369136861
    ,369136862
    ,369136869
    ,369136870
    ,369136873
    ,369136874
    ,369136877
    ,369136878
    ,369136885
    ,369136886
    ,369136889
    ,369136890
    ,369136893
    ,369136894
    ,369136901
    ,369136902
    ,369136905
    ,369136906
    ,369136909
    ,369136910
    ,369136917
    ,369136918
    ,369136921
    ,369136922
    ,369136925
    ,369136926
    ,369136933
    ,369136934
    ,369136937
    ,369136938
    ,369136941
    ,369136942
    ,369136949
    ,369136950
    ,369136953
    ,369136954
    ,369136957
    ,369136958
    ,369136965
    ,369136966
    ,369136969
    ,369136970
    ,369136973
    ,369136974
    ,369136981
    ,369136982
    ,369136985
    ,369136986
    ,369136989
    ,369136990
    ,369136997
    ,369136998
    ,369137001
    ,369137002
    ,369137005
    ,369137006
    ,369137013
    ,369137014
    ,369137017
    ,369137018
    ,369137021
    ,369137022
    ,369137029
    ,369137030
    ,369137033
    ,369137034
    ,369137037
    ,369137038
    ,369137045
    ,369137046
    ,369137049
    ,369137050
    ,369137053
    ,369137054
    ,369137061
    ,369137062
    ,369137065
    ,369137066
    ,369137069
    ,369137070
    ,369137077
    ,369137078
    ,369137081
    ,369137082
    ,369137085
    ,369137086
    ,369137093
    ,369137094
    ,369137097
    ,369137098
    ,369137101
    ,369137102
    ,369137109
    ,369137110
    ,369137113
    ,369137114
    ,369137117
    ,369137118
    ,369137125
    ,369137126
    ,369137129
    ,369137130
    ,369137133
    ,369137134
    ,369137141
    ,369137142
    ,369137145
    ,369137146
    ,369137149
    ,369137150
    ,369137157
    ,369137158
    ,369137161
    ,369137162
    ,369137165
    ,369137166
    ,369137173
    ,369137174
    ,369137177
    ,369137178
    ,369137181
    ,369137182
    ,369137189
    ,369137190
    ,369137193
    ,369137194
    ,369137197
    ,369137198
    ,369137685
    ,369137686
    ,369137689
    ,369137690
    ,369137693
    ,369137694
    ,369137701
    ,369137702
    ,369137705
    ,369137706
    ,369137709
    ,369137710
    ,369137717
    ,369137718
    ,369137721
    ,369137722
    ,369137725
    ,369137726
    ,369137733
    ,369137734
    ,369137737
    ,369137738
    ,369137741
    ,369137742
    ,369137749
    ,369137750
    ,369137753
    ,369137754
    ,369137757
    ,369137758
    ,369137765
    ,369137766
    ,369137769
    ,369137770
    ,369137773
    ,369137774
    ,369137781
    ,369137782
    ,369137785
    ,369137786
    ,369137789
    ,369137790
    ,369137797
    ,369137798
    ,369137801
    ,369137802
    ,369137805
    ,369137806
    ,369137813
    ,369137814
    ,369137817
    ,369137818
    ,369137821
    ,369137822
    ,369137829
    ,369137830
    ,369137833
    ,369137834
    ,369137837
    ,369137838
    ,369137845
    ,369137846
    ,369137849
    ,369137850
    ,369137853
    ,369137854
    ,369137861
    ,369137862
    ,369137865
    ,369137866
    ,369137869
    ,369137870
    ,369137877
    ,369137878
    ,369137881
    ,369137882
    ,369137885
    ,369137886
    ,369137893
    ,369137894
    ,369137897
    ,369137898
    ,369137901
    ,369137902
    ,369137909
    ,369137910
    ,369137913
    ,369137914
    ,369137917
    ,369137918
    ,369137925
    ,369137926
    ,369137929
    ,369137930
    ,369137933
    ,369137934
    ,369137941
    ,369137942
    ,369137945
    ,369137946
    ,369137949
    ,369137950
    ,369137957
    ,369137958
    ,369137961
    ,369137962
    ,369137965
    ,369137966
    ,369137973
    ,369137974
    ,369137977
    ,369137978
    ,369137981
    ,369137982
    ,369137989
    ,369137990
    ,369137993
    ,369137994
    ,369137997
    ,369137998
    ,369138005
    ,369138006
    ,369138009
    ,369138010
    ,369138013
    ,369138014
    ,369138021
    ,369138022
    ,369138025
    ,369138026
    ,369138029
    ,369138030
    ,369138037
    ,369138038
    ,369138041
    ,369138042
    ,369138045
    ,369138046
    ,369138053
    ,369138054
    ,369138057
    ,369138058
    ,369138061
    ,369138062
    ,369138069
    ,369138070
    ,369138073
    ,369138074
    ,369138077
    ,369138078
    ,369138085
    ,369138086
    ,369138089
    ,369138090
    ,369138093
    ,369138094
    ,369138101
    ,369138102
    ,369138105
    ,369138106
    ,369138109
    ,369138110
    ,369138117
    ,369138118
    ,369138121
    ,369138122
    ,369138125
    ,369138126
    ,369138133
    ,369138134
    ,369138137
    ,369138138
    ,369138141
    ,369138142
    ,369138149
    ,369138150
    ,369138153
    ,369138154
    ,369138157
    ,369138158
    ,369138165
    ,369138166
    ,369138169
    ,369138170
    ,369138173
    ,369138174
    ,369138181
    ,369138182
    ,369138185
    ,369138186
    ,369138189
    ,369138190
    ,369138197
    ,369138198
    ,369138201
    ,369138202
    ,369138205
    ,369138206
    ,369138213
    ,369138214
    ,369138217
    ,369138218
    ,369138221
    ,369138222
    ,369138229
    ,369138230
    ,369138233
    ,369138234
    ,369138237
    ,369138238
    ,369138245
    ,369138246
    ,369138249
    ,369138250
    ,369138253
    ,369138254
    ,369138261
    ,369138262
    ,369138265
    ,369138266
    ,369138269
    ,369138270
    ,369138277
    ,369138278
    ,369138281
    ,369138282
    ,369138285
    ,369138286
    ,369140822
    ,369140826
    ,369140830
    ,369140837
    ,369140838
    ,369140841
    ,369140842
    ,369140845
    ,369140846
    ,369140853
    ,369140854
    ,369140857
    ,369140858
    ,369140861
    ,369140862
    ,369140869
    ,369140870
    ,369140873
    ,369140874
    ,369140877
    ,369140878
    ,369140885
    ,369140886
    ,369140889
    ,369140890
    ,369140893
    ,369140894
    ,369140901
    ,369140902
    ,369140905
    ,369140906
    ,369140909
    ,369140910
    ,369140917
    ,369140918
    ,369140921
    ,369140922
    ,369140925
    ,369140926
    ,369140933
    ,369140934
    ,369140937
    ,369140938
    ,369140941
    ,369140942
    ,369140949
    ,369140950
    ,369140953
    ,369140954
    ,369140957
    ,369140958
    ,369140965
    ,369140966
    ,369140969
    ,369140970
    ,369140973
    ,369140974
    ,369140981
    ,369140982
    ,369140985
    ,369140986
    ,369140989
    ,369140990
    ,369140997
    ,369140998
    ,369141001
    ,369141002
    ,369141005
    ,369141006
    ,369141013
    ,369141014
    ,369141017
    ,369141018
    ,369141021
    ,369141022
    ,369141029
    ,369141030
    ,369141033
    ,369141034
    ,369141037
    ,369141038
    ,369141045
    ,369141046
    ,369141049
    ,369141050
    ,369141053
    ,369141054
    ,369141061
    ,369141062
    ,369141065
    ,369141066
    ,369141069
    ,369141070
    ,369141077
    ,369141078
    ,369141081
    ,369141082
    ,369141085
    ,369141086
    ,369141093
    ,369141094
    ,369141097
    ,369141098
    ,369141101
    ,369141102
    ,369141109
    ,369141110
    ,369141113
    ,369141114
    ,369141117
    ,369141118
    ,369141125
    ,369141126
    ,369141129
    ,369141130
    ,369141133
    ,369141134
    ,369141141
    ,369141142
    ,369141145
    ,369141146
    ,369141149
    ,369141150
    ,369141157
    ,369141158
    ,369141161
    ,369141162
    ,369141165
    ,369141166
    ,369141173
    ,369141174
    ,369141177
    ,369141178
    ,369141181
    ,369141182
    ,369141189
    ,369141190
    ,369141193
    ,369141194
    ,369141197
    ,369141198
    ,369141205
    ,369141206
    ,369141209
    ,369141210
    ,369141213
    ,369141214
    ,369141221
    ,369141222
    ,369141225
    ,369141226
    ,369141229
    ,369141230
    ,369141237
    ,369141238
    ,369141241
    ,369141242
    ,369141245
    ,369141246
    ,369141253
    ,369141254
    ,369141257
    ,369141258
    ,369141261
    ,369141262
    ,369141269
    ,369141270
    ,369141273
    ,369141274
    ,369141277
    ,369141278
    ,369141285
    ,369141286
    ,369141289
    ,369141290
    ,369141293
    ,369141294
    ,369141781
    ,369141782
    ,369141785
    ,369141786
    ,369141789
    ,369141790
    ,369141797
    ,369141798
    ,369141801
    ,369141802
    ,369141805
    ,369141806
    ,369141813
    ,369141814
    ,369141817
    ,369141818
    ,369141821
    ,369141822
    ,369141829
    ,369141830
    ,369141833
    ,369141834
    ,369141837
    ,369141838
    ,369141845
    ,369141846
    ,369141849
    ,369141850
    ,369141853
    ,369141854
    ,369141861
    ,369141862
    ,369141865
    ,369141866
    ,369141869
    ,369141870
    ,369141877
    ,369141878
    ,369141881
    ,369141882
    ,369141885
    ,369141886
    ,369141893
    ,369141894
    ,369141897
    ,369141898
    ,369141901
    ,369141902
    ,369141909
    ,369141910
    ,369141913
    ,369141914
    ,369141917
    ,369141918
    ,369141925
    ,369141926
    ,369141929
    ,369141930
    ,369141933
    ,369141934
    ,369141941
    ,369141942
    ,369141945
    ,369141946
    ,369141949
    ,369141950
    ,369141957
    ,369141958
    ,369141962
    ,369141965
    ,369141966
    ,369141973
    ,369141974
    ,369141977
    ,369141978
    ,369141981
    ,369141982
    ,369141989
    ,369141990
    ,369141993
    ,369141994
    ,369141997
    ,369141998
    ,369142005
    ,369142006
    ,369142009
    ,369142010
    ,369142013
    ,369142014
    ,369142021
    ,369142022
    ,369142025
    ,369142026
    ,369142029
    ,369142030
    ,369142037
    ,369142038
    ,369142041
    ,369142042
    ,369142045
    ,369142046
    ,369142053
    ,369142054
    ,369142057
    ,369142058
    ,369142061
    ,369142062
    ,369142069
    ,369142070
    ,369142073
    ,369142074
    ,369142077
    ,369142078
    ,369142085
    ,369142086
    ,369142089
    ,369142090
    ,369142093
    ,369142094
    ,369142101
    ,369142102
    ,369142105
    ,369142106
    ,369142109
    ,369142110
    ,369142117
    ,369142118
    ,369142121
    ,369142122
    ,369142125
    ,369142126
    ,369142133
    ,369142134
    ,369142137
    ,369142138
    ,369142141
    ,369142142
    ,369142149
    ,369142150
    ,369142153
    ,369142154
    ,369142157
    ,369142158
    ,369142165
    ,369142166
    ,369142169
    ,369142170
    ,369142173
    ,369142174
    ,369142181
    ,369142182
    ,369142185
    ,369142186
    ,369142189
    ,369142190
    ,369142197
    ,369142198
    ,369142201
    ,369142202
    ,369142205
    ,369142206
    ,369142213
    ,369142214
    ,369142217
    ,369142218
    ,369142221
    ,369142222
    ,369142229
    ,369142230
    ,369142233
    ,369142234
    ,369142237
    ,369142238
    ,369142245
    ,369142246
    ,369142249
    ,369142250
    ,369142253
    ,369142254
    ,369142262
    ,369142266
    ,369142270
    ,369142277
    ,369142278
    ,369142281
    ,369142282
    ,369142285
    ,369142286
    ,369142293
    ,369142294
    ,369142297
    ,369142298
    ,369142301
    ,369142302
    ,369142309
    ,369142310
    ,369142313
    ,369142314
    ,369142317
    ,369142318
    ,369142325
    ,369142326
    ,369142329
    ,369142330
    ,369142333
    ,369142334
    ,369142341
    ,369142342
    ,369142345
    ,369142346
    ,369142349
    ,369142350
    ,369142357
    ,369142358
    ,369142361
    ,369142362
    ,369142365
    ,369142366
    ,369142373
    ,369142374
    ,369142377
    ,369142378
    ,369142381
    ,369142382
    ,369153044
    ,369153048
    ,369153052
    ,369153060
    ,369153064
    ,369153068
    ,369153076
    ,369153080
    ,369153084
    ,369153108
    ,369153112
    ,369153116
    ,369153124
    ,369153128
    ,369153132
    ,369153140
    ,369153144
    ,369153148
    ,369153156
    ,369153160
    ,369153164
    ,369153172
    ,369153176
    ,369153180
    ,369153188
    ,369153192
    ,369153196
    ,369153204
    ,369153208
    ,369153212
    ,369153220
    ,369153224
    ,369153228
    ,369153236
    ,369153240
    ,369153244
    ,369153252
    ,369153256
    ,369153260
    ,369153268
    ,369153272
    ,369153276
    ,369153284
    ,369153288
    ,369153292
    ,369153300
    ,369153304
    ,369153308
    ,369153316
    ,369153320
    ,369153324
    ,369153332
    ,369153336
    ,369153340
    ,369153348
    ,369153352
    ,369153356
    ,369153364
    ,369153368
    ,369153372
    ,369153380
    ,369153384
    ,369153388
    ,369153396
    ,369153400
    ,369153404
    ,369153412
    ,369153416
    ,369153420
    ,369153428
    ,369153432
    ,369153436
    ,369153444
    ,369153448
    ,369153452
    ,369153460
    ,369153464
    ,369153468
    ,369153476
    ,369153480
    ,369153484
    ,369153492
    ,369153496
    ,369153500
    ,369153508
    ,369153512
    ,369153516
    ,369153524
    ,369153528
    ,369153532
    ,369153540
    ,369153544
    ,369153548
    ,369153556
    ,369153560
    ,369153564
    ,369153572
    ,369153576
    ,369153580
    ,369153588
    ,369153592
    ,369153596
    ,369153604
    ,369153608
    ,369153612
    ,369153620
    ,369153624
    ,369153628
    ,369153636
    ,369153640
    ,369153644
    ,369153652
    ,369153656
    ,369153660
    ,369153668
    ,369153672
    ,369153676
    ,369153684
    ,369153688
    ,369153692
    ,369153700
    ,369153704
    ,369153708
    ,369153716
    ,369153720
    ,369153724
    ,369153732
    ,369153736
    ,369153740
    ,369154068
    ,369154072
    ,369154076
    ,369154084
    ,369154088
    ,369154092
    ,369154100
    ,369154104
    ,369154108
    ,369154132
    ,369154136
    ,369154140
    ,369154148
    ,369154152
    ,369154156
    ,369154164
    ,369154168
    ,369154172
    ,369154180
    ,369154184
    ,369154188
    ,369154196
    ,369154200
    ,369154204
    ,369154212
    ,369154216
    ,369154220
    ,369154228
    ,369154232
    ,369154236
    ,369154244
    ,369154248
    ,369154252
    ,369154260
    ,369154264
    ,369154268
    ,369154276
    ,369154280
    ,369154284
    ,369154292
    ,369154296
    ,369154300
    ,369154308
    ,369154312
    ,369154316
    ,369154324
    ,369154328
    ,369154332
    ,369154340
    ,369154344
    ,369154348
    ,369154356
    ,369154360
    ,369154364
    ,369154372
    ,369154376
    ,369154388
    ,369154392
    ,369154396
    ,369154404
    ,369154408
    ,369154412
    ,369154420
    ,369154424
    ,369154428
    ,369154436
    ,369154440
    ,369154444
    ,369154452
    ,369154456
    ,369154460
    ,369154468
    ,369154472
    ,369154476
    ,369154484
    ,369154488
    ,369154492
    ,369154500
    ,369154504
    ,369154508
    ,369154516
    ,369154520
    ,369154524
    ,369154532
    ,369154536
    ,369154540
    ,369154548
    ,369154552
    ,369154556
    ,369154564
    ,369154568
    ,369154572
    ,369154580
    ,369154584
    ,369154588
    ,369154596
    ,369154600
    ,369154604
    ,369154612
    ,369154616
    ,369154620
    ,369154628
    ,369154632
    ,369154636
    ,369154644
    ,369154648
    ,369154652
    ,369154660
    ,369154664
    ,369154668
    ,369154676
    ,369154680
    ,369154684
    ,369154692
    ,369154696
    ,369154700
    ,369154708
    ,369154712
    ,369154716
    ,369154724
    ,369154728
    ,369154732
    ,369154740
    ,369154744
    ,369154748
    ,369154756
    ,369154760
    ,369154764
    ,369154772
    ,369154776
    ,369154780
    ,369154788
    ,369154792
    ,369154796
    ,369157140
    ,369157144
    ,369157148
    ,369157156
    ,369157160
    ,369157164
    ,369157172
    ,369157176
    ,369157180
    ,369157204
    ,369157208
    ,369157212
    ,369157220
    ,369157224
    ,369157228
    ,369157236
    ,369157240
    ,369157244
    ,369157252
    ,369157256
    ,369157260
    ,369157268
    ,369157272
    ,369157276
    ,369157284
    ,369157288
    ,369157292
    ,369157300
    ,369157304
    ,369157308
    ,369157316
    ,369157320
    ,369157324
    ,369157332
    ,369157336
    ,369157340
    ,369157348
    ,369157352
    ,369157356
    ,369157364
    ,369157368
    ,369157372
    ,369157380
    ,369157384
    ,369157388
    ,369157396
    ,369157400
    ,369157404
    ,369157412
    ,369157416
    ,369157420
    ,369157428
    ,369157432
    ,369157436
    ,369157444
    ,369157448
    ,369157452
    ,369157460
    ,369157464
    ,369157468
    ,369157476
    ,369157480
    ,369157484
    ,369157492
    ,369157496
    ,369157500
    ,369157508
    ,369157512
    ,369157516
    ,369157524
    ,369157528
    ,369157532
    ,369157540
    ,369157544
    ,369157548
    ,369157556
    ,369157560
    ,369157564
    ,369157572
    ,369157576
    ,369157580
    ,369157588
    ,369157592
    ,369157596
    ,369157604
    ,369157608
    ,369157612
    ,369157620
    ,369157624
    ,369157628
    ,369157636
    ,369157640
    ,369157644
    ,369157652
    ,369157656
    ,369157660
    ,369157668
    ,369157672
    ,369157676
    ,369157684
    ,369157688
    ,369157692
    ,369157700
    ,369157704
    ,369157708
    ,369157716
    ,369157720
    ,369157724
    ,369157732
    ,369157736
    ,369157740
    ,369157748
    ,369157752
    ,369157756
    ,369157764
    ,369157768
    ,369157772
    ,369157780
    ,369157784
    ,369157788
    ,369157796
    ,369157800
    ,369157804
    ,369157812
    ,369157816
    ,369157820
    ,369157828
    ,369157832
    ,369157836
    ,369158164
    ,369158168
    ,369158172
    ,369158180
    ,369158184
    ,369158188
    ,369158196
    ,369158200
    ,369158204
    ,369158212
    ,369158216
    ,369158220
    ,369158260
    ,369158264
    ,369158268
    ,369158276
    ,369158280
    ,369158284
    ,369158292
    ,369158296
    ,369158300
    ,369158308
    ,369158312
    ,369158316
    ,369158324
    ,369158328
    ,369158332
    ,369158340
    ,369158344
    ,369158348
    ,369158356
    ,369158360
    ,369158364
    ,369158372
    ,369158376
    ,369158380
    ,369158388
    ,369158392
    ,369158396
    ,369158404
    ,369158408
    ,369158412
    ,369158420
    ,369158424
    ,369158428
    ,369158436
    ,369158440
    ,369158444
    ,369158452
    ,369158456
    ,369158460
    ,369158468
    ,369158472
    ,369158476
    ,369158484
    ,369158488
    ,369158492
    ,369158500
    ,369158504
    ,369158508
    ,369158516
    ,369158520
    ,369158524
    ,369158532
    ,369158536
    ,369158540
    ,369158548
    ,369158552
    ,369158556
    ,369158564
    ,369158568
    ,369158572
    ,369158580
    ,369158584
    ,369158588
    ,369158596
    ,369158600
    ,369158604
    ,369158612
    ,369158616
    ,369158620
    ,369158628
    ,369158632
    ,369158636
    ,369158644
    ,369158648
    ,369158652
    ,369158660
    ,369158664
    ,369158668
    ,369158676
    ,369158680
    ,369158684
    ,369158692
    ,369158696
    ,369158700
    ,369158708
    ,369158712
    ,369158716
    ,369158724
    ,369158728
    ,369158732
    ,369158756
    ,369158760
    ,369158764
    ,369158772
    ,369158776
    ,369158780
    ,369158788
    ,369158792
    ,369158796
    ,369158804
    ,369158808
    ,369158812
    ,369158820
    ,369158824
    ,369158828
    ,369158836
    ,369158840
    ,369158844
    ,369158852
    ,369158856
    ,369158860
    ,369158868
    ,369158872
    ,369158876
    ,369158884
    ,369158888
    ,369158892
    ,369169428
    ,369169432
    ,369169436
    ,369169444
    ,369169448
    ,369169452
    ,369169460
    ,369169464
    ,369169468
    ,369169476
    ,369169480
    ,369169484
    ,369169492
    ,369169496
    ,369169500
    ,369169508
    ,369169512
    ,369169516
    ,369169524
    ,369169528
    ,369169532
    ,369169556
    ,369169560
    ,369169564
    ,369169572
    ,369169576
    ,369169580
    ,369169588
    ,369169592
    ,369169596
    ,369169620
    ,369169624
    ,369169628
    ,369169636
    ,369169640
    ,369169644
    ,369169652
    ,369169656
    ,369169660
    ,369169668
    ,369169672
    ,369169676
    ,369169684
    ,369169688
    ,369169692
    ,369169700
    ,369169704
    ,369169708
    ,369169716
    ,369169720
    ,369169724
    ,369169732
    ,369169736
    ,369169740
    ,369169764
    ,369169768
    ,369169772
    ,369169780
    ,369169784
    ,369169788
    ,369169796
    ,369169800
    ,369169804
    ,369169812
    ,369169816
    ,369169820
    ,369169828
    ,369169832
    ,369169836
    ,369169844
    ,369169848
    ,369169852
    ,369169860
    ,369169864
    ,369169868
    ,369169876
    ,369169880
    ,369169884
    ,369169892
    ,369169896
    ,369169900
    ,369169908
    ,369169912
    ,369169916
    ,369169924
    ,369169928
    ,369169932
    ,369169940
    ,369169944
    ,369169948
    ,369169956
    ,369169960
    ,369169964
    ,369169972
    ,369169976
    ,369169980
    ,369169988
    ,369169992
    ,369169996
    ,369170004
    ,369170008
    ,369170012
    ,369170020
    ,369170024
    ,369170028
    ,369170036
    ,369170040
    ,369170044
    ,369170052
    ,369170056
    ,369170060
    ,369170068
    ,369170072
    ,369170076
    ,369170084
    ,369170088
    ,369170092
    ,369170100
    ,369170104
    ,369170108
    ,369170116
    ,369170120
    ,369170124
    ,369170132
    ,369170136
    ,369170140
    ,369170148
    ,369170152
    ,369170156
    ,369170164
    ,369170168
    ,369170172
    ,369170180
    ,369170184
    ,369170188
    ,369170196
    ,369170200
    ,369170204
    ,369170212
    ,369170216
    ,369170220
    ,369170228
    ,369170232
    ,369170236
    ,369170244
    ,369170248
    ,369170252
    ,369170468
    ,369170472
    ,369170476
    ,369170484
    ,369170488
    ,369170492
    ,369170516
    ,369170520
    ,369170524
    ,369170532
    ,369170536
    ,369170540
    ,369170548
    ,369170552
    ,369170556
    ,369170564
    ,369170568
    ,369170572
    ,369170580
    ,369170584
    ,369170588
    ,369170596
    ,369170600
    ,369170604
    ,369170612
    ,369170616
    ,369170620
    ,369170628
    ,369170632
    ,369170636
    ,369170644
    ,369170648
    ,369170652
    ,369170660
    ,369170664
    ,369170668
    ,369170676
    ,369170680
    ,369170684
    ,369170692
    ,369170696
    ,369170700
    ,369170708
    ,369170712
    ,369170716
    ,369170724
    ,369170728
    ,369170732
    ,369170740
    ,369170744
    ,369170748
    ,369170756
    ,369170760
    ,369170764
    ,369170772
    ,369170776
    ,369170780
    ,369170788
    ,369170792
    ,369170796
    ,369170804
    ,369170808
    ,369170812
    ,369170820
    ,369170824
    ,369170828
    ,369170836
    ,369170840
    ,369170844
    ,369170852
    ,369170856
    ,369170860
    ,369170868
    ,369170872
    ,369170876
    ,369170884
    ,369170888
    ,369170892
    ,369170900
    ,369170904
    ,369170908
    ,369170916
    ,369170920
    ,369170924
    ,369170932
    ,369170936
    ,369170940
    ,369170948
    ,369170952
    ,369170956
    ,369170964
    ,369170968
    ,369170972
    ,369170980
    ,369170984
    ,369170988
    ,369170996
    ,369171000
    ,369171004
    ,369171028
    ,369171032
    ,369171036
    ,369171060
    ,369171064
    ,369171068
    ,369171076
    ,369171080
    ,369171084
    ,369171092
    ,369171096
    ,369171100
    ,369171108
    ,369171112
    ,369171116
    ,369171124
    ,369171128
    ,369171132
    ,369171140
    ,369171144
    ,369171148
    ,369171156
    ,369171160
    ,369171164
    ,369171172
    ,369171176
    ,369171180
    ,369171204
    ,369171208
    ,369171212
    ,369171252
    ,369171256
    ,369171260
    ,369171268
    ,369171272
    ,369171276
    ,369171284
    ,369171288
    ,369171292
    ,369171332
    ,369171336
    ,369171340
    ,369173524
    ,369173528
    ,369173532
    ,369173540
    ,369173544
    ,369173548
    ,369173556
    ,369173560
    ,369173564
    ,369173572
    ,369173576
    ,369173580
    ,369173588
    ,369173592
    ,369173596
    ,369173604
    ,369173608
    ,369173612
    ,369173620
    ,369173624
    ,369173628
    ,369173636
    ,369173640
    ,369173644
    ,369173652
    ,369173656
    ,369173660
    ,369173668
    ,369173672
    ,369173676
    ,369173684
    ,369173688
    ,369173692
    ,369173700
    ,369173704
    ,369173708
    ,369173716
    ,369173720
    ,369173724
    ,369173732
    ,369173736
    ,369173740
    ,369173748
    ,369173752
    ,369173756
    ,369173764
    ,369173768
    ,369173772
    ,369173780
    ,369173784
    ,369173788
    ,369173796
    ,369173800
    ,369173804
    ,369173812
    ,369173816
    ,369173820
    ,369173828
    ,369173832
    ,369173836
    ,369173844
    ,369173848
    ,369173852
    ,369173860
    ,369173864
    ,369173868
    ,369173876
    ,369173880
    ,369173884
    ,369173892
    ,369173896
    ,369173900
    ,369173908
    ,369173912
    ,369173916
    ,369173924
    ,369173928
    ,369173932
    ,369173940
    ,369173944
    ,369173948
    ,369173956
    ,369173960
    ,369173964
    ,369173972
    ,369173976
    ,369173980
    ,369173988
    ,369173992
    ,369173996
    ,369174004
    ,369174008
    ,369174012
    ,369174020
    ,369174024
    ,369174028
    ,369174036
    ,369174040
    ,369174044
    ,369174052
    ,369174056
    ,369174060
    ,369174068
    ,369174072
    ,369174076
    ,369174084
    ,369174088
    ,369174092
    ,369174100
    ,369174104
    ,369174108
    ,369174116
    ,369174120
    ,369174124
    ,369174132
    ,369174136
    ,369174140
    ,369174148
    ,369174152
    ,369174156
    ,369174164
    ,369174168
    ,369174172
    ,369174180
    ,369174184
    ,369174188
    ,369174196
    ,369174200
    ,369174204
    ,369174212
    ,369174216
    ,369174220
    ,369174228
    ,369174232
    ,369174236
    ,369174244
    ,369174248
    ,369174252
    ,369174260
    ,369174264
    ,369174268
    ,369174276
    ,369174280
    ,369174284
    ,369174292
    ,369174296
    ,369174300
    ,369174308
    ,369174312
    ,369174316
    ,369174324
    ,369174328
    ,369174332
    ,369174340
    ,369174344
    ,369174348
    ,369174548
    ,369174552
    ,369174556
    ,369174564
    ,369174568
    ,369174572
    ,369174580
    ,369174584
    ,369174588
    ,369174596
    ,369174600
    ,369174604
    ,369174612
    ,369174616
    ,369174620
    ,369174628
    ,369174632
    ,369174636
    ,369174644
    ,369174648
    ,369174652
    ,369174660
    ,369174664
    ,369174668
    ,369174676
    ,369174680
    ,369174684
    ,369174692
    ,369174696
    ,369174700
    ,369174708
    ,369174712
    ,369174716
    ,369174724
    ,369174728
    ,369174732
    ,369174740
    ,369174744
    ,369174748
    ,369174756
    ,369174760
    ,369174764
    ,369174772
    ,369174776
    ,369174780
    ,369174788
    ,369174792
    ,369174796
    ,369174804
    ,369174808
    ,369174812
    ,369174820
    ,369174824
    ,369174828
    ,369174836
    ,369174840
    ,369174844
    ,369174852
    ,369174856
    ,369174860
    ,369174868
    ,369174872
    ,369174876
    ,369174884
    ,369174888
    ,369174892
    ,369174900
    ,369174904
    ,369174908
    ,369174916
    ,369174920
    ,369174924
    ,369174932
    ,369174936
    ,369174940
    ,369174948
    ,369174952
    ,369174956
    ,369174980
    ,369174984
    ,369174988
    ,369174996
    ,369175000
    ,369175004
    ,369175012
    ,369175016
    ,369175020
    ,369175028
    ,369175032
    ,369175036
    ,369175044
    ,369175048
    ,369175052
    ,369175060
    ,369175064
    ,369175068
    ,369175076
    ,369175080
    ,369175084
    ,369175092
    ,369175096
    ,369175100
    ,369175108
    ,369175112
    ,369175116
    ,369175124
    ,369175128
    ,369175132
    ,369175140
    ,369175144
    ,369175148
    ,369175156
    ,369175160
    ,369175164
    ,369175172
    ,369175176
    ,369175180
    ,369175188
    ,369175192
    ,369175196
    ,369175204
    ,369175208
    ,369175212
    ,369175220
    ,369175224
    ,369175228
    ,369175236
    ,369175240
    ,369175244
    ,369175252
    ,369175256
    ,369175260
    ,369175268
    ,369175272
    ,369175276
    ,369175284
    ,369175288
    ,369175292
    ,369175300
    ,369175304
    ,369175308
    ,369175316
    ,369175320
    ,369175324
    ,369175332
    ,369175336
    ,369175340
    ,369175348
    ,369175352
    ,369175356
    ,369175364
    ,369175368
    ,369175372
    ,369175380
    ,369175384
    ,369175388
    ,369175396
    ,369175400
    ,369175404
    ,369175412
    ,369175416
    ,369175420
    ,369175428
    ,369175432
    ,369175436
};
  const int N_test=1;
  // int detids_test[N_test]={/*369121381,*/369121382/*,369121605,369121606,369125861,369125862*/};  
  // int detids_test[N_test]={369141941, 369141946, 369141949, 369141942, 369141945, 369141950};

  // Fit("TIB", "20120405_run190459", detids_2012, N_2012); // lack stats
  // Fit("TIB", "20120506_run193541", detids_2012, N_2012);// PS current  not in DB

  // L2 2012
  const int N_L2_2012=6;
  int detids_L2_2012[N_L2_2012]={369136677, 369136678, 369136681, 369136682, 369136685, 369136686};
  //Fit("TIB_L2", "20120405_run190459", detids_L2_2012, N_L2_2012);
  
  // August 2012 , only PS currents (L1+L2)
  const int N=4;
  int detids[N]={369125861, 369125862, 369136681, 369136682};
  //Fit("TIB", "L1_20120812_run200786", detids, N);  
  
  const int N_1PS=6;
  int detids_1PS[N_1PS]={369121605,369121606,369121609,369121610,369121613,369121614};  
  //Fit("TIB_L1", "20120928_run203832", detids_1PS, N_1PS, "Steps/bad_periods_20120928_run203832.txt");
  // Fit("TIB_L1", "20120928_run203832", detids_2012, N_2012, "Steps/bad_periods_20120928_run203832.txt");



// Fit("TIB", "20120812_run200786", detids_test, N_test, "");


// Fit("TIB", "20121130_run208339", detids_test, N_test, "");//scan is not good : two points per bin due to the scan

//---------run to analyse------------//
// 

// Fit("TIB", "20160612_run274969", detids_2012, N_2012, "");//Lack of stast : need the exra points


// // Fit("TIB", "20181118_run326883", detids_2012, N_2012, "");//there is an outlier

  const int NFit = 10;
  std::vector<std::vector<float>> VfdFits;

if (subdet == "TIB")
  {
    // VfdFits.push_back(Fit("TIB", "20120506_run193541", detids_2012, N_2012, ""));
    // // // // VfdFits.push_back(Fit("TIB", "20120728_run199832", detids_2012, N_2012, ""));
    // // // // VfdFits.push_back(Fit("TIB", "20120928_run203832", detids_2012, N_2012, ""));
    // // // // VfdFits.push_back(Fit("TIB", "20121130_run208339", detids_2012, N_2012, ""));


    //   VfdFits.push_back(Fit("TIB", "20151121_run262254", detids_2012, N_2012, ""));
    //   VfdFits.push_back(Fit("TIB", "20160706_run276437", detids_2012, N_2012, ""));
    //   // VfdFits.push_back(Fit("TIB", "20160803_run278167", detids_2012, N_2012, ""));
    //   VfdFits.push_back(Fit("TIB", "20160909_run280385", detids_2012, N_2012, ""));
    //   // VfdFits.push_back(Fit("TIB", "20170527_run295324", detids_2012, N_2012, ""));
    //   VfdFits.push_back(Fit("TIB", "20171030_run305862", detids_2012, N_2012, ""));

      // VfdFits.push_back(Fit("TIB", "20180530_run317182", detids_2012, N_2012, ""));
      // VfdFits.push_back(Fit("TIB", "20180611_run317683", detids_2012, N_2012, ""));
      // VfdFits.push_back(Fit("TIB", "20181018_run324841", detids_2012, N_2012, ""));
      // VfdFits.push_back(Fit("TIB", "20181115_run326776", detids_2012, N_2012, ""));

      // VfdFits.push_back(Fit("TIB", "20211029_run346395", detids_2012, N_2012, ""));
      // VfdFits.push_back(Fit("TIB", "20220605_run353060", detids_2012, N_2012, ""));//Full
      // // Fit("TIB", "20220922_run359691", detids_2012, N_2012, "");//do not run on detids_2012
      // // Fit("TIB", "20221126_run362696", detids_2012, N_2012, "");//do not run on detids_2012
      // VfdFits.push_back(Fit("TIB", "20230407_run365843", detids_2012, N_2012, ""));//FUll
      // VfdFits.push_back(Fit("TIB", "20230609_run368669", detids_2012, N_2012, ""));
      // VfdFits.push_back(Fit("TIB", "20230907_run373060", detids_2012, N_2012, ""));

//--------------Full Scans -----------------//

      // // VfdFits.push_back(Fit("TIB", "20110315_run160497",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20120510_run193928",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20160423_run271056",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20170527_run295376",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20170924_run303824",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20180418_run314574",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20180923_run323370",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20220605_run353060",  Full_detid, N_Full, ""));
      VfdFits.push_back(Fit("TIB", "20230407_run365843",  Full_detid, N_Full, ""));
// ------------------------------------------------//
    // Missign leakae current information
      // VfdFits.push_back(Fit("TIB", "20231025_run375658", detids_2012, N_2012, ""));

      for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
        {
          for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
            {
              // FitsVfd[j]+=RunVfd[i][j];
              // std::cout<<"FitsVfd[j] : "<<FitsVfd[j]<<std::endl;
              // std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<RunVfd[i][j]<<std::endl;
              std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
            }
        }
        return VfdFits;
  }
//--------end of -run to analyse------------//

////////////////////////////////////////////
//////////////////////////////////////////


  //Fit("TIB", "20180418_run314574", detids_1PS, 2, "");
  //Fit("TIB", "20180418_run314574", detids_2012, N_2012, "");
  //Fit("TIB", "20180418_run314574", detids_test, N_test, "");

    // Fit("TIB", "20180419_run314755", detids_test, N_test, "");
  // Fit("TIB", "20180801_run320674", detids_test, N_test, "Steps/bad_periods_20180801_run320674.txt"); 
  //Fit("TIB", "20180923_run323370", detids_test, N_test, "Steps/bad_periods_20180923_run323370.txt");
  //  Fit("TIB", "20181115_run326776", detids_test, N_test, "");



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

    const int nTEST=2;
  int detids_TEST[nTEST]={369124397, 369124414};
  // // Fit("TIB", "20230407_run365843", detids_TEST, nTEST, "");
  // // Fit("TIB", "20231025_run375658", detids_1PS, N_1PS, ""); 
  // //  Fit("TIB", "20170831_run302131", detids_1PS, N_1PS, ""); 
  
  // Fit("TIB", "20181118_run326883", detids_test, N_test, "");//there is an outlier
//     Fit("TIB", "20230407_run365843", detids_test, N_test, "");
//     Fit("TIB", "20211029_run346395", detids_test, N_test, ""); //(first set of detids_test)

  //---------------------------------//
  //---------------------------------//
  //               TOB               //
  //---------------------------------//
  //---------------------------------//
  const int N_TOB=6;
  int detids_TOB[N_TOB]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528};
  
  const int N_TOB_2012=18;//18
  int detids_TOB_2012[N_TOB_2012]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528,
                         436232901, 436232902, 436232905, 436232906, 436232909, 436232910,
						 436232913, 436232914, 436232917, 436232918, 436232921, 436232922};

  // const int N_TOB_2012_R3=12;//18
  // int detids_TOB_2012[N_TOB_2012]={436281508, 436281512, 436281516, 436281520, 436281524, 436281528,
  //                        436232901, 436232902, 436232905, 436232906, 436232909, 436232910,
	// 					 436232913, 436232914, 436232917, 436232918, 436232921, 436232922};

             //---------run to analyse------------//
	     
if (subdet == "TOB")
  {

  const int N_FullTOB_2012=5137;//18
  int detids_FullTOB_2012[N_FullTOB_2012]={436228133
    ,436228134
    ,436228137
    ,436228138
    ,436228141
    ,436228142
    ,436228145
    ,436228146
    ,436228149
    ,436228150
    ,436228153
    ,436228154
    ,436228165
    ,436228166
    ,436228169
    ,436228170
    ,436228173
    ,436228174
    ,436228177
    ,436228178
    ,436228181
    ,436228182
    ,436228185
    ,436228186
    ,436228197
    ,436228198
    ,436228201
    ,436228202
    ,436228205
    ,436228206
    ,436228209
    ,436228210
    ,436228213
    ,436228214
    ,436228217
    ,436228218
    ,436228229
    ,436228230
    ,436228233
    ,436228234
    ,436228237
    ,436228238
    ,436228241
    ,436228242
    ,436228245
    ,436228246
    ,436228249
    ,436228250
    ,436228261
    ,436228262
    ,436228265
    ,436228266
    ,436228269
    ,436228270
    ,436228273
    ,436228274
    ,436228277
    ,436228278
    ,436228281
    ,436228282
    ,436228293
    ,436228294
    ,436228297
    ,436228298
    ,436228301
    ,436228302
    ,436228305
    ,436228306
    ,436228309
    ,436228310
    ,436228313
    ,436228314
    ,436228325
    ,436228326
    ,436228329
    ,436228330
    ,436228333
    ,436228334
    ,436228337
    ,436228338
    ,436228341
    ,436228342
    ,436228345
    ,436228346
    ,436228357
    ,436228358
    ,436228361
    ,436228362
    ,436228365
    ,436228366
    ,436228369
    ,436228370
    ,436228373
    ,436228374
    ,436228377
    ,436228378
    ,436228389
    ,436228390
    ,436228393
    ,436228394
    ,436228397
    ,436228398
    ,436228401
    ,436228402
    ,436228405
    ,436228406
    ,436228409
    ,436228410
    ,436228421
    ,436228422
    ,436228425
    ,436228426
    ,436228429
    ,436228430
    ,436228433
    ,436228434
    ,436228437
    ,436228438
    ,436228441
    ,436228442
    ,436228453
    ,436228454
    ,436228457
    ,436228458
    ,436228461
    ,436228462
    ,436228465
    ,436228466
    ,436228469
    ,436228470
    ,436228473
    ,436228474
    ,436228485
    ,436228486
    ,436228489
    ,436228490
    ,436228493
    ,436228494
    ,436228497
    ,436228498
    ,436228501
    ,436228502
    ,436228505
    ,436228506
    ,436228517
    ,436228518
    ,436228521
    ,436228522
    ,436228525
    ,436228526
    ,436228529
    ,436228530
    ,436228533
    ,436228534
    ,436228537
    ,436228538
    ,436228549
    ,436228550
    ,436228553
    ,436228554
    ,436228557
    ,436228558
    ,436228561
    ,436228562
    ,436228565
    ,436228566
    ,436228569
    ,436228570
    ,436228581
    ,436228582
    ,436228585
    ,436228586
    ,436228589
    ,436228590
    ,436228593
    ,436228594
    ,436228597
    ,436228598
    ,436228601
    ,436228602
    ,436228613
    ,436228614
    ,436228617
    ,436228618
    ,436228621
    ,436228622
    ,436228625
    ,436228626
    ,436228629
    ,436228630
    ,436228633
    ,436228634
    ,436228645
    ,436228646
    ,436228649
    ,436228650
    ,436228653
    ,436228654
    ,436228657
    ,436228658
    ,436228661
    ,436228662
    ,436228665
    ,436228666
    ,436228677
    ,436228678
    ,436228681
    ,436228682
    ,436228685
    ,436228686
    ,436228689
    ,436228690
    ,436228693
    ,436228694
    ,436228697
    ,436228698
    ,436228709
    ,436228710
    ,436228713
    ,436228714
    ,436228717
    ,436228718
    ,436228721
    ,436228722
    ,436228725
    ,436228726
    ,436228729
    ,436228730
    ,436228741
    ,436228742
    ,436228745
    ,436228746
    ,436228749
    ,436228750
    ,436228753
    ,436228754
    ,436228757
    ,436228758
    ,436228761
    ,436228762
    ,436228773
    ,436228774
    ,436228777
    ,436228778
    ,436228781
    ,436228782
    ,436228785
    ,436228786
    ,436228789
    ,436228790
    ,436228793
    ,436228794
    ,436228805
    ,436228806
    ,436228809
    ,436228810
    ,436228813
    ,436228814
    ,436228817
    ,436228818
    ,436228821
    ,436228822
    ,436228825
    ,436228826
    ,436228837
    ,436228838
    ,436228841
    ,436228842
    ,436228845
    ,436228846
    ,436228849
    ,436228850
    ,436228853
    ,436228854
    ,436228857
    ,436228858
    ,436228869
    ,436228870
    ,436228873
    ,436228874
    ,436228877
    ,436228878
    ,436228881
    ,436228882
    ,436228885
    ,436228886
    ,436228889
    ,436228890
    ,436228901
    ,436228902
    ,436228905
    ,436228906
    ,436228909
    ,436228910
    ,436228913
    ,436228914
    ,436228917
    ,436228918
    ,436228921
    ,436228922
    ,436228933
    ,436228934
    ,436228937
    ,436228938
    ,436228941
    ,436228942
    ,436228945
    ,436228946
    ,436228949
    ,436228950
    ,436228953
    ,436228954
    ,436228965
    ,436228966
    ,436228969
    ,436228970
    ,436228973
    ,436228974
    ,436228977
    ,436228978
    ,436228981
    ,436228982
    ,436228985
    ,436228986
    ,436228997
    ,436228998
    ,436229001
    ,436229002
    ,436229005
    ,436229006
    ,436229009
    ,436229010
    ,436229013
    ,436229014
    ,436229017
    ,436229018
    ,436229029
    ,436229030
    ,436229033
    ,436229034
    ,436229037
    ,436229038
    ,436229041
    ,436229042
    ,436229045
    ,436229046
    ,436229049
    ,436229050
    ,436229061
    ,436229062
    ,436229065
    ,436229066
    ,436229069
    ,436229070
    ,436229073
    ,436229074
    ,436229077
    ,436229078
    ,436229081
    ,436229082
    ,436229093
    ,436229094
    ,436229097
    ,436229098
    ,436229101
    ,436229102
    ,436229105
    ,436229106
    ,436229109
    ,436229110
    ,436229113
    ,436229114
    ,436229125
    ,436229126
    ,436229129
    ,436229130
    ,436229133
    ,436229134
    ,436229137
    ,436229138
    ,436229141
    ,436229142
    ,436229145
    ,436229146
    ,436229157
    ,436229158
    ,436229161
    ,436229162
    ,436229165
    ,436229166
    ,436229169
    ,436229170
    ,436229173
    ,436229174
    ,436229177
    ,436229178
    ,436229189
    ,436229190
    ,436229193
    ,436229194
    ,436229197
    ,436229198
    ,436229201
    ,436229202
    ,436229205
    ,436229206
    ,436229209
    ,436229210
    ,436229221
    ,436229222
    ,436229225
    ,436229226
    ,436229229
    ,436229230
    ,436229233
    ,436229234
    ,436229237
    ,436229238
    ,436229241
    ,436229242
    ,436229253
    ,436229254
    ,436229257
    ,436229258
    ,436229261
    ,436229262
    ,436229265
    ,436229266
    ,436229269
    ,436229270
    ,436229273
    ,436229274
    ,436229285
    ,436229286
    ,436229289
    ,436229290
    ,436229293
    ,436229294
    ,436229297
    ,436229298
    ,436229301
    ,436229302
    ,436229305
    ,436229306
    ,436229317
    ,436229318
    ,436229321
    ,436229322
    ,436229325
    ,436229326
    ,436229329
    ,436229330
    ,436229333
    ,436229334
    ,436229337
    ,436229338
    ,436229349
    ,436229350
    ,436229353
    ,436229354
    ,436229357
    ,436229358
    ,436229361
    ,436229362
    ,436229365
    ,436229366
    ,436229369
    ,436229370
    ,436229381
    ,436229382
    ,436229385
    ,436229386
    ,436229389
    ,436229390
    ,436229393
    ,436229394
    ,436229397
    ,436229398
    ,436229401
    ,436229402
    ,436229413
    ,436229414
    ,436229417
    ,436229418
    ,436229421
    ,436229422
    ,436229425
    ,436229426
    ,436229429
    ,436229430
    ,436229433
    ,436229434
    ,436229445
    ,436229446
    ,436229449
    ,436229450
    ,436229453
    ,436229454
    ,436229457
    ,436229458
    ,436229461
    ,436229462
    ,436229465
    ,436229466
    ,436232229
    ,436232230
    ,436232233
    ,436232234
    ,436232237
    ,436232238
    ,436232241
    ,436232242
    ,436232245
    ,436232246
    ,436232249
    ,436232250
    ,436232261
    ,436232262
    ,436232265
    ,436232266
    ,436232269
    ,436232270
    ,436232273
    ,436232274
    ,436232277
    ,436232278
    ,436232281
    ,436232282
    ,436232293
    ,436232294
    ,436232297
    ,436232298
    ,436232301
    ,436232302
    ,436232305
    ,436232306
    ,436232309
    ,436232310
    ,436232313
    ,436232314
    ,436232325
    ,436232326
    ,436232329
    ,436232330
    ,436232333
    ,436232334
    ,436232337
    ,436232338
    ,436232341
    ,436232342
    ,436232345
    ,436232346
    ,436232357
    ,436232358
    ,436232361
    ,436232362
    ,436232365
    ,436232366
    ,436232369
    ,436232370
    ,436232373
    ,436232374
    ,436232377
    ,436232378
    ,436232389
    ,436232390
    ,436232393
    ,436232394
    ,436232397
    ,436232398
    ,436232401
    ,436232402
    ,436232405
    ,436232406
    ,436232409
    ,436232410
    ,436232421
    ,436232422
    ,436232425
    ,436232426
    ,436232429
    ,436232430
    ,436232433
    ,436232434
    ,436232437
    ,436232438
    ,436232441
    ,436232442
    ,436232453
    ,436232454
    ,436232457
    ,436232458
    ,436232461
    ,436232462
    ,436232465
    ,436232466
    ,436232469
    ,436232470
    ,436232473
    ,436232474
    ,436232485
    ,436232486
    ,436232489
    ,436232490
    ,436232493
    ,436232494
    ,436232497
    ,436232498
    ,436232501
    ,436232502
    ,436232505
    ,436232506
    ,436232517
    ,436232518
    ,436232521
    ,436232522
    ,436232525
    ,436232526
    ,436232529
    ,436232530
    ,436232533
    ,436232534
    ,436232537
    ,436232538
    ,436232549
    ,436232550
    ,436232553
    ,436232554
    ,436232557
    ,436232558
    ,436232561
    ,436232562
    ,436232565
    ,436232566
    ,436232569
    ,436232570
    ,436232581
    ,436232582
    ,436232585
    ,436232586
    ,436232589
    ,436232590
    ,436232593
    ,436232594
    ,436232597
    ,436232598
    ,436232601
    ,436232602
    ,436232613
    ,436232614
    ,436232617
    ,436232618
    ,436232621
    ,436232622
    ,436232625
    ,436232626
    ,436232629
    ,436232630
    ,436232633
    ,436232634
    ,436232645
    ,436232646
    ,436232649
    ,436232650
    ,436232653
    ,436232654
    ,436232657
    ,436232658
    ,436232661
    ,436232662
    ,436232665
    ,436232666
    ,436232677
    ,436232678
    ,436232681
    ,436232682
    ,436232685
    ,436232686
    ,436232689
    ,436232690
    ,436232693
    ,436232694
    ,436232697
    ,436232698
    ,436232709
    ,436232710
    ,436232713
    ,436232714
    ,436232717
    ,436232718
    ,436232721
    ,436232722
    ,436232725
    ,436232726
    ,436232729
    ,436232730
    ,436232741
    ,436232742
    ,436232745
    ,436232746
    ,436232749
    ,436232750
    ,436232753
    ,436232754
    ,436232757
    ,436232758
    ,436232761
    ,436232762
    ,436232773
    ,436232777
    ,436232778
    ,436232781
    ,436232782
    ,436232785
    ,436232786
    ,436232789
    ,436232790
    ,436232793
    ,436232794
    ,436232805
    ,436232806
    ,436232809
    ,436232810
    ,436232813
    ,436232814
    ,436232817
    ,436232818
    ,436232821
    ,436232822
    ,436232825
    ,436232826
    ,436232837
    ,436232838
    ,436232841
    ,436232842
    ,436232845
    ,436232846
    ,436232849
    ,436232850
    ,436232853
    ,436232854
    ,436232857
    ,436232858
    ,436232869
    ,436232870
    ,436232873
    ,436232874
    ,436232877
    ,436232878
    ,436232881
    ,436232882
    ,436232885
    ,436232886
    ,436232889
    ,436232890
    ,436232901
    ,436232902
    ,436232905
    ,436232906
    ,436232909
    ,436232910
    ,436232913
    ,436232914
    ,436232917
    ,436232918
    ,436232921
    ,436232922
    ,436232933
    ,436232934
    ,436232937
    ,436232938
    ,436232941
    ,436232942
    ,436232945
    ,436232946
    ,436232949
    ,436232950
    ,436232953
    ,436232954
    ,436232965
    ,436232966
    ,436232969
    ,436232970
    ,436232973
    ,436232974
    ,436232977
    ,436232978
    ,436232981
    ,436232982
    ,436232985
    ,436232986
    ,436232997
    ,436232998
    ,436233001
    ,436233002
    ,436233005
    ,436233006
    ,436233009
    ,436233010
    ,436233013
    ,436233014
    ,436233017
    ,436233018
    ,436233029
    ,436233030
    ,436233033
    ,436233034
    ,436233037
    ,436233038
    ,436233041
    ,436233042
    ,436233045
    ,436233046
    ,436233049
    ,436233050
    ,436233061
    ,436233062
    ,436233065
    ,436233066
    ,436233069
    ,436233070
    ,436233073
    ,436233074
    ,436233077
    ,436233078
    ,436233081
    ,436233082
    ,436233093
    ,436233094
    ,436233097
    ,436233098
    ,436233101
    ,436233102
    ,436233105
    ,436233106
    ,436233109
    ,436233110
    ,436233113
    ,436233114
    ,436233125
    ,436233126
    ,436233129
    ,436233130
    ,436233133
    ,436233134
    ,436233137
    ,436233138
    ,436233141
    ,436233142
    ,436233145
    ,436233146
    ,436233157
    ,436233158
    ,436233161
    ,436233162
    ,436233165
    ,436233166
    ,436233169
    ,436233170
    ,436233173
    ,436233174
    ,436233177
    ,436233178
    ,436233189
    ,436233190
    ,436233193
    ,436233194
    ,436233197
    ,436233198
    ,436233201
    ,436233202
    ,436233205
    ,436233206
    ,436233209
    ,436233210
    ,436233221
    ,436233222
    ,436233225
    ,436233226
    ,436233229
    ,436233230
    ,436233233
    ,436233234
    ,436233237
    ,436233238
    ,436233241
    ,436233242
    ,436233253
    ,436233254
    ,436233257
    ,436233258
    ,436233261
    ,436233262
    ,436233265
    ,436233266
    ,436233269
    ,436233270
    ,436233273
    ,436233274
    ,436233285
    ,436233286
    ,436233289
    ,436233290
    ,436233293
    ,436233294
    ,436233297
    ,436233298
    ,436233301
    ,436233302
    ,436233305
    ,436233306
    ,436233317
    ,436233318
    ,436233321
    ,436233322
    ,436233325
    ,436233326
    ,436233329
    ,436233330
    ,436233333
    ,436233334
    ,436233337
    ,436233338
    ,436233349
    ,436233350
    ,436233353
    ,436233354
    ,436233357
    ,436233358
    ,436233361
    ,436233362
    ,436233365
    ,436233366
    ,436233369
    ,436233370
    ,436233381
    ,436233382
    ,436233385
    ,436233386
    ,436233389
    ,436233390
    ,436233393
    ,436233394
    ,436233397
    ,436233398
    ,436233401
    ,436233402
    ,436233413
    ,436233414
    ,436233417
    ,436233418
    ,436233421
    ,436233422
    ,436233425
    ,436233426
    ,436233429
    ,436233430
    ,436233433
    ,436233434
    ,436233445
    ,436233446
    ,436233449
    ,436233450
    ,436233453
    ,436233454
    ,436233457
    ,436233458
    ,436233461
    ,436233462
    ,436233465
    ,436233466
    ,436233477
    ,436233478
    ,436233481
    ,436233482
    ,436233485
    ,436233486
    ,436233489
    ,436233490
    ,436233493
    ,436233494
    ,436233497
    ,436233498
    ,436233509
    ,436233510
    ,436233513
    ,436233514
    ,436233517
    ,436233518
    ,436233521
    ,436233522
    ,436233525
    ,436233526
    ,436233529
    ,436233530
    ,436233541
    ,436233542
    ,436233545
    ,436233546
    ,436233549
    ,436233550
    ,436233553
    ,436233554
    ,436233557
    ,436233558
    ,436233561
    ,436233562
    ,436244517
    ,436244518
    ,436244521
    ,436244522
    ,436244525
    ,436244526
    ,436244529
    ,436244530
    ,436244533
    ,436244534
    ,436244537
    ,436244538
    ,436244549
    ,436244550
    ,436244553
    ,436244554
    ,436244557
    ,436244558
    ,436244561
    ,436244562
    ,436244565
    ,436244566
    ,436244569
    ,436244570
    ,436244581
    ,436244582
    ,436244585
    ,436244586
    ,436244589
    ,436244590
    ,436244593
    ,436244594
    ,436244597
    ,436244598
    ,436244601
    ,436244602
    ,436244613
    ,436244614
    ,436244617
    ,436244618
    ,436244621
    ,436244622
    ,436244625
    ,436244626
    ,436244629
    ,436244630
    ,436244633
    ,436244634
    ,436244645
    ,436244646
    ,436244649
    ,436244650
    ,436244653
    ,436244654
    ,436244657
    ,436244658
    ,436244661
    ,436244662
    ,436244665
    ,436244666
    ,436244677
    ,436244678
    ,436244681
    ,436244682
    ,436244685
    ,436244686
    ,436244689
    ,436244690
    ,436244693
    ,436244694
    ,436244697
    ,436244698
    ,436244709
    ,436244710
    ,436244713
    ,436244714
    ,436244717
    ,436244718
    ,436244721
    ,436244722
    ,436244725
    ,436244726
    ,436244729
    ,436244730
    ,436244741
    ,436244742
    ,436244745
    ,436244746
    ,436244749
    ,436244750
    ,436244753
    ,436244754
    ,436244757
    ,436244758
    ,436244761
    ,436244762
    ,436244773
    ,436244774
    ,436244777
    ,436244778
    ,436244781
    ,436244782
    ,436244785
    ,436244786
    ,436244789
    ,436244790
    ,436244793
    ,436244794
    ,436244805
    ,436244806
    ,436244809
    ,436244810
    ,436244813
    ,436244814
    ,436244817
    ,436244818
    ,436244821
    ,436244822
    ,436244825
    ,436244826
    ,436244837
    ,436244838
    ,436244841
    ,436244842
    ,436244845
    ,436244846
    ,436244849
    ,436244850
    ,436244853
    ,436244854
    ,436244857
    ,436244858
    ,436244869
    ,436244870
    ,436244873
    ,436244874
    ,436244877
    ,436244878
    ,436244881
    ,436244882
    ,436244885
    ,436244886
    ,436244889
    ,436244890
    ,436244901
    ,436244902
    ,436244905
    ,436244906
    ,436244909
    ,436244910
    ,436244913
    ,436244914
    ,436244917
    ,436244918
    ,436244921
    ,436244922
    ,436244933
    ,436244934
    ,436244937
    ,436244938
    ,436244941
    ,436244942
    ,436244945
    ,436244946
    ,436244949
    ,436244950
    ,436244953
    ,436244954
    ,436244965
    ,436244966
    ,436244969
    ,436244970
    ,436244973
    ,436244974
    ,436244977
    ,436244978
    ,436244981
    ,436244982
    ,436244985
    ,436244986
    ,436244997
    ,436244998
    ,436245001
    ,436245002
    ,436245005
    ,436245006
    ,436245009
    ,436245010
    ,436245013
    ,436245014
    ,436245017
    ,436245018
    ,436245029
    ,436245030
    ,436245033
    ,436245034
    ,436245037
    ,436245038
    ,436245041
    ,436245042
    ,436245045
    ,436245046
    ,436245049
    ,436245050
    ,436245061
    ,436245062
    ,436245065
    ,436245066
    ,436245069
    ,436245070
    ,436245073
    ,436245074
    ,436245077
    ,436245078
    ,436245081
    ,436245082
    ,436245093
    ,436245094
    ,436245097
    ,436245098
    ,436245101
    ,436245102
    ,436245105
    ,436245106
    ,436245109
    ,436245110
    ,436245113
    ,436245114
    ,436245125
    ,436245126
    ,436245129
    ,436245130
    ,436245133
    ,436245134
    ,436245137
    ,436245138
    ,436245141
    ,436245142
    ,436245145
    ,436245146
    ,436245157
    ,436245158
    ,436245161
    ,436245162
    ,436245165
    ,436245166
    ,436245169
    ,436245170
    ,436245173
    ,436245174
    ,436245177
    ,436245178
    ,436245189
    ,436245190
    ,436245193
    ,436245194
    ,436245197
    ,436245198
    ,436245201
    ,436245202
    ,436245205
    ,436245206
    ,436245209
    ,436245210
    ,436245221
    ,436245222
    ,436245225
    ,436245226
    ,436245229
    ,436245230
    ,436245233
    ,436245234
    ,436245237
    ,436245238
    ,436245241
    ,436245242
    ,436245253
    ,436245254
    ,436245257
    ,436245258
    ,436245261
    ,436245262
    ,436245265
    ,436245266
    ,436245269
    ,436245270
    ,436245273
    ,436245274
    ,436245285
    ,436245286
    ,436245289
    ,436245290
    ,436245293
    ,436245294
    ,436245297
    ,436245298
    ,436245301
    ,436245302
    ,436245305
    ,436245306
    ,436245317
    ,436245318
    ,436245321
    ,436245322
    ,436245325
    ,436245326
    ,436245329
    ,436245330
    ,436245333
    ,436245334
    ,436245337
    ,436245338
    ,436245349
    ,436245350
    ,436245353
    ,436245354
    ,436245357
    ,436245358
    ,436245361
    ,436245362
    ,436245365
    ,436245366
    ,436245369
    ,436245370
    ,436245381
    ,436245382
    ,436245385
    ,436245386
    ,436245389
    ,436245390
    ,436245393
    ,436245394
    ,436245397
    ,436245398
    ,436245401
    ,436245402
    ,436245413
    ,436245414
    ,436245417
    ,436245418
    ,436245421
    ,436245422
    ,436245425
    ,436245426
    ,436245429
    ,436245430
    ,436245433
    ,436245434
    ,436245445
    ,436245446
    ,436245449
    ,436245450
    ,436245453
    ,436245454
    ,436245457
    ,436245458
    ,436245461
    ,436245462
    ,436245465
    ,436245466
    ,436245477
    ,436245478
    ,436245481
    ,436245482
    ,436245485
    ,436245486
    ,436245489
    ,436245490
    ,436245493
    ,436245494
    ,436245497
    ,436245498
    ,436245509
    ,436245510
    ,436245513
    ,436245514
    ,436245517
    ,436245518
    ,436245521
    ,436245522
    ,436245525
    ,436245526
    ,436245529
    ,436245530
    ,436245541
    ,436245542
    ,436245545
    ,436245546
    ,436245549
    ,436245550
    ,436245553
    ,436245554
    ,436245557
    ,436245558
    ,436245561
    ,436245562
    ,436245573
    ,436245574
    ,436245577
    ,436245578
    ,436245581
    ,436245582
    ,436245585
    ,436245586
    ,436245589
    ,436245590
    ,436245593
    ,436245594
    ,436245605
    ,436245606
    ,436245609
    ,436245610
    ,436245613
    ,436245614
    ,436245617
    ,436245618
    ,436245621
    ,436245622
    ,436245625
    ,436245626
    ,436245637
    ,436245638
    ,436245641
    ,436245642
    ,436245645
    ,436245646
    ,436245649
    ,436245650
    ,436245653
    ,436245654
    ,436245657
    ,436245658
    ,436245669
    ,436245670
    ,436245673
    ,436245674
    ,436245677
    ,436245678
    ,436245681
    ,436245682
    ,436245685
    ,436245686
    ,436245689
    ,436245690
    ,436245701
    ,436245702
    ,436245705
    ,436245706
    ,436245709
    ,436245710
    ,436245713
    ,436245714
    ,436245717
    ,436245718
    ,436245721
    ,436245722
    ,436245733
    ,436245734
    ,436245737
    ,436245738
    ,436245741
    ,436245742
    ,436245745
    ,436245746
    ,436245749
    ,436245750
    ,436245753
    ,436245754
    ,436245765
    ,436245766
    ,436245769
    ,436245770
    ,436245773
    ,436245774
    ,436245777
    ,436245778
    ,436245781
    ,436245782
    ,436245785
    ,436245786
    ,436245797
    ,436245798
    ,436245801
    ,436245802
    ,436245805
    ,436245806
    ,436245809
    ,436245810
    ,436245813
    ,436245814
    ,436245817
    ,436245818
    ,436245829
    ,436245830
    ,436245833
    ,436245834
    ,436245837
    ,436245838
    ,436245841
    ,436245842
    ,436245846
    ,436245849
    ,436245850
    ,436245861
    ,436245862
    ,436245865
    ,436245866
    ,436245869
    ,436245870
    ,436245873
    ,436245874
    ,436245877
    ,436245878
    ,436245881
    ,436245882
    ,436245893
    ,436245894
    ,436245897
    ,436245898
    ,436245901
    ,436245902
    ,436245905
    ,436245906
    ,436245909
    ,436245910
    ,436245913
    ,436245914
    ,436245925
    ,436245926
    ,436245929
    ,436245930
    ,436245933
    ,436245934
    ,436245937
    ,436245938
    ,436245941
    ,436245942
    ,436245945
    ,436245946
    ,436245957
    ,436245958
    ,436245961
    ,436245962
    ,436245965
    ,436245966
    ,436245969
    ,436245970
    ,436245973
    ,436245974
    ,436245977
    ,436245978
    ,436245989
    ,436245990
    ,436245993
    ,436245994
    ,436245997
    ,436245998
    ,436246001
    ,436246002
    ,436246005
    ,436246006
    ,436246009
    ,436246010
    ,436246021
    ,436246022
    ,436246025
    ,436246026
    ,436246029
    ,436246030
    ,436246033
    ,436246034
    ,436246037
    ,436246038
    ,436246041
    ,436246042
    ,436248613
    ,436248614
    ,436248617
    ,436248618
    ,436248621
    ,436248622
    ,436248625
    ,436248626
    ,436248629
    ,436248630
    ,436248633
    ,436248634
    ,436248645
    ,436248646
    ,436248649
    ,436248650
    ,436248653
    ,436248654
    ,436248657
    ,436248658
    ,436248661
    ,436248662
    ,436248665
    ,436248666
    ,436248677
    ,436248678
    ,436248681
    ,436248682
    ,436248685
    ,436248686
    ,436248689
    ,436248690
    ,436248693
    ,436248694
    ,436248697
    ,436248698
    ,436248709
    ,436248710
    ,436248713
    ,436248714
    ,436248717
    ,436248718
    ,436248721
    ,436248722
    ,436248725
    ,436248726
    ,436248729
    ,436248730
    ,436248741
    ,436248742
    ,436248745
    ,436248746
    ,436248749
    ,436248750
    ,436248753
    ,436248754
    ,436248757
    ,436248758
    ,436248761
    ,436248762
    ,436248773
    ,436248774
    ,436248777
    ,436248778
    ,436248781
    ,436248782
    ,436248785
    ,436248786
    ,436248789
    ,436248790
    ,436248793
    ,436248794
    ,436248805
    ,436248806
    ,436248809
    ,436248810
    ,436248813
    ,436248814
    ,436248817
    ,436248818
    ,436248821
    ,436248822
    ,436248825
    ,436248826
    ,436248837
    ,436248838
    ,436248841
    ,436248842
    ,436248845
    ,436248846
    ,436248849
    ,436248850
    ,436248853
    ,436248854
    ,436248857
    ,436248858
    ,436248869
    ,436248870
    ,436248873
    ,436248874
    ,436248877
    ,436248878
    ,436248881
    ,436248882
    ,436248885
    ,436248886
    ,436248889
    ,436248890
    ,436248901
    ,436248902
    ,436248905
    ,436248906
    ,436248909
    ,436248910
    ,436248913
    ,436248914
    ,436248917
    ,436248918
    ,436248921
    ,436248922
    ,436248933
    ,436248934
    ,436248937
    ,436248938
    ,436248941
    ,436248942
    ,436248945
    ,436248946
    ,436248949
    ,436248950
    ,436248953
    ,436248954
    ,436248965
    ,436248966
    ,436248969
    ,436248970
    ,436248973
    ,436248974
    ,436248977
    ,436248978
    ,436248981
    ,436248982
    ,436248985
    ,436248986
    ,436248997
    ,436248998
    ,436249001
    ,436249002
    ,436249005
    ,436249006
    ,436249009
    ,436249010
    ,436249013
    ,436249014
    ,436249017
    ,436249018
    ,436249029
    ,436249030
    ,436249033
    ,436249034
    ,436249037
    ,436249038
    ,436249041
    ,436249042
    ,436249045
    ,436249046
    ,436249049
    ,436249050
    ,436249061
    ,436249062
    ,436249065
    ,436249066
    ,436249069
    ,436249070
    ,436249073
    ,436249074
    ,436249077
    ,436249078
    ,436249081
    ,436249082
    ,436249093
    ,436249094
    ,436249097
    ,436249098
    ,436249101
    ,436249102
    ,436249105
    ,436249106
    ,436249109
    ,436249110
    ,436249113
    ,436249114
    ,436249125
    ,436249126
    ,436249129
    ,436249130
    ,436249133
    ,436249134
    ,436249137
    ,436249138
    ,436249141
    ,436249142
    ,436249145
    ,436249146
    ,436249157
    ,436249158
    ,436249161
    ,436249162
    ,436249165
    ,436249166
    ,436249169
    ,436249170
    ,436249173
    ,436249174
    ,436249177
    ,436249178
    ,436249189
    ,436249190
    ,436249193
    ,436249194
    ,436249197
    ,436249198
    ,436249201
    ,436249202
    ,436249205
    ,436249206
    ,436249209
    ,436249210
    ,436249221
    ,436249222
    ,436249225
    ,436249226
    ,436249229
    ,436249230
    ,436249233
    ,436249234
    ,436249237
    ,436249238
    ,436249241
    ,436249242
    ,436249253
    ,436249254
    ,436249257
    ,436249258
    ,436249261
    ,436249262
    ,436249265
    ,436249266
    ,436249269
    ,436249270
    ,436249273
    ,436249274
    ,436249285
    ,436249286
    ,436249289
    ,436249290
    ,436249293
    ,436249294
    ,436249297
    ,436249298
    ,436249301
    ,436249302
    ,436249305
    ,436249306
    ,436249317
    ,436249318
    ,436249321
    ,436249322
    ,436249325
    ,436249326
    ,436249329
    ,436249330
    ,436249333
    ,436249334
    ,436249337
    ,436249338
    ,436249350
    ,436249353
    ,436249354
    ,436249357
    ,436249358
    ,436249361
    ,436249362
    ,436249365
    ,436249366
    ,436249369
    ,436249370
    ,436249381
    ,436249382
    ,436249385
    ,436249386
    ,436249389
    ,436249390
    ,436249393
    ,436249394
    ,436249397
    ,436249398
    ,436249401
    ,436249402
    ,436249413
    ,436249414
    ,436249417
    ,436249418
    ,436249421
    ,436249422
    ,436249425
    ,436249426
    ,436249429
    ,436249430
    ,436249433
    ,436249434
    ,436249445
    ,436249446
    ,436249449
    ,436249450
    ,436249453
    ,436249454
    ,436249457
    ,436249458
    ,436249461
    ,436249462
    ,436249465
    ,436249466
    ,436249477
    ,436249478
    ,436249481
    ,436249482
    ,436249485
    ,436249486
    ,436249489
    ,436249490
    ,436249493
    ,436249494
    ,436249497
    ,436249498
    ,436249509
    ,436249510
    ,436249513
    ,436249514
    ,436249517
    ,436249518
    ,436249521
    ,436249522
    ,436249525
    ,436249526
    ,436249529
    ,436249530
    ,436249541
    ,436249542
    ,436249545
    ,436249546
    ,436249549
    ,436249550
    ,436249553
    ,436249554
    ,436249557
    ,436249561
    ,436249562
    ,436249573
    ,436249574
    ,436249577
    ,436249578
    ,436249581
    ,436249582
    ,436249585
    ,436249586
    ,436249589
    ,436249590
    ,436249593
    ,436249594
    ,436249605
    ,436249606
    ,436249609
    ,436249610
    ,436249613
    ,436249614
    ,436249617
    ,436249618
    ,436249621
    ,436249622
    ,436249625
    ,436249626
    ,436249637
    ,436249638
    ,436249641
    ,436249642
    ,436249645
    ,436249646
    ,436249649
    ,436249650
    ,436249653
    ,436249654
    ,436249657
    ,436249658
    ,436249669
    ,436249670
    ,436249673
    ,436249674
    ,436249677
    ,436249678
    ,436249681
    ,436249682
    ,436249685
    ,436249686
    ,436249689
    ,436249690
    ,436249701
    ,436249702
    ,436249705
    ,436249706
    ,436249709
    ,436249710
    ,436249713
    ,436249714
    ,436249717
    ,436249718
    ,436249721
    ,436249722
    ,436249733
    ,436249734
    ,436249737
    ,436249738
    ,436249741
    ,436249742
    ,436249745
    ,436249746
    ,436249749
    ,436249750
    ,436249753
    ,436249754
    ,436249765
    ,436249766
    ,436249769
    ,436249770
    ,436249773
    ,436249774
    ,436249777
    ,436249778
    ,436249781
    ,436249782
    ,436249785
    ,436249786
    ,436249797
    ,436249798
    ,436249801
    ,436249802
    ,436249805
    ,436249806
    ,436249809
    ,436249810
    ,436249813
    ,436249814
    ,436249817
    ,436249818
    ,436249829
    ,436249830
    ,436249833
    ,436249834
    ,436249837
    ,436249838
    ,436249841
    ,436249842
    ,436249845
    ,436249846
    ,436249849
    ,436249850
    ,436249861
    ,436249862
    ,436249865
    ,436249866
    ,436249869
    ,436249870
    ,436249873
    ,436249874
    ,436249877
    ,436249878
    ,436249881
    ,436249882
    ,436249893
    ,436249894
    ,436249897
    ,436249898
    ,436249901
    ,436249902
    ,436249905
    ,436249906
    ,436249909
    ,436249910
    ,436249913
    ,436249914
    ,436249925
    ,436249926
    ,436249929
    ,436249930
    ,436249933
    ,436249934
    ,436249937
    ,436249938
    ,436249941
    ,436249942
    ,436249945
    ,436249946
    ,436249957
    ,436249958
    ,436249961
    ,436249962
    ,436249965
    ,436249966
    ,436249969
    ,436249970
    ,436249973
    ,436249974
    ,436249977
    ,436249978
    ,436249989
    ,436249990
    ,436249993
    ,436249994
    ,436249997
    ,436249998
    ,436250001
    ,436250002
    ,436250005
    ,436250006
    ,436250009
    ,436250010
    ,436250021
    ,436250022
    ,436250025
    ,436250026
    ,436250029
    ,436250030
    ,436250033
    ,436250034
    ,436250037
    ,436250038
    ,436250041
    ,436250042
    ,436250053
    ,436250054
    ,436250057
    ,436250058
    ,436250061
    ,436250062
    ,436250065
    ,436250066
    ,436250069
    ,436250070
    ,436250073
    ,436250074
    ,436250085
    ,436250086
    ,436250089
    ,436250090
    ,436250093
    ,436250094
    ,436250097
    ,436250098
    ,436250101
    ,436250102
    ,436250105
    ,436250106
    ,436250117
    ,436250118
    ,436250121
    ,436250122
    ,436250125
    ,436250126
    ,436250129
    ,436250130
    ,436250133
    ,436250134
    ,436250137
    ,436250138
    ,436260900
    ,436260904
    ,436260908
    ,436260912
    ,436260916
    ,436260920
    ,436260932
    ,436260936
    ,436260940
    ,436260944
    ,436260948
    ,436260952
    ,436260964
    ,436260968
    ,436260972
    ,436260976
    ,436260980
    ,436260984
    ,436260996
    ,436261000
    ,436261004
    ,436261008
    ,436261012
    ,436261016
    ,436261028
    ,436261032
    ,436261036
    ,436261040
    ,436261044
    ,436261048
    ,436261060
    ,436261064
    ,436261068
    ,436261072
    ,436261076
    ,436261080
    ,436261092
    ,436261096
    ,436261100
    ,436261104
    ,436261108
    ,436261112
    ,436261124
    ,436261128
    ,436261132
    ,436261136
    ,436261140
    ,436261144
    ,436261156
    ,436261160
    ,436261164
    ,436261168
    ,436261172
    ,436261176
    ,436261188
    ,436261192
    ,436261196
    ,436261200
    ,436261204
    ,436261208
    ,436261220
    ,436261224
    ,436261228
    ,436261232
    ,436261236
    ,436261240
    ,436261252
    ,436261256
    ,436261260
    ,436261264
    ,436261268
    ,436261272
    ,436261284
    ,436261288
    ,436261292
    ,436261296
    ,436261300
    ,436261304
    ,436261316
    ,436261320
    ,436261324
    ,436261328
    ,436261332
    ,436261336
    ,436261348
    ,436261352
    ,436261356
    ,436261360
    ,436261364
    ,436261368
    ,436261380
    ,436261384
    ,436261388
    ,436261392
    ,436261396
    ,436261400
    ,436261412
    ,436261416
    ,436261420
    ,436261424
    ,436261428
    ,436261432
    ,436261444
    ,436261448
    ,436261452
    ,436261456
    ,436261460
    ,436261464
    ,436261476
    ,436261480
    ,436261484
    ,436261488
    ,436261492
    ,436261496
    ,436261508
    ,436261512
    ,436261516
    ,436261520
    ,436261524
    ,436261528
    ,436261540
    ,436261544
    ,436261548
    ,436261552
    ,436261556
    ,436261560
    ,436261572
    ,436261576
    ,436261580
    ,436261584
    ,436261588
    ,436261592
    ,436261604
    ,436261608
    ,436261612
    ,436261616
    ,436261620
    ,436261624
    ,436261636
    ,436261640
    ,436261644
    ,436261648
    ,436261652
    ,436261656
    ,436261668
    ,436261672
    ,436261676
    ,436261680
    ,436261684
    ,436261688
    ,436261700
    ,436261704
    ,436261708
    ,436261712
    ,436261716
    ,436261720
    ,436261732
    ,436261736
    ,436261740
    ,436261744
    ,436261748
    ,436261752
    ,436261764
    ,436261768
    ,436261772
    ,436261776
    ,436261780
    ,436261784
    ,436261796
    ,436261800
    ,436261804
    ,436261808
    ,436261812
    ,436261816
    ,436261828
    ,436261832
    ,436261836
    ,436261840
    ,436261844
    ,436261848
    ,436261860
    ,436261864
    ,436261868
    ,436261872
    ,436261876
    ,436261880
    ,436261892
    ,436261896
    ,436261900
    ,436261904
    ,436261908
    ,436261912
    ,436261924
    ,436261928
    ,436261932
    ,436261936
    ,436261940
    ,436261944
    ,436261956
    ,436261960
    ,436261964
    ,436261968
    ,436261972
    ,436261976
    ,436261988
    ,436261992
    ,436261996
    ,436262000
    ,436262004
    ,436262008
    ,436262020
    ,436262024
    ,436262028
    ,436262032
    ,436262036
    ,436262040
    ,436262052
    ,436262056
    ,436262060
    ,436262064
    ,436262068
    ,436262072
    ,436262084
    ,436262088
    ,436262092
    ,436262096
    ,436262100
    ,436262104
    ,436262116
    ,436262120
    ,436262124
    ,436262128
    ,436262132
    ,436262136
    ,436262148
    ,436262152
    ,436262156
    ,436262160
    ,436262164
    ,436262168
    ,436262180
    ,436262184
    ,436262188
    ,436262192
    ,436262196
    ,436262200
    ,436262212
    ,436262216
    ,436262220
    ,436262224
    ,436262228
    ,436262232
    ,436262244
    ,436262248
    ,436262252
    ,436262256
    ,436262260
    ,436262264
    ,436262276
    ,436262280
    ,436262284
    ,436262288
    ,436262292
    ,436262296
    ,436262308
    ,436262312
    ,436262316
    ,436262320
    ,436262324
    ,436262328
    ,436262340
    ,436262344
    ,436262348
    ,436262352
    ,436262356
    ,436262360
    ,436262372
    ,436262376
    ,436262380
    ,436262384
    ,436262388
    ,436262392
    ,436262404
    ,436262408
    ,436262412
    ,436262416
    ,436262420
    ,436262424
    ,436262436
    ,436262440
    ,436262444
    ,436262448
    ,436262452
    ,436262456
    ,436262468
    ,436262472
    ,436262476
    ,436262480
    ,436262484
    ,436262488
    ,436262500
    ,436262504
    ,436262508
    ,436262512
    ,436262516
    ,436262520
    ,436262532
    ,436262536
    ,436262540
    ,436262544
    ,436262548
    ,436262552
    ,436262564
    ,436262568
    ,436262572
    ,436262576
    ,436262580
    ,436262584
    ,436262596
    ,436262600
    ,436262604
    ,436262608
    ,436262612
    ,436262616
    ,436264996
    ,436265000
    ,436265004
    ,436265008
    ,436265012
    ,436265016
    ,436265028
    ,436265032
    ,436265036
    ,436265040
    ,436265044
    ,436265048
    ,436265060
    ,436265064
    ,436265068
    ,436265072
    ,436265076
    ,436265080
    ,436265092
    ,436265096
    ,436265100
    ,436265104
    ,436265108
    ,436265112
    ,436265124
    ,436265128
    ,436265132
    ,436265136
    ,436265140
    ,436265144
    ,436265156
    ,436265160
    ,436265164
    ,436265168
    ,436265172
    ,436265176
    ,436265188
    ,436265192
    ,436265196
    ,436265200
    ,436265204
    ,436265208
    ,436265220
    ,436265224
    ,436265228
    ,436265232
    ,436265236
    ,436265240
    ,436265252
    ,436265256
    ,436265260
    ,436265264
    ,436265268
    ,436265272
    ,436265284
    ,436265288
    ,436265292
    ,436265296
    ,436265300
    ,436265304
    ,436265316
    ,436265320
    ,436265324
    ,436265328
    ,436265332
    ,436265336
    ,436265348
    ,436265352
    ,436265356
    ,436265360
    ,436265364
    ,436265368
    ,436265380
    ,436265384
    ,436265388
    ,436265392
    ,436265396
    ,436265400
    ,436265412
    ,436265416
    ,436265420
    ,436265424
    ,436265428
    ,436265432
    ,436265444
    ,436265448
    ,436265452
    ,436265456
    ,436265460
    ,436265464
    ,436265476
    ,436265480
    ,436265484
    ,436265488
    ,436265492
    ,436265496
    ,436265508
    ,436265512
    ,436265516
    ,436265520
    ,436265524
    ,436265528
    ,436265540
    ,436265544
    ,436265548
    ,436265552
    ,436265556
    ,436265560
    ,436265572
    ,436265576
    ,436265580
    ,436265584
    ,436265588
    ,436265592
    ,436265604
    ,436265608
    ,436265612
    ,436265616
    ,436265620
    ,436265624
    ,436265636
    ,436265640
    ,436265644
    ,436265648
    ,436265652
    ,436265656
    ,436265668
    ,436265672
    ,436265676
    ,436265680
    ,436265684
    ,436265688
    ,436265700
    ,436265704
    ,436265708
    ,436265712
    ,436265716
    ,436265720
    ,436265732
    ,436265736
    ,436265740
    ,436265744
    ,436265748
    ,436265752
    ,436265764
    ,436265768
    ,436265772
    ,436265776
    ,436265780
    ,436265784
    ,436265796
    ,436265800
    ,436265804
    ,436265808
    ,436265812
    ,436265816
    ,436265828
    ,436265832
    ,436265836
    ,436265840
    ,436265844
    ,436265848
    ,436265860
    ,436265864
    ,436265868
    ,436265872
    ,436265876
    ,436265880
    ,436265892
    ,436265896
    ,436265900
    ,436265904
    ,436265908
    ,436265912
    ,436265924
    ,436265928
    ,436265932
    ,436265936
    ,436265940
    ,436265944
    ,436265956
    ,436265960
    ,436265964
    ,436265968
    ,436265972
    ,436265976
    ,436265988
    ,436265992
    ,436265996
    ,436266000
    ,436266004
    ,436266008
    ,436266020
    ,436266024
    ,436266028
    ,436266032
    ,436266036
    ,436266040
    ,436266052
    ,436266056
    ,436266060
    ,436266064
    ,436266068
    ,436266072
    ,436266084
    ,436266088
    ,436266092
    ,436266096
    ,436266100
    ,436266104
    ,436266116
    ,436266120
    ,436266124
    ,436266128
    ,436266132
    ,436266136
    ,436266148
    ,436266152
    ,436266156
    ,436266160
    ,436266164
    ,436266168
    ,436266180
    ,436266184
    ,436266188
    ,436266192
    ,436266196
    ,436266200
    ,436266212
    ,436266216
    ,436266220
    ,436266224
    ,436266228
    ,436266232
    ,436266244
    ,436266248
    ,436266252
    ,436266256
    ,436266260
    ,436266264
    ,436266276
    ,436266280
    ,436266284
    ,436266288
    ,436266292
    ,436266296
    ,436266308
    ,436266312
    ,436266316
    ,436266320
    ,436266324
    ,436266328
    ,436266340
    ,436266344
    ,436266348
    ,436266352
    ,436266356
    ,436266360
    ,436266372
    ,436266376
    ,436266380
    ,436266384
    ,436266388
    ,436266392
    ,436266404
    ,436266408
    ,436266412
    ,436266416
    ,436266420
    ,436266424
    ,436266436
    ,436266440
    ,436266444
    ,436266448
    ,436266452
    ,436266456
    ,436266468
    ,436266472
    ,436266476
    ,436266480
    ,436266484
    ,436266488
    ,436266500
    ,436266504
    ,436266508
    ,436266512
    ,436266516
    ,436266520
    ,436266532
    ,436266536
    ,436266540
    ,436266544
    ,436266548
    ,436266552
    ,436266564
    ,436266568
    ,436266572
    ,436266576
    ,436266580
    ,436266584
    ,436266596
    ,436266600
    ,436266604
    ,436266608
    ,436266612
    ,436266616
    ,436266628
    ,436266632
    ,436266636
    ,436266640
    ,436266644
    ,436266648
    ,436266660
    ,436266664
    ,436266668
    ,436266672
    ,436266676
    ,436266680
    ,436266692
    ,436266696
    ,436266700
    ,436266704
    ,436266708
    ,436266712
    ,436277284
    ,436277288
    ,436277292
    ,436277296
    ,436277300
    ,436277304
    ,436277316
    ,436277320
    ,436277324
    ,436277328
    ,436277332
    ,436277336
    ,436277348
    ,436277352
    ,436277356
    ,436277360
    ,436277364
    ,436277368
    ,436277380
    ,436277384
    ,436277388
    ,436277392
    ,436277396
    ,436277400
    ,436277412
    ,436277416
    ,436277420
    ,436277424
    ,436277428
    ,436277432
    ,436277444
    ,436277448
    ,436277452
    ,436277456
    ,436277460
    ,436277464
    ,436277476
    ,436277480
    ,436277484
    ,436277488
    ,436277492
    ,436277496
    ,436277508
    ,436277512
    ,436277516
    ,436277520
    ,436277524
    ,436277528
    ,436277540
    ,436277544
    ,436277548
    ,436277552
    ,436277556
    ,436277560
    ,436277572
    ,436277576
    ,436277580
    ,436277584
    ,436277588
    ,436277592
    ,436277604
    ,436277608
    ,436277612
    ,436277616
    ,436277620
    ,436277624
    ,436277636
    ,436277640
    ,436277644
    ,436277648
    ,436277652
    ,436277656
    ,436277668
    ,436277672
    ,436277676
    ,436277680
    ,436277684
    ,436277688
    ,436277700
    ,436277704
    ,436277708
    ,436277712
    ,436277716
    ,436277720
    ,436277732
    ,436277736
    ,436277740
    ,436277744
    ,436277748
    ,436277752
    ,436277764
    ,436277768
    ,436277772
    ,436277776
    ,436277780
    ,436277784
    ,436277796
    ,436277800
    ,436277804
    ,436277808
    ,436277812
    ,436277816
    ,436277828
    ,436277832
    ,436277836
    ,436277840
    ,436277844
    ,436277848
    ,436277860
    ,436277864
    ,436277868
    ,436277872
    ,436277876
    ,436277880
    ,436277892
    ,436277896
    ,436277900
    ,436277904
    ,436277908
    ,436277912
    ,436277924
    ,436277928
    ,436277932
    ,436277936
    ,436277940
    ,436277944
    ,436277956
    ,436277960
    ,436277964
    ,436277968
    ,436277972
    ,436277976
    ,436277988
    ,436277992
    ,436277996
    ,436278000
    ,436278004
    ,436278008
    ,436278020
    ,436278024
    ,436278028
    ,436278032
    ,436278036
    ,436278040
    ,436278052
    ,436278056
    ,436278060
    ,436278064
    ,436278068
    ,436278072
    ,436278084
    ,436278088
    ,436278092
    ,436278096
    ,436278100
    ,436278104
    ,436278116
    ,436278120
    ,436278124
    ,436278128
    ,436278132
    ,436278136
    ,436278148
    ,436278152
    ,436278156
    ,436278160
    ,436278164
    ,436278168
    ,436278180
    ,436278184
    ,436278188
    ,436278192
    ,436278196
    ,436278200
    ,436278212
    ,436278216
    ,436278220
    ,436278224
    ,436278228
    ,436278232
    ,436278244
    ,436278248
    ,436278252
    ,436278256
    ,436278260
    ,436278264
    ,436278276
    ,436278280
    ,436278284
    ,436278288
    ,436278292
    ,436278296
    ,436278308
    ,436278312
    ,436278316
    ,436278320
    ,436278324
    ,436278328
    ,436278340
    ,436278344
    ,436278348
    ,436278352
    ,436278356
    ,436278360
    ,436278372
    ,436278376
    ,436278380
    ,436278384
    ,436278388
    ,436278392
    ,436278404
    ,436278408
    ,436278412
    ,436278416
    ,436278420
    ,436278424
    ,436278436
    ,436278440
    ,436278444
    ,436278448
    ,436278452
    ,436278456
    ,436278468
    ,436278472
    ,436278476
    ,436278480
    ,436278484
    ,436278488
    ,436278500
    ,436278504
    ,436278508
    ,436278512
    ,436278516
    ,436278520
    ,436278532
    ,436278536
    ,436278540
    ,436278544
    ,436278548
    ,436278552
    ,436278564
    ,436278568
    ,436278572
    ,436278576
    ,436278580
    ,436278584
    ,436278596
    ,436278600
    ,436278604
    ,436278608
    ,436278612
    ,436278616
    ,436278628
    ,436278632
    ,436278636
    ,436278640
    ,436278644
    ,436278648
    ,436278660
    ,436278664
    ,436278668
    ,436278672
    ,436278676
    ,436278680
    ,436278692
    ,436278696
    ,436278700
    ,436278704
    ,436278708
    ,436278712
    ,436278724
    ,436278728
    ,436278732
    ,436278736
    ,436278740
    ,436278744
    ,436278756
    ,436278760
    ,436278764
    ,436278768
    ,436278772
    ,436278776
    ,436278788
    ,436278792
    ,436278796
    ,436278800
    ,436278804
    ,436278808
    ,436278820
    ,436278824
    ,436278828
    ,436278832
    ,436278836
    ,436278840
    ,436278852
    ,436278856
    ,436278860
    ,436278864
    ,436278868
    ,436278872
    ,436278884
    ,436278888
    ,436278892
    ,436278896
    ,436278900
    ,436278904
    ,436278916
    ,436278920
    ,436278924
    ,436278928
    ,436278932
    ,436278936
    ,436278948
    ,436278952
    ,436278956
    ,436278960
    ,436278964
    ,436278968
    ,436278980
    ,436278984
    ,436278988
    ,436278992
    ,436278996
    ,436279000
    ,436279012
    ,436279016
    ,436279020
    ,436279024
    ,436279028
    ,436279032
    ,436279044
    ,436279048
    ,436279052
    ,436279056
    ,436279060
    ,436279064
    ,436279076
    ,436279080
    ,436279084
    ,436279088
    ,436279092
    ,436279096
    ,436279108
    ,436279112
    ,436279116
    ,436279120
    ,436279124
    ,436279128
    ,436279140
    ,436279144
    ,436279148
    ,436279152
    ,436279156
    ,436279160
    ,436279172
    ,436279176
    ,436279180
    ,436279184
    ,436279188
    ,436279192
    ,436281380
    ,436281384
    ,436281388
    ,436281392
    ,436281396
    ,436281400
    ,436281412
    ,436281416
    ,436281420
    ,436281424
    ,436281428
    ,436281432
    ,436281444
    ,436281448
    ,436281452
    ,436281456
    ,436281460
    ,436281464
    ,436281476
    ,436281480
    ,436281484
    ,436281488
    ,436281492
    ,436281496
    ,436281508
    ,436281512
    ,436281516
    ,436281520
    ,436281524
    ,436281528
    ,436281540
    ,436281544
    ,436281548
    ,436281552
    ,436281556
    ,436281560
    ,436281572
    ,436281576
    ,436281580
    ,436281584
    ,436281588
    ,436281592
    ,436281604
    ,436281608
    ,436281612
    ,436281616
    ,436281620
    ,436281624
    ,436281636
    ,436281640
    ,436281644
    ,436281648
    ,436281652
    ,436281656
    ,436281668
    ,436281672
    ,436281676
    ,436281680
    ,436281684
    ,436281688
    ,436281700
    ,436281704
    ,436281708
    ,436281712
    ,436281716
    ,436281720
    ,436281732
    ,436281736
    ,436281740
    ,436281744
    ,436281748
    ,436281752
    ,436281764
    ,436281768
    ,436281772
    ,436281776
    ,436281780
    ,436281784
    ,436282052
    ,436282056
    ,436282060
    ,436282064
    ,436282068
    ,436282072
    ,436282084
    ,436282088
    ,436282092
    ,436282096
    ,436282100
    ,436282104
    ,436282116
    ,436282120
    ,436282124
    ,436282128
    ,436282132
    ,436282136
    ,436282148
    ,436282152
    ,436282156
    ,436282160
    ,436282164
    ,436282168
    ,436282180
    ,436282184
    ,436282188
    ,436282192
    ,436282196
    ,436282200
    ,436282212
    ,436282216
    ,436282220
    ,436282224
    ,436282228
    ,436282232
    ,436282244
    ,436282248
    ,436282252
    ,436282256
    ,436282260
    ,436282264
    ,436282276
    ,436282280
    ,436282284
    ,436282288
    ,436282292
    ,436282296
    ,436282308
    ,436282312
    ,436282316
    ,436282320
    ,436282324
    ,436282328
    ,436282340
    ,436282344
    ,436282348
    ,436282352
    ,436282356
    ,436282360
    ,436282372
    ,436282376
    ,436282380
    ,436282384
    ,436282388
    ,436282392
    ,436282404
    ,436282408
    ,436282412
    ,436282416
    ,436282420
    ,436282424
    ,436282436
    ,436282440
    ,436282444
    ,436282448
    ,436282452
    ,436282456
    ,436282468
    ,436282472
    ,436282476
    ,436282480
    ,436282484
    ,436282488
    ,436282500
    ,436282504
    ,436282508
    ,436282512
    ,436282516
    ,436282520
    ,436282532
    ,436282536
    ,436282540
    ,436282544
    ,436282548
    ,436282552
    ,436282564
    ,436282568
    ,436282572
    ,436282576
    ,436282580
    ,436282584
    ,436282596
    ,436282600
    ,436282604
    ,436282608
    ,436282612
    ,436282616
    ,436282628
    ,436282632
    ,436282636
    ,436282640
    ,436282644
    ,436282648
    ,436282660
    ,436282664
    ,436282668
    ,436282672
    ,436282676
    ,436282680
    ,436282692
    ,436282696
    ,436282700
    ,436282704
    ,436282708
    ,436282712
    ,436282724
    ,436282728
    ,436282732
    ,436282736
    ,436282740
    ,436282744
    ,436282756
    ,436282760
    ,436282764
    ,436282768
    ,436282772
    ,436282776
    ,436282788
    ,436282792
    ,436282796
    ,436282800
    ,436282804
    ,436282808
    ,436282820
    ,436282824
    ,436282828
    ,436282832
    ,436282836
    ,436282840
    ,436282852
    ,436282856
    ,436282860
    ,436282864
    ,436282868
    ,436282872
    ,436282884
    ,436282888
    ,436282892
    ,436282896
    ,436282900
    ,436282904
    ,436282916
    ,436282920
    ,436282924
    ,436282928
    ,436282932
    ,436282936
    ,436282948
    ,436282952
    ,436282956
    ,436282960
    ,436282964
    ,436282968
    ,436282980
    ,436282984
    ,436282988
    ,436282992
    ,436282996
    ,436283000
    ,436283012
    ,436283016
    ,436283020
    ,436283024
    ,436283028
    ,436283032
    ,436283044
    ,436283048
    ,436283052
    ,436283056
    ,436283060
    ,436283064
    ,436283076
    ,436283080
    ,436283084
    ,436283088
    ,436283092
    ,436283096
    ,436283108
    ,436283112
    ,436283116
    ,436283120
    ,436283124
    ,436283128
    ,436283140
    ,436283144
    ,436283148
    ,436283152
    ,436283156
    ,436283160
    ,436283172
    ,436283176
    ,436283180
    ,436283184
    ,436283188
    ,436283192
    ,436283204
    ,436283208
    ,436283212
    ,436283216
    ,436283220
    ,436283224
    ,436283236
    ,436283240
    ,436283244
    ,436283248
    ,436283252
    ,436283256
    ,436283268
    ,436283272
    ,436283276
    ,436283280
    ,436283284
    ,436283288
    ,436293668
    ,436293672
    ,436293676
    ,436293680
    ,436293684
    ,436293688
    ,436293700
    ,436293704
    ,436293708
    ,436293712
    ,436293716
    ,436293720
    ,436293732
    ,436293736
    ,436293740
    ,436293744
    ,436293748
    ,436293752
    ,436293764
    ,436293768
    ,436293772
    ,436293776
    ,436293780
    ,436293784
    ,436293796
    ,436293800
    ,436293804
    ,436293808
    ,436293812
    ,436293816
    ,436293828
    ,436293832
    ,436293836
    ,436293840
    ,436293844
    ,436293848
    ,436293860
    ,436293864
    ,436293868
    ,436293872
    ,436293876
    ,436293880
    ,436293892
    ,436293896
    ,436293900
    ,436293904
    ,436293908
    ,436293912
    ,436293924
    ,436293928
    ,436293932
    ,436293936
    ,436293940
    ,436293944
    ,436293956
    ,436293960
    ,436293964
    ,436293968
    ,436293972
    ,436293976
    ,436293988
    ,436293992
    ,436293996
    ,436294000
    ,436294004
    ,436294008
    ,436294020
    ,436294024
    ,436294028
    ,436294032
    ,436294036
    ,436294040
    ,436294052
    ,436294056
    ,436294060
    ,436294064
    ,436294068
    ,436294072
    ,436294084
    ,436294088
    ,436294092
    ,436294096
    ,436294100
    ,436294104
    ,436294116
    ,436294120
    ,436294124
    ,436294128
    ,436294132
    ,436294136
    ,436294148
    ,436294152
    ,436294156
    ,436294160
    ,436294164
    ,436294168
    ,436294180
    ,436294184
    ,436294188
    ,436294192
    ,436294196
    ,436294200
    ,436294212
    ,436294216
    ,436294220
    ,436294224
    ,436294228
    ,436294232
    ,436294244
    ,436294248
    ,436294252
    ,436294256
    ,436294260
    ,436294264
    ,436294276
    ,436294280
    ,436294284
    ,436294288
    ,436294292
    ,436294296
    ,436294308
    ,436294312
    ,436294316
    ,436294320
    ,436294324
    ,436294328
    ,436294340
    ,436294344
    ,436294348
    ,436294352
    ,436294356
    ,436294360
    ,436294372
    ,436294376
    ,436294380
    ,436294384
    ,436294388
    ,436294392
    ,436294404
    ,436294408
    ,436294412
    ,436294416
    ,436294420
    ,436294424
    ,436294436
    ,436294440
    ,436294444
    ,436294448
    ,436294452
    ,436294456
    ,436294468
    ,436294472
    ,436294476
    ,436294480
    ,436294484
    ,436294488
    ,436294500
    ,436294504
    ,436294508
    ,436294512
    ,436294516
    ,436294520
    ,436294532
    ,436294536
    ,436294540
    ,436294544
    ,436294548
    ,436294552
    ,436294564
    ,436294568
    ,436294572
    ,436294576
    ,436294580
    ,436294584
    ,436294596
    ,436294600
    ,436294604
    ,436294608
    ,436294612
    ,436294616
    ,436294628
    ,436294632
    ,436294636
    ,436294640
    ,436294644
    ,436294648
    ,436294660
    ,436294664
    ,436294668
    ,436294672
    ,436294676
    ,436294680
    ,436294692
    ,436294696
    ,436294700
    ,436294704
    ,436294708
    ,436294712
    ,436294724
    ,436294728
    ,436294732
    ,436294736
    ,436294740
    ,436294744
    ,436294756
    ,436294760
    ,436294764
    ,436294768
    ,436294772
    ,436294776
    ,436294788
    ,436294792
    ,436294796
    ,436294800
    ,436294804
    ,436294808
    ,436294820
    ,436294824
    ,436294828
    ,436294832
    ,436294836
    ,436294840
    ,436294852
    ,436294856
    ,436294860
    ,436294864
    ,436294868
    ,436294872
    ,436294884
    ,436294888
    ,436294892
    ,436294896
    ,436294900
    ,436294904
    ,436294916
    ,436294920
    ,436294924
    ,436294928
    ,436294932
    ,436294936
    ,436294948
    ,436294952
    ,436294956
    ,436294960
    ,436294964
    ,436294968
    ,436294980
    ,436294984
    ,436294988
    ,436294992
    ,436294996
    ,436295000
    ,436295016
    ,436295020
    ,436295024
    ,436295028
    ,436295032
    ,436295076
    ,436295080
    ,436295084
    ,436295088
    ,436295092
    ,436295096
    ,436295108
    ,436295112
    ,436295116
    ,436295120
    ,436295124
    ,436295128
    ,436295140
    ,436295144
    ,436295148
    ,436295152
    ,436295156
    ,436295160
    ,436295172
    ,436295176
    ,436295180
    ,436295184
    ,436295188
    ,436295192
    ,436295204
    ,436295208
    ,436295212
    ,436295216
    ,436295220
    ,436295224
    ,436295236
    ,436295240
    ,436295244
    ,436295248
    ,436295252
    ,436295256
    ,436295268
    ,436295272
    ,436295276
    ,436295280
    ,436295284
    ,436295288
    ,436295300
    ,436295304
    ,436295308
    ,436295312
    ,436295316
    ,436295320
    ,436295332
    ,436295336
    ,436295340
    ,436295344
    ,436295348
    ,436295352
    ,436295364
    ,436295368
    ,436295372
    ,436295376
    ,436295380
    ,436295384
    ,436295396
    ,436295400
    ,436295404
    ,436295408
    ,436295412
    ,436295416
    ,436295428
    ,436295432
    ,436295436
    ,436295440
    ,436295444
    ,436295448
    ,436295460
    ,436295464
    ,436295468
    ,436295472
    ,436295476
    ,436295480
    ,436295492
    ,436295496
    ,436295500
    ,436295504
    ,436295508
    ,436295512
    ,436295524
    ,436295532
    ,436295540
    ,436295556
    ,436295560
    ,436295564
    ,436295568
    ,436295572
    ,436295576
    ,436295588
    ,436295592
    ,436295596
    ,436295600
    ,436295604
    ,436295608
    ,436295620
    ,436295624
    ,436295628
    ,436295632
    ,436295636
    ,436295640
    ,436295652
    ,436295656
    ,436295660
    ,436295664
    ,436295668
    ,436295672
    ,436295684
    ,436295688
    ,436295692
    ,436295696
    ,436295700
    ,436295704
    ,436295716
    ,436295720
    ,436295724
    ,436295728
    ,436295732
    ,436295736
    ,436295748
    ,436295752
    ,436295756
    ,436295760
    ,436295764
    ,436295768
    ,436297764
    ,436297768
    ,436297772
    ,436297776
    ,436297780
    ,436297784
    ,436297796
    ,436297800
    ,436297804
    ,436297808
    ,436297812
    ,436297816
    ,436297828
    ,436297832
    ,436297836
    ,436297840
    ,436297844
    ,436297848
    ,436297860
    ,436297864
    ,436297868
    ,436297872
    ,436297876
    ,436297880
    ,436297892
    ,436297896
    ,436297900
    ,436297904
    ,436297908
    ,436297912
    ,436297924
    ,436297928
    ,436297932
    ,436297936
    ,436297940
    ,436297944
    ,436297956
    ,436297960
    ,436297964
    ,436297968
    ,436297972
    ,436297976
    ,436297988
    ,436297992
    ,436297996
    ,436298000
    ,436298004
    ,436298008
    ,436298020
    ,436298024
    ,436298028
    ,436298032
    ,436298036
    ,436298040
    ,436298052
    ,436298056
    ,436298060
    ,436298064
    ,436298068
    ,436298072
    ,436298084
    ,436298088
    ,436298092
    ,436298096
    ,436298100
    ,436298104
    ,436298116
    ,436298120
    ,436298124
    ,436298128
    ,436298132
    ,436298136
    ,436298148
    ,436298152
    ,436298156
    ,436298160
    ,436298164
    ,436298168
    ,436298180
    ,436298184
    ,436298188
    ,436298192
    ,436298196
    ,436298200
    ,436298212
    ,436298216
    ,436298220
    ,436298224
    ,436298228
    ,436298232
    ,436298244
    ,436298248
    ,436298252
    ,436298256
    ,436298260
    ,436298264
    ,436298276
    ,436298280
    ,436298284
    ,436298288
    ,436298292
    ,436298296
    ,436298308
    ,436298312
    ,436298316
    ,436298320
    ,436298324
    ,436298328
    ,436298340
    ,436298344
    ,436298348
    ,436298352
    ,436298356
    ,436298360
    ,436298372
    ,436298376
    ,436298380
    ,436298384
    ,436298388
    ,436298392
    ,436298404
    ,436298408
    ,436298412
    ,436298416
    ,436298420
    ,436298424
    ,436298436
    ,436298444
    ,436298448
    ,436298452
    ,436298456
    ,436298468
    ,436298472
    ,436298476
    ,436298480
    ,436298484
    ,436298488
    ,436298500
    ,436298504
    ,436298508
    ,436298512
    ,436298516
    ,436298520
    ,436298532
    ,436298536
    ,436298540
    ,436298544
    ,436298548
    ,436298552
    ,436298564
    ,436298568
    ,436298572
    ,436298576
    ,436298580
    ,436298584
    ,436298596
    ,436298600
    ,436298604
    ,436298608
    ,436298612
    ,436298616
    ,436298628
    ,436298632
    ,436298636
    ,436298640
    ,436298644
    ,436298648
    ,436298660
    ,436298664
    ,436298668
    ,436298672
    ,436298676
    ,436298680
    ,436298692
    ,436298696
    ,436298700
    ,436298704
    ,436298708
    ,436298712
    ,436298724
    ,436298728
    ,436298732
    ,436298736
    ,436298740
    ,436298744
    ,436298756
    ,436298760
    ,436298764
    ,436298768
    ,436298772
    ,436298776
    ,436298788
    ,436298792
    ,436298800
    ,436298804
    ,436298808
    ,436298820
    ,436298824
    ,436298828
    ,436298832
    ,436298836
    ,436298840
    ,436298852
    ,436298856
    ,436298860
    ,436298864
    ,436298868
    ,436298872
    ,436298884
    ,436298888
    ,436298892
    ,436298896
    ,436298900
    ,436298904
    ,436298916
    ,436298920
    ,436298924
    ,436298928
    ,436298932
    ,436298936
    ,436298948
    ,436298952
    ,436298956
    ,436298960
    ,436298964
    ,436298968
    ,436298980
    ,436298984
    ,436298988
    ,436298992
    ,436298996
    ,436299000
    ,436299012
    ,436299016
    ,436299020
    ,436299024
    ,436299028
    ,436299032
    ,436299044
    ,436299048
    ,436299052
    ,436299056
    ,436299060
    ,436299064
    ,436299076
    ,436299080
    ,436299084
    ,436299088
    ,436299092
    ,436299096
    ,436299108
    ,436299112
    ,436299116
    ,436299120
    ,436299124
    ,436299128
    ,436299140
    ,436299144
    ,436299148
    ,436299152
    ,436299156
    ,436299160
    ,436299172
    ,436299176
    ,436299180
    ,436299184
    ,436299188
    ,436299192
    ,436299204
    ,436299208
    ,436299212
    ,436299216
    ,436299220
    ,436299224
    ,436299236
    ,436299240
    ,436299244
    ,436299248
    ,436299252
    ,436299256
    ,436299268
    ,436299272
    ,436299276
    ,436299280
    ,436299284
    ,436299288
    ,436299300
    ,436299304
    ,436299308
    ,436299312
    ,436299316
    ,436299320
    ,436299332
    ,436299336
    ,436299340
    ,436299344
    ,436299348
    ,436299352
    ,436299364
    ,436299368
    ,436299372
    ,436299376
    ,436299380
    ,436299384
    ,436299396
    ,436299400
    ,436299404
    ,436299408
    ,436299412
    ,436299416
    ,436299428
    ,436299432
    ,436299436
    ,436299440
    ,436299444
    ,436299448
    ,436299460
    ,436299464
    ,436299468
    ,436299472
    ,436299476
    ,436299480
    ,436299492
    ,436299496
    ,436299500
    ,436299504
    ,436299508
    ,436299512
    ,436299524
    ,436299528
    ,436299532
    ,436299536
    ,436299540
    ,436299544
    ,436299556
    ,436299560
    ,436299564
    ,436299568
    ,436299572
    ,436299576
    ,436299588
    ,436299592
    ,436299596
    ,436299600
    ,436299604
    ,436299608
    ,436299620
    ,436299624
    ,436299628
    ,436299632
    ,436299636
    ,436299640
    ,436299652
    ,436299656
    ,436299660
    ,436299664
    ,436299668
    ,436299672
    ,436299684
    ,436299688
    ,436299692
    ,436299696
    ,436299700
    ,436299704
    ,436299716
    ,436299720
    ,436299724
    ,436299728
    ,436299732
    ,436299736
    ,436299748
    ,436299752
    ,436299756
    ,436299760
    ,436299764
    ,436299768
    ,436299780
    ,436299784
    ,436299788
    ,436299792
    ,436299796
    ,436299800
    ,436299812
    ,436299816
    ,436299820
    ,436299824
    ,436299828
    ,436299832
    ,436299844
    ,436299848
    ,436299852
    ,436299856
    ,436299860
    ,436299864
    ,436310052
    ,436310056
    ,436310060
    ,436310064
    ,436310068
    ,436310072
    ,436310084
    ,436310088
    ,436310092
    ,436310096
    ,436310100
    ,436310104
    ,436310116
    ,436310120
    ,436310124
    ,436310128
    ,436310132
    ,436310136
    ,436310148
    ,436310152
    ,436310156
    ,436310160
    ,436310164
    ,436310168
    ,436310180
    ,436310184
    ,436310188
    ,436310192
    ,436310196
    ,436310200
    ,436310212
    ,436310216
    ,436310220
    ,436310224
    ,436310228
    ,436310232
    ,436310244
    ,436310248
    ,436310252
    ,436310256
    ,436310260
    ,436310264
    ,436310276
    ,436310280
    ,436310284
    ,436310288
    ,436310292
    ,436310296
    ,436310308
    ,436310312
    ,436310316
    ,436310320
    ,436310324
    ,436310328
    ,436310340
    ,436310344
    ,436310348
    ,436310352
    ,436310356
    ,436310360
    ,436310372
    ,436310376
    ,436310380
    ,436310384
    ,436310388
    ,436310392
    ,436310404
    ,436310408
    ,436310412
    ,436310416
    ,436310420
    ,436310424
    ,436310436
    ,436310440
    ,436310444
    ,436310448
    ,436310452
    ,436310456
    ,436310468
    ,436310472
    ,436310476
    ,436310480
    ,436310484
    ,436310488
    ,436310500
    ,436310504
    ,436310508
    ,436310512
    ,436310516
    ,436310520
    ,436310532
    ,436310536
    ,436310540
    ,436310544
    ,436310548
    ,436310552
    ,436310564
    ,436310568
    ,436310572
    ,436310576
    ,436310580
    ,436310584
    ,436310596
    ,436310600
    ,436310604
    ,436310608
    ,436310612
    ,436310616
    ,436310628
    ,436310632
    ,436310636
    ,436310640
    ,436310644
    ,436310648
    ,436310660
    ,436310664
    ,436310668
    ,436310672
    ,436310676
    ,436310680
    ,436310692
    ,436310696
    ,436310700
    ,436310704
    ,436310708
    ,436310712
    ,436310724
    ,436310728
    ,436310732
    ,436310736
    ,436310740
    ,436310744
    ,436310756
    ,436310760
    ,436310764
    ,436310768
    ,436310772
    ,436310776
    ,436310788
    ,436310792
    ,436310796
    ,436310800
    ,436310804
    ,436310808
    ,436310820
    ,436310824
    ,436310828
    ,436310832
    ,436310836
    ,436310840
    ,436310852
    ,436310856
    ,436310860
    ,436310864
    ,436310868
    ,436310872
    ,436310884
    ,436310888
    ,436310892
    ,436310896
    ,436310900
    ,436310904
    ,436310916
    ,436310920
    ,436310924
    ,436310928
    ,436310932
    ,436310936
    ,436310952
    ,436310960
    ,436310968
    ,436310980
    ,436310984
    ,436310988
    ,436310992
    ,436310996
    ,436311000
    ,436311012
    ,436311016
    ,436311020
    ,436311024
    ,436311028
    ,436311032
    ,436311044
    ,436311048
    ,436311052
    ,436311056
    ,436311060
    ,436311064
    ,436311076
    ,436311080
    ,436311084
    ,436311088
    ,436311092
    ,436311096
    ,436311108
    ,436311112
    ,436311116
    ,436311120
    ,436311124
    ,436311128
    ,436311140
    ,436311144
    ,436311148
    ,436311152
    ,436311156
    ,436311160
    ,436311172
    ,436311176
    ,436311180
    ,436311184
    ,436311188
    ,436311192
    ,436311204
    ,436311208
    ,436311212
    ,436311216
    ,436311220
    ,436311224
    ,436311236
    ,436311240
    ,436311244
    ,436311248
    ,436311252
    ,436311256
    ,436311268
    ,436311272
    ,436311276
    ,436311280
    ,436311284
    ,436311288
    ,436311300
    ,436311304
    ,436311308
    ,436311312
    ,436311316
    ,436311320
    ,436311332
    ,436311336
    ,436311340
    ,436311344
    ,436311348
    ,436311352
    ,436311364
    ,436311368
    ,436311372
    ,436311376
    ,436311380
    ,436311384
    ,436311396
    ,436311400
    ,436311404
    ,436311408
    ,436311412
    ,436311416
    ,436311428
    ,436311432
    ,436311436
    ,436311440
    ,436311444
    ,436311448
    ,436311460
    ,436311464
    ,436311468
    ,436311472
    ,436311476
    ,436311480
    ,436311492
    ,436311496
    ,436311500
    ,436311504
    ,436311508
    ,436311512
    ,436311524
    ,436311528
    ,436311532
    ,436311536
    ,436311540
    ,436311544
    ,436311556
    ,436311560
    ,436311564
    ,436311568
    ,436311572
    ,436311576
    ,436311588
    ,436311592
    ,436311596
    ,436311600
    ,436311604
    ,436311608
    ,436311620
    ,436311628
    ,436311652
    ,436311656
    ,436311660
    ,436311664
    ,436311668
    ,436311672
    ,436311684
    ,436311688
    ,436311692
    ,436311696
    ,436311700
    ,436311704
    ,436311716
    ,436311720
    ,436311724
    ,436311728
    ,436311732
    ,436311736
    ,436311748
    ,436311752
    ,436311756
    ,436311760
    ,436311764
    ,436311768
    ,436311780
    ,436311784
    ,436311788
    ,436311792
    ,436311796
    ,436311800
    ,436311812
    ,436311816
    ,436311820
    ,436311824
    ,436311828
    ,436311832
    ,436311844
    ,436311848
    ,436311852
    ,436311856
    ,436311860
    ,436311864
    ,436311876
    ,436311880
    ,436311884
    ,436311888
    ,436311892
    ,436311896
    ,436311908
    ,436311912
    ,436311916
    ,436311920
    ,436311924
    ,436311928
    ,436311940
    ,436311944
    ,436311948
    ,436311952
    ,436311956
    ,436311960
    ,436311972
    ,436311976
    ,436311980
    ,436311984
    ,436311988
    ,436311992
    ,436312004
    ,436312008
    ,436312012
    ,436312016
    ,436312020
    ,436312024
    ,436312036
    ,436312040
    ,436312044
    ,436312048
    ,436312052
    ,436312056
    ,436312068
    ,436312072
    ,436312076
    ,436312080
    ,436312084
    ,436312088
    ,436312100
    ,436312104
    ,436312108
    ,436312112
    ,436312116
    ,436312120
    ,436312132
    ,436312136
    ,436312140
    ,436312144
    ,436312148
    ,436312152
    ,436312164
    ,436312168
    ,436312172
    ,436312176
    ,436312180
    ,436312184
    ,436312196
    ,436312200
    ,436312204
    ,436312208
    ,436312212
    ,436312216
    ,436312228
    ,436312232
    ,436312236
    ,436312240
    ,436312244
    ,436312248
    ,436312260
    ,436312264
    ,436312268
    ,436312272
    ,436312276
    ,436312280
    ,436312292
    ,436312296
    ,436312300
    ,436312304
    ,436312308
    ,436312312
    ,436312324
    ,436312328
    ,436312332
    ,436312336
    ,436312340
    ,436312344
    ,436312356
    ,436312360
    ,436312364
    ,436312368
    ,436312372
    ,436312376
    ,436312388
    ,436312392
    ,436312396
    ,436312400
    ,436312404
    ,436312408
    ,436314148
    ,436314152
    ,436314156
    ,436314160
    ,436314164
    ,436314168
    ,436314180
    ,436314184
    ,436314188
    ,436314192
    ,436314196
    ,436314200
    ,436314212
    ,436314216
    ,436314220
    ,436314224
    ,436314228
    ,436314232
    ,436314244
    ,436314248
    ,436314252
    ,436314256
    ,436314260
    ,436314264
    ,436314276
    ,436314280
    ,436314284
    ,436314288
    ,436314292
    ,436314296
    ,436314308
    ,436314312
    ,436314316
    ,436314320
    ,436314324
    ,436314328
    ,436314340
    ,436314344
    ,436314348
    ,436314352
    ,436314356
    ,436314360
    ,436314372
    ,436314376
    ,436314380
    ,436314384
    ,436314388
    ,436314392
    ,436314404
    ,436314408
    ,436314412
    ,436314416
    ,436314420
    ,436314424
    ,436314436
    ,436314440
    ,436314444
    ,436314448
    ,436314452
    ,436314456
    ,436314468
    ,436314472
    ,436314476
    ,436314480
    ,436314484
    ,436314488
    ,436314500
    ,436314504
    ,436314508
    ,436314512
    ,436314516
    ,436314520
    ,436314532
    ,436314536
    ,436314540
    ,436314544
    ,436314548
    ,436314552
    ,436314564
    ,436314568
    ,436314572
    ,436314576
    ,436314580
    ,436314584
    ,436314596
    ,436314600
    ,436314604
    ,436314608
    ,436314612
    ,436314616
    ,436314628
    ,436314632
    ,436314636
    ,436314640
    ,436314644
    ,436314648
    ,436314660
    ,436314664
    ,436314668
    ,436314672
    ,436314676
    ,436314680
    ,436314692
    ,436314696
    ,436314700
    ,436314704
    ,436314708
    ,436314712
    ,436314724
    ,436314728
    ,436314732
    ,436314736
    ,436314740
    ,436314744
    ,436314756
    ,436314760
    ,436314764
    ,436314768
    ,436314772
    ,436314776
    ,436314788
    ,436314792
    ,436314796
    ,436314800
    ,436314804
    ,436314808
    ,436314820
    ,436314824
    ,436314828
    ,436314832
    ,436314836
    ,436314840
    ,436314852
    ,436314856
    ,436314860
    ,436314864
    ,436314868
    ,436314872
    ,436314884
    ,436314888
    ,436314892
    ,436314896
    ,436314900
    ,436314904
    ,436314916
    ,436314920
    ,436314924
    ,436314928
    ,436314932
    ,436314936
    ,436314948
    ,436314952
    ,436314956
    ,436314960
    ,436314964
    ,436314968
    ,436314980
    ,436314984
    ,436314988
    ,436314992
    ,436314996
    ,436315000
    ,436315012
    ,436315016
    ,436315020
    ,436315024
    ,436315028
    ,436315032
    ,436315044
    ,436315048
    ,436315052
    ,436315056
    ,436315060
    ,436315064
    ,436315076
    ,436315080
    ,436315084
    ,436315088
    ,436315092
    ,436315096
    ,436315108
    ,436315112
    ,436315116
    ,436315120
    ,436315124
    ,436315128
    ,436315140
    ,436315144
    ,436315148
    ,436315152
    ,436315156
    ,436315160
    ,436315172
    ,436315176
    ,436315180
    ,436315184
    ,436315188
    ,436315192
    ,436315204
    ,436315208
    ,436315212
    ,436315216
    ,436315220
    ,436315224
    ,436315236
    ,436315240
    ,436315244
    ,436315248
    ,436315252
    ,436315256
    ,436315268
    ,436315272
    ,436315276
    ,436315280
    ,436315284
    ,436315288
    ,436315300
    ,436315304
    ,436315308
    ,436315312
    ,436315316
    ,436315320
    ,436315332
    ,436315336
    ,436315340
    ,436315344
    ,436315348
    ,436315352
    ,436315364
    ,436315368
    ,436315372
    ,436315376
    ,436315380
    ,436315384
    ,436315396
    ,436315400
    ,436315404
    ,436315408
    ,436315412
    ,436315416
    ,436315428
    ,436315432
    ,436315436
    ,436315440
    ,436315444
    ,436315448
    ,436315460
    ,436315464
    ,436315468
    ,436315472
    ,436315476
    ,436315480
    ,436315492
    ,436315496
    ,436315500
    ,436315504
    ,436315508
    ,436315512
    ,436315524
    ,436315528
    ,436315532
    ,436315536
    ,436315540
    ,436315544
    ,436315556
    ,436315560
    ,436315564
    ,436315568
    ,436315572
    ,436315576
    ,436315588
    ,436315592
    ,436315596
    ,436315600
    ,436315604
    ,436315608
    ,436315620
    ,436315624
    ,436315628
    ,436315632
    ,436315636
    ,436315640
    ,436315652
    ,436315656
    ,436315660
    ,436315664
    ,436315668
    ,436315672
    ,436315684
    ,436315688
    ,436315692
    ,436315696
    ,436315700
    ,436315704
    ,436315716
    ,436315720
    ,436315724
    ,436315728
    ,436315732
    ,436315736
    ,436315748
    ,436315752
    ,436315756
    ,436315760
    ,436315764
    ,436315768
    ,436315780
    ,436315784
    ,436315788
    ,436315792
    ,436315796
    ,436315800
    ,436315812
    ,436315816
    ,436315820
    ,436315824
    ,436315828
    ,436315832
    ,436315844
    ,436315848
    ,436315852
    ,436315856
    ,436315860
    ,436315864
    ,436315876
    ,436315880
    ,436315884
    ,436315888
    ,436315892
    ,436315896
    ,436315908
    ,436315912
    ,436315916
    ,436315920
    ,436315924
    ,436315928
    ,436315940
    ,436315944
    ,436315948
    ,436315952
    ,436315956
    ,436315960
    ,436315972
    ,436315976
    ,436315980
    ,436315984
    ,436315988
    ,436315992
    ,436316004
    ,436316008
    ,436316012
    ,436316016
    ,436316020
    ,436316024
    ,436316036
    ,436316040
    ,436316044
    ,436316048
    ,436316052
    ,436316056
    ,436316068
    ,436316072
    ,436316076
    ,436316080
    ,436316084
    ,436316088
    ,436316100
    ,436316104
    ,436316108
    ,436316112
    ,436316116
    ,436316120
    ,436316132
    ,436316136
    ,436316140
    ,436316144
    ,436316148
    ,436316152
    ,436316164
    ,436316168
    ,436316172
    ,436316176
    ,436316180
    ,436316184
    ,436316196
    ,436316200
    ,436316204
    ,436316208
    ,436316212
    ,436316216
    ,436316228
    ,436316232
    ,436316236
    ,436316240
    ,436316244
    ,436316248
    ,436316260
    ,436316264
    ,436316268
    ,436316272
    ,436316276
    ,436316280
    ,436316292
    ,436316296
    ,436316300
    ,436316304
    ,436316308
    ,436316312
    ,436316324
    ,436316328
    ,436316332
    ,436316336
    ,436316340
    ,436316344
    ,436316356
    ,436316360
    ,436316364
    ,436316368
    ,436316372
    ,436316376
    ,436316388
    ,436316392
    ,436316396
    ,436316400
    ,436316404
    ,436316408
    ,436316420
    ,436316424
    ,436316428
    ,436316432
    ,436316436
    ,436316440
    ,436316452
    ,436316456
    ,436316460
    ,436316464
    ,436316468
    ,436316472
    ,436316484
    ,436316488
    ,436316492
    ,436316496
    ,436316500
    ,436316504
};
    // VfdFits.push_back(Fit("TOB", "20120506_run193541", detids_TOB_2012, N_TOB_2012, ""));
    // // // VfdFits.push_back(Fit("TOB", "20120728_run199832", detids_TOB_2012, N_TOB_2012, ""));
    // // // VfdFits.push_back(Fit("TOB", "20120928_run203832", detids_TOB_2012, N_TOB_2012, ""));
    // // // VfdFits.push_back(Fit("TOB", "20121130_run208339", detids_TOB_2012, N_TOB_2012, ""));


    //   VfdFits.push_back(Fit("TOB", "20151121_run262254", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20160706_run276437", detids_TOB_2012, N_TOB_2012, ""));
    //   // VfdFits.push_back(Fit("TOB", "20160803_run278167", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20160909_run280385", detids_TOB_2012, N_TOB_2012, ""));
    //   // VfdFits.push_back(Fit("TOB", "20170527_run295324", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20171030_run305862", detids_TOB_2012, N_TOB_2012, ""));

    //   VfdFits.push_back(Fit("TOB", "20180530_run317182", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20180611_run317683", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20181018_run324841", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20181115_run326776", detids_TOB_2012, N_TOB_2012, ""));

    //   VfdFits.push_back(Fit("TOB", "20211029_run346395", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20220605_run353060", detids_TOB_2012, N_TOB_2012, ""));
    // //   // Fit("TOB", "20220922_run359691", detids_TOB_2012, N_TOB_2012, "");//do not run on detids_2012
    // //   // Fit("TOB", "20221126_run362696", detids_TOB_2012, N_TOB_2012, "");//do not run on detids_2012
    //   VfdFits.push_back(Fit("TOB", "20230407_run365843", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20230609_run368669", detids_TOB_2012, N_TOB_2012, ""));
    //   VfdFits.push_back(Fit("TOB", "20230907_run373060", detids_TOB_2012, N_TOB_2012, ""));

      VfdFits.push_back(Fit("TOB", "20120510_run193928",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20160423_run271056",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20170527_run295376",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20170924_run303824",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20180418_run314574",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20180923_run323370",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20220605_run353060",  detids_FullTOB_2012, N_FullTOB_2012, ""));
      VfdFits.push_back(Fit("TOB", "20230407_run365843",  detids_FullTOB_2012, N_FullTOB_2012, ""));

//     // Missign leakae current information
//       // VfdFits.push_back(Fit("TOB", "20231025_run375658", detids_2012, N_TOB_2012, ""));

      for (unsigned int i = 0 ; i < VfdFits.size(); i++)//loop on runs
        {
          for (unsigned int j = 0 ; j < NFit; j++)//loop on fits
            {
              // FitsVfd[j]+=RunVfd[i][j];
              // std::cout<<"FitsVfd[j] : "<<FitsVfd[j]<<std::endl;
              // std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<RunVfd[i][j]<<std::endl;
              std::cout<<"Run "<<i<<" , Fit : "<<j<<", Vfd value : "<<VfdFits[i][j]<<std::endl;
            }
        }
        return VfdFits;
  }

  
//--------end of -run to analyse------------//



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

int PlotMeanVfdPerRunwPerFit (std::vector<std::vector<float>> VFD, std::vector<float> LUMI,TString subdet) 
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
        gX[i]->SetLineColor(i+1);
        if (i == Nfit-1) gX[i]->SetLineColor(Nfit+1);
        gX[i]->SetLineWidth(2);
        gX[i]->SetMarkerColor(4);
        gX[i]->SetMarkerSize(1.5);
        gX[i]->SetMarkerStyle(21);
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
    leg->AddEntry(gX[0], "sigmoid + pol1", "l");
    leg->AddEntry(gX[1], "atan + pol1", "l");
    leg->AddEntry(gX[2], "sigmoid + pol2", "l");
    leg->AddEntry(gX[3], "atan + pol2", "l");
    leg->AddEntry(gX[7], "General", "l");
    leg->AddEntry(gX[4], "1st Order Deriv : exp or gaussian", "l");
    leg->AddEntry(gX[5], "1st Order Deriv : Trilinear", "l");
    leg->AddEntry(gX[8], "1st Order Deriv : General", "l");
    leg->AddEntry(gX[6], "2nd Order Deriv.:  Gaussian", "l");
    leg->AddEntry(gX[9], "2nd Order Deriv.:  General", "l");
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
        TString text = "Tracker Inner Barrel Layer 1";
        float dx = 355;
        if (subdet=="TOB") {text = "Tracker Outer Barrel";dx = 215;}

        TLatex *t4 = new TLatex(dx,355,text);
        t4->SetTextFont(61);
        t4->SetTextAlign(11);
        t4->SetTextSize(fac*0.3);
        t4->Draw();

    TLine* lvdrop = new TLine(194, 0, 194, 260);
    lvdrop->SetLineStyle(2);
    lvdrop->SetLineColor(1);//red
    lvdrop->Draw("SAME");

    TLine* lvdrop2 = new TLine(28, 0, 28, 350);
    lvdrop2->SetLineStyle(2);
    lvdrop2->SetLineColor(1);//red
    lvdrop2->Draw("SAME");
    TString save_text = "TIB";
    if (subdet=="TOB") {save_text = "TOBL5_FullScans";}
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
    bool SmallScan = true;

    if (SmallScan)
      {
        // // // Run1
        //    Lumi.push_back(7);//193541
        //     // Lumi.push_back(15);//199832
        //     // Lumi.push_back(21);//203832
        //     // Lumi.push_back(28);//208339

        // // // // Run2
        //     Lumi.push_back(29);//     20151121_run262254
        //     Lumi.push_back(44);// 20160706_run276437
        //     // Lumi.push_back(54);// 20160803_run278167
        //     Lumi.push_back(65);// 20160909_run280385
        //     // Lumi.push_back(76);// 20170527_run295324
        //     Lumi.push_back(121);// 20171030_run305862 : first good run for tob

            Lumi.push_back(144);// 20180530_run317182
            Lumi.push_back(151);// 20180611_run317683
            Lumi.push_back(192);// 20181018_run324841
            Lumi.push_back(194);// 20181115_run326776

        // Run3
            Lumi.push_back(194);//run :20211029_run346395
            Lumi.push_back(195);//run :20220605_run353060 : Full
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


    MeanVfdTOB = FitLeakageCurrent("TOB");
    PlotMeanVfdPerRunwPerFit(MeanVfdTOB,Lumi,"TOB");

    // MeanVfdTIB = FitLeakageCurrent("TIB");
    // PlotMeanVfdPerRunwPerFit(MeanVfdTIB,Lumi,"TIB");
    return 0;
  }
