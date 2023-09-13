#include "../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.h"

#include "TROOT.h"
#include "TCanvas.h"
#include "TLine.h"


void ShowOneCurve(string dirname, string subdet, string run, ULong64_t modid, bool showfit, bool print)
{

 int err_type=0;

 cout<<"run "<<run<<endl; 
 // Get graph
 TGraphErrors* g = GetGraph(dirname, subdet, run, modid, err_type);
 if(!g) {cout<<"No graph for such module : "<<modid<<endl; return;}
 TGraphErrors* gd = GetDerivativeGraph( g );
 TGraphErrors* gd2 = GetDerivativeGraph( gd );
 TGraphErrors* gs = GetSmoothGraph( gd2 );
 if(g->GetN()<3) {cout<<"Only "<<g->GetN()<<" points for module "<<modid<<". Not enought."<<endl; return;}

 // Draw graph
 TCanvas c1("c1","",0,0,700,500);
 TH1F* h = g->GetHistogram();
 h->SetMinimum(0);
 if(subdet=="TIB") h->SetMaximum(100);//100//140//65
 if(subdet=="TOB") h->SetMaximum(170);
 g->SetTitle(Form("Detid %llu",modid));
 g->Draw("ALP");
 
 float ymin = h->GetMinimum(); //g->GetYaxis()->GetXmin();
 float ymax = g->GetYaxis()->GetXmax();
 float x=GetMin(gs);
 
 TLine* line = new TLine(x, ymin, x, ymax);
 line->SetLineStyle(2);
 line->SetLineColor(1);
 line->Draw();
 
 TGraphErrors* gc = (TGraphErrors*) g->Clone();
 string corr_name="_"+subdet+run;
 int corrected = CorrectGraphForLeakageCurrent(gc, modid, corr_name);
 if(corrected)
 {
  gc->SetMarkerColor(13);
  gc->Draw("P");
 }

 //TGraphErrors* gd;
 if(corrected) gd = GetDerivativeGraph( gc );
 else gd = GetDerivativeGraph( g );
 //TGraphErrors* 
 gd2 = GetDerivativeGraph( gd );
 //TGraphErrors* 
 gs = GetSmoothGraph( gd2 );
 
 TGraph* g3pts = new TGraph();
 //float xopt=GetOptimalMin3Pts(gs, g3pts);
 float xopt=GetOptimalMinNPts(gs, g3pts);
   
 TLine* lopt = new TLine(xopt, ymin, xopt, ymax);
 lopt->SetLineStyle(1);
 lopt->SetLineColor(1);
 lopt->Draw();
 
 c1.Modified();
 c1.Update();
 //gc->Print("all");

 TCanvas c2;
 //gd->Draw("AP");
 //line->Draw();
 //c2.Modified();
 //c2.Update();
 
 TCanvas c3;
 g3pts->SetMarkerStyle(20);
 g3pts->Draw("AP");
 g3pts->Fit("pol2");
 TF1* fit = g3pts->GetFunction("pol2");
 cout<<"Xmin : "<<fit->GetMinimumX(30, 300)<<endl;
 //gd2->Draw("AP");
 //line->Draw();
 c3.Modified();
 c3.Update();
 //gd2->Print("all");
 
 
 c2.cd();
 gs->Draw("AP");
 float ymean, yrms;
 GetYMeanRMS(gs, ymean, yrms);
 
 float xmin=20;
 float xmax=360;
 TLine* lcut = new TLine(xmin, ymean-yrms, xmax, ymean-yrms);
 lcut->Draw();
 c2.Modified();
 c2.Update();
 

 if(print) c1.Print(Form("detid_%llu%s.eps", modid, run.c_str()));
 getchar();
 c1.Close();

}

