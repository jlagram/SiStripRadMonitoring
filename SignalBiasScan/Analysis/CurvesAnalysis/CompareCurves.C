#include "../CommonTools/CurvesFunctions.C"
#include "../CommonTools/tdrstyle.C"
#include "TCanvas.h"
#include "TLine.h"
#include "TColor.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TText.h"

void CompareCurve(string dirname, string subdet, const int NF, string* runs, ULong64_t* modid, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{

 cout<<"entering CompareCurve(...)"<<endl;
 int err_type=0;
 //int modid=369121606;
 //int modid=369120501;
// Gde stat à petit angle
/*
369120405
369120341
369120357
*/
// au pif
// 369158404
// 369124773


 TF1* signal;
 //if(subdet=="TIB") signal = new TF1("signal", fitSignalDeco300, 25, 360, 2); 
 //else signal = new TF1("signal", fitSignalDeco500, 25, 360, 2);


 TFile* f[NF];
 TCanvas* c[NF];
 TGraphErrors* g[NF];
 TF1* func[NF];
 TLine* line[NF];
 float lastpoints[NF];
 
 int colors[20];
 colors[0] = 2;
 colors[1] = 4;
 colors[2] = 8;
 colors[3] = 6;
 colors[4] = 7;
 colors[5] = kOrange;
 colors[6] = kGray;
 colors[7] = kGray+2;
 for(int icol=8; icol<20; icol++) colors[icol] = 1;

 // black and grey for the 3 runs of 2011
 colors[0] = 1;
 colors[1] = 14;
 colors[2] = 17;
 
 // Rainbow colors for 2012-13
 gStyle->SetPalette(1); // 50 colors
 int ncol=7;
 if(NF<11) ncol=NF-3;
 if(ncol<2) ncol=2;
 for(int i=0; i<ncol; i++)
 { 
   if(i%2==0) colors[i+3] = 	TColor::GetColorPalette(int(50./(ncol-1)*i/2.));
   if(i%2==1) colors[i+3] = 	TColor::GetColorPalette(int(50./(ncol-1)*(ncol-(i+1.5)/2.)));
   if(i%2==0) cout<<i+3<<" "<<50./(ncol-1)<<"*"<<i/2.<<"="<<colors[i+3]<<endl;
   if(i%2==1) cout<<i+3<<" "<<50./(ncol-1)<<"*"<<(ncol-(i+1.5)/2.)<<"="<<colors[i+3]<<endl;
 }
 colors[6]=kOrange+1;
 colors[9]=kGreen+1;

 // For 2015
 colors[10]=kBlue+1;
 colors[11]=kAzure+10;
 colors[12]=kAzure+1;
 
 int ifirst=-1;
  
 // Get TF1
 if(showfit)
 for(int i=0; i<NF; i++)
 {

   double params[3];
   GetFitParams(subdet, runs[i], modid[i], params[0], params[1], params[2]);
   signal->SetParameters(params);
   
   func[i] = (TF1*) signal->Clone();
   if(!func[i]) cout<<"Error : no function in '"<<modid[i]<<"' in "<<runs[i]<<endl;

 }

 // Get Graph
 cout<<"get graph"<<endl;
 double x0, y0, x1, y1, x2, y2;
 float maxend=0;
 for(int i=0; i<NF; i++)
 {
   if(type=="Signal") g[i] = GetGraph(dirname, subdet, runs[i], modid[i], err_type); 
   if(type=="ClusterWidth") g[i] = GetClusterWidthGraph(dirname, subdet, runs[i], modid[i], err_type); 
   if(!g[i]) continue;
   if(ifirst<0) ifirst=i;
   
   // Correct for voltage drop due to leakage current
   string corr_name="_"+subdet+runs[i];
   int temp_modid = modid[i];
   if(subdet=="TOB") temp_modid = modid[i] / 10; // In order to have the same correction for the two sensors of the same module
   int corrected = CorrectGraphForLeakageCurrent(g[i], temp_modid, corr_name);
   if(corrected) g[i]->SetMarkerColor(13);
   g[i]->SetMarkerSize(0.8);

   g[i]->SetLineColor(colors[i]);
   g[i]->SetLineWidth(2);
   int npt = g[i]->GetN();
   if(npt>2)
   {
     g[i]->GetPoint(npt-3, x0, y0);
     g[i]->GetPoint(npt-2, x1, y1);
     g[i]->GetPoint(npt-1, x2, y2);
     lastpoints[i] = (y0+y1+y2)/3.;
	 if(lastpoints[i]>maxend) maxend=lastpoints[i];
     // Curve from 2015 ends at 300V! Do a kind of extrapolation to 330V
     if(runs[i]=="_246963"){
       lastpoints[i] = y2+(y2-y0);
     }
   }   
 }
 
 // Scale graphs
 if(normalize)
 for(int i=0; i<NF; i++)
 {
   if(!g[i]) continue;
   
   float scale=1;
   //For signal scale at 90;
   if(type=="Signal") scale = lastpoints[i]/90;
   //scale at plateau of first curve
   else scale = lastpoints[i]/lastpoints[ifirst];
   //float scale = lastpoints[i]/maxend;
   
   int npt = g[i]->GetN();
   cout<<"Scale "<<i<<" : "<<scale<<endl;
   for(int ipt=0; ipt<npt; ipt++)
   {
     g[i]->GetPoint(ipt, x0, y0);
	 g[i]->SetPoint(ipt, x0, y0/scale);
   }
 }
   
 TCanvas c1("c1", "", 600, 400);//1200,800
 if(!g[ifirst]) return;
 TH1F* h = g[ifirst]->GetHistogram();
 if(type=="Signal") h->SetMinimum(10);
 else h->SetMinimum(1);
 h->SetMaximum(ceil(maxend));
 if(normalize && type=="Signal") h->SetMaximum(100);
 h->GetXaxis()->SetTitle("V_{bias} [V]");
 h->GetXaxis()->SetLimits(0, 380);
 h->Draw();

 int detid;
 if(subdet=="TOB") detid = modid[ifirst] / 10;
 else detid=modid[ifirst];

 //h->SetTitle(Form("Detid %llu - %s L%i",modid[ifirst], subdet.c_str(), GetLayer(detid)));
 TText title(0.35, 0.96, Form("Detid %llu - %s L%i",modid[ifirst], subdet.c_str(), GetLayer(detid)));
 title.SetNDC();
 title.Draw();
 
 float ymin = h->GetMinimum(); //g[ifirst]->GetYaxis()->GetXmin();
 float ymax = g[ifirst]->GetYaxis()->GetXmax();
 float x = 0;
 if(showfit) x = func[ifirst]->GetParameter(0);

 TLegend *leg = new TLegend(0.15, 0.65, 0.35, 0.85);

 for(int i=ifirst; i<NF; i++)
 {
   //if(!g[i] || !func[i]) continue;
   if(!g[i]) continue;
   g[i]->Draw("LP");
   if(showfit) func[i]->SetLineColor(colors[i]);
   if(showfit) func[i]->Draw("same");
   if(showfit) x = func[i]->GetParameter(0);
   cout<<"color "<<colors[i]<<" run "<<runs[i]<<"  id "<<modid[i]<<" \tVdepl "<<x<<" Chi2 "<<endl;
   
   line[i] = new TLine(x, ymin, x, ymax);
   line[i]->SetLineStyle(2);
   line[i]->SetLineColor(colors[i]);
   if(g[i]->GetN() && showfit) line[i]->Draw();
   leg->AddEntry(g[i], Form("%s", runs[i].c_str()), "l");
 } 
 
 //leg->Draw();

 c1.Modified();
 c1.Update();
 if(print) c1.Print(Form("modid_%llu%s.eps", modid[ifirst], suffix.c_str()));
 //getchar();
 //c1.Close();
 
 
 //-------------------------------------------------------
 // Compute and show ratios
 
 TGraphErrors* gratio[NF];
 for(int i=1; i<NF; i++)
 { 
   gratio[i] = new TGraphErrors();
   int npt = g[ifirst]->GetN();
   //cout<<"Graph "<<i<<endl;
   
   if(!g[i]) continue;

   int npt1 = g[i]->GetN();
   double* X1 = g[i]->GetX();
   int IPT1=-1;
   int ipt_ratio=0;
   float ratio = 0;
   
   for(int ipt=0; ipt<npt; ipt++)
   {
     g[ifirst]->GetPoint(ipt, x0, y0);
	 IPT1=-1;
	 for(int ipt1=0; ipt1<npt1; ipt1++)
	   if(X1[ipt1]==x0) { IPT1=ipt1; break; }
 
     if(IPT1<0) continue;

     g[i]->GetPoint(IPT1, x1, y1);
     ratio = 0;
	 if(x0==x1 && y0>0) ratio = y1/y0;
	 gratio[i]->SetPoint(ipt_ratio, x0, ratio);
	 //cout<<" "<<ipt_ratio<<"  "<<x0<<" "<<y0<<" "<<IPT1<<" "<<x1<<" "<<y1<<"  "<<ratio<<endl;
	 ipt_ratio++;
   }
   gratio[i]->SetLineColor(colors[i]);
   gratio[i]->SetMarkerStyle(22);
 }
 
/* TCanvas c2;
 TH1F* h1 = gratio[1]->GetHistogram();
 gratio[1]->Draw("APL");
 gratio[1]->SetMinimum(0.8);
 gratio[1]->SetMaximum(1.2);
 for(int i=2; i<NF; i++)
 { 
   gratio[i]->Draw("PL");
 }
 TLine l1(h1->GetXaxis()->GetXmin(), 1, h1->GetXaxis()->GetXmax(), 1);
 l1.Draw();
 
 c2.Modified();
 c2.Update();
 //if(print) c2.Print(Form("modid_%llu_ratio%s.eps", modid[ifirst], suffix.c_str()));

*/
 getchar();
 c1.Close();
 //c2.Close();
 
}


void CompareCurve(string dirname, string subdet, const int NF, string* runs, ULong64_t modid, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
  ULong64_t modids[NF];
  for(int i=0; i<NF; i++) modids[i] = modid;
  CompareCurve(dirname, subdet, NF, runs, modids, type, normalize, showfit, print, suffix);
}


void CompareTIBCurves(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 const int NL1 = 6;
 int modidsL1[] = {369121606, 369124429, 369124550, 369121365, 369121302, 369170776}; //L1
 const int NL2 = 5;
 int modidsL2[] = {369137157, 369140921, 369141949, 369136758, 369138105}; //L2
 const int NL3 = 4;
 int modidsL3[] = {369154196, 369154296, 369158756, 369157468}; //L3
 const int NL4 = 4;
 int modidsL4[] = {369169980, 369169864, 369170636, 369173944}; //L4
 
 // add mini-scan modules
 
 cout<<" >> TIB << "<<endl;
 for(int i=0; i<NL1; ++i) CompareCurve(dirname, "TIB", NF, runs, modidsL1[i], type, normalize, showfit, print, suffix);
 for(int i=0; i<NL2; i++) CompareCurve(dirname, "TIB", NF, runs, modidsL2[i], type, normalize, showfit, print, suffix);
 for(int i=0; i<NL3; i++) CompareCurve(dirname, "TIB", NF, runs, modidsL3[i], type, normalize, showfit, print, suffix);
 for(int i=0; i<NL4; i++) CompareCurve(dirname, "TIB", NF, runs, modidsL4[i], type, normalize, showfit, print, suffix);
}

void CompareTIBCurves_SmallScan(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 //TIBminus_1_2_2_1
 CompareCurve(dirname, "TIB", NF, runs, 369121381, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121382, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121385, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121386, type, normalize, showfit, print, suffix);
 //TIBminus_1_4_2_5
 CompareCurve(dirname, "TIB", NF, runs, 369121605, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121606, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121609, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121610, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121613, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369121614, type, normalize, showfit, print, suffix);
 //TIBplus_1_6_2_5
 CompareCurve(dirname, "TIB", NF, runs, 369125861, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369125862, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369125866, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369125869, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TIB", NF, runs, 369125870, type, normalize, showfit, print, suffix);
}

void CompareTIDCurves(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 CompareCurve(dirname, "TID", NF, runs, 402674846, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TID", NF, runs, 402673352, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TID", NF, runs, 402673440, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TID", NF, runs, 402664109, type, normalize, showfit, print, suffix);
}

void CompareTOBCurves_SmallScan(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 CompareCurve(dirname, "TOB", NF, runs, 4362815081, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815082, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815121, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815122, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815161, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815162, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815201, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815202, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815241, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815242, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815281, type, normalize, showfit, print, suffix);
 CompareCurve(dirname, "TOB", NF, runs, 4362815282, type, normalize, showfit, print, suffix);
}

void CompareTOBCurves(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 const int NL1 = 6;
 //int modidsL1[] = {436228249, 436232694, 436228805}; //L1
 ULong64_t modidsL1[] = {4362282491, 4362282492, 4362326941, 4362326942, 4362288051, 4362288052}; //L1

 const int NL2 = 6;
 //int modidsL2[] = {436244722, 436245110, 436249546}; //L2
 ULong64_t modidsL2[] = {4362447221, 4362447222, 4362451101, 4362451102, 4362495461, 4362495462}; //L2
 const int NL6 = 6;
 //int modidsL6[] = {436310808, 436312136, 436315600}; //L6
 ULong64_t modidsL6[] = {4363108081, 4363108082, 4363121361, 4363121362, 4363156001, 4363156002}; //L6

 cout<<" >> TOB << "<<endl;
 
 ULong64_t modids[NF];
 
 /*for(int i=0; i<NL1; i++) 
 {
   modids[0] = (ULong64_t) modidsL1[i];
   modids[1] = modids[0]*10+1;
   modids[2] = modids[0]*10+2;
   CompareCurve("TOB", NF, runs, modids, normalize, showfit);
 }
 for(int i=0; i<NL2; i++) 
 {
   modids[0] = (ULong64_t) modidsL2[i];
   modids[1] = modids[0]*10+1;
   modids[2] = modids[0]*10+2;
   CompareCurve("TOB", NF, runs, modids, normalize, showfit);
 }
 for(int i=0; i<NL6; i++) 
 {
   modids[0] = (ULong64_t) modidsL6[i];
   modids[1] = modids[0]*10+1;
   modids[2] = modids[0]*10+2;
   CompareCurve("TOB", NF, runs, modids, normalize, showfit);
 }*/
 
  for(int i=0; i<NL1; i++) CompareCurve(dirname, "TOB", NF, runs, modidsL1[i], type, normalize, showfit, print, suffix);
  for(int i=0; i<NL2; i++) CompareCurve(dirname, "TOB", NF, runs, modidsL2[i], type, normalize , showfit, print, suffix);
  for(int i=0; i<NL6; i++) CompareCurve(dirname, "TOB", NF, runs, modidsL6[i], type, normalize , showfit, print, suffix);

}

void CompareTOBCurves_SensorsSeparation(string dirname, const int NF, string* runs, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="")
{
 const int NL1 = 3;
 int modidsL1[] = {436228249, 436232694, 436228805}; //L1

 const int NL2 = 3;
 int modidsL2[] = {436244722, 436245110, 436249546}; //L2

 const int NL6 = 3;
 int modidsL6[] = {436310808, 436312136, 436315600}; //L6

 const int N = 6;
 int modidsGlobal[] = {436281508, 436281512, 436281516, 436281520, 436281524, 436281528}; //TOB  

 cout<<" >> TOB << "<<endl;

 ULong64_t modids[NF];
/* for(int i=0; i<NL1; i++) 
 {
   modids[2] = (ULong64_t) modidsL1[i];
   modids[0] = modids[2]*10+1;
   modids[1] = modids[2]*10+2;
   CompareCurve("TOB", NF, runs, modids, type, normalize, showfit, print, suffix);
 }
 for(int i=0; i<NL2; i++) 
 {
   modids[2] = (ULong64_t) modidsL2[i];
   modids[0] = modids[2]*10+1;
   modids[1] = modids[2]*10+2;
   CompareCurve("TOB", NF, runs, modids, type, normalize, showfit, print, suffix);
 }
 for(int i=0; i<NL6; i++) 
 {
   modids[2] = (ULong64_t) modidsL6[i];
   modids[0] = modids[2]*10+1;
   modids[1] = modids[2]*10+2;
   CompareCurve("TOB", NF, runs, modids, type, normalize, showfit, print, suffix);
 }*/

 for(int i=0; i<N; i++) 
 {
   modids[2] = (ULong64_t) modidsGlobal[i];
   modids[0] = modids[2]*10+1;
   modids[1] = modids[2]*10+2;
   CompareCurve(dirname, "TOB", NF, runs, modids, type, normalize, showfit, print, suffix);
 }

}

void CompareCurves()
{
 cout<<"entering CompareCurves()"<<endl;
 //gROOT->ProcessLine(".L ~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/signalFunction.cpp");
 //gROOT->ProcessLine(".L ~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/fitSignal.cpp");
 //gSystem->Load("~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/signalFunction.so");

 cout<<"setting styles"<<endl;
 setTDRStyle();

 cout<<"setting options"<<endl;
 // directory of root files
 string dirname = "/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/SignalCurves";
 string type = "Signal"; 
 //string dirname = "/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/ClusterWidthCurves";
 //string type = "ClusterWidth";

 const int NF=14;
 string runs[NF];

 //runs[0] = "_200786_steps";
 //runs[1] = "_200786_ptgt1dot5";
 //runs[2] = "_200786_ptgt2dot5";

/* runs[0] = "_203832_plt0dot5";
 runs[1] = "_203832_p0dot5-1";
 runs[2] = "_203832_p1-1dot5";
 runs[3] = "_203832_p1dot5-3";
 runs[4] = "_203832_pgt3";
*/

/* runs[0] = "_160497_plt0dot5";
 runs[1] = "_160497_p0dot5-1";
 runs[2] = "_160497_p1-1dot5";
*/

/* runs[0] = "_199832_merged_steps";
 runs[1] = "_200786";
 runs[2] = "_203832_merged_steps";
*/

 
 /*runs[0] = "_190459";
 runs[1] = "";//190459_anglecor_wo0-0.15
 runs[2] = "_";
 runs[3] = "_190459_angle0-0.2";
 runs[4] = "_190459_angle0.2-0.4";
 runs[5] = "_190459_angle0.4-0.6";
 runs[6] = "_190459_angle0.6-0.8";*/
 
/* runs[0] = "_160497";
 runs[1] = "";
 runs[2] = "";
 runs[3] = "_190459";
 runs[4] = "";//_193928";
 runs[5] = "";//_199832";
 runs[6] = "";//_200786";
 runs[7] = "_203832";
 runs[8] = "_208339";
 runs[9] = "";//_211797";
 */
 
 //clusThresh
/* runs[0] = "";
 runs[1] = "";
 runs[2] = "";
 runs[3] = "_203832";
 runs[4] = "_203832_clusThresh6";
 runs[5] = "_203832_clusThresh7";
 runs[6] = "_203832_clusThresh8";
 runs[7] = "_203832_clusThresh9";*/


 //stripThresh
/* runs[0] = "_203832";
 runs[1] = "_203832_stripThresh2dot5";
 runs[2] = "_203832_stripThresh3";
 runs[3] = "";
 runs[4] = "";
 runs[5] = "";
 runs[6] = "";
 runs[7] = "";
*/

 //seedThresh
/* runs[0] = "";
 runs[1] = "";
 runs[2] = "";
 runs[3] = "_203832";
 runs[4] = "_203832_seedThresh3dot5";
 runs[5] = "_203832_seedThresh4";
 runs[6] = "_203832_seedThresh4dot5";
 runs[7] = "_203832_seedThresh5";
*/

 //signalEff
/* runs[0] = "_203832";
 runs[1] = "_203832_signalEff0dot95";
 runs[2] = "_203832_signalEff0dot90";
 runs[3] = "";
 runs[4] = "";
 runs[5] = "";
 runs[6] = "";
 runs[7] = "";
*/


/* runs[0] = "_20110315_run160497";
 runs[1] = "";
 runs[2] = "";
 runs[3] = "_20120405_run190459";
 runs[4] = "";//_193928";
 runs[5] = "";//_199832";
 runs[6] = "_20120812_run200786";
 runs[7] = "_20120928_run203832";
 runs[8] = "_20121130_run208339";
 runs[9] = "";//_211797";*/

 // Full scans 
/* runs[0] = "_160497";
 runs[1] = "_170000";
 runs[2] = "";
 runs[3] = "_190459";
 runs[4] = "_193928";
 runs[5] = "";
 runs[6] = "_200786";
 runs[7] = "";
 runs[8] = "";
 runs[9] = "";
 runs[10] = "_246963";
*/

 // All scans
 runs[0] = "_190459";//_160497";
 runs[1] = "_190459";//_170000";
 runs[2] = "_190459";
 runs[3] = "_190459";
 runs[4] = "_193928";
 runs[5] = "_199832";
 runs[6] = "_200786";
 runs[7] = "_203832";
 runs[8] = "_208339";
 runs[9] = "_211797";
 runs[10] = "_246963";
 runs[11] = "_254790";
 runs[12] = "_258443";
 runs[13] = "_271056";

 
 bool normalize=true;
 bool print=false;
 bool showfit=false;
 
 // suffix for plot file name
 //string suffix = "";
 //string suffix = "_2011-12-13";
 //string suffix = "_203832_signalEff";
 string suffix = "_S_258443"; 

 //CompareCurve(dirname, "TIB", NF, runs, 369125869, type, normalize, showfit, print, suffix);
 //CompareTIBCurves(dirname, NF, runs, type, normalize, showfit, print, suffix);

 //CompareCurve(dirname, "TIB", NF, runs, 369121381, type, normalize, showfit, print, suffix);
 CompareTIBCurves_SmallScan(dirname, NF, runs, type, normalize, showfit, print, suffix);
 
 //CompareTOBCurves(dirname, NF, runs, type, normalize, showfit, print, suffix);

 //runs[0] = "_160497_Sensors";
 //runs[1] = "_160497_Sensors";
 //CompareTOBCurves_SensorsSeparation(dirname, NF, runs, type, normalize, showfit, print, suffix);
 
 //CompareTOBCurves_SmallScan(dirname, NF, runs, type, normalize, showfit, print, suffix);

 //CompareTIDCurves(dirname, NF, runs, type, normalize, showfit, print, suffix);

}

int main(int argc, char** argv)
{
  TApplication app("ROOT Application", &argc, argv);
  //app.SetReturnFromRun(true);
  CompareCurves();
  app.Run();
  //app.Terminate();
  return 0;
}
