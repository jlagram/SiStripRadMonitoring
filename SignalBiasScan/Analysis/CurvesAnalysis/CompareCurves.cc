#include "../CommonTools/CurvesFunctions.h"
#include "Fit.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TColor.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TText.h"
#include <string>
#include <vector>
#include "TROOT.h"
#include "TLatex.h"


//Find a number into a TString, and returns it as a float
TString Change_Data_Format(TString ts)
{
	TString year = "", month = "";

	for (int i=0; i < 6; i++)
	{
		if( i<4 )
		{
			year+= ts[i]; //4 first chars
		}
		else
		{
			month+= ts[i]; //2 next chars
		}
	}

	if(month=="01") {month="January";}
	else if(month=="02") {month="February";}
	else if(month=="03") {month="March";}
	else if(month=="04") {month="April";}
	else if(month=="05") {month="May";}
	else if(month=="06") {month="June";}
	else if(month=="07") {month="July";}
	else if(month=="08") {month="August";}
	else if(month=="09") {month="September";}
	else if(month=="10") {month="October";}
	else if(month=="11") {month="November";}
	else if(month=="12") {month="December";}


	TString result = month + " " + year;

	return result;
}


//-----------------


void CompareCurve(string dirname, string subdet, const int NF, vector<string> dates, vector<string> runs, vector<float> lumis, ULong64_t modid, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="", bool draw=true, bool draw_vdep = false)
{
 //cout<<"entering CompareCurve(...)"<<endl;

 if(!draw) gROOT->SetBatch(kTRUE);

 int err_type=0;

 TF1* signal;
 //if(subdet=="TIB") signal = new TF1("signal", fitSignalDeco300, 25, 360, 2);
 //else signal = new TF1("signal", fitSignalDeco500, 25, 360, 2);


 TFile* f[NF];
 TGraphErrors* g[NF];
 TGraphErrors* g2[NF];
 TF1* func[NF];
 TLine* line[NF];
 float lastpoints[NF];

 for(int i=0; i<NF; i++) //Initialize (easier debugging)
 {
 	f[i]=0;
 	g[i]=0;
	g2[i]=0;
 	func[i]=0;
 	line[i]=0;
 	lastpoints[i]=0;
 }

 vector<int> colors; int nof_aborted_iterations=0; //So that if scan not draw --> color remains free
 colors.push_back(kBlack);
 colors.push_back(kBlue);
 colors.push_back(TColor::GetColorDark(kGreen));
 colors.push_back(kRed);
 colors.push_back(kViolet);
 colors.push_back(kOrange);
 colors.push_back(kGray+2);
 colors.push_back(kMagenta);
 colors.push_back(kAzure+1);
 colors.push_back(kBlue+3);
 colors.push_back(kRed+1);


 int ifirst=-1;

 // Get TF1
 if(showfit)
 for(int i=0; i<NF; i++)
 {
   double params[3];
   GetFitParams(subdet, runs[i], modid, params[0], params[1], params[2]);
   signal->SetParameters(params);

   func[i] = (TF1*) signal->Clone();
   if(!func[i]) cout<<"Error : no function in '"<<modid<<"' in "<<runs[i]<<endl;
 }

 // Get Graph
 cout<<"get graph"<<endl;
 double x0, y0, x1, y1, x2, y2;
 //float maxend=0;
 double ymin = 1000, ymax = -1;

 
 bool compare_Ileak = true;


 for(int i=0; i<NF; i++)
 {
   if(type=="Signal") {g[i] = GetGraph(dirname, subdet, runs[i], modid, err_type);g2[i] = GetGraph(dirname, subdet, runs[i], modid, err_type);}
   if(type=="ClusterWidth") {g[i] = GetClusterWidthGraph(dirname, subdet, runs[i], modid, err_type);g2[i] = GetClusterWidthGraph(dirname, subdet, runs[i], modid, err_type);}

   if(ifirst<0) ifirst=i;

   if(!g[i]) {nof_aborted_iterations++; continue;}

   //cout<<__LINE__<<endl;

   // Correct for voltage drop due to leakage current
   string corr_name="_" + subdet + "_" + dates[i] + "_run" + runs[i];
   int temp_modid = modid;
   if(subdet=="TOB" || subdet=="TEC") {temp_modid = modid / 10;} // In order to have the same correction for the two sensors of the same module


	bool apply_Ileak_corr = true; //if true, will apply Ileak corrections
	 // Better when looking at only one run since it comapres the same run with and witout leakage current corrections
   int corrected = -1;

   if(apply_Ileak_corr)
   {
   	corrected = CorrectGraphForLeakageCurrent(g[i], temp_modid, corr_name);
   }
   else
   {
   	corrected = 0;
   }


   if(corrected) g[i]->SetMarkerColor(13); //Gray markers if corrected
   if(corrected) g2[i]->SetMarkerColor(1); //Gray markers if corrected
   else cout<<"(run "<<runs[i]<<")"<<endl;

   g[i]->SetMarkerSize(0.8);
   g2[i]->SetMarkerSize(0.8);

   if(i>=colors.size()) {cout<<"Error : need more colors !"<<endl;}
   if(compare_Ileak){g[i]->SetLineColor(kBlue);}
   else {g[i]->SetLineColor(colors[i-nof_aborted_iterations]);}
   //colors[i-nof_aborted_iterations]
   g2[i]->SetLineColor(kRed);
   g[i]->SetLineWidth(2);
   g2[i]->SetLineWidth(2);
   int npt = g[i]->GetN();
   if(npt>2)
   {
     g[i]->GetPoint(npt-3, x0, y0);
     g[i]->GetPoint(npt-2, x1, y1);
     g[i]->GetPoint(npt-1, x2, y2);
	 g2[i]->GetPoint(npt-3, x0, y0);
     g2[i]->GetPoint(npt-2, x1, y1);
     g2[i]->GetPoint(npt-1, x2, y2);
     lastpoints[i] = (y0+y1+y2)/3.;

	 for(int k=0; k<g[i]->GetN(); k++)
	 {
	 	g[i]->GetPoint(k, x0, y0);
		g2[i]->GetPoint(k, x0, y0);
	 	{
	 		if(y0 > ymax) ymax = y0;
	 		else if(y0 < ymin) ymin = y0;
	 	}
		
	 }

	 //if(lastpoints[i]>maxend) maxend=lastpoints[i]; //Find maximum y-value

     // Curve from 2015 ends at 300V! Do a kind of extrapolation to 330V
     //if(runs[i]=="_246963"){lastpoints[i] = y2+(y2-y0);}
   }
 }


 // Scale graphs
 if(normalize)
 {
	 ymin = 1000; ymax = -1; //if normalize, need to recompute min & max

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
		 g2[i]->GetPoint(ipt, x0, y0);
		 g2[i]->SetPoint(ipt, x0, y0/scale);
		 if(y0/scale > ymax) {ymax = y0/scale;}
		 if(y0/scale < ymin) {ymin = y0/scale;}
	   }
	 }
 }


 TCanvas* c1 = new TCanvas("c1","c1", 1000, 800);
 c1->SetTopMargin(0.10); //NEW
 c1->SetBottomMargin(0.1); //NEW

 if(!g[ifirst]) return;
 TH1F* h = g[ifirst]->GetHistogram();
 //if(type=="Signal") h->SetMinimum(10);
 //else h->SetMinimum(1);
 //h->SetMaximum(ceil(maxend));

 if(type=="Signal")
 {
   h->SetMaximum(ymax+10);
   h->SetMinimum(ymin-10);
 }
 else
 {
   h->SetMaximum(ymax+0.2);
   h->SetMinimum(ymin-0.2);
 }


  int detid;
 if(subdet=="TOB" || subdet == "TEC") detid = modid / 10;
 else detid=modid;

 TString hname = "Detid " + Convert_Number_To_TString(modid) + " - "+subdet+" L"+Convert_Number_To_TString((ULong64_t) GetLayer(detid));
 if(subdet=="TEC") hname = "Detid " + Convert_Number_To_TString(modid) + " - "+subdet+" R"+Convert_Number_To_TString((ULong64_t) GetLayer(detid));
 //h->SetTitle(hname.Data() ); //NEW
 h->SetTitle("");

 if(normalize && type=="Signal") h->SetMaximum(100);
 //h->GetXaxis()->SetTitle("V_{Bias} [V]");
 h->GetXaxis()->SetTitle("Bias voltage [V]");
 h->GetXaxis()->SetTitleSize(.05);
 h->GetXaxis()->SetLimits(0, 380);
 //h->GetXaxis()->SetTitleOffset(1.18);
 //h->GetXaxis()->SetLabelSize(0.04);

 // h->GetYaxis()->SetTitle("[Arbitrary units]");
 if(type == "Signal") h->GetYaxis()->SetTitle("Cluster charge [a.u.]");
 else if(type == "ClusterWidth") h->GetYaxis()->SetTitle("Cluster width [a.u.]");

 h->GetYaxis()->SetTitleSize(.05);
 h->GetYaxis()->SetTitleOffset(0.95);
 //h->GetYaxis()->SetLabelSize(0.04);

 //if(type=="Signal") h->GetYaxis()->SetTitle("[ADC Counts]");
 //else h->GetYaxis()->SetTitle("Cluster Width");


 //h->SetTitle(Form("Detid %llu - %s L%i",modid, subdet.c_str(), GetLayer(detid)));

 h->Draw();


 //TText title(0.35, 0.96, Form("Detid %i - %s L%i",detid, subdet.c_str(), GetLayer(detid)));
 //title.SetNDC();
 //title.Draw();

 //float ymin = h->GetMinimum(); //g[ifirst]->GetYaxis()->GetXmin();
 //float ymax = g[ifirst]->GetYaxis()->GetXmax();
 float x = 0; int color_vdep_line=1; TString run_vdep = "";
 if(showfit) x = func[ifirst]->GetParameter(0);


//  TLegend *leg = new TLegend(0.6, 0.6, 0.95, 0.8);//en haut à droite => signal
 TLegend *leg = new TLegend(0.6, 0.15, 0.95, 0.55);//en bas à droite => CW
//  TLegend *leg = new TLegend(0.45, 0.15, 0.92, 0.55);//en bas à droite => CW
 //leg->SetBorderSize(0.1);
 leg->SetTextFont(42);
 leg->SetHeader("Bias voltage scans");

 for(int i=ifirst; i<NF; i++)
 {
   //if(!g[i] || !func[i]) continue;
   if(!g[i]) continue;
   g[i]->SetLineWidth(3);
   g[i]->SetMarkerSize(1.4);
   g2[i]->SetLineWidth(3);
   g2[i]->SetMarkerSize(1.4);
   g[i]->Draw("LP");
   if(compare_Ileak) {g2[i]->Draw("SAME");}

   if(showfit) func[i]->SetLineColor(colors[i]);
   if(showfit) func[i]->Draw("same");
   if(showfit) x = func[i]->GetParameter(0);

   line[i] = new TLine(x, ymin, x, ymax);
   line[i]->SetLineStyle(2);
   line[i]->SetLineColor(colors[i-nof_aborted_iterations]);
   if(g[i]->GetN() && showfit) line[i]->Draw();
   if(g2[i]->GetN() && showfit && compare_Ileak) line[i]->Draw();
   color_vdep_line = colors[i-nof_aborted_iterations]; run_vdep = runs[i];

   TString legend = Change_Data_Format(dates[i]) + ", " + Convert_Number_To_TString(lumis[i]) + " fb^{-1}  ";
   leg->AddEntry(g[i], legend, "l");
   if (compare_Ileak){leg->AddEntry(g2[i], "Without Leak. curr. corr.", "l");}
 }

 leg->Draw();
//-------------------------------


//-- Add line representing Vfd value for one run
// NB : draw line up to ymax --> more readable if the vdep is rpz for "highest" curve !

  int olayer_detid = 0; //save layer info for display
  if(draw_vdep)
  {
	  TString input_name = "DECO_files/DECO_"+type+"_"+subdet+"_line_"+run_vdep+".root";
	  //TString input_name = "DECO_files/DECO_"+type+"_"+subdet+"_kink_"+run_vdep+".root";

	  std::cout << "Opening input file " <<input_name<< std::endl;

	  if(!Check_File_Existence(input_name) ) {cout<<FRED("File "<<input_name<<" not found !")<<endl;}
	  else
	  {
	  TFile* myFile = TFile::Open(input_name.Data());
	  TTree* tr = (TTree*)myFile->FindObjectAny("tout");
	  std::cout << "tree opened" << std::endl;

	  ULong64_t odetid=-99;
	  int olayer=-99;
	  double odepvolt=-99;

	  tr->SetBranchAddress("DETID",&odetid);
	  tr->SetBranchAddress("LAYER",&olayer);
	  tr->SetBranchAddress("DEPVOLT",&odepvolt);

	  int nentries = tr->GetEntries();

	  double vdep = -99;

	  for(int ientry=0; ientry<nentries; ientry++)
	  {
	  	tr->GetEntry(ientry);

	  	if( ((subdet=="TOB" || subdet == "TEC") && odetid/10 == detid) || odetid == detid)
	  	{
	  		vdep = odepvolt;
			olayer_detid = olayer;

			// cout<<"modid "<<detid<<endl;
			// cout<<"odepvolt "<<odepvolt<<endl;
			// cout<<"olayer "<<olayer<<endl;
	  	}
	  }

	 if(vdep != 0)
	 {
	 	TLine *l_vdep = new TLine(vdep, h->GetMinimum(), vdep, ymax*1.01);
		l_vdep->SetLineStyle(2);
		l_vdep->SetLineColor(color_vdep_line); //Same color as curve
		l_vdep->SetLineWidth(3);
		l_vdep->Draw();
	 }

	 }
  }

//----------------
	// CAPTIONS //
//----------------

// -- using https://twiki.cern.ch/twiki/pub/CMS/Internal/FigGuidelines

	// TString cmsText     = "CMS";
	TString cmsText     = "";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c1->GetLeftMargin(),0.93,cmsText);

	bool writeExtraText = false;
	// TString extraText   = "Preliminary 2017";
	TString extraText   = "";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c1->GetLeftMargin() + 0.1, 0.932, extraText);

    // cout<<"subdet = "<<subdet<<" / Convert_Number_To_TString((ULong64_t) GetLayer(modid)) = "<<Convert_Number_To_TString((ULong64_t) GetLayer(modid))<<" / detid = "<<modid<<endl;

	TString subdet_text = subdet + " Layer " + Convert_Number_To_TString((ULong64_t) GetLayer(modid));
	if(subdet == "TEC") subdet_text = subdet + " Ring " + Convert_Number_To_TString(olayer_detid+1);
	TLatex subdetinfo;
	subdetinfo.SetNDC();
	subdetinfo.SetTextSize(0.045);
	subdetinfo.SetTextFont(42);
	subdetinfo.DrawLatex(0.80,0.93, subdet_text);

    TString detid_text   = "Detid " + Convert_Number_To_TString(detid);
	latex.SetTextFont(42);
	latex.SetTextSize(0.03);
	latex.DrawLatex(0.78, 0.85, detid_text);



//-------------------------------
 c1->Modified();
 c1->Update();

 Create_Plot_Directories();


 TString name = "plots/";
 if(type=="Signal") name+= "signal/";
 else name+= "CW/";

 TString ext = ".png";
 // TString ext = ".pdf";

 name+= "compareCurve/368669_compareCurves_2023_" + subdet + "_detid" + Convert_Number_To_TString((ULong64_t) modid) + "_" + type + ext;
 if(print) {c1->SaveAs(name.Data());}
 //getchar();
 //c1->Close();


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
   gratio[i]->SetLineColor(colors[i-nof_aborted_iterations]);
   gratio[i]->SetMarkerStyle(22);
 }


 c1->Close();

  return;
}



//-------------------------------
//-------------------------------
void CompareTIBCurves(string dirname, const int NF, vector<string> dates, vector<string> runs, vector<float> lumis, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="", bool draw_plots=true, bool draw_vdep=false)
{
 string subdet = "TIB";

 //TIBminus_1_2_2_1
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121381, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, subdet, NF, dates, runs, lumis,  369121382, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121385, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121386, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

 //TIBminus_1_4_2_5
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121605, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121606, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121609, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121610, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121613, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369121614, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //removed

 //TIBplus_1_6_2_5
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125861, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //segfault
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125869, type, normalize, showfit, print, suffix, draw_plots, draw_vdep); //segfault
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125870, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);


	//--Included in full scans
	
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124666, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124662, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

//-------------to decomment when casual analysis -- 

 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125862, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125866, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124670, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124669, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124665, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124661, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124654, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124650, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124646, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124422, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124653, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124649, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124645, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124421, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124758, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141941, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141942, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141945, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141946, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141949, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369141950, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

	//-------------------------------------Ivan request 2-----------------------------
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124397, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124414, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124422, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124662, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124666, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124694, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124710, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124726, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369124774, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
	// CompareCurve(dirname, subdet, NF, dates, runs, lumis, 369125870, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);


}

//-------------------------------
void CompareTOBCurves(string dirname, const int NF, vector<string> dates, vector<string> runs, vector<float> lumis, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="", bool draw_plots=false, bool draw_vdep=false, TString run_vdep="")
{
//NB : "1 & 2" at the end of detif --> sensor number

 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815081, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815082, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815121, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815122, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815161, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815162, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815201, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815202, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815241, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815242, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815281, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362815282, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);

 // TOB + 1.3.1.6 //NEW -- ADDED 04/2018
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329011, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329021, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 /*
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329051, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329061, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329091, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329101, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329131, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329141, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329171, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329181, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329211, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329221, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329012, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329022, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329052, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329062, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329092, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329102, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329132, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329142, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329172, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329182, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329212, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 CompareCurve(dirname, "TOB", NF, dates, runs, lumis, 4362329222, type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
 */

 return;
}


//-------------------------------
void CompareTECCurves(string dirname, const int NF, vector<string> dates, vector<string> runs, vector<float> lumis, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="", bool draw_plots=false, bool draw_vdep=false, TString run_vdep="")
{
//NB : "1 & 2" at the end of detif --> sensor number

	vector<ULong64_t> v_modids;


//--- FULL LIST
	//1 sensor (+0)
	v_modids.push_back(4701481960);
	v_modids.push_back(4701482280);
	v_modids.push_back(4701482400);
	v_modids.push_back(4701482000);
	v_modids.push_back(4701482320);
	v_modids.push_back(4701482040);
	v_modids.push_back(4701482360);
	//2 sensors (+1/2)
	v_modids.push_back(4701482651);
	v_modids.push_back(4701482961);
	v_modids.push_back(4701483241);
	v_modids.push_back(4701483361);
	v_modids.push_back(4701482611);
	v_modids.push_back(4701482661);
	v_modids.push_back(4701483001);
	v_modids.push_back(4701483281);
	v_modids.push_back(4701483401);
	v_modids.push_back(4701482621);
	v_modids.push_back(4701482921);
	v_modids.push_back(4701483321);
	//...
	v_modids.push_back(4701482652);
	v_modids.push_back(4701482962);
	v_modids.push_back(4701483242);
	v_modids.push_back(4701483362);
	v_modids.push_back(4701482612);
	v_modids.push_back(4701482662);
	v_modids.push_back(4701483002);
	v_modids.push_back(4701483282);
	v_modids.push_back(4701483402);
	v_modids.push_back(4701482622);
	v_modids.push_back(4701482922);
	v_modids.push_back(4701483322);

  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	CompareCurve(dirname, "TEC", NF, dates, runs, lumis, v_modids[i_modid], type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
  }

}



//-------------------------------
void CompareTIDCurves(string dirname, const int NF, vector<string> dates, vector<string> runs, vector<float> lumis, string type, bool normalize=false, bool showfit=true, bool print=false, string suffix="", bool draw_plots=false, bool draw_vdep=false, TString run_vdep="")
{
//NB : "1 & 2" at the end of detif --> sensor number

	vector<ULong64_t> v_modids;

//--- FULL LIST
	//1 sensor (+0)

	// v_modids.push_back(402666133);
	// v_modids.push_back(402666129);
	// v_modids.push_back(402666125);
	// v_modids.push_back(402666134);
	// v_modids.push_back(402666130);
	// v_modids.push_back(402666126);

	//------TID minus 
	// TIDminus_4_2_1_1/
	v_modids.push_back(402666258);//channel002 // done
	v_modids.push_back(402666262);//channel002
	v_modids.push_back(402666266);//channel002
	// TIDminus_4_2_1_1/
	v_modids.push_back(402666257);//channel003 //done
	v_modids.push_back(402666261);//channel003
	v_modids.push_back(402666265);//channel003
	// TIDminus_4_2_1_2
	v_modids.push_back(402666269);//channel003 // can be done
	v_modids.push_back(402666273);//channel003
	v_modids.push_back(402666277);//channel003
	// TIDminus_4_2_1_2
	v_modids.push_back(402666270);//channel002 //can be done
	v_modids.push_back(402666274);//channel002
	v_modids.push_back(402666278);//channel002
	// TIDminus_4_2_1_4
	v_modids.push_back(402666137);//channel003 // can be done
	v_modids.push_back(402666141);//channel003
	v_modids.push_back(402666145);//channel003
	// TIDminus_4_2_1_4
	v_modids.push_back(402666138);//channel002 // can be done
	v_modids.push_back(402666142);//channel002
	v_modids.push_back(402666146);//channel002

	// TIDminus_4_2_2_1
	v_modids.push_back(402666777);//channel003 // can be done
	v_modids.push_back(402666781);//channel003
	v_modids.push_back(402666785);//channel003
	// TIDminus_4_2_2_1
	v_modids.push_back(402666778);//channel002 // can be done
	v_modids.push_back(402666782);//channel002
	v_modids.push_back(402666786);//channel002

  for(int i_modid = 0; i_modid < v_modids.size(); i_modid++)
  {
  	CompareCurve(dirname, "TID", NF, dates, runs, lumis, v_modids[i_modid], type, normalize, showfit, print, suffix, draw_plots, draw_vdep);
  }

}




