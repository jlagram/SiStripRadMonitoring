#ifndef CurvesFunctions_C
#define CurvesFunctions_C

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include <sys/stat.h> // to be able to check file existence


/* BASH COLORS */
#define RST   "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"no l
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST
#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

//---------------------------

using namespace std;

//Convert a ULong64_t into a TString
TString Convert_Number_To_TString(ULong64_t number)
{
	stringstream ss;
	ss << number;
	TString ts = ss.str();
	return ts;
}

//Convert a TString into a float
double Convert_TString_To_Number(TString ts)
{
	double number = 0;
	string s = ts.Data();
	stringstream ss(s);
	ss >> std::setprecision(10) >> number;
	return number;
}

//Find a number into a TString, and returns it as a float
float Find_Number_In_TString(TString ts)
{
	TString tmp = ""; int number = 0;
	string s = ts.Data(); int ts_size = s.size(); //Only to get TString size

	for (int i=0; i < ts_size; i++)
	{
		if( isdigit(ts[i]) )
		{
			do
			{
				tmp += ts[i];
				i++;
			} while(isdigit(ts[i]) || ts[i] == '.'); //NB : Pay attention to quotes : "a" creates a 2-char array (letter+terminator) -> string. 'a' identifies a single character !

		  break;
		}
	}

	return Convert_TString_To_Number(tmp);
}

//Use stat function (from library sys/stat) to check if a file exists
bool Check_File_Existence(const TString& name)
{
  struct stat buffer;
  return (stat (name.Data(), &buffer) == 0); //true if file exists
}


// Detids infos
//--------------

int GetSubdet(ULong64_t modid)
{
  if(modid>999999999) modid/=10; // sensor option for TOB
  // 3->6 : TIB, TID, TOB, TEC
  static const Int_t kSubdetOffset = 25;
  int subdet = (modid>>kSubdetOffset)&0x7;

  return subdet;
}

char* GetSubdetString(ULong64_t modid)
{
  char* subdet;
  int isubdet = GetSubdet(modid);
  if(isubdet==3) sprintf(subdet, "TIB");
  if(isubdet==4) sprintf(subdet, "TID");
  if(isubdet==5) sprintf(subdet, "TOB");
  if(isubdet==6) sprintf(subdet, "TEC");
  else           sprintf(subdet, "");

  return subdet;
}
int GetLayer(ULong64_t modid)
{
  if(modid>999999999) modid/=10; // sensor option for TOB & TEC
  Int_t subdet = GetSubdet(modid);
  
  static const unsigned layerStartBit = 14;
  static const unsigned layerMask = 0x7;
  static const unsigned ringStartBitTID = 9;
  static const unsigned ringMaskTID = 0x3;
  static const unsigned ringStartBitTEC = 5;
  static const unsigned ringMaskTEC = 0x7;
  Int_t layer = 0;
  
  // For TIB and TOB
  if(subdet==3 || subdet==5) layer = ((modid>>layerStartBit) & layerMask);
  // For TID, returns ring
  if(subdet==4) layer = ((modid>>ringStartBitTID) & ringMaskTID);
  // For TEC, returns ring
  if(subdet==6) layer = ((modid>>ringStartBitTEC) & ringMaskTEC);

  return layer;

}

int GetWheel(ULong64_t modid)
{
  if(modid>999999999) modid/=10; // sensor option for TOB & TEC
  Int_t subdet = GetSubdet(modid);

  static const unsigned wheelStartBitTID = 11;
  static const unsigned wheelMaskTID = 0x3;
  static const unsigned wheelStartBitTEC = 14;
  static const unsigned wheelMaskTEC = 0xF;
  Int_t wheel = 0;

  // For TID 
  if(subdet==4) wheel = ((modid>>wheelStartBitTID) & wheelMaskTID);
  // For TEC
  if(subdet==6) wheel = ((modid>>wheelStartBitTEC) & wheelMaskTEC);
  // For barrel, return 0;
 
  return wheel;
}


// TGraph infos
//--------------

void GetYMeanRMS(TGraph* g, float &mean, float &rms)
{
  double x,y;
  mean=0;
  float mean2=0;
  TH1F h("h", "", 100, -0.1, 0.1);
  for (int i=0; i<g->GetN(); i++)
  {
    g->GetPoint(i, x, y);
	h.Fill(y);
    mean+=y;
    mean2+=y*y;
  }
  
  mean /= g->GetN();
  //mean = h.GetMean();

  //rms = h.GetRMS();
  mean2 /= g->GetN();
  //if(h.Integral()==0)
  rms = sqrt(mean2 - mean*mean);
}

double GetMin(TGraph* g)
{
  double x,y;
  double ymin = 9e+9;
  double xmin = 9e+9;
  
  for (int i=0; i<g->GetN(); i++)
  {
    g->GetPoint(i, x, y);
	if(y<ymin) {xmin=x; ymin=y;}
  }
  
  return xmin;
}