void ShowBestCurve(string dirname, string subdet, string run, ULong64_t modid, bool showfit, bool print)
{

 int err_type=0;

 cout<<"run "<<run<<endl; 
 // Get graph
 TGraphErrors* g = GetGraph(dirname, subdet, run, modid, err_type);
 if(!g) {cout<<"No graph for such module : "<<modid<<endl; return;}
 if(g->GetN()<3) {cout<<"Only "<<g->GetN()<<" points for module "<<modid<<". Not enought."<<endl; return;}

 // Draw graph
 TCanvas c1("c1","",0,0,700,500);
 TH1F* h = g->GetHistogram();
 h->SetMinimum(0);
 if(subdet=="TIB") h->SetMaximum(100);//100//140//65
 if(subdet=="TOB") h->SetMaximum(170);
 g->SetTitle(Form("Detid %llu",modid));
 g->Draw("ALP");
 
 float ymin = h->GetMinimum(); //g->GetYaxis()->GetXmin();
 float ymax = g->GetYaxis()->GetXmax();
 float x=0;//GetMin(gs);
 
 TLine* line = new TLine(x, ymin, x, ymax);
 line->SetLineStyle(2);
 line->SetLineColor(1);
 line->Draw();
 
 TGraphErrors* gc = (TGraphErrors*) g->Clone();
 string corr_name="_"+subdet+run;
 int corrected = CorrectGraphForLeakageCurrent(gc, modid, corr_name);
 if(corrected)
 {
  gc->SetMarkerColor(13);
  gc->Draw("P");
 }

      TGraphErrors* gmedian;
	  if(corrected) gmedian = MedianFilter( gc );
	  else gmedian = MedianFilter( g );
      int nfilt=1;
      while (!IsMonoton(gmedian) && nfilt<4) {
        gmedian = MedianFilter( gmedian );
        nfilt++;
      }
      gmedian = HanningFilter(gmedian);
	  cout<<nfilt<<" median filter applied"<<endl;

      TGraphErrors* gscurv = GetCurvatureGraph( gmedian );
	  //gscurv = GetSmoothGraph_NPtsMean(gscurv, 5);
	  if(subdet=="TOB") gscurv = HanningFilter(gscurv, 5);
      gscurv->SetMarkerStyle(20);
	  TGraph* g3pts = new TGraph();
      float xopt = GetOptimalMinNPts(gscurv, g3pts);

   
 TLine* lopt = new TLine(xopt, ymin, xopt, ymax);
 lopt->SetLineStyle(1);
 lopt->SetLineColor(1);
 lopt->Draw();
 
 c1.Modified();
 c1.Update();
 //gc->Print("all");

 TCanvas c2;
 //gd->Draw("AP");
 //line->Draw();
 //c2.Modified();
 //c2.Update();
 
 TCanvas c3;
 g3pts->SetMarkerStyle(20);
 g3pts->Draw("AP");
 g3pts->Fit("pol2");
 TF1* fit = g3pts->GetFunction("pol2");
 cout<<"Xmin : "<<fit->GetMinimumX(30, 300)<<endl;
 //gd2->Draw("AP");
 //line->Draw();
 c3.Modified();
 c3.Update();
 //gd2->Print("all");
 
 
 c2.cd();
 gscurv->Draw("AP");
 float ymean, yrms;
 GetYMeanRMS(gscurv, ymean, yrms);
 
 float xmin=20;
 float xmax=360;
 TLine* lcut = new TLine(xmin, ymean-yrms, xmax, ymean-yrms);
 lcut->Draw();
 c2.Modified();
 c2.Update();
 

 if(print) c1.Print(Form("detid_%llu%s.eps", modid, run.c_str()));
 getchar();
 c1.Close();

}

void ShowTIBCurves(string dirname, string run, bool showfit=true, bool print=false)
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
 for(int i=0; i<NL1; i++) ShowOneCurve(dirname, "TIB", run, modidsL1[i], showfit, print);
 for(int i=0; i<NL2; i++) ShowOneCurve(dirname, "TIB", run, modidsL2[i], showfit, print);
 for(int i=0; i<NL3; i++) ShowOneCurve(dirname, "TIB", run, modidsL3[i], showfit, print);
 for(int i=0; i<NL4; i++) ShowOneCurve(dirname, "TIB", run, modidsL4[i], showfit, print);

}

void ShowTIBCurves_SmallScan(string dirname, string run, bool showfit=true, bool print=false)
{
  //TIBminus_1_2_2_1
  ShowOneCurve(dirname, "TIB", run, 369121381, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121382, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121385, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121386, showfit, print);
 //TIBminus_1_4_2_5
  ShowOneCurve(dirname, "TIB", run, 369121605, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121606, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121609, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121610, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121613, showfit, print);
  ShowOneCurve(dirname, "TIB", run, 369121614, showfit, print);
 //TIBplus_1_6_2_5
  ShowOneCurve(dirname, "TIB", run, 369125861, showfit, print);
}

void GetKink()
{
 gROOT->ProcessLine(".L CurvesFunctions.C");

 string dirname = "./";
 string subdet = "TIB";

 //string run = "_190459_timestamp_preamp_corrected2"; 
 string run = "_160497_preamp_corrected2_32ns"; 
 bool showfit=false;
 bool print=false;

 cout<<endl<<"Run "<<run<<endl<<endl;

 //if(subdet=="TIB") ShowTIBCurves_SmallScan(run, showfit, print); // TIB
 //if(subdet=="TIB") ShowTIBCurves(run, showfit, print); // TIB
 /*ShowOneCurve("TIB", run, 369154148, showfit, print);
 ShowOneCurve("TIB", run, 369153244, showfit, print);
 ShowOneCurve("TIB", run, 369142205, showfit, print);
 ShowOneCurve("TIB", run, 369141933, showfit, print);
 ShowOneCurve("TIB", run, 369141174, showfit, print);*/
 
 //ShowOneCurve("TIB", "_160497_preamp_corrected2_32ns", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_190459_timestamp", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_180076_newlandau_S_wchi2_wnhits", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_199832_merged_steps", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_200786", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_203832_merged_steps", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_208339_merged_steps", 369125861, showfit, print);
 //ShowOneCurve("TIB", "_211797", 369125861, showfit, print);

 ShowBestCurve(dirname, "TOB", "_190459_timestamp_Sensors", 4362815121, showfit, print);
 ShowBestCurve(dirname, "TOB", "_190459_timestamp_Sensors", 4362815122, showfit, print);
 ShowBestCurve(dirname, "TOB", "_190459_timestamp_Sensors", 4362815281, showfit, print);
 ShowBestCurve(dirname, "TOB", "_190459_timestamp_Sensors", 4362815282, showfit, print);
 
}