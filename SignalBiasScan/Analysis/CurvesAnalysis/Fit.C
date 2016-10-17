#include "../CommonTools/CurvesFunctions.C"
#include "TLine.h"
#include "TCanvas.h"

Double_t fitsigmo(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0]/(1+exp(-1*par[1]*(x[0]+par[3])));//+ par[4]*x[0]; // sigmoid
  }
  else
  {
    value_th = par[0]/(1+exp(-1*par[1]*(par[2]+par[3])));
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}

Double_t fitpol(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[4]) {
	value = par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0];
  }
  else
  {
    value_th = par[0] + par[1]*par[4] + par[2]*par[4]*par[4] + par[3]*par[4]*par[4]*par[4];
    value_0 = value_th - par[5]*par[4];
    value = value_0 + par[5]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}

double getCrossingPoint ( TF1* function1, TF1* function2)
{
	double x_crossingpoint;
	double oao_function1, cd_function1, oao_function2, cd_function2; 
	oao_function1 = function1->GetParameter(0); // ordonnée à l'origine
	cd_function1 = function1->GetParameter(1); // coefficient directeur
	oao_function2 = function2->GetParameter(0);// ordonnée à l'origine
	cd_function2 = function2->GetParameter(1);// coefficient directeur

	// at the crossing point, we have cd_function1*x_crossingpoint + oao_function1 = cd_function2*x_crossingpoint + oao_function2
	// it is the same as having x_crossingpoint = (oao_function2 - oao_function1)/(cd_function1 - cd_function2 )
	x_crossingpoint = (oao_function2 - oao_function1)/(cd_function1 - cd_function2 );

return x_crossingpoint;
}

Double_t GetKinkByFit(Double_t *x, Double_t *par)
{
  Double_t value, value_th, value_0;
  if (x[0]<par[2])
  {
        value = par[0] + par[1]*x[0] + par[3]*x[0]*x[0]; // pol2
        //value = par[0] + par[1]*x[0];
  }
  else
  {
        //value_th = par[0] + par[1]*par[2]
        value_th = par[0] + par[1]*par[2] + par[3]*par[2]*par[2] ;
        value_0 = value_th - par[4]*par[2];
        value = value_0 + par[4]*x[0]; // pol1
  }

  return value;
}