bool isGoodCurve(TGraph* g, string type)
{

  bool isGood = true;

  double x1=0;
  double y1=0;
  double x2=0;
  double y2=0;
  for(int ipt=0; ipt<g->GetN(); ipt++)
  {
    g->GetPoint(ipt, x2, y2);
    //cout<<ipt<<" "<<x2<<" "<<y2<<endl;


    if(ipt==0 && x2>90) {cerr<<"First step above 90V: "<<x2<<"V"<<endl; return false;}
    if(ipt!=0 && fabs(x2-x1)>40 && x2 > 75) {cerr<<"x difference too large b/w 2 pts: "<<x1<<"-"<<x2<<"V"<<endl; return false;}
    
    //FIXME -- changed 09/10, because with Vfd~0 we actually expect large y differences !
    //if(ipt!=0 && x2>150 && (y2-y1)<-15 && type=="Signal") {cerr<<"y difference too large b/w 2 pts: "<<x1<<"V: "<<y1<<"  "<<x2<<"V: "<<y2<<endl; return false;}
    //if(ipt!=0 && fabs(y2-y1)>1. && type=="ClusterWidth") {cerr<<"y difference too large b/w 2 pts: "<<x1<<"V: "<<y1<<"  "<<x2<<"V: "<<y2<<endl; return false;}

    x1=x2;
    y1=y2;
  }
  // last point
  if(x2<250) {cerr<<"Last step below 250V: "<<x2<<"V"<<endl; return false;}

  return isGood;  
}


//////////////////////////////////////////////////////////////////////////////////////////////////

// Get graphs and fits infos
//----------------------------

void GetFitParams(string subdet, string run, ULong64_t modid, double & param0, double & param1, double & param2)
{ 

  ULong64_t detid_1;
  //int layer_1;
  double depvolt_1=0;
  double errdepvolt_1;
  double plateau_1=0;
  double fitchi2_1;
  int fitstatus_1;
  //double chi2_1;

  char* file = Form("DECO_Vdeplchi_%s%s.root", subdet.c_str(), run.c_str());
  TFile* f = TFile::Open(file);
  if(!f) {cout<<FRED("Error : no file ")<<file<<endl; param0=0; param1=0; param2=0; return;}

  TTree *T1  = (TTree*)f->Get("tout");

  T1->SetBranchAddress("DETID",&detid_1);
  //T1->SetBranchAddress("LAYER",&layer_1);
  T1->SetBranchAddress("DEPVOLT",&depvolt_1);
  T1->SetBranchAddress("ERRDEPVOLT",&errdepvolt_1);
  T1->SetBranchAddress("PLATEAU",&plateau_1);
  T1->SetBranchAddress("FITCHI2",&fitchi2_1);
  T1->SetBranchAddress("FITSTATUS",&fitstatus_1);
  //T1->SetBranchAddress("CHI2",&chi2_1);
  unsigned int nentries1 = T1->GetEntries();

  for(unsigned int j=0; j<nentries1; j++)
  {
	T1->GetEntry(j);
	if(detid_1==modid) break;  
  }


  //double params[2];
  param0= depvolt_1;
  param1= plateau_1;
  //cout<<"Fit : plateau "<<plateau_1<<"  depvolt "<<depvolt_1<<endl; 
 
  if(detid_1!=modid) cout<<FRED("Error : no function in ")<<modid<<" in "<<run<<endl;
  f->Close();

  return;
}

void GetFitInfos(string subdet, string run, ULong64_t modid, double & info0, double & info1)
{ 

  ULong64_t detid_1;
  //int layer_1;
  double depvolt_1=0;
  double errdepvolt_1;
  double plateau_1=0;
  double fitchi2_1;
  int fitstatus_1;
  double chi2_1;

  char* file = Form("DECO_Vdeplchi_%s%s.root", subdet.c_str(), run.c_str());
  TFile* f = TFile::Open(file);
  if(!f) {cout<<FRED("Error : no file '")<<file<<"'"<<endl; return;}

  TTree *T1  = (TTree*)f->Get("tout");

  T1->SetBranchAddress("DETID",&detid_1);
  //T1->SetBranchAddress("LAYER",&layer_1);
  T1->SetBranchAddress("DEPVOLT",&depvolt_1);
  T1->SetBranchAddress("ERRDEPVOLT",&errdepvolt_1);
  T1->SetBranchAddress("PLATEAU",&plateau_1);
  T1->SetBranchAddress("FITCHI2",&fitchi2_1);
  T1->SetBranchAddress("FITSTATUS",&fitstatus_1);
  T1->SetBranchAddress("CHI2",&chi2_1);
  unsigned int nentries1 = T1->GetEntries();

  for(unsigned int j=0; j<nentries1; j++)
  {
	T1->GetEntry(j);
	if(detid_1==modid) break;  
  }

  info0= depvolt_1;
  info1= fitchi2_1;
 
  if(detid_1!=modid) cout<<FRED("Error : no function in '")<<modid<<"' in "<<run<<endl;
  f->Close();
  
  return;
}