//-------------------------------
//-------------------------------
int main()
{
	//cout<<"entering CompareCurves()"<<endl;
	//setTDRStyle(); //needed?
	Modified_tdr_style();



	vector<string> v_analysis;
	v_analysis.push_back("Signal");
	// v_analysis.push_back("ClusterWidth");

	vector<string> v_subdet;
	v_subdet.push_back("TIB");
	v_subdet.push_back("TOB");
	v_subdet.push_back("TEC");
	v_subdet.push_back("TID");


	vector<string> runs, dates; vector<float> lumis;  //NB : Lumi Run I = 29.46 fb-1
	//--- RUN 2
	//2015 (4)

	// runs.push_back("246963");	dates.push_back("20150603");	lumis.push_back(0.001+29.46);
	// //runs.push_back("254790");	dates.push_back("20150821");  	lumis.push_back(0.17+29.46);
	// //runs.push_back("258443");	dates.push_back("20151007");	lumis.push_back(2.09+29.46);
	// runs.push_back("262254");	dates.push_back("20151121");  	lumis.push_back(4.23+29.46);

	// //2016 (6)
	// runs.push_back("271056");	dates.push_back("20160423");	lumis.push_back(4.26+29.46);
	// //runs.push_back("274969");	dates.push_back("20160612");	lumis.push_back(7.58+29.46);
	// runs.push_back("276437");	dates.push_back("20160706");	lumis.push_back(14.48+29.46);
	// //runs.push_back("278167");	dates.push_back("20160803");	lumis.push_back(23.64+29.46);
	// runs.push_back("280385");	dates.push_back("20160909");	lumis.push_back(35.06+29.46);
	//-- runs.push_back("285371");	dates.push_back("20161116"); 		lumis.push_back(45.70+29.46); //P-pb collisions ; not shown in final results (~ outlier)
	//2017 (5)
	// runs.push_back("295324");	dates.push_back("20170527"); 	lumis.push_back(45.71+29.46); //Full
	//runs.push_back("295376");	dates.push_back("20170527"); lumis.push_back(45.71+29.46); //Full
	// runs.push_back("298996");	dates.push_back("20170714");	lumis.push_back(52.18+29.46);
	// runs.push_back("302131");	dates.push_back("20170831");	lumis.push_back(65.84+29.46);
	// runs.push_back("303824");	dates.push_back("20170924");	lumis.push_back(70.55+29.46); //Full
	// runs.push_back("305862");	dates.push_back("20171030");	lumis.push_back(91.65+29.46);

	//2018
	// runs.push_back("314574");	dates.push_back("20180418");	lumis.push_back(97.37+29.46); //Full -20°
	// runs.push_back("314755");	dates.push_back("20180420");	lumis.push_back(97.37+29.46); //Full -10°
	// runs.push_back("317182");	dates.push_back("20180530");	lumis.push_back(113.01+29.46);
    // runs.push_back("317683");	dates.push_back("20180611");	lumis.push_back(119.21+29.46);
    // runs.push_back("320674");	dates.push_back("20180801");	lumis.push_back(127.08+29.46);
    // runs.push_back("323374");	dates.push_back("20180923");	lumis.push_back(152.45+29.46); //Full
    // runs.push_back("324841");	dates.push_back("20181018");	lumis.push_back(161.40+29.46);//---------------------------
    // runs.push_back("326883");	dates.push_back("20181118");	lumis.push_back(xxx+29.46); //HI

    //---------------------------

		//Lumi  RUN II  = 165.22 fb-1: 
	//2021

	// runs.push_back("346395");   dates.push_back("20211029"); lumis.push_back(0.00000213+194.68); //----------------------------
	//2022
	//  runs.push_back("353060");	dates.push_back("20220605");  lumis.push_back(0.00000534+194.68); // //-- FULL
	//  runs.push_back("359691");	dates.push_back("20221001"); lumis.push_back(11.59+194.68); //----------------------
	// runs.push_back("362696");	dates.push_back("20221126"); lumis.push_back(40.35+194.68); //----------------------
	// runs.push_back("365843");	dates.push_back("20230407"); lumis.push_back(41.42+194.68); //----------FULL
	// runs.push_back("368669");	dates.push_back("20230609"); lumis.push_back(58.16+194.68); 
	runs.push_back("373060");	dates.push_back("20230907"); lumis.push_back(72.69+194.68);

	int NF = runs.size();

	bool normalize=true;
	bool print=true;
	bool showfit=false;
	bool draw_plots = true;
	bool draw_vdep = false; //Draw vertical line which rpz the Vfd value obtained with lines method for "run_vdep"

	// suffix for plot file name
	string suffix = "";


  for(int i=0; i<v_analysis.size(); i++)
  {
    //  directory of root files
    string dirname = "../"+v_analysis[i]+"Analysis/Code";

 	for(int j=0; j<v_subdet.size(); j++)
 	{
 		if(v_subdet[j]=="TIB") CompareTIBCurves(dirname, NF, dates, runs, lumis, v_analysis[i], normalize, showfit, print, suffix, draw_plots, draw_vdep);
    	if(v_subdet[j]=="TOB") CompareTOBCurves(dirname, NF, dates, runs, lumis, v_analysis[i], normalize, showfit, print, suffix, draw_plots, draw_vdep);
		if(v_subdet[j]=="TEC") CompareTECCurves(dirname, NF, dates, runs, lumis, v_analysis[i], normalize, showfit, print, suffix, draw_plots, draw_vdep);
		if(v_subdet[j]=="TID") CompareTIDCurves(dirname, NF, dates, runs, lumis, v_analysis[i], normalize, showfit, print, suffix, draw_plots, draw_vdep);
 	}

  }
}