double FitCurve(TGraphErrors* g, int debug=0, bool filter_twice=false)
{

  // option to use SavitzkyGolaySmoother that can gives derivatives on a larger range
  bool SGSmooth=false;
  
  double vdep=0;
  if(!g) return vdep;
  
  
  TF1* f3 = new TF1("fp1", "pol1", 20, 360);
  f3->SetLineColor(4);


  // Clean-up of the curve with the MedianFilter
  TGraphErrors* gmedian = MedianFilter( g );
  /*int nfilt=1;
  while (!IsMonoton(gmedian) && nfilt<4) {
    gmedian = MedianFilter( gmedian );
    nfilt++;
  }*/
  //cout<<nfilt<<" median filter applied"<<endl;
  gmedian = MedianFilter( gmedian );
  
  
  double y;
  double xlow=350;
  double ymax=0;
  double ymin=999;
  double vdep1=0;
  // prendre le point le plus haut est robuste et marche bien pour les hauts vdep
  for(int ipt=0; ipt<gmedian->GetN(); ipt++)
  {
    gmedian->GetPoint(ipt, xlow, y);
    if(y>ymax) {ymax=y; vdep1=xlow;}
    if(y<ymin) ymin=y;
  }
  cout<<"Vdep by highest point = "<<vdep1<<endl;


  // Compute a voltage threshold for Kink finding algo
  // 80% of ymax
  // ymin depends on the steps taken at low HV -> use 1 as a common minimum for both Signal and ClusterWidth
  double ylow = 1;
  double ythresh = ylow + (ymax-ylow)*0.8;
  double xthresh=0;
  for(int ipt=0; ipt<gmedian->GetN(); ipt++)
  {
    gmedian->GetPoint(ipt, xlow, y);
	if(y>ythresh)
	{
	  ipt--;
	  gmedian->GetPoint(ipt, xlow, y);
	  xthresh = xlow;
	  ipt = gmedian->GetN();
	}
  }
  cout<<"x thresh : "<<xthresh<<endl;
  
  
  // Fitting the curve by line from the right until it is no more compatible with a line
  // Stopping point can be considerered as the saturation point
  //------------------------------------------------------------------------------------
  //g=gmedian;
  int npt = g->GetN()-5;
  xlow=350;
  double chi2=0;
  int status = 0;
  while(chi2<5. && xlow>100 && npt>=0)
  {
    g->GetPoint(npt, xlow, y);
    f3->SetRange(xlow, 350);
    status = g->Fit("fp1", "rqn");
    chi2 = f3->GetChisquare()/f3->GetNDF();
    if(debug>=1) cout<<" xlow "<<xlow<<" chi2 "<<chi2<<endl;
	npt--;
  }
  g->GetPoint(npt+2, xlow, y);
  f3->SetRange(xlow, 350);
  g->Fit("fp1", "rqn");
//  vdep = xlow;

  
  //if(vdep>230) vdep = vdep1;
  //else if(fabs(vdep-vdep1)>40) vdep = vdep1;
  vdep=vdep1;

  cout<<"Vdep by 1 line = "<<xlow<<endl;
  
  
  
  // Fitting the curves with 2 lines (done for cluster width curves in TOB)
  // Crossing point is the saturation point
  //-----------------------------------------
  
  TF1* f3_high = new TF1("fphigh", "pol1", 20, 360);
  f3_high->SetLineColor(4);
  TF1* f3_low = new TF1("fplow", "pol1", 20, 360);
  f3_low->SetLineColor(kGreen);
  
  //g=gmedian;
 
  int npt_high = g->GetN()-3; //to take at least the 4 last points into account for the fit with the blue line
  int npt_low_init  = 2; //don't take the 4 first points into account for the fit with the green line
  int npt_low  = 7; //to start the fit with the 4th, 5th, 6th and 7th point (arbitrary at least 4points)
  double vdep_high = 350;
  int tmp_marker = 0; //Marker not to take into account the points which have a too big first chi2
  double vdep_low  = 0; //No matter the value 
  double vdep_low_init = 0; //No matter the value
  g->GetPoint(npt_low_init, vdep_low_init, y);//To set the minimum of the range to the V of the 4th point 
  //cout << "Vdep_low_init= " << vdep_low_init <<  endl;
  xlow = 350;
  double chi2_high=0, chi2_low=0;
  
  //Process the fit from the right
  // chi2_high < 0.012 in TOB CW
  while(chi2_high < 2. && vdep_high > 100 && npt_high >= 0)
  {
    g->GetPoint(npt_high, vdep_high, y);
	//cout<<"fit right: "<<npt_high<<"  vdep "<<vdep_high<<endl;
    f3_high->SetRange(vdep_high, 350);
    status = g->Fit("fphigh", "rqn");
    chi2_high = f3_high->GetChisquare()/f3_high->GetNDF();
    if(chi2_high > 50 && npt_high == g->GetN() - 4) tmp_marker = 100; // Not to deal with the bad modules
	//cout<<"fit right: "<<npt_high<<"  chi2 "<<chi2_high<<endl;
	npt_high--;
  }
  if(npt_high+2<g->GetN()-3)
  {
    g->GetPoint(npt_high+2, vdep_high, y);
    f3_high->SetRange(vdep_high, 350);
    g->Fit("fphigh", "rqn");
  }

  status = 0;
  
  //Process the fit from the left
  while(chi2_low < 0.02 && vdep_low < 350 && npt_low < g->GetN() - 4)
  {
    g->GetPoint(npt_low, vdep_low, y);
    f3_low->SetRange(vdep_low_init, vdep_low);
    status = g->Fit("fplow", "rqn");
    chi2_low = f3_low->GetChisquare()/f3_low->GetNDF();
    if(chi2_low > 50 && npt_low == 7) tmp_marker = 100; // Not to deal with the bad modules
	npt_low++;
  }
  g->GetPoint(npt_low-2, vdep_low, y);
  f3_low->SetRange(vdep_low_init, vdep_low);
  g->Fit("fplow", "rqn");

  double vdep_crossingpoint = getCrossingPoint(f3_low , f3_high);
  cout << "Vdep by 2 lines = " << vdep_crossingpoint << endl;
  
  
  
  // Fitting the curve with a sigmoid or a composite function
  //----------------------------------------------------------
  
  TF1* f1 = new TF1("fitsigmo", fitsigmo, 50, 350, 5);
  f1->SetParameter(0, 3.);
  f1->SetParameter(1, 0.02);
  f1->SetParameter(2, vdep);
  f1->SetParameter(3, -10.);
  f1->SetParameter(4, -0.00001);
  f1->SetParLimits(2, 100., 300.);
  f1->SetParLimits(4, -0.1, 0);
  
  
  
  
  TF1* f2 = new TF1("fitpol", fitpol, 20, 360, 6);
  f2->SetParLimits(4, 100., 300.);
  f2->SetParLimits(5, -0.1, 0);
  
 /* xlow = 30;
  chi2 = 100;
  double xmin = xlow;
  double chi2min = chi2;
  string opt="rq";
  if(debug>=2) opt="r";
  while(chi2>0.1 && xlow<vdep-80)
  {
    f2->SetParameter(0, 1.);
    f2->SetParameter(1, 0.005);
    f2->SetParameter(2, 0.0001);
    f2->SetParameter(3, 0.);
    f2->SetParameter(4, vdep);
    f2->SetParameter(5, -0.00001);
    f2->SetRange(xlow, 350);
    status = g->Fit("fitpol", opt.c_str());
    if(status!=0) status = g->Fit("fitpol", opt.c_str());
    chi2 = f2->GetChisquare()/f2->GetNDF();
    if(debug>=1) cout<<"xlow "<<xlow<<" chi2 "<<chi2<<endl;
    if(debug>=2) cout<<" chi2 "<<f2->GetChisquare()<<" ndf "<<f2->GetNDF()<<endl;
	if(chi2<chi2min) {chi2min=chi2; xmin=xlow;}
	xlow+=10;
  }
  f2->SetParameter(0, 1.);
  f2->SetParameter(1, 0.005);
  f2->SetParameter(2, 0.0001);
  f2->SetParameter(3, 0.);
  f2->SetParameter(4, vdep);
  f2->SetParameter(5, -0.00001);
  f2->SetRange(xmin, 350);
  status = g->Fit("fitpol", opt.c_str());
  if(status!=0) status = g->Fit("fitpol", opt.c_str());
  vdep = f2->GetParameter(4);
  cout<<" Vdepl2 = "<<vdep<<" xmin "<<xmin<<endl;
  if(fabs(vdep-vdep1)>30) vdep = vdep1;
*/


  // Getting the kink of the curve after smoothing
  // In using the curvature
  //-----------------------------------------------
  TGraphErrors* gsmooth = gmedian;
  if(SGSmooth) gsmooth = SavitzkyGolaySmoother(gmedian, 5, 0);
  else{
    gsmooth = HanningFilter(gmedian);
    // To check : 2nd hanningFilter better with 3 or 5 points ?
    if(filter_twice) gsmooth = HanningFilter(gsmooth); // For TOB, some scans for TID
  }

  TGraphErrors* gscurv;
  if(SGSmooth) gscurv = GetSavitzkyGolayCurvatureGraph( gmedian );
  else gscurv = GetCurvatureGraph( gsmooth );
  gscurv->SetMarkerStyle(20);
  TGraph* g3pts = new TGraph();
  float xopt = GetOptimalMinNPts(gscurv, g3pts, xthresh);

  vdep = xopt;

  cout<<"Vdep by kink = "<<xopt<<endl;


  // Drawing the curves and fits
  TCanvas *c1;
  TCanvas *c2;
  if(debug>=1)
  {
    c1 = new TCanvas();
    g->Draw("AP");
    g->GetXaxis()->SetTitle("V_{bias} [V]");
    //g->GetYaxis()->SetTitle("ClusterWidth [#strips]");
    //f2->Draw("same");
    //f3->Draw("same");
	gsmooth->SetMarkerColor(17);
	gsmooth->Draw("P");
	gmedian->SetMarkerColor(15);
	//gmedian->Draw("P");	
	TLine *l = new TLine(vdep, ymin, vdep, ymax+0.1);
	l->SetLineStyle(3);
	l->Draw();
	
	// draw low threshold
	TLine *lthresh = new TLine(xthresh, ymin, xthresh, ymax+0.1);
	lthresh->SetLineStyle(2);
	//lthresh->Draw();
	g->GetPoint(0, xlow, y);
	TGraph* gthresh = new TGraph(4);
	gthresh->SetPoint(0, xlow, ymax+0.1);
	gthresh->SetPoint(1, xthresh, ymax+0.1);
	gthresh->SetPoint(2, xthresh, ymin);
	gthresh->SetPoint(3, xlow, ymin);
	gthresh->SetFillStyle(3354);
	//gthresh->Draw("f");
 
    // draw 2 line fit and crossing point position
	f3_high->Draw("same");  // blue line 
    f3_low->Draw("same");  // green line 
	TLine *lcross = new TLine(vdep_crossingpoint, ymin, vdep_crossingpoint, ymax+0.1);
	lcross->SetLineStyle(2);
	lcross->Draw(); //dashed vdep line 

    c1->Modified();
    c1->Update();

    string detid = g->GetTitle();
    detid.erase(0,6);
    //c1->Print(Form("ClusterWidthVsVbias_detid_%s_run170000.eps", detid.c_str()));

    c2 = new TCanvas;
    gscurv->Draw("AP");
    gscurv->GetXaxis()->SetTitle("V_{bias} [V]");
    gscurv->GetYaxis()->SetTitle("Curvature");
    g3pts->Draw("P");
    g3pts->Fit("pol2", "q");
    c2->Modified();
    c2->Update();

    //c2->Print(Form("ClusterWidthVsVbias_detid_%s_curv_run170000.eps", detid.c_str()));

    getchar();
    c1->Close();
    c2->Close();
  }

  return vdep;
}