TGraphErrors* GetGraph(string dirname, string subdet, string run, ULong64_t modid, int err_type)
{

  TGraphErrors* g = 0;

  //Int_t detid_2;
  ULong64_t detid_2;
  double volt_2;
  int id_2;
  double evolt_2;
  double mpv_2;
  double empv_2;
  //int tempdetid_2;
  double chi2overndf_2;

  const int step = 200;
  double avolt[step];
  double aevolt[step];
  double ampv[step];
  double aempv[step];

  char* file = Form("%s/%s_output_DecoMode_%s.root", dirname.c_str(), subdet.c_str(), run.c_str());
  TFile* f = TFile::Open(file);
  if(!f) {cout<<FRED("Error : no file '"<<file<<"'")<<endl; return g;}
  //f->ls();
  
  TTree* tr = (TTree*) f->FindObjectAny("T");
  tr->SetBranchAddress("DetID",&detid_2);
  tr->SetBranchAddress("Voltage",&volt_2);
  tr->SetBranchAddress("Index",&id_2);
  tr->SetBranchAddress("errVoltage",&evolt_2);
  tr->SetBranchAddress("MPV",&mpv_2);
  tr->SetBranchAddress("errMPV",&empv_2);
  tr->SetBranchAddress("Chi2OverNdf",&chi2overndf_2);

  UInt_t nentries = tr->GetEntries();

  int k=0;
  bool rms_tooHigh_allEntries = true; //Stays true if all entries are rejected due to high RMS
  
  for(unsigned int j = 0; j <nentries; j++){
    tr->GetEntry(j);
    
    //if(run=="285371") {cout<<"mpv="<<mpv_2<<endl;}

    if(k>0 && detid_2!=modid) break;
	if(detid_2==modid){
	
		//cout<<"empv = "<<empv_2<<endl;
	
	  if(empv_2<5)
	  {
	    avolt[k]=volt_2;
	    aevolt[k]=evolt_2;
	    ampv[k]=mpv_2;
	    aempv[k]=empv_2;
		if(err_type==1) aempv[k]=2.5;
		if(err_type==2) aempv[k]=5;
		if(err_type==3) aempv[k]=empv_2*5;
		if(err_type==4) aempv[k]=empv_2*10;
		if(err_type==5) aempv[k]=10;
	    k++;
	    rms_tooHigh_allEntries = false;
	  }	  
	}
  }
  
  f->Close();
  
  
  if(rms_tooHigh_allEntries) {cout<<FRED("RMS too high for all entries ; not using run="<<run<<"/modid="<<modid<<"/"<<subdet<<"")<<endl; return 0;}
  if(!k) {cout<<FRED("GetGraph -> return 0")<<endl; return 0;}
  
  g = new TGraphErrors(k, avolt, ampv, aevolt, aempv);
  g->SetLineColor(2);
  g->SetMarkerStyle(20);

  return g;
}

TGraphErrors* GetClusterWidthGraph(TFile* f, ULong64_t modid, int err_type)
{

  TGraphErrors* g = 0;

  //Int_t detid_2;
  ULong64_t detid_2;
  double volt_2;
  int id_2;
  double evolt_2;
  double mean_2;
  double emean_2;
  //int tempdetid_2;

  const int step = 200;
  double avolt[step];
  double aevolt[step];
  double amean[step];
  double aemean[step];

  if(!f) {cout<<FRED("Error : no input file ")<<endl; return g;}

  TTree* tr = (TTree*) f->FindObjectAny("T");
  tr->SetBranchAddress("DetID",&detid_2);
  tr->SetBranchAddress("Voltage",&volt_2);
  tr->SetBranchAddress("Index",&id_2);
  tr->SetBranchAddress("errVoltage",&evolt_2);
  tr->SetBranchAddress("Mean",&mean_2);
  tr->SetBranchAddress("errMean",&emean_2);
  

  UInt_t nentries = tr->GetEntries();

  int k=0;
  for(unsigned int j = 0; j <nentries; j++){
    tr->GetEntry(j);

    if(k>0 && detid_2!=modid) break;
	if(detid_2==modid){
	  if(emean_2<5){
	    avolt[k]=volt_2;
	    aevolt[k]=evolt_2;
	    amean[k]=mean_2;
	    aemean[k]=emean_2;
		if(err_type==1) aemean[k]=2.5;
		if(err_type==2) aemean[k]=5;
		if(err_type==3) aemean[k]=emean_2*5;
		if(err_type==4) aemean[k]=emean_2*10;
		if(err_type==5) aemean[k]=10;
	    k++;

	  }
	}
  }
  
  f->Close();
  f->Delete(); 
 
  if(!k) {cout<<FRED("GetGraph -> return 0 (rms too high ?)")<<endl; return 0;}
  
  g = new TGraphErrors(k, avolt, amean, aevolt, aemean);
  g->SetLineColor(2);
  g->SetMarkerStyle(20);

  return g;
}

TGraphErrors* GetClusterWidthGraph(string dirname, string subdet, string run, ULong64_t modid, int err_type)
{
  TGraphErrors* g = 0;

  char* file = Form("%s/%s_output_DecoMode_%s.root", dirname.c_str(), subdet.c_str(), run.c_str());
  TFile* f = TFile::Open(file);
  if(!f) {cout<<FRED("Error : no file '")<<file<<"'"<<endl; return g;}

  g = GetClusterWidthGraph(f, modid, err_type);
  return g;
}


TGraphErrors* GetOneOverS2Graph(TGraphErrors* g)
{

  TGraphErrors* g2 = new TGraphErrors();
  double voltage=0;
  double signal=0;
  int ipt=0;
  for(unsigned int ip=0; ip<(unsigned) g->GetN(); ip++)
  {
   g->GetPoint(ip, voltage, signal);
   g2->SetPoint(ipt, voltage, 1./pow(signal,2));
   ipt++;
  }
  g2->SetMarkerStyle(20);
 
  return g2;
}

TGraphErrors* GetDerivativeGraph(TGraphErrors* g)
{

  TGraphErrors* gderivative = new TGraphErrors();
  double voltage=0;
  double previous_voltage=0;
  double signal=0;
  double previous_signal=0;
  int ipt=0;
  double volt=0;
  double deriv=0;
  for(unsigned int ip=0; ip<(unsigned) g->GetN(); ip++)
  {
   previous_voltage = voltage;
   previous_signal = signal;
   g->GetPoint(ip, voltage, signal);
   if(!ip) continue;
   if(ip && voltage==previous_voltage) continue;
   volt=(previous_voltage+voltage)/2.;
   deriv = (signal-previous_signal)/(voltage-previous_voltage);
   gderivative->SetPoint(ipt, volt, deriv);
   ipt++;
  }
  gderivative->SetMarkerStyle(20);
 
  return gderivative;
}


//Use first & second derivatives graphs gd and gd2 to compute curvature : k = y'' / ( (1 + y'^2)^3/2 )
TGraphErrors* GetCurvatureGraph(TGraphErrors* gd, TGraphErrors* gd2)
{  
  TGraphErrors* gcurv = new TGraphErrors();
  double x1=0, y1=0, x2=0, y2=0, curv=0;
  double xmin=-1;
  double ymin=0;
  double xmax=-1;
  double ymax=0;
  for(int i=0; i<gd2->GetN(); i++)
  {
    gd2->GetPoint(i, x2, y2);
    
    // Need to interpolate to get y'
    int id=0;
    while(x1<=x2 && id<gd->GetN())
    {
      gd->GetPoint(id, x1, y1);
      if(x1<=x2) {xmin=x1; ymin=y1;}
      if(x1>x2) {xmax=x1; ymax=y1;}
      id++;
    }
    // no point with x1<=x2 found
    if(xmin==-1) continue;
    // point with x1==x2 found
    if(xmin==x2) y1=ymin;
    else
      // linear interpolatation
      if(xmin!=-1 && xmax!=-1)
      {
        float mu=(x2-xmin)/(xmax-xmin);
        if(mu<0 || mu>1) continue;
        y1=ymin*(1-mu)+ymax*mu;
      }
    
    // compute curvature
    curv=y2/TMath::Power(1+y1*y1, 3./2);
    gcurv->SetPoint(i, x2, curv);
  }
  
  //cout<<"Curvature Graph : "<<gd->GetN()<<" pts"<<endl; // laisser sinon pb a la Root
  
  /*cout<<"------"<<endl;
  g->Print("all");
  gd->Print("all");
  gd2->Print("all");
  gcurv->Print("all");
  cout<<"------"<<endl;*/

  return gcurv;
}


TGraphErrors* GetCurvatureGraph(TGraphErrors* g)
{
  TGraphErrors* gd = GetDerivativeGraph( g );
  TGraphErrors* gd2 = GetDerivativeGraph( gd );
  return GetCurvatureGraph(gd, gd2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// Smoothing functions 
//---------------------
TGraphErrors* MedianFilter(TGraphErrors* g)
{
  TGraphErrors* gsmooth = new TGraphErrors(g->GetN(), g->GetX(), g->GetY() , g->GetEX(), g->GetEY());
  //TGraphErrors* gsmooth = new TGraphErrors();
  
  const int N=3; // FUNCTION WRITTEN FOR N=3, CHECKS NEEDED IF WANT TO GENERALIZE
  double* voltage = new double[N];
  double* signal = new double[N];

  int* index = new int[N];
  int ipt=1; // start at 2nd point
  if(N<2 || N>g->GetN()) return gsmooth;
  
  //For each point, take previous and next --> median !
  for(unsigned int ip=0; ip<(unsigned)(g->GetN()+1-N); ip++)
  {
    for(int i=0; i<N; i++)
    {
      g->GetPoint(ip+i, voltage[i], signal[i]);
    }
    
    TMath::Sort(N , signal, index, false); // increasing order
    
    // For N=3 median is the 2nd value
    gsmooth->SetPoint(ipt, voltage[1], signal[index[1]]);
    ipt++;
  }
  
  // For first point
  g->GetPoint(0, voltage[0], signal[0]);
  gsmooth->GetPoint(1, voltage[1], signal[1]);
  gsmooth->GetPoint(2, voltage[2], signal[2]);
  //cout<<signal[2]<<" "<<signal[1]<<" "<<signal[0]<<endl;
  signal[2] = 3*signal[1] - 2*signal[2];
  TMath::Sort(N , signal, index, false);
  gsmooth->SetPoint(0, voltage[0], signal[index[1]]);
  
  //cout<<signal[2]<<" "<<signal[index[1]]<<endl;
  
  // For last point
  g->GetPoint(g->GetN()+2-N, voltage[0], signal[0]);
  gsmooth->GetPoint(gsmooth->GetN()-1, voltage[1], signal[1]);
  gsmooth->GetPoint(gsmooth->GetN()-2, voltage[2], signal[2]);
  signal[2] = 3*signal[1] - 2*signal[2];
  TMath::Sort(N , signal, index, false);
  gsmooth->SetPoint(ipt, voltage[0], signal[index[1]]);

  gsmooth->SetMarkerStyle(20);
  //gsmooth->Print("all");
  
  return gsmooth;  
}

bool IsMonoton(TGraphErrors* g)
{
  bool ismonoton=true;
  bool isincreasing=true;
  double voltage=0;
  double signal=0;
  double previous_signal=0;
  
  int ip=0;
  while(ismonoton && ip<g->GetN())
  {
    previous_signal = signal;
    g->GetPoint(ip, voltage, signal);
    if(ip==1 && previous_signal>signal) isincreasing=false;
    if(ip>1 && 
       ((isincreasing && previous_signal>signal) ||
        (!isincreasing && previous_signal<signal))) ismonoton=false;
    ip++;
  }
  
  return ismonoton;
}

TGraphErrors* HanningFilter(TGraphErrors* g, int npt=3)
{
  //NB : don't clone gsmooth from g ; because removes points ? (-->bug)
  TGraphErrors* gsmooth = new TGraphErrors();
  
  if(npt!=3 && npt!=5) { cout<<FRED("Error : Lissage de Hanning prevu seulement sur 3 ou 5 points.")<<endl;; return gsmooth; }
  
  const int N=npt;// FUNCTION WRITTEN FOR N=3, CHECKS NEEDED IF WANT TO GENERALIZE
  // coef for hanning
  double* coef = new double[N];
  if(npt==3)
  {
   coef[0]=1./4;
   coef[1]=1./2;
   coef[2]=1./4;
  }
  if(npt==5)
  {
   coef[0]=1./8;
   coef[1]=1./4;
   coef[2]=1./4;
   coef[3]=1./4;
   coef[4]=1./8;
  }
  
  double voltage=0;
  double signal=0;
  //int ipt=(N-1)/2;
  int ipt=0;
  double signal_mean=0;
  double voltage_mean=0;
  if(N<2 || N>g->GetN()) return gsmooth;
  
  for(int ip=0; ip<g->GetN()-N+1; ip++)
  {
    voltage_mean = 0;
    signal_mean = 0;
    for(int i=0; i<N; i++)
    {
      g->GetPoint(ip+i, voltage, signal);
      voltage_mean+=voltage;
      signal_mean+=signal*coef[i];
    }
    voltage_mean/=N;
    gsmooth->SetPoint(ipt, voltage_mean, signal_mean);
    ipt++;
  }
  //gsmooth->Print("all");
  
  // first point : linear extrapolation of smoothed curve (no slope change)
  double* voltages = new double[N];
  double* signals = new double[N];
  
  //g->Print("all");
  //gsmooth->Print("all");
  
  g->GetPoint(0, voltages[0], signals[0]);
  gsmooth->GetPoint(1, voltages[1], signals[1]);
  gsmooth->GetPoint(2, voltages[2], signals[2]);
  voltage_mean=2*voltages[1]-voltages[2];
  signal_mean=(signals[2]-signals[1])/(voltages[2]-voltages[1])*(voltage_mean-voltages[1])+signals[1];
  gsmooth->SetPoint(0, voltage_mean, signal_mean);
  
  // last point : linear extrapolation of smoothed curve (no slope change)
  g->GetPoint(g->GetN()-N+2, voltages[0], signals[0]);
  gsmooth->GetPoint(g->GetN()-N, voltages[1], signals[1]);
  gsmooth->GetPoint(g->GetN()-N+1, voltages[2], signals[2]);
  voltage_mean=2*voltages[2]-voltages[1];
  signal_mean=(signals[2]-signals[1])/(voltages[2]-voltages[1])*(voltage_mean-voltages[2])+signals[2];
  gsmooth->SetPoint(ipt, voltage_mean, signal_mean);
  
  gsmooth->SetMarkerStyle(20);
  //gsmooth->Print("all");
  
  return gsmooth;
  
}


// copy-paste of the code of the HanningFilter function
// Savitzky-Golay smoothing method
// Equivalent to fit (least squares method) range of points (at least 5) with cubic polynomials to recompute the middle point
// New coordinates can be computed from the points in the range and coefficients.
// Derivates can also be directly computed with special coefficients.
TGraphErrors* SavitzkyGolaySmoother(TGraphErrors* g, int npt=5, int deriv=0)
{
  TGraphErrors* gsmooth = new TGraphErrors();
  
  if(npt!=5 && npt!=7) { cout<<FRED("Error : works with npt=5 or 7")<<endl;; return gsmooth; }
  
  const int N=npt;
  double* coef = new double[N];
  if(npt==5)
  {
    coef[0]=-3./35;
    coef[1]=12./35;
    coef[2]=17./35;
    coef[3]=12./35;
    coef[4]=-3./35;

    if(deriv==1)
      // rajouter division par le pas
    {
      coef[0]=-2./10;
      coef[1]=-1./10;
      coef[2]=0./10;
      coef[3]=1./10;
      coef[4]=2./10;      
    }
    if(deriv==2)
      // rajouter division par le pas au carré
    {
      coef[0]=2./7;
      coef[1]=-1./7;
      coef[2]=-2./7;
      coef[3]=-1./7;
      coef[4]=2./7;      
    }
  }
  if(npt==7)
  {
    coef[0]=-2./21;
    coef[1]=3./21;
    coef[2]=6./21;
    coef[3]=7./21;
    coef[4]=6./21;
    coef[5]=3./21;
    coef[6]=-2./21;

    if(deriv==1)
    {
      coef[0]=-3./28;
      coef[1]=-2./28;
      coef[2]=-1./28;
      coef[3]=0./28;
      coef[4]=1./28;
      coef[5]=2./28;
      coef[6]=3./28;
    }
    if(deriv==2)
    {
      coef[0]=5./42;
      coef[1]=0./42;
      coef[2]=-3./42;
      coef[3]=-4./42;
      coef[4]=-3./42;
      coef[5]=0./42;
      coef[6]=5./42;
    }
    
  }

  double voltage=0;
  double signal=0;
  //int ipt=(N-1)/2;
  int ipt=0;
  double signal_mean=0;
  double voltage_mean=0;
  if(N<2 || N>g->GetN()) return gsmooth;
  
  // Choose arbitrarly the first step as the one used for all the points. // For scaling of derivative values
  float step=0;
  double v1=0;
  double v2=0;
  g->GetPoint(0, v1, signal);
  g->GetPoint(1, v2, signal);
  step = abs(v2-v1);
  
  for(int ip=0; ip<g->GetN()-N+1; ip++)
  {
    voltage_mean = 0;
    signal_mean = 0;
    for(int i=0; i<N; i++)
    {
      g->GetPoint(ip+i, voltage, signal);
      voltage_mean+=voltage;
      signal_mean+=signal*coef[i];
    }
    voltage_mean/=N;
    if(deriv==1) signal_mean/=(step);
    if(deriv==2) signal_mean/=(step*step); 
    gsmooth->SetPoint(ipt, voltage_mean, signal_mean);
    //cout<<"pt "<<ipt<<" "<< voltage_mean<<" "<< signal_mean<<endl;
    ipt++;
  }
  //gsmooth->Print("all");

  gsmooth->SetMarkerStyle(20);
  //gsmooth->Print("all");

  return gsmooth;
  
}

TGraphErrors* GetSavitzkyGolayCurvatureGraph(TGraphErrors* g, int npt=5)
{
  TGraphErrors* gd = SavitzkyGolaySmoother( g, npt, 1 );
  TGraphErrors* gd2 = SavitzkyGolaySmoother( g, npt, 2 );
  return GetCurvatureGraph(gd, gd2);
}



TGraphErrors* GetSmoothGraph(TGraphErrors* g)
{

  TGraphErrors* gsmooth = new TGraphErrors();
  double voltage=0;
  double previous_voltage=0;
  double next_voltage=0;
  double signal=0;
  double previous_signal=0;
  double next_signal=0;
  int ipt=0;
  double signal_mean=0;
  double voltage_mean=0;
  
  for(unsigned int ip=0; ip<(unsigned) g->GetN(); ip++)
  {
   previous_signal = signal;
   previous_voltage = voltage;
   g->GetPoint(ip, voltage, signal);
   g->GetPoint(ip+1, next_voltage, next_signal);
   signal_mean = ( previous_signal + signal + next_signal )/3.;
   voltage_mean = ( previous_voltage + voltage + next_voltage )/3.;
   if(!ip){
    signal_mean = ( signal + next_signal )/2.;
    voltage_mean = ( voltage + next_voltage )/2.;
   }
   if(ip==(unsigned)(g->GetN()-1)){
    signal_mean = ( previous_signal + signal )/2.;
    voltage_mean = ( previous_voltage + voltage )/2.;	
   }
   gsmooth->SetPoint(ipt, voltage_mean, signal_mean);
   ipt++;
  }
  gsmooth->SetMarkerStyle(20);
 
  return gsmooth;
}

TGraphErrors* GetSmoothGraph_NPtsMean(TGraphErrors* g, int N=4)
{

  TGraphErrors* gsmooth = new TGraphErrors();
  double voltage=0;
  double signal=0;
  int ipt=0;
  double signal_mean=0;
  double voltage_mean=0;
  if(N<2 || N>g->GetN()) return gsmooth;
  
  for(unsigned int ip=0; ip<(unsigned)(g->GetN()+1-N); ip++)
  {
    voltage_mean = 0;
    signal_mean = 0;
	for(int i=0; i<N; i++)
	{
      g->GetPoint(ip+i, voltage, signal);
	  voltage_mean+=voltage;
	  signal_mean+=signal;
	}
	voltage_mean/=N;
    signal_mean/=N;
    gsmooth->SetPoint(ipt, voltage_mean, signal_mean);
    ipt++;
  }
  gsmooth->SetMarkerStyle(20);
 
  return gsmooth;
}

TGraphErrors* Interpolate(TGraphErrors* g)
{
  TGraphErrors* gsmooth = new TGraphErrors();
  
  // WRITTEN TO DOUBLE N POINTS
  
  const int N=4; // FUNCTION WRITTEN FOR CUBIC INTERPOLATION -> 4 POINTS NEEDED
  double* voltage = new double[N];
  double* signal = new double[N];
  int ipt=0;
  
  for(int ip=0; ip<g->GetN(); ip++)
  {
    // Set existing point
    g->GetPoint(ip, voltage[0], signal[0]);
    gsmooth->SetPoint(ipt, voltage[0], signal[0]);
    ipt++;
    
    // Add interpolated point
    if(ip>=(N/2-1) && ip<=(g->GetN()-1-N/2))
    {
      for(int i=0; i<N; i++)
      {
        g->GetPoint(ip+1-N/2+i, voltage[i], signal[i]);
      }
      double mu=0.5; // add 1 point in the middle of the interval
      double a0,a1,a2,a3; 
      a0 = -0.5*signal[0] + 1.5*signal[1] - 1.5*signal[2] + 0.5*signal[3];
      a1 = signal[0] - 2.5*signal[1] + 2*signal[2] - 0.5*signal[3];
      a2 = -0.5*signal[0] + 0.5*signal[2];
      a3 = signal[1];
      gsmooth->SetPoint(ipt, voltage[1]*(1-mu)+voltage[2]*mu, a0*mu*mu*mu+a1*mu*mu+a2*mu+a3);
      ipt++;
    }
  }
  
  gsmooth->SetMarkerStyle(20);
  //gsmooth->Print("all");
  
  return gsmooth;  
}


//////////////////////////////////////////////////////////////////////////////////////////////////

// Minimum finding
//-----------------

double GetOptimalMin3Pts(TGraph* g, TGraph* gout)
{
  double x,y;
  double ymin = 9e+9;
  double xmin = 0;
  double imin = 0;
  
  // empty gout
  for (int i=g->GetN()-1; i>=0; i--)
    gout->RemovePoint(i);
  if(gout->GetN()>0) cout<<FRED("WARNING : gout not empty (GetOptimalMin3Pts)")<<endl;

  // Get minimum
  for (int i=0; i<g->GetN(); i++)
  {
    g->GetPoint(i, x, y);
	if(y<ymin) {imin=i; xmin=x; ymin=y;}
  }
  double xopt = xmin;

  // Get 3 points around min
  double x3pts[3];
  double y3pts[3];
  if(imin==0) imin=1;
  if(imin==g->GetN()-1) imin=g->GetN()-2;
  
  g->GetPoint(imin-1, x3pts[0], y3pts[0]);
  g->GetPoint(imin, x3pts[1], y3pts[1]);
  g->GetPoint(imin+1, x3pts[2], y3pts[2]);
  
  if(x3pts[0]!=0 && x3pts[2]!=0)
  {
    // move xmin depending of symetry of positions of the previous and the next points
    //double dy0 = y3pts[0]-y3pts[1];
    //double dy2 = y3pts[2]-y3pts[1];
	gout->SetPoint(0, x3pts[0], y3pts[0]);
	gout->SetPoint(1, x3pts[1], y3pts[1]);
	gout->SetPoint(2, x3pts[2], y3pts[2]); 
	gout->Fit("pol2", "q");
    TF1* fit = gout->GetFunction("pol2");
    xopt = fit->GetMinimumX(30, 300);

  }
  
  return xopt;
}

double GetOptimalMinNPts(TGraph* g, TGraph *gout, double xthresh=0, double xthreshmax = 0, int N=0, double *chi2=0, double *ndf=0,
  double *n_rms=0, double *err=0)
{
  double x,y;
  float ymean, yrms;
  
  if(!g->GetN()) {cout<<FRED("GetOptimalMinNPts() : graph has no points.")<<endl; return -1;}

  // empty gout
  for (int i=gout->GetN()-1; i>=0; i--)
    gout->RemovePoint(i);
  if(gout->GetN()>0) cout<<FRED("WARNING : gout not empty (GetOptimalMinNPts)")<<endl;
  
  // Get minimum point index
  double ymin = 9e+9;
  int imin = 999999;
  for (int i=0; i<g->GetN(); i++)
  {
    g->GetPoint(i, x, y);
  	
  	//if(x>xthresh && y<ymin) {imin=i; ymin=y;}
   	if(x>xthresh && x<xthreshmax && y<ymin) {imin=i; ymin=y;}
  }
  if(imin>g->GetN()) {cout<<FRED("GetOptimalMinNPts() : min not found.")<<endl; return -1;}

  
  // Get Mean and RMS of y distrib.
  GetYMeanRMS(g, ymean, yrms );  

  //cout<<"imin : "<<imin<<endl;
  //cout<<"Threshold : "<<ymean-yrms<<endl;
  
  g->GetPoint(imin, x, y);

  if(y>ymean-yrms) //if lowest point (in y) too cloose to mean of y distrib
  {
  	//cout<<FRED("GetOptimalMinNPts() : ymin not far enough from other points.")<<endl; 
  	//return -1; //FIXME
  }
  
  
  // automatic choice of number of points
  if(!N)
  {
    // Get points around minimum and below threshold
    int i=imin;
    int ifirst=imin;
    g->GetPoint(imin, x, y);
    while(y<ymean-yrms && i>0)
    {
      i--;
      g->GetPoint(i, x, y);
    //cout<<i<<" "<<y<<endl;
    if(y<ymean-yrms) ifirst=i;
    }
    i=imin;
    int ilast=imin;
    g->GetPoint(imin, x, y);
    while(y<ymean-yrms && i<g->GetN())
    {
      i++;
      g->GetPoint(i, x, y);
    //cout<<i<<" "<<y<<endl;
    if(y<ymean-yrms) ilast=i;
    }
    
    // Use points below threshold ymean-yrms plus 1 point before and 1 point after
    if(ifirst>0) ifirst--;
    if(ilast<g->GetN()-1) ilast++;
    
    for(i=ifirst; i<=ilast; i++)
    {
      g->GetPoint(i, x, y);
      gout->SetPoint(i-ifirst, x, y);
    } 
  }
  else
  {
    // N impair
    int Nside=N/2;
    for (int i=imin-Nside; i<=imin+Nside; i++)
    {
      g->GetPoint(i, x, y);
      gout->SetPoint(i-imin+Nside, x, y);
    }      
  }
  
  
  gout->Fit("pol2", "q");
  TF1* fit = gout->GetFunction("pol2");
  double xopt = fit->GetMinimumX(30, 300);
  if(chi2) *chi2 = fit->GetChisquare();
  if(ndf) *ndf = fit->GetNDF();
  if(n_rms)
  {
    *n_rms = 0;
	if(yrms !=0) *n_rms = fabs(ymin-ymean)/yrms;
  }
  if(err)
  {
    ymin = fit->Eval(xopt);
    double xmin = fit->GetX( ymin+0.1*yrms, 30, xopt);
    double xmax = fit->GetX( ymin+0.1*yrms, xopt, 300);
	*err = TMath::Max(xopt-xmin, xmax-xopt);
	//cout<<"err "<<xopt-xmin<<" "<<xmax-xopt<<" "<<*err<<endl;
  
  }

  //g->Print("all");
  //gout->Print("all");
  //cout<<"Npts : "<<gout->GetN()<<endl;
  return xopt;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

// Leackage current corrections
//------------------------------

int CorrectGraphForLeakageCurrent(TGraph* g, double detid, TFile* f)
{
  // Get correction function
  	
  if(!f) {return 0;}	
  
  
  TF1* func = (TF1*) f->Get( ("fit_" +Convert_Number_To_TString(detid)).Data() );   
  
  //WARNING on missing Leakage correction, only if module is in list of monitored detectors
  if(!func) 
  {
  	bool isInList = false;
  
  	ifstream det_file;
  	TString ts = Convert_Number_To_TString(detid);
  	if(ts.Contains("36912")) {det_file.open("detid_lists/TIB_detit_list.txt");}
  	else if(ts.Contains("43628")) {det_file.open("detid_lists/TOB_detit_list.txt");}
  	else if(ts.Contains("47014")) {det_file.open("detid_lists/TEC_detit_list.txt");}
  	else {det_file.open("detid_lists/TID_detit_list.txt");}
  	//else {cout<<"Corresponding detector list not found ! Detid = "<<ts<<endl; return 0;}
  	//cout<<"detID = "<<detid<<endl;
  	
  	string line; double id=0;
  	while(getline(det_file,line))
  	{
  		std::istringstream iss(line);
        iss >> id;
        cout<<id<<endl;
        if(detid == id || detid/10 == id || detid == id/10) {isInList = true;}
  	}
  
  	//if(isInList) 
  	{
  		cout<<FRED("No leakage current correction for detid : ")<<std::setprecision(11)<<detid<<endl;
  	}
  	
  	return 0;
  }

  // Correct points
  double x,y;
  for(int i=0; i<g->GetN(); i++)
  {
    g->GetPoint(i, x, y);
    g->SetPoint(i, x-func->Eval(x), y);
  }
  
  return 1;
}

int CorrectGraphForLeakageCurrent(TGraph* g, double detid, string output_name)
{
  // Get correction function
  string filename="LeakCurCorr"+output_name+".root";
  //string filepath="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/"+filename;
  
 
  string filepath="../../../LeakageCurrentBiasScan/Analysis/LeakCurCorr_files/"+filename;
  
  if (filename.find("170000") != std::string::npos) {return 0;} //No leakage correction for this old run, neglected
  if(!Check_File_Existence(filepath)) {cout<<BOLD(FRED("No leakage current correction file : "))<<filepath<<endl; return 0;}
  
  TFile* f = new TFile(filepath.c_str(), "read");

  
  int out = CorrectGraphForLeakageCurrent(g, detid, f);

  f->Close();
  f->Delete();
  
  return out;
  
}

#endif