void FitOneCurve(string dirname, string subdet, string run, ULong64_t modid, string type, int debug=0)
{
  if(type!="ClusterWidth" && type!="Signal") 
  {cout<<"Error in FitOneCurve() : curve type "<<type<<" is not allowed."<<endl; return;}
  
  TGraphErrors* g;
  if(type=="ClusterWidth") g = GetClusterWidthGraph(dirname, subdet, run, modid, 0);
  if(type=="Signal") g = GetGraph(dirname, subdet, run, modid, 0);
  if(!g) return;

  isGoodCurve(g, type);
  //if(!isGoodCurve(g, type)) return;

  // Correct for voltage drop due to leakage current
  string corr_name="_"+subdet+run;
  if(subdet == "TOB") modid /= 10;
  int corrected = CorrectGraphForLeakageCurrent(g, modid, corr_name);
  if(corrected) g->SetMarkerColor(13);

  g->SetTitle(Form("DetID %i", (int) modid));
  g->SetName("g");

  FitCurve(g, debug);
}


void FitTIBCurves(string dirname, string run, string type)
{
  // L1
  FitOneCurve(dirname, "TIB", run, 369121606, type, 1);
  FitOneCurve(dirname, "TIB", run, 369124429, type, 1);
  FitOneCurve(dirname, "TIB", run, 369124550, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121365, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121302, type, 1);
  FitOneCurve(dirname, "TIB", run, 369170776, type, 1);

  // L4
  FitOneCurve(dirname, "TIB", run, 369169980, type, 1);
  FitOneCurve(dirname, "TIB", run, 369169864, type, 1);
  FitOneCurve(dirname, "TIB", run, 369170636, type, 1);
  FitOneCurve(dirname, "TIB", run, 369173944, type, 1);

}

void FitTIBLowVfdCurves(string dirname, string run, string type)
{
  // L1
  FitOneCurve(dirname, "TIB", run, 369121302, type, 1);
  FitOneCurve(dirname, "TIB", run, 369170776, type, 1);

  // L4
  FitOneCurve(dirname, "TIB", run, 369170636, type, 1);
  FitOneCurve(dirname, "TIB", run, 369173944, type, 1);

}

void FitTIBSmallScan(string dirname, string run, string type)
{
 //TIBminus_1_2_2_1
  FitOneCurve(dirname, "TIB", run, 369121381, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121382, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121385, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121386, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121389, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121390, type, 1);
 //TIBminus_1_4_2_5
  FitOneCurve(dirname, "TIB", run, 369121605, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121606, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121609, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121610, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121613, type, 1);
  FitOneCurve(dirname, "TIB", run, 369121614, type, 1);
 //TIBplus_1_6_2_5
  FitOneCurve(dirname, "TIB", run, 369125861, type, 1);
  FitOneCurve(dirname, "TIB", run, 369125862, type, 1);
  FitOneCurve(dirname, "TIB", run, 369125865, type, 1);
  FitOneCurve(dirname, "TIB", run, 369125866, type, 1);
  FitOneCurve(dirname, "TIB", run, 369125869, type, 1);
  FitOneCurve(dirname, "TIB", run, 369125870, type, 1);
}


void FitTIDCurves(string dirname, string run, string type)
{
  /*FitOneCurve(dirname, "TID", run, 402674846, type, 1);
  FitOneCurve(dirname, "TID", run, 402673352, type, 1);
  FitOneCurve(dirname, "TID", run, 402673440, type, 1);
  FitOneCurve(dirname, "TID", run, 402664109, type, 1);
*/

  FitOneCurve(dirname, "TID", run, 402668829, type, 1);
  FitOneCurve(dirname, "TID", run, 402672930, type, 1);
  FitOneCurve(dirname, "TID", run, 402677025, type, 1);
  FitOneCurve(dirname, "TID", run, 402677041, type, 1);
  
}

void FitTOBSmallScan(string dirname, string run, string type)
{
	FitOneCurve(dirname, "TOB", run, 4362815081, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815082, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815121, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815122, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815161, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815162, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815201, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815202, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815241, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815242, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815281, type, 1);
	FitOneCurve(dirname, "TOB", run, 4362815282, type, 1);

}

void Fit()
{
  //FitOneCurve("TIB", "_208339", 369125869, "Signal", 1);
  //FitOneCurve("TIB", "_200786", 369136953, "Signal", 1);
  //FitOneCurve("~/work/public/SiStripRadMonitoring/SignalCurves/", "TIB", "_178367", 369125862, "Signal", 1);
  //FitOneCurve("~/work/public/SiStripRadMonitoring/ClusterWidthCurves/", "TIB", "_246963", 369121302, "ClusterWidth", 1);
  //FitTIDCurves("~/work/public/SiStripRadMonitoring/SignalCurves/",  "_193928","Signal");
  //FitOneCurve("~/work/public/SiStripRadMonitoring/SignalCurves/", "TID", "_246963", 402664070, "Signal", 1);
  //FitTIBLowVfdCurves("~/work/public/SiStripRadMonitoring/SignalCurves/",  "_193928","Signal");
  
  FitTIBSmallScan("~/work/public/SiStripRadMonitoring/SignalCurves/",  "_271056","Signal");
}
