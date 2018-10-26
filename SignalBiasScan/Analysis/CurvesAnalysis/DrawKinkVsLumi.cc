#include "../CommonTools/CurvesFunctions.h"
#include "../../../LeakageCurrentBiasScan/Analysis/GetKink.C"
#include "../CommonTools/VdeplRef.h"
#include "../CommonTools/ModFluence.h"
#include "Fit.h"

#include <sstream>
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMarker.h"
#include "TLine.h"
#include "TString.h"
#include <vector>
#include "TGaxis.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TLatex.h"
#include "THStack.h"


//--------------------------------------------
// ##     ## ######## ##       ########  ######## ########
// ##     ## ##       ##       ##     ## ##       ##     ##
// ##     ## ##       ##       ##     ## ##       ##     ##
// ######### ######   ##       ########  ######   ########
// ##     ## ##       ##       ##        ##       ##   ##
// ##     ## ##       ##       ##        ##       ##    ##
// ##     ## ######## ######## ##        ######## ##     ##
//--------------------------------------------

// Correct for the difference of total inelastic crosfs section when center of mass energy change (because we use a single fluence file for Run1 lumi)
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
	double fluence = -1;

	if(subdet == "TOB" || subdet == "TEC") {detid/= 10;}

	// Fluence files
	ModFluence DetFluenceRun1;
	DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
	ModFluence DetFluenceRun2;
	DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");


	double fluenceRun1 = DetFluenceRun1.GetFluence(detid);
	double fluenceRun2 = DetFluenceRun2.GetFluence(detid);


	if(lumi <= lumi_Run1)
	{
		fluence = CorrectForCME(lumi) * fluenceRun1;
	}
	else
	{
		fluence = CorrectForCME(lumi_Run1) * fluenceRun1 + lumi_Run2 * fluenceRun2;
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
	double fluence_per_lumi_7tev = DetFluenceRun1.GetFluence(detid);
	double fluence_per_lumi_13tev = DetFluenceRun2.GetFluence(detid);


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


bool Is_Scan_Bad(TString subdet, TString run, TString observable, ULong64_t detid)
{
    bool isBad = false;

	if(subdet == "TOB" || subdet=="TEC") //low stat, ...
	{
		if(run=="170000" || run=="178367" || run=="180076" || run=="193541" || run=="193928" || run=="254790" || run=="258443" || run=="262254" || run=="178367" || run=="180076" || (run == "208339" && subdet == "TEC") ) {isBad = true;} //Scans w/ low stat or else --> remove for TOB
	}
	else if(subdet=="TIB" && run=="302131" && observable=="Signal" && detid==369125862) {isBad=true;} //Line algo failing for this detector

	return isBad;
}


void CheckFits(string dirname, string subdet, const int NF, vector<string> runs, ULong64_t detid)
{
  for(int i=0; i<NF; i++)
  {
    ShowBestCurve(dirname, subdet, runs[i], detid, true, false);
  }
}











//--------------------------------------------
// ######## ##     ##  #######  ##                 ##      ##     ##  #######  ########  ##     ## ##       ########
// ##       ##     ## ##     ## ##               ####      ###   ### ##     ## ##     ## ##     ## ##       ##
// ##       ##     ## ##     ## ##                 ##      #### #### ##     ## ##     ## ##     ## ##       ##
// ######   ##     ## ##     ## ##                 ##      ## ### ## ##     ## ##     ## ##     ## ##       ######
// ##        ##   ##  ##     ## ##                 ##      ##     ## ##     ## ##     ## ##     ## ##       ##
// ##         ## ##   ##     ## ##       ###       ##      ##     ## ##     ## ##     ## ##     ## ##       ##
// ########    ###     #######  ######## ###     ######    ##     ##  #######  ########   #######  ######## ########
//--------------------------------------------
void DrawOneModule(string dirname, string subdet, string antype, string ref, const int NF, vector<string> runs, vector<float> lumis, ULong64_t detid, bool useflu=false, bool print=false, bool use_curvature=true, bool superimpose_simu=false, bool draw_vdep_lab=true, bool draw_fit=false)
{
	bool draw_vs_fluence = false; //If true, main axis is fluence ; else, main axis is lumi

  if(subdet!="TIB" && subdet!="TOB" && subdet!="TID" && subdet!="TEC")
  {cout<<__LINE__<<" : Subdet '"<<subdet<<"' not allowed."<<endl; return;}

  string filename = "DECO_"+antype+"_"+subdet;
  if(use_curvature) {filename+="_kink";}
  else {filename+="_line";}

  gStyle->SetOptStat(0);

/*
  // Fluence
  ModFluence DetFluenceRun1;
  DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
  ModFluence DetFluenceRun2;
  DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");
*/

  ULong64_t odetid;
  int olayer;
  double odepvolt;
  double oerrdepvolt;
  double oplateau;
  double ofitchisquare;
  int ofitstatus;
  double olastpty;
  double ochi2;

  double lumi_7TeV=6.12;
  double lumi_8TeV=29.46 - 6.12;
  double lumi_Run1=29.46;


  TGraphErrors *g = new TGraphErrors();
  int ipt=0;

  cout<<endl<<"------------------"<<endl;

  for(int i=0; i<NF; i++)
  {
  	// cout<<"run "<<runs[i]<<endl;

  	//if(antype=="ClusterWidth" && runs[i]=="258443") {continue;}

  	string inputfile_name = dirname+"/"+filename+"_"+runs[i]+".root";

    if(!Check_File_Existence(inputfile_name)) {cout<<FRED("No data file : ")<<inputfile_name<<endl; continue;}

    TFile *f = TFile::Open(inputfile_name.c_str());
    // cout<<"File "<<inputfile_name<<" opened"<<endl;

	if(!f) { cout<<"No file : "<<(dirname+"/"+filename+"_"+runs[i]+".root")<<endl; continue;}
	TTree* tr = (TTree*) f->FindObjectAny("tout");

    tr->SetBranchAddress("DETID",&odetid);
    tr->SetBranchAddress("LAYER",&olayer);
    tr->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
    tr->SetBranchAddress("DEPVOLT",&odepvolt);
    tr->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
    tr->SetBranchAddress("FITCHI2",&ofitchisquare);
    tr->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
    tr->SetBranchAddress("LASTPOINTS",&olastpty);
    tr->SetBranchAddress("CHI2",&ochi2); // significance min

    double lumi=0;

	//if(useflu) {cout<<lumis[i]<<" --> ";}

	UInt_t nentries = tr->GetEntries();

    for(UInt_t ie = 0; ie <nentries; ie++)
    {
      tr->GetEntry(ie);



//----------------
//CAN REMOVE DETIDS/RUNS/TYPE COMBINATIONS HERE (if bad curve --> not to appear on plot)

    bool remove_badscans = true;

    if(remove_badscans)
    {
		// cout<<"run "<<runs[i];

    	if( Is_Scan_Bad(subdet, runs[i], antype, odetid) == true)
		{
			continue;
		}
    }

//----------------

	// cout<<"odetid = "<<odetid<<endl;
	// if(odetid == 4362329091) {cout<<"!! odetid = "<<odetid<<endl;}
	// if(odetid == 4362329091) {cout<<"!!! odepvolt = "<<odepvolt<<endl;}

	  if(odetid==detid && odepvolt>=0) //interested only in 1 module
	  {
		lumi = lumis[i];

		//cout<<" --> Write point!";

		g->SetPoint(ipt, lumi, odepvolt);
		g->SetPointError(ipt, 0, oerrdepvolt);

		// cout<<"lumi = "<<lumi<<endl;
		// cout<<"run "<<runs[i]<<endl;


		ipt++;

		break; //module has been found --> next entry
	  }
	} //end entries loop

	//cout<<endl;

	//if(useflu) {cout<<lumi<<endl;}

	delete f;

  }//end NF loop
	ipt = -1; //reinit

//--- DRAWING ---//

  TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
  c1->SetTopMargin(0.1);
  c1->SetBottomMargin(0.1);


  //In case of superposition with simu curve, g is drawn in second
  //Make sure that the y-axis covers all of g points
  double y_tmp = 0, x_tmp=0, y_max=0, y_min=9999, flumax=0;
  for(int i=0; i<g->GetN(); i++)
  {
  	g->GetPoint(i, x_tmp, y_tmp);

	// cout<<"i / lumi / y / flu : "<<i<<" / "<<x_tmp<<" / "<<y_tmp<<" / "<<ComputeFluence(x_tmp, detid, subdet)<<endl;
	// cout<<"GetLumiFromFluence(x) = "<<GetLumiFromFluence(ComputeFluence(x_tmp, detid, subdet), detid, subdet)<<endl<<endl;


	if(draw_vs_fluence) {g->SetPoint(i, ComputeFluence(x_tmp, detid, subdet)*pow(10, -12), y_tmp);}

  	if(y_tmp > y_max) {y_max = y_tmp;}
  	else if(y_tmp < y_min) {y_min = y_tmp;}

	// if(i==g->GetN()-1) {flumax = ComputeFluence(x_tmp, detid, subdet)*pow(10, -12);}
  }

  TGraph* g_simu = 0;

  //cout<<"--- Superimpose simulation : Fluence --> Lumi"<<endl;

  if(superimpose_simu)
  {
	  TString simufile_name = dirname+"/simulation/VdepGraphs_TIB_2017.root";
	if(Check_File_Existence(simufile_name) )
	{
		TFile* f_simu = TFile::Open( simufile_name );

		TString g_simu_name = "graph_" + Convert_Number_To_TString(detid);
		//cout<<g_simu_name.Data()<<endl;

		g_simu = (TGraph*) f_simu->Get(g_simu_name);
		if(g_simu == 0) {cout<<"Error : "<<g_simu_name<<" not found in "<<simufile_name<<" !"<<endl; return;}

		for(int i=0; i<g_simu->GetN(); i++)
		{
			g_simu->GetPoint(i, x_tmp, y_tmp);

			//cout<<"i =="<<i<<endl;

			if(!draw_vs_fluence) {g_simu->SetPoint(i, GetLumiFromFluence(x_tmp, detid, subdet), y_tmp);}
			else {g_simu->SetPoint(i, x_tmp*pow(10, -12), y_tmp);}

			if(y_tmp > y_max) {y_max = y_tmp;}
			else if(y_tmp < y_min) {y_min = y_tmp;}
		}
		delete f_simu;
 	}
 	else {cout<<FRED(""<<simufile_name<<" not found !"<<)<<endl;}
  }



//----------------------
  //cout<<"g->GetN() = "<<g->GetN()<<endl;

  //bool draw_vdep_lab = true; //Draw lab measurement of initial Vfd
  //bool draw_fit = false; //Draw linear fit of vfd evolution

  if(subdet=="TEC") draw_vdep_lab = false;

  VdeplRef SubdetRef;
  SubdetRef.loadFile(subdet);


//--- Draw Vfd curve
  TH1F* h = g->GetHistogram();

/*
  //Get g Y max
  int n = g->GetN();
  double* y = g->GetY();
  int locmax = TMath::LocMax(n,y);
  double tmax = y[locmax];
*/

  if(draw_vdep_lab) {h->GetXaxis()->SetLimits(0, h->GetXaxis()->GetXmax());} //start at x=0

  if(draw_vdep_lab && SubdetRef.GetVdepl(detid) > y_max)
  {
  	g->SetMaximum(SubdetRef.GetVdepl(detid)*1.1);
  }
  else
  {
  	g->SetMaximum(y_max * 1.1);
  }

  g->SetMinimum(y_min * 0.90 - 10);
  if(y_min * 0.90 - 10 < 0) {g->SetMinimum(0);}

  g->SetLineWidth(2);

  //if(superimpose_simu) h->GetXaxis()->SetTitle("Fluence [cm^{-2}]"); //single fluence axis
  //else h->GetXaxis()->SetTitle("L_{int} [fb^{-1} ]");

  //h->GetXaxis()->SetTitle("L_{int} [fb^{-1} ]");

  //h->GetXaxis()->SetTitle("#scale[0.5]{#int} L [fb^{-1}]");
  if(!draw_vs_fluence){h->GetXaxis()->SetTitle("Integrated luminosity [fb^{-1}]");}
  else {h->GetXaxis()->SetTitle("Simulated fluence [1 MeV neutron equivalent . cm^{-2} . 10^{12}]");}
  h->GetXaxis()->SetTitleSize(.04);
  h->GetXaxis()->SetTitleOffset(1.18);

  //h->GetYaxis()->SetTitle("Full depletion voltage V_{FD} [V]");
  h->GetYaxis()->SetTitle("Full depletion voltage [V]");
  h->GetYaxis()->SetTitleSize(.04);
  h->GetYaxis()->SetTitleOffset(1.4);
  //h->GetYaxis()->SetLabelOffset(0.02);

  //if(subdet == "TEC") h->SetTitle(Form("Detid %llu - %s R%i", detid, subdet.c_str(), olayer));
  //else h->SetTitle(Form("Detid %llu - %s L%i", detid, subdet.c_str(), olayer));

  //if(subdet != "TEC") h->SetTitle(subdet + " Layer " + Convert_Number_To_TString(olayer) + " module");
  //else h->SetTitle("TEC Ring " + Convert_Number_To_TString(olayer) + " module");

  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.2);
  g->Draw("APL");



//----------------
	// CAPTIONS //
//----------------

// -- using https://twiki.cern.ch/twiki/pub/CMS/Internal/FigGuidelines

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c1->GetLeftMargin(),0.95,cmsText);

	bool writeExtraText = false;
	TString extraText   = "Preliminary";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c1->GetLeftMargin() + 0.1, 0.953, extraText);

	TString energy_text   = "#sqrt{s}=13 TeV (25 ns)";
	latex.SetTextFont(42);
	latex.SetTextSize(0.04);
	//latex.DrawLatex(0.75, 0.954, energy_text);

	TString detid_text   = "Detid " + Convert_Number_To_TString(detid);
	latex.SetTextFont(42);
	latex.SetTextSize(0.03);
	latex.DrawLatex(0.80, 0.954, detid_text);

	TLatex subdetinfo;
	subdetinfo.SetNDC();
	subdetinfo.SetTextSize(0.04);
	subdetinfo.SetTextFont(42);
	// subdetinfo.DrawLatex(0.20,0.25,"TIB Layer 1");

	//TString fluence_label = "Fluence #scale[0.9]{[10^{12} . cm^{-2}]}";
	TString fluence_label = "Simulated fluence [1 MeV neutron equivalent . cm^{-2} . 10^{12}]";
	latex.SetTextFont(42);
	latex.SetTextSize(0.035);
	if(!draw_vs_fluence) {latex.DrawLatex(0.30, 0.83, fluence_label);}
	else
	{
		fluence_label = "Integrated luminosity [fb^{-1}]";
		latex.DrawLatex(0.65, 0.83, fluence_label);
	}



//--- Superimpose Vfd prediction curve from C. Barth
  TH1F* h_simu = 0;

  TLegend *leg = new TLegend(.60,.60,.80,.75);
  // TLegend *leg = new TLegend(.60,.60,.85,.70);
  gStyle->SetLegendTextSize(0.03);
  leg->SetBorderSize(1.);
  gStyle->SetLegendBorderSize(1);
  gStyle->SetLegendFillColor(0);
  gStyle->SetLegendFont(42);

  if(superimpose_simu && g_simu != 0)
  {
	  h->SetMinimum(0.); //So that we can ~see where simu predicts inversion

 	  g_simu->SetMarkerColor(kRed); g_simu->SetLineColor(kRed);
	  g_simu->SetLineWidth(2);
	  g_simu->SetMarkerSize(1.2);
	  //g_simu->Draw("APL");
	  g_simu->Draw("PL");

	  //leg = new TLegend(.65,.70,.85,.80);
	  //leg = new TLegend(.65,.65,.85,.75);

	  leg->AddEntry(g_simu, "Prediction", "P");
	  leg->AddEntry(g, "Measurement", "P");
	  leg->Draw("same");
  }

  g->Draw("PL"); //Draw again to get data points on top

//----------

  TF1* fit;
  if(!superimpose_simu && draw_fit)
  {
	  //Draw Linear Fit of all scans
	  fit = new TF1("fit", "pol1", 0, h->GetXaxis()->GetXmax());
	  fit->SetLineColor(1);
	  fit->SetLineStyle(2);
	  fit->SetLineWidth(1);
	  g->Fit("fit", "rqn");
	  fit->Draw("same");

	  //cout<<"Fit : "<<fit->GetParameter(1)<<".x + "<<fit->GetParameter(0)<<endl; //--- Display the fit parameters (in lumi^-1 units)
  }



  //Draw separate X-axis for each CME energy (different equivalence between lumi and fluence) -- divide by 10^12 to remove exponent

	double flu_7TeV = ComputeFluence(lumi_7TeV, detid, subdet) / pow(10,12);
	double flu_8TeV = ComputeFluence(lumi_8TeV, detid, subdet) / pow(10,12);
	double flu_Run1 = ComputeFluence(lumi_Run1, detid, subdet) / pow(10,12);

	double flu_max = ComputeFluence(h->GetXaxis()->GetXmax(), detid, subdet) / pow(10, 12);
	// cout<<"flumax = "<<flu_max<<endl;

	TLatex zero;
	zero.SetNDC();
	zero.SetTextSize(0.035);
	zero.SetTextFont(42);
	zero.DrawLatex(c1->GetLeftMargin(),0.906,"0");

   TGaxis *axis2 = 0, *axis3 = 0;
   if(!draw_vs_fluence)
   {
	   if(subdet == "TEC" && olayer == 3) {axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 504, "-S");}
	   else {axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 503, "-S");}
   }
   else
   {
	   axis2 = new TGaxis(flu_7TeV,h->GetMaximum(),flu_Run1,h->GetMaximum(),lumi_7TeV,lumi_Run1, 504, "-S");
   }
   double axis2_length = (lumi_Run1 - lumi_7TeV) / h->GetXaxis()->GetXmax();
   if(draw_vs_fluence) {axis2_length = (flu_Run1 - flu_7TeV) / h->GetXaxis()->GetXmax();}
   axis2->SetTickLength(0.03/axis2_length);
   axis2->SetLabelSize(0.035);
   axis2->SetLabelFont(42);
   axis2->SetNoExponent(kTRUE);
   axis2->SetLabelOffset(-0.008);
   axis2->Draw();

   if(!draw_vs_fluence)
   {
	   axis3 = new TGaxis(lumi_Run1,h->GetMaximum(),h->GetXaxis()->GetXmax(),h->GetMaximum(),flu_Run1,flu_max, 510, "-S");
	}
	else
	{
		axis3 = new TGaxis(flu_Run1,h->GetMaximum(),h->GetXaxis()->GetXmax(),h->GetMaximum(),lumi_Run1,GetLumiFromFluence(h->GetXaxis()->GetXmax()*pow(10, 12), detid, subdet), 510, "-S");
	}
   double axis3_length = (h->GetXaxis()->GetXmax() - lumi_Run1) / h->GetXaxis()->GetXmax();
   if(draw_vs_fluence) {axis3_length = (h->GetXaxis()->GetXmax() - flu_Run1) / h->GetXaxis()->GetXmax();}
   axis3->SetTickLength(0.03/axis3_length);
   axis3->SetLabelSize(0.035);
   axis3->SetLabelFont(42);
   axis3->SetNoExponent(kTRUE);
   axis3->SetLabelOffset(-0.008);
   //axis3->SetTitle("Fluence #scale[0.9]{[10^{12} . cm^{-2}]}");
   axis3->SetTitleSize(0.035);
   axis3->SetTitleOffset(-0.85);
   axis3->Draw();

   // cout<<"axis2_length "<<axis2_length<<endl;
   // cout<<"axis3_length "<<axis3_length<<endl;
   // cout<<"h->GetXaxis()->GetXmax()*pow(10, 12) = "<<h->GetXaxis()->GetXmax()*pow(10, 12)<<endl;
   // cout<<"GetLumiFromFluence(h->GetXaxis()->GetXmax(), detid, subdet) = "<<GetLumiFromFluence(h->GetXaxis()->GetXmax()*pow(10, 12), detid, subdet)<<endl;


  // Draw Ref measurement (measured in lab.);
  TMarker *pref = new TMarker( 0, SubdetRef.GetVdepl(detid), 21);
  if(!superimpose_simu)
  {
  	pref->SetMarkerColor(2);
  	pref->SetMarkerSize(1.8); //NEW
  	leg->AddEntry(pref, "Laboratory measurement", "P");
  	leg->Draw("same");
  	if(draw_vdep_lab) pref->Draw("same");
  }

//--------------------------------------------

	//Draw gray band below ~30V, to emphasize unsensitivity at low Vfd
	bool draw_gray_band = false;

	TH1F* h_gray = new TH1F("", "", 1, 0, 10000);
	h_gray->SetBinContent(1, 30);
	h_gray->SetFillColor(kGray);
	h_gray->SetFillStyle(3004);
	h_gray->SetLineWidth(0.);
	if(draw_gray_band) {h_gray->Draw("same");}



//------------------

  // Print values
  //cout<<"DetID "<<detid;
  //cout<<" lab. meas. for "<<detid<<": "<<SubdetRef.GetVdepl(detid)<<endl;

  double lumi;
  double Vfd;
  for(unsigned int ip=0; ip<(unsigned) g->GetN(); ip++)
  {
    g->GetPoint(ip, lumi, Vfd);
	//cout<<ip+1<<". Fluence = "<<lumi<<" //  Vfd = "<<Vfd<<endl;

	//cout<<ip+1<<". Lumi = "<<lumi<<" //  Vfd = "<<Vfd<<endl;
  }

  cout<<endl;

  c1->Modified();
  c1->Update();
  if(print)
  {
  	Create_Plot_Directories();

  	TString output_name = "plots/";
	if(antype=="Signal") output_name+= "signal/";
	else output_name+= "CW/";

	if(use_curvature) {output_name+="kink/";}
  	else {output_name+="line/";}

  	if(superimpose_simu) {output_name+= "simu/";}

  	output_name+= "KinkVSLumi_"+subdet;

   	if(use_curvature) {output_name+="_kink";}
  	else {output_name+="_line";}

    TString ts_detid = Convert_Number_To_TString(detid);
    output_name+="_detid"+ts_detid+"_"+antype;
    if(superimpose_simu) {output_name+="_simu";}
    output_name+=+".png";
    // output_name+=+".pdf";

    c1->SaveAs(output_name);
  }

  //getchar(); //Waits for user to press enter to continue
  if(superimpose_simu && g_simu != 0) {delete leg;}
  if(!superimpose_simu && draw_fit) {delete fit;}
  delete g;
  delete c1;
  delete pref; //delete marker;
  delete h_gray;
  //delete axis;
  if(axis2) {delete axis2;}
  if(axis3) {delete axis3;}

  return;
}



//--------------------------------------------
//--------------------------------------------
//--------------------------------------------
//--------------------------------------------




//--------------------------------------------
// ########  ########     ###    ##      ##       ###    ##       ##
// ##     ## ##     ##   ## ##   ##  ##  ##      ## ##   ##       ##
// ##     ## ##     ##  ##   ##  ##  ##  ##     ##   ##  ##       ##
// ##     ## ########  ##     ## ##  ##  ##    ##     ## ##       ##
// ##     ## ##   ##   ######### ##  ##  ##    ######### ##       ##
// ##     ## ##    ##  ##     ## ##  ##  ##    ##     ## ##       ##
// ########  ##     ## ##     ##  ###  ###     ##     ## ######## ########

//  ######  ##     ##    ###    ##       ##           ######   ######     ###    ##    ##
// ##    ## ###   ###   ## ##   ##       ##          ##    ## ##    ##   ## ##   ###   ##
// ##       #### ####  ##   ##  ##       ##          ##       ##        ##   ##  ####  ##
//  ######  ## ### ## ##     ## ##       ##           ######  ##       ##     ## ## ## ##
//       ## ##     ## ######### ##       ##                ## ##       ######### ##  ####
// ##    ## ##     ## ##     ## ##       ##          ##    ## ##    ## ##     ## ##   ###
//  ######  ##     ## ##     ## ######## ########     ######   ######  ##     ## ##    ##
//--------------------------------------------

void DrawModules_SmallScan(string dirname, string subdet, string antype, string ref, const int NF, vector<string> runs, vector<float> lumis, bool draw_vdep_lab=true, bool draw_fit=false)
{
 if(subdet!="TIB" && subdet!="TOB" && subdet!="TEC") {cout<<__LINE__<<" : Subdet '"<<subdet<<"' not allowed."<<endl; return;}

 if(subdet=="TIB")
 {
   //TIBminus_1_2_2_1
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121381, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121382, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121385, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121386, draw_vdep_lab, draw_fit);
   //TIBminus_1_4_2_5
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121605, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121606, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121609, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121610, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121613, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121614, draw_vdep_lab, draw_fit);
   //TIBplus_1_6_2_5
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125861, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125862, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125866, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125869, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125870, draw_vdep_lab, draw_fit);
 }
 if(subdet=="TOB")
 {
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815121, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815122, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815281, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815282, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815081, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815082, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815241, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815242, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815201, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815202, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815161, draw_vdep_lab, draw_fit);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815162, draw_vdep_lab, draw_fit);
 }
}



//--------------------------------------------
// ########  ########     ###    ##      ##    ########  #### ######## ########
// ##     ## ##     ##   ## ##   ##  ##  ##    ##     ##  ##  ##       ##
// ##     ## ##     ##  ##   ##  ##  ##  ##    ##     ##  ##  ##       ##
// ##     ## ########  ##     ## ##  ##  ##    ##     ##  ##  ######   ######
// ##     ## ##   ##   ######### ##  ##  ##    ##     ##  ##  ##       ##
// ##     ## ##    ##  ##     ## ##  ##  ##    ##     ##  ##  ##       ##       ###
// ########  ##     ## ##     ##  ###  ###     ########  #### ##       ##       ###

//  ######  ##     ##    ###    ##       ##           ######   ######     ###    ##    ##
// ##    ## ###   ###   ## ##   ##       ##          ##    ## ##    ##   ## ##   ###   ##
// ##       #### ####  ##   ##  ##       ##          ##       ##        ##   ##  ####  ##
//  ######  ## ### ## ##     ## ##       ##           ######  ##       ##     ## ## ## ##
//       ## ##     ## ######### ##       ##                ## ##       ######### ##  ####
// ##    ## ##     ## ##     ## ##       ##          ##    ## ##    ## ##     ## ##   ###
//  ######  ##     ## ##     ## ######## ########     ######   ######  ##     ## ##    ##
//--------------------------------------------


TH1F* DrawHistoDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, string run, bool show=true, bool use_curvature=true, int layer=0)
{
  if(subdet!="TIB" && subdet!="TOB" && subdet!="TEC") {cout<<__LINE__<<" : Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}

  const int Ndet_TIB=9;
  // ULong64_t Detids_TIB[Ndet_TIB] = {369121381, 369121382, 369121385, 369121386, 369125862, 369125866, 369125870}; //previous list
  ULong64_t Detids_TIB[Ndet_TIB] = {369121381, 369121382, 369121385, 369121386, 369121389, 369121390, 369125862, 369125866, 369125870};
  double vfd_ref_TIB[Ndet_TIB];
  for(int i=0; i<Ndet_TIB; i++) {vfd_ref_TIB[i] = 0;}

  const int Ndet_TOB=12;
  ULong64_t Detids_TOB[Ndet_TOB] = {4362815121, 4362815122, 4362815281, 4362815282,
  4362815081, 4362815082, 4362815241, 4362815242, 4362815201, 4362815202, 4362815161, 4362815162};
  double vfd_ref_TOB[Ndet_TOB];
  for(int i=0; i<Ndet_TOB; i++) {vfd_ref_TOB[i] = 0;}

  const int Ndet_TEC=31;
  ULong64_t Detids_TEC[Ndet_TEC] = {4701481960, 4701482280, 4701482400, 4701482000, 4701482320, 4701482040, 4701482360, 4701482651, 4701482961, 4701483241, 4701483361, 4701482611, 4701482661, 4701483001, 4701483281, 4701483401, 4701482621, 4701482921, 4701483321, 4701482652, 4701482962, 4701483242, 4701483362, 4701482612, 4701482662, 4701483002, 4701483282, 4701483402, 4701482622, 4701482922, 4701483322};
  double vfd_ref_TEC[Ndet_TEC];
  for(int i=0; i<Ndet_TEC; i++) {vfd_ref_TEC[i] = 0;}


  unsigned int Ndet;
  ULong64_t* Detids;
  double* vfd_ref;

  if(subdet=="TIB") {Ndet=Ndet_TIB; Detids=Detids_TIB; vfd_ref=vfd_ref_TIB;}
  else if(subdet=="TOB") {Ndet=Ndet_TOB; Detids=Detids_TOB; vfd_ref=vfd_ref_TOB;}
  else if(subdet=="TEC") {Ndet=Ndet_TEC; Detids=Detids_TEC; vfd_ref=vfd_ref_TEC;}

  string filename = dirname+"/"+"DECO_"+antype+"_"+subdet;
  if(use_curvature) filename+="_kink";
  else filename+= "_line";

  ULong64_t odetid;
  int olayer;
  double odepvolt;
  double oerrdepvolt;
  double oplateau;
  double ofitchisquare;
  int ofitstatus;
  double olastpty;
  double ochi2;

  TCanvas *c1 ;
  if(show) c1 = new TCanvas("c1","c1", 1000, 800);
  TH1F* h = new TH1F("h", "", 200, -500, 500); //NB : need to adjust range here so that GetMean() works !!

  if(!Check_File_Existence(filename+"_"+ref+".root") ) {cout<<FRED("File "<<filename+"_"+ref+".root not found !")<<endl; return 0;}

  TFile *fref = TFile::Open( (filename+"_"+ref+".root").c_str());
  //TFile *fref = TFile::Open( (filename+"_"+ref+"_nosmooth.root").c_str()); //no smoothing - change name

  if(!fref) { cout<<"No file : "<<filename<<endl; return 0;}
  TTree* tref = (TTree*) fref->FindObjectAny("tout");

  tref->SetBranchAddress("DETID",&odetid);
  tref->SetBranchAddress("LAYER",&olayer);
  tref->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
  tref->SetBranchAddress("DEPVOLT",&odepvolt);
  tref->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
  tref->SetBranchAddress("FITCHI2",&ofitchisquare);
  tref->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
  tref->SetBranchAddress("LASTPOINTS",&olastpty);
  tref->SetBranchAddress("CHI2",&ochi2); // significance min

  UInt_t nentries = tref->GetEntries();
  bool dont_repeat_warning = false;
  for(UInt_t ie = 0; ie <nentries; ie++) //Loop on modules
  {

  	tref->GetEntry(ie);


  	//NEW -- separate TEC by rings
  	if(subdet=="TEC" && layer!= 0)
  	{
  		if(olayer != layer) {continue;}
  	}


  	//----------------
	//CAN REMOVE DETIDS/RUNS/TYPE COMBINATIONS HERE (if bad curve --> not to appear on plot)
    bool remove_badscans = true;

    if(remove_badscans)
    {
    	if( Is_Scan_Bad(subdet, run, antype, odetid) == true)
    	{
    		if(!dont_repeat_warning) {cout<<FRED("Module "<<odetid<<" ignored (Is_Scan_Bad==true for run "<<run<<")")<<" -- Don't print more warnings"<<endl; dont_repeat_warning = true; }
    		//continue;
    		return 0; //Skip scan even if only 1 module bad ?
    	}
    }




    for(UInt_t idet = 0; idet < Ndet; idet++)
    {
      if(odetid==Detids[idet] && odepvolt>=0) vfd_ref[idet] = odepvolt;
    }
  }


  if(run!="labref")
  {
  	TFile *f = TFile::Open( (filename+"_"+run+".root").c_str());
	//TFile *f = TFile::Open( (filename+"_"+run+"_nosmooth.root").c_str()); // no smoothing - change name

	if(!f) { cout<<"No file : "<<filename<<endl; return 0;}
	TTree* t = (TTree*) f->FindObjectAny("tout");

	t->SetBranchAddress("DETID",&odetid);
	t->SetBranchAddress("LAYER",&olayer);
	t->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
	t->SetBranchAddress("DEPVOLT",&odepvolt);
	t->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
	t->SetBranchAddress("FITCHI2",&ofitchisquare);
	t->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
	t->SetBranchAddress("LASTPOINTS",&olastpty);
	t->SetBranchAddress("CHI2",&ochi2); // significance min

	nentries = t->GetEntries();
	for(UInt_t ie = 0; ie <nentries; ie++)
	{
	  t->GetEntry(ie);

	  //NEW -- separate TEC by rings
  	  if(subdet=="TEC" && layer!= 0)
  	  {
  		  if(olayer != layer) {continue;}
  	  }


      for(UInt_t idet  = 0; idet < Ndet; idet++)
      {
    	if(odetid==Detids[idet] && odepvolt>=0)
		{
		  h->Fill(odepvolt-vfd_ref[idet]);

		  //cout<<Detids[idet]<<" "<<odepvolt<<"-"<<vfd_ref[idet]<<"= "<<odepvolt-vfd_ref[idet]<<endl;
		}
      }
	}


	delete f;
  }
  else
  {
    VdeplRef SubdetRef;
    SubdetRef.loadFile(subdet);

	for(UInt_t idet = 0; idet < Ndet; idet++)
	{
      odepvolt = SubdetRef.GetVdepl(Detids[idet]);
	  if(odepvolt>=0) h->Fill(odepvolt-vfd_ref[idet]);
	}
  }


  if(show)
  {
   h->Draw();
   c1->Modified();
   c1->Update();
   getchar();
  }

  delete tref; delete fref; //CHANGED
  if(show) delete c1;


  return h;
}




TGraphErrors* DrawDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, const int NF, vector<string> runs, vector<float> lumis, bool useflu=false, bool use_curvature=true, int layer=0, bool draw_fit=true)
{

  bool useRmsAsErrors=true;
  cout<<"Use RMS error : "<<useRmsAsErrors<<" // Use Mean Error : "<<1-useRmsAsErrors<<endl;

  // Fluence
  ModFluence DetFluenceRun1;
  DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
  ModFluence DetFluenceRun2;
  DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");
  double fluenceRun1=0;
  double fluenceRun2=0;
  // use fluence of one of the modules
  if(useflu) {
    fluenceRun1 = DetFluenceRun1.GetFluence(369121381);
    fluenceRun2 = DetFluenceRun2.GetFluence(369121381);
  }


  TGraphErrors *g = new TGraphErrors();
  int ipt=0; //need independant index, because some runs are skipped

  TH1F* href = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, "labref", useflu, use_curvature, layer); //Last arg : TEC layer we consider
  if(!href) {cout<<FRED("hdiff is null! Abort")<<endl; return 0;}

	double lumi_7TeV=6.12;
	double lumi_8TeV=29.46 - 6.12;
	double lumi_Run1=29.46;
	double lumi=0;

  for(int i=0; i<NF; i++)
  {
    //------------------

    TH1F* hdiff = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, runs[i], useflu, use_curvature, layer);

    if(!hdiff) {cout<<FRED("hdiff is null! Continue")<<endl; continue;}

    lumi = lumis[i];


	cout<<"--- Run "<<runs[i]<<endl;
	cout<<"hdiff : "<<hdiff->GetEntries()<<" entries / Mean = "<<hdiff->GetMean()<<" / RMS : "<<hdiff->GetRMS()<<endl;
	cout<<"href : "<<href->GetEntries()<<" entries / Mean = "<<href->GetMean()<<" / RMS : "<<href->GetRMS()<<endl;

	g->SetPoint(ipt, lumi, hdiff->GetMean());//-href->GetMean());


	if(useRmsAsErrors) g->SetPointError(ipt, 0, hdiff->GetRMS());
    else g->SetPointError(ipt, 0, hdiff->GetMeanError());


	if(i==0)
	{
        if(useRmsAsErrors) g->SetPointError(ipt, 0, href->GetRMS());
        else g->SetPointError(ipt, 0, href->GetMeanError());
    }


	//cout<<ipt<<" mean "<<hdiff->GetMean()-href->GetMean();
	//if(useRmsAsErrors) cout<<" / rms = "<<hdiff->GetRMS()<<endl;
	//else cout<<" / Mean error = "<<hdiff->GetMeanError()<<endl;

	ipt++;
  }

//Display points
/*
  for(int i=0; i<g->GetN(); i++)
  {
  	double x=0, y=0;
  	g->GetPoint(i,x,y);
  	cout<<i<<" / "<<x<<" / "<<y<<endl;
  }
*/


  TH1F* h = g->GetHistogram();

  h->GetXaxis()->SetTitle("Integrated luminosity [fb^{-1}]");
  //if(useflu) h->GetXaxis()->SetTitle("Fluence #scale[0.9]{[10^{12} . cm^{-2}]}");
  if(useflu) h->GetXaxis()->SetTitle("Fluence [cm^{-2}]");
  h->GetXaxis()->SetTitleSize(.04);
  h->GetXaxis()->SetTitleOffset(1.18);

  //h->GetYaxis()->SetTitle("Full depletion voltage [V]");
  h->GetYaxis()->SetTitle("#DeltaV_{FD} [V]");
  h->GetYaxis()->SetTitleSize(.05);
  h->GetYaxis()->SetTitleOffset(0.9);

  g->SetMarkerStyle(20);


  TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
  c1->SetTopMargin(0.1);
  g->Draw("APL");


  TF1* fit;
  if(draw_fit)
  {
	  //Draw Linear Fit of all scans
	  fit = new TF1("fit", "pol1", 0, h->GetXaxis()->GetXmax());
	  fit->SetLineColor(1);
	  fit->SetLineStyle(2);
	  fit->SetLineWidth(1);
	  g->Fit("fit", "rqn");
	  fit->Draw("same");

	  //cout<<"Fit : "<<fit->GetParameter(1)<<".x + "<<fit->GetParameter(0)<<endl; //--- Display the fit parameters (in lumi^-1 units)
  }

/*
  // Draw Ref measurement;
  TMarker *pref = new TMarker( 0, 0, 21);
  pref->SetMarkerColor(2);
  pref->Draw();
  TLine *lref = new TLine(0, -1*href->GetRMS(), 0, href->GetRMS());
  lref->SetLineColor(2);
  lref->Draw();
*/
  // Fit with Hamburg model
  //TF1* fit = new TF1("ham", "[3]-([0]*(1-exp(-[1]*x))+[2]*x)", 0, 30);
  //g->Fit("ham");
  //fit->SetParameters(50, 0.1, 1, 250);
  //fit->Draw();





//----------------
	// CAPTIONS //
//----------------

// -- using https://twiki.cern.ch/twiki/pub/CMS/Internal/FigGuidelines

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c1->GetLeftMargin(),0.95,cmsText);

	bool writeExtraText = false;
	TString extraText   = "Preliminary";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c1->GetLeftMargin() + 0.1, 0.953, extraText);


	//TString fluence_label = "Fluence #scale[0.9]{[10^{12} . cm^{-2}]}";
	TString fluence_label = "Simulated fluence [1 MeV neutron equivalent . cm^{-2} . 10^{12}]";
	latex.SetTextFont(42);
	latex.SetTextSize(0.035);
	latex.DrawLatex(0.30, 0.83, fluence_label);

	//Draw separate X-axis for each CME energy (different equivalence between lumi and fluence) -- divide by 10^12 to remove exponent
	//---- ADD ONLY FLUENCE AXIS FOR TIB MODULES, SINCE TOB/TEC HAVE DIFFERENT FLUENCES IN MODULES !!

	double flu_7TeV = ComputeFluence(lumi_7TeV, 369121381, subdet) / pow(10,12);

	double flu_Run1 = ComputeFluence(lumi_Run1, 369121381, subdet) / pow(10,12);

	double flu_max = ComputeFluence(h->GetXaxis()->GetXmax(), 369121381, subdet) / pow(10, 12);

   TGaxis *axis2 = 0;
   //if(subdet == "TEC" && olayer == 3) {axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 504, "-S");}
   //else {axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 503, "-S");}
   axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 503, "-S");
   double axis2_length = (lumi_Run1 - lumi_7TeV) / h->GetXaxis()->GetXmax();
   axis2->SetTickLength(0.03/axis2_length);
   axis2->SetLabelSize(0.035);
   axis2->SetLabelFont(42);
   axis2->SetNoExponent(kTRUE);
   axis2->SetLabelOffset(-0.008);
   //axis2->Draw();

   TGaxis *axis3 = new TGaxis(lumi_Run1,h->GetMaximum(),h->GetXaxis()->GetXmax(),h->GetMaximum(),flu_Run1,flu_max, 510, "-S");
   double axis3_length = (h->GetXaxis()->GetXmax() - lumi_Run1) / h->GetXaxis()->GetXmax();
   axis3->SetTickLength(0.03/axis3_length);
   axis3->SetLabelSize(0.035);
   axis3->SetLabelFont(42);
   axis3->SetNoExponent(kTRUE);
   axis3->SetLabelOffset(-0.008);
   //axis3->SetTitle("Fluence #scale[0.9]{[10^{12} . cm^{-2}]}");
   axis3->SetTitleSize(0.035);
   axis3->SetTitleOffset(-0.85);
   //axis3->Draw();


	if(subdet == "TIB")
	{
		TLatex zero;
		zero.SetNDC();
		zero.SetTextSize(0.035);
		zero.SetTextFont(42);
		zero.DrawLatex(c1->GetLeftMargin(),0.906,"0");

		TLatex subdetinfo;
		subdetinfo.SetNDC();
		subdetinfo.SetTextSize(0.045);
		subdetinfo.SetTextFont(42);
		subdetinfo.DrawLatex(0.72,0.60,"TIB Layer 1");

		axis2->Draw();
		axis3->Draw();
	}

  c1->Modified();
  c1->Update();

  Create_Plot_Directories();

  string name = "plots/";
  if(antype=="Signal") name+= "signal/relative/";
  else name+="CW/relative/";

  if(!use_curvature) {name += subdet + "_line_diff"+antype;}
  else {name += subdet + "_kink_diff"+antype;}
  if(subdet == "TEC")
  {
  	if(layer != 0) {name+= "_R"+Convert_Number_To_TString(layer);}
  	else {name+= "_allRings";}
  }

  name+=".png";
  //name+=".pdf";
  c1->SaveAs(name.c_str());
  //getchar();

  delete c1; delete href;
  if(draw_fit) {delete fit;}
  delete axis2; delete axis3;

  return g;
}


//--------------------------------------------
//  ######  ##     ## ########  ######## ########  #### ##     ## ########   #######   ######  ########    ########  #### ######## ########
// ##    ## ##     ## ##     ## ##       ##     ##  ##  ###   ### ##     ## ##     ## ##    ## ##          ##     ##  ##  ##       ##
// ##       ##     ## ##     ## ##       ##     ##  ##  #### #### ##     ## ##     ## ##       ##          ##     ##  ##  ##       ##
//  ######  ##     ## ########  ######   ########   ##  ## ### ## ########  ##     ##  ######  ######      ##     ##  ##  ######   ######
//       ## ##     ## ##        ##       ##   ##    ##  ##     ## ##        ##     ##       ## ##          ##     ##  ##  ##       ##
// ##    ## ##     ## ##        ##       ##    ##   ##  ##     ## ##        ##     ## ##    ## ##          ##     ##  ##  ##       ##       ###
//  ######   #######  ##        ######## ##     ## #### ##     ## ##         #######   ######  ########    ########  #### ##       ##       ###
//--------------------------------------------

//Superimpose 2 "diff kink" curves (to have Signal & Clusterwidth on same plot)
void Superimpose_DrawDiffModules_SmallScan(string dirname, string subdet, string ref, const int NF, vector<string> runs, vector<float> lumis, bool useflu, bool use_curvature, bool draw_fit=true)
{
	TGraphErrors* g_sig = DrawDiffModules_SmallScan(dirname, subdet, "Signal", ref, NF, runs, lumis, useflu, use_curvature, 0, draw_fit);
	TGraphErrors* g_cluster = DrawDiffModules_SmallScan(dirname, subdet, "ClusterWidth", ref, NF, runs, lumis, useflu, use_curvature, 0, draw_fit);

	if(!g_sig || !g_cluster) {cout<<"Null TGraph ! Abort !"<<endl; return;}

	TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);


  TH1F* h1 = g_cluster->GetHistogram(); //Access g content via TH1F*
  if(useflu) h1->GetXaxis()->SetTitle("Fluence [cm^{-2}]");
  //else h1->GetXaxis()->SetTitle("L_{int} [fb^{-1}]");
  //else h1->GetXaxis()->SetTitle("#scale[0.6]{#int} L [fb^{-1} ]");
  else h1->GetXaxis()->SetTitle("Integrated luminosity [fb^{-1}]");
  h1->GetYaxis()->SetTitle("V_{FD} [V]");
  g_cluster->SetMarkerStyle(20);

  g_cluster->Draw("APL");

  c1->Modified();
  c1->Update();

  g_sig->SetMarkerStyle(20);
  g_sig->SetMarkerColor(kRed);
  g_sig->SetLineColor(kRed);
  g_sig->Draw("PL"); //Don't redraw axis ("A")

  TLegend* leg = new TLegend(.65,.70,.85,.80);
  gStyle->SetLegendTextSize(0.03);
  leg->AddEntry(g_cluster, "ClusterWidth", "P");
  leg->AddEntry(g_sig, "Signal", "P");
  leg->Draw("same");

  c1->Modified();
  c1->Update();

  Create_Plot_Directories();

  TString name = "plots/superimpose_CW_signal/superimpose_diff_";
  if(use_curvature) {name+= "kink";}
  else {name+= "line";}
  name+=".png";

  c1->SaveAs(name);

  delete c1; delete leg;
  delete g_sig; delete g_cluster;

  return;
}



TProfile* PrintDiffModules_FullScan(string dirname, string subdet, string antype, string ref, string run, int layer=0, bool print=false, bool show=false)
{

  TH1F* href;
  TH1F* histo;
  TProfile* profile;
  TProfile2D* pVfdTemp;
  gStyle->SetPalette(1);

  if(subdet!="TIB") {cout<<__LINE__<<" : Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}

 // Vdepl references from db
 //--------------------------

  VdeplRef SubdetRef;
  SubdetRef.loadFile(subdet);


  // Temperature measurements
  //-------------------------

  string tempfilename="Tdcu_0fb_"+subdet+".txt";
  ifstream ftemp(tempfilename.c_str());
  std::string line;
  unsigned int detid;
  float temperature=-1;
  unsigned int Temp_DetIDs[11000];
  float Temp[11000];
  unsigned int Ntemp=0;

  if(ftemp.is_open())
  {
    while( getline ( ftemp, line) && Ntemp < 11000)
	{
	  if(ftemp.eof()) continue;
	  std::stringstream ss(line);
	  ss >> detid >> temperature;

      Temp_DetIDs[Ntemp] = detid;
	  Temp[Ntemp] = temperature;
	  Ntemp++;
	}
	ftemp.close();
  }
  std::cout<<Ntemp<<" temperature meas."<<std::endl;


  // Vdepl for March 2011 scan
  //----------------------------

  ULong64_t odetid;
  int olayer;
  double odepvolt;
  double oerrdepvolt;
  double oplateau;
  double ofitchisquare;
  int ofitstatus;
  double olastpty;
  double ochi2;

  unsigned int ScanRef_DetIDs[11000];
  float ScanRef_Vdepl[11000];
  unsigned int Nscanref=0;

  href = new TH1F("h", "", 100, -100, 100);

  string filename = dirname+"/"+"DECO_"+antype+"_"+subdet;
  TFile *fscanref = TFile::Open((filename+"_"+ref+".root").c_str());
  if(!fscanref) { cout<<"No file : "<<(filename+"_"+ref+".root")<<endl; return 0;}
  TTree* tscanref = (TTree*) fscanref->FindObjectAny("tout");

  tscanref->SetBranchAddress("DETID",&odetid);
  tscanref->SetBranchAddress("LAYER",&olayer);
  tscanref->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
  tscanref->SetBranchAddress("DEPVOLT",&odepvolt);
  //tscanref->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
  //tscanref->SetBranchAddress("FITCHI2",&ofitchisquare);
  //tscanref->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
  //tscanref->SetBranchAddress("LASTPOINTS",&olastpty);
  //tscanref->SetBranchAddress("CHI2",&ochi2); // significance min

  UInt_t nentries = tscanref->GetEntries();
  for(UInt_t ie = 0; ie <nentries; ie++){
	tscanref->GetEntry(ie);
	if(odepvolt>=0 && (olayer==layer || layer==0) )
	{
      ScanRef_DetIDs[Nscanref] = odetid;
	  ScanRef_Vdepl[Nscanref] = odepvolt;
	  Nscanref++;
	}
  }
  fscanref->Close();

  TCanvas *c1 = 0;
  TCanvas *c2 = 0;
  if(show) c1 = new TCanvas("c1","c1", 1000, 800);
  if(show) c2 = new TCanvas("c2","c2", 1000, 800);

  // Fill first histo
  for(unsigned int iscanref=0; iscanref<Nscanref; iscanref++)
  {
    href->Fill(SubdetRef.GetVdepl(ScanRef_DetIDs[iscanref])-ScanRef_Vdepl[iscanref]);
  }

  if(show)
  {
    href->Draw();
	c1->Modified();
	c1->Update();
	getchar();
  }


  // Look for other scans
  //-----------------------
    histo = new TH1F("h", "", 100, -100, 100);
	profile = new TProfile("p", "", 50, 50, 300);
	pVfdTemp = new TProfile2D("pVfdTemp", "", 60, 0, 300, 20, 5, 45, -25, 25);

	TFile *f = TFile::Open((filename+"_"+run+".root").c_str());
	if(!f) { cout<<"No file : "<<(filename+"_"+run+".root")<<endl; return 0;}
	TTree* t = (TTree*) f->FindObjectAny("tout");

	t->SetBranchAddress("DETID",&odetid);
	t->SetBranchAddress("LAYER",&olayer);
	t->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
	t->SetBranchAddress("DEPVOLT",&odepvolt);
	t->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
	t->SetBranchAddress("FITCHI2",&ofitchisquare);
	t->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
	t->SetBranchAddress("LASTPOINTS",&olastpty);
	t->SetBranchAddress("CHI2",&ochi2); // significance min

	nentries = t->GetEntries();
	for(UInt_t ie = 0; ie <nentries; ie++){
	  t->GetEntry(ie);
      for(unsigned int iscanref=0; iscanref<Nscanref; iscanref++)
      {
    	if(odetid==ScanRef_DetIDs[iscanref] && odepvolt>=0 && (olayer==layer || layer==0) )
		{
           histo->Fill(odepvolt-ScanRef_Vdepl[iscanref]);
		   profile->Fill(ScanRef_Vdepl[iscanref], odepvolt-ScanRef_Vdepl[iscanref]);
		   //if(odepvolt-ScanRef_Vdepl[iscanref]>15) h->Fill(olayer);
		   if(print) cout<<odetid<<" "<<odepvolt-ScanRef_Vdepl[iscanref]<<endl;
		   if(Ntemp)
		   {
		     unsigned int it=0;
			 while(odetid!=Temp_DetIDs[it] && it<Ntemp)
			 {
			   it++;
			 }
			 if(odetid==Temp_DetIDs[it]) temperature=Temp[it];
			 else temperature=-999;
		     pVfdTemp->Fill(ScanRef_Vdepl[iscanref], temperature, odepvolt-ScanRef_Vdepl[iscanref]);
		   }
		 }
      }
	}
	f->Close();

	if(show)
	{
      c1->cd();
	  histo->Draw();
	  c1->Modified();
	  c1->Update();
	  c2->cd();
	  profile->Draw();
	  c2->Modified();
	  c2->Update();
	  c1->cd();
	  pVfdTemp->Draw("colz");
	  getchar();
	}

	delete tscanref; delete fscanref; delete href;
	if(show) {delete c1; delete c2;}

  return profile;
}





//--------------------------------------------
// ########  ########     ###    ##      ##    ######## ##     ##  #######  ##                 ###    ##       ##          ##     ##  #######  ########
// ##     ## ##     ##   ## ##   ##  ##  ##    ##       ##     ## ##     ## ##                ## ##   ##       ##          ###   ### ##     ## ##     ##
// ##     ## ##     ##  ##   ##  ##  ##  ##    ##       ##     ## ##     ## ##               ##   ##  ##       ##          #### #### ##     ## ##     ##
// ##     ## ########  ##     ## ##  ##  ##    ######   ##     ## ##     ## ##              ##     ## ##       ##          ## ### ## ##     ## ##     ##
// ##     ## ##   ##   ######### ##  ##  ##    ##        ##   ##  ##     ## ##              ######### ##       ##          ##     ## ##     ## ##     ##
// ##     ## ##    ##  ##     ## ##  ##  ##    ##         ## ##   ##     ## ##       ###    ##     ## ##       ##          ##     ## ##     ## ##     ## ###
// ########  ##     ## ##     ##  ###  ###     ########    ###     #######  ######## ###    ##     ## ######## ########    ##     ##  #######  ########  ###
//--------------------------------------------


void DrawKinkVsLumi(string dirname, string subdet, string type, vector<string> runs, vector<float> lumis, bool usefluence, bool use_curvature, bool superimpose_simu=false, bool draw_vdep_lab=true, bool draw_fit=false)
{
  int NF = runs.size();

//--- Modules

  if(subdet=="TIB")
  {
	  //TIB-
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121381, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121382, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121385, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121386, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121389, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121390, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  //TIB+
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125862, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125866, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125870, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);

	  //OTHER TIB MODULES, ONLY USED IN FULL SCANS, THEREFORE NOT TO BE USED FOR EVOLUTION PLOTS !
	  /*
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121605, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121606, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121609, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121610, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121613, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369121614, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125861, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125865, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TIB", type, "", NF, runs, lumis, 369125869, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);*/
  }

  else if(subdet=="TOB")
  {
  //TOB
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815081, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815082, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815121, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815122, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815161, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815162, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815201, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815202, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815241, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815242, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815281, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362815282, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);


	  // TOB + 1.3.1.6 //NEW -- ADDED 04/2018
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329011, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329021, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329051, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329061, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329091, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329101, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329131, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329141, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329171, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329181, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329211, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TOB", type, "", NF, runs, lumis, 4362329221, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
  }


  else if(subdet=="TEC")
  {
	  //TEC
	  //1 sensor
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701481960, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482280, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482400, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482000, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482320, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482040, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482360, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  //2 sensors
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482651, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482652, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482961, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482962, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483241, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483242, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);

	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483361, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483362, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482611, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482612, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482661, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482662, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483001, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483002, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482621, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482622, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482921, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701482922, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483321, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
	  DrawOneModule(dirname, "TEC", type, "", NF, runs, lumis, 4701483322, usefluence, true, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);
  }


  //TIB & TOB --> ref = 160497 -- TEC --> ref = 246963 (
  //DrawDiffModules_SmallScan(dirname, subdet, type, "160497", NF, runs, lumis, usefluence, use_curvature);

  //Superimpose_DrawDiffModules_SmallScan(dirname, subdet, "280385", NF, runs, lumis, usefluence);
}









//--------------------------------------------
// ##     ## ########  ########   #######  ########     ########  ######## ########     ##          ###    ##    ## ######## ########
// ##     ## ##     ## ##     ## ##     ## ##     ##    ##     ## ##       ##     ##    ##         ## ##    ##  ##  ##       ##     ##
// ##     ## ##     ## ##     ## ##     ## ##     ##    ##     ## ##       ##     ##    ##        ##   ##    ####   ##       ##     ##
// ##     ## ##     ## ########  ##     ## ########     ########  ######   ########     ##       ##     ##    ##    ######   ########
//  ##   ##  ##     ## ##   ##   ##     ## ##           ##        ##       ##   ##      ##       #########    ##    ##       ##   ##
//   ## ##   ##     ## ##    ##  ##     ## ##           ##        ##       ##    ##     ##       ##     ##    ##    ##       ##    ##
//    ###    ########  ##     ##  #######  ##           ##        ######## ##     ##    ######## ##     ##    ##    ######## ##     ##
//--------------------------------------------

/**
 * Compute the mean Vfd drop (wrt initial lab values) for each layer of the partition
 * Also print the mean drops for 1) all modules, 2) only small scans modules => Comparison
 */
pair<vector<double>, vector<double> > Compute_Mean_Vfd_Drop_Per_Layer(TString dirname, TString subdet, TString antype, TString run, double lumi, bool return_fluence)
{
	bool debug = false;

	bool use_SmallScanModules_only = false; //If false, use all available modules

	pair< vector<double>, vector<double> > result;

	int size = 4;
	if(subdet == "TEC") {size = 7;}
	else if(subdet == "TOB") {size = 6;}
	else if(subdet == "TID") {size = 3;}
	vector<double> v_mean_vfd_layers(size);
	vector<int> v_nofModules_layers(size);
	vector<double> v_mean_fluence(size);

	TString file_tmp = "./detid_lists/"+subdet+"_detid_list.txt";
	ifstream detid_list_small(file_tmp.Data() );

	VdeplRef SubdetRef;
    SubdetRef.loadFile(subdet.Data() );

	vector<ULong64_t> v_smallDetids;

	string line = "";
	while(getline(detid_list_small, line) )
	{
		//For TEC & TOB, add a suffix (0, 1, 2) to distinguish sensors !
		if(subdet == "TEC" || subdet == "TOB")
		{
			double tmp = Convert_TString_To_Number(line) * 10;
			v_smallDetids.push_back(tmp);
			tmp = Convert_TString_To_Number(line) * 10 + 1;
			v_smallDetids.push_back(tmp);
			tmp = Convert_TString_To_Number(line) * 10 + 2;
			v_smallDetids.push_back(tmp);
		}
		else
		{
			v_smallDetids.push_back(Convert_TString_To_Number(line));
		}
	}


	TString inputfile_name = dirname+"/all_modules/DECO_allModules_"+antype+"_"+subdet+"_line_"+run+".root";

	if(!Check_File_Existence(inputfile_name) || use_SmallScanModules_only)
	{
		cout<<endl<<"Using small scan modules only !"<<endl<<endl;
		inputfile_name = dirname+"/DECO_"+antype+"_"+subdet+"_line_"+run+".root";
		if(!Check_File_Existence(inputfile_name)) {cout<<FRED("No file : ")<<inputfile_name<<endl; return result;}
	}

    TFile *f = TFile::Open(inputfile_name);
    cout<<endl<<endl<<"File "<<inputfile_name<<" opened"<<endl;

	TTree* tr = (TTree*) f->FindObjectAny("tout");

	ULong64_t odetid;
	int olayer;
	double odepvolt;
	double oerrdepvolt;
	double oplateau;
	double ofitchisquare;
	int ofitstatus;
	double olastpty;
	double ochi2;

    tr->SetBranchAddress("DETID",&odetid);
    tr->SetBranchAddress("LAYER",&olayer);
    tr->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
    tr->SetBranchAddress("DEPVOLT",&odepvolt);
    tr->SetBranchAddress("PLATEAU",&oplateau); // corrected for ILeak or not
    tr->SetBranchAddress("FITCHI2",&ofitchisquare);
    tr->SetBranchAddress("FITSTATUS",&ofitstatus); // ndf
    tr->SetBranchAddress("LASTPOINTS",&olastpty);
    tr->SetBranchAddress("CHI2",&ochi2); // significance min

	UInt_t nentries = tr->GetEntries();

	double mean_vfd_smallModules = 0;
	double mean_vfd_allModules = 0;

	cout<<"//--------------------------------------------"<<endl;
	cout<<"nentries = "<<nentries<<endl<<endl;
	int nof_skippedModules = 0; //Some modules are not found in Vdepl Ref file
    for(UInt_t ie = 0; ie < nentries; ie++)
    {
		tr->GetEntry(ie);

		//-- use only first TEC wheels for testing
		// if(subdet == "TEC")
		// {
		// 	if(GetWheel(odetid) != 4) {continue;}
		// }

		double vfd_drop = fabs(odepvolt - SubdetRef.GetVdepl(odetid));

		if(vfd_drop == 0 || SubdetRef.GetVdepl(odetid) == 0) {nof_skippedModules++; continue;}

		if(return_fluence) {v_mean_fluence[olayer-1]+= ComputeFluence(lumi, odetid, subdet);}

		// cout<<endl<<"odetid = "<<odetid<<endl;

		for(int idet=0; idet<v_smallDetids.size(); idet++)
		{
			if(odetid == v_smallDetids[idet])
			{
				mean_vfd_smallModules+= vfd_drop;
			}
		}

		// cout<<"vfd_drop = "<<vfd_drop<<endl;

		mean_vfd_allModules+= vfd_drop;
		// if(debug) {cout<<"mean_vfd_allModules = "<<mean_vfd_allModules<<endl;}

		//NB : vector index start at 0
		v_mean_vfd_layers[olayer-1]+= vfd_drop;
		v_nofModules_layers[olayer-1]++;

		if(debug) {cout<<"v_mean_vfd_layers[olayer-1] = "<<v_mean_vfd_layers[olayer-1]<<endl;
		}
	} //end entries loop


	mean_vfd_smallModules/= v_smallDetids.size(); //Small scan modules
	mean_vfd_allModules/= (nentries-nof_skippedModules); //All modules
	for(int ilayer=0; ilayer<v_mean_vfd_layers.size(); ilayer++)
	{
		if(!v_nofModules_layers[ilayer]) {v_mean_vfd_layers[ilayer] = 1;}
		else {v_mean_vfd_layers[ilayer]/= v_nofModules_layers[ilayer];}

		if(debug) cout<<"v_mean_vfd_layers[ilayer] = "<<v_mean_vfd_layers[ilayer]<<endl;
	}

	cout<<"--- Partition : "<<subdet<<endl;
	if(return_fluence)
	{
		for(int ilayer=0; ilayer<size; ilayer++)
		{
			v_mean_fluence[ilayer]/= v_nofModules_layers[ilayer];

			if(debug) cout<<"mean_fluence layer "<<ilayer+1<<" = "<<v_mean_fluence[ilayer]<<endl;
		}
	}

	if(debug) cout<<"* Mean VFD difference [Small Scan Modules] : "<<mean_vfd_smallModules<<endl;
	if(debug) cout<<"* Mean VFD difference [All Modules] : "<<mean_vfd_allModules<<endl;

	f->Close();

	result.first = v_mean_vfd_layers;
	result.second = v_mean_fluence;

	return result;
	// return v_mean_vfd_layers;
}




//--------------------------------------------
// ########  ##        #######  ########    ##     ## ########  ########   #######  ########
// ##     ## ##       ##     ##    ##       ##     ## ##     ## ##     ## ##     ## ##     ##
// ##     ## ##       ##     ##    ##       ##     ## ##     ## ##     ## ##     ## ##     ##
// ########  ##       ##     ##    ##       ##     ## ##     ## ########  ##     ## ########
// ##        ##       ##     ##    ##        ##   ##  ##     ## ##   ##   ##     ## ##
// ##        ##       ##     ##    ##         ## ##   ##     ## ##    ##  ##     ## ##
// ##        ########  #######     ##          ###    ########  ##     ##  #######  ##
//--------------------------------------------

/**
 * Plot the mean Vfd drop (wrt initial lab values) for each layer
 */

void Plot_Mean_Vfd_Drop_Per_Layer(TString dirname, TString antype, TString run = "", double lumi = 0)
{
	bool plot_fluence = true; // false <-> do not superimpose fluence for each layer //faster

	vector<TString> v_subdet;
	v_subdet.push_back("TIB");
    v_subdet.push_back("TOB");
	v_subdet.push_back("TID");
	v_subdet.push_back("TEC");

	//If want to plot multiple runs at once, remove 'run' arg and list the runs here instead !
	vector<TString> v_runs;
	vector<double> lumis;
	if(run == "")
	{
		v_runs.push_back("314574"); lumis.push_back(97.37+29.46);
		v_runs.push_back("303824"); lumis.push_back(70.55+29.46);
		v_runs.push_back("295376"); lumis.push_back(45.71+29.46);
	}
	else {v_runs.push_back(run); lumis.push_back(lumi);}

	vector<TH1F*> v_h;
	if(v_runs.size() > 1) {v_h.resize(v_runs.size());} //1 histo per run
	else {v_h.resize(v_subdet.size());} //else 1 histo per subdet (-> different colors)

	for(int i=0; i<v_h.size(); i++)
	{
		v_h[i] = new TH1F("", "", 20, 0, 20);
	}

    TH1F* h_flu = new TH1F("", "", 20, 0, 20);
    TH1F* h_color = new TH1F("", "", 20, 0, 20); //Overlay colored bins for layers with thick sensors

	TLegend* leg = 0;
	if(plot_fluence) {leg = new TLegend(0.24, 0.75, 0.46, 0.89);}
	else {leg = new TLegend(0.73, 0.75, 0.95, 0.88);}
	// leg = new TLegend(0.73, 0.75, 0.95, 0.88);
	leg->SetBorderSize(0);
    gStyle->SetLegendFillColor(0);
    gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.03);

	vector<int> v_colors;
	v_colors.push_back(kOrange);
	v_colors.push_back(kRed);
	v_colors.push_back(kPink-9);
	v_colors.push_back(kViolet-1);
	v_colors.push_back(kBlack);
	v_colors.push_back(kBlue);
	v_colors.push_back(kAzure+6);


	TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
    c1->SetTopMargin(0.1);
	c1->SetBottomMargin(0.1);
	c1->SetBottomMargin(0.1);
	if(plot_fluence)
	{
		c1->SetRightMargin(0.1);
		gPad->SetTicky(0); //remove right-hand ticks
	}


	for(int irun=0; irun<v_runs.size(); irun++)
	{
		int ibin_tmp = 0;
		for(int idet=0; idet<v_subdet.size(); idet++)
		{
			pair<vector<double>, vector<double> > v_mean_vfd_fluence = Compute_Mean_Vfd_Drop_Per_Layer(dirname, v_subdet[idet], antype, v_runs[irun], lumis[irun], plot_fluence);

			for(int ibin=0; ibin<v_mean_vfd_fluence.first.size(); ibin++)
			{
				h_flu->SetBinContent(ibin_tmp + ibin + 1, v_mean_vfd_fluence.second[ibin]); //set layer fluence

				if(v_runs.size() > 1) //1 histo per run
				{
					v_h[irun]->SetBinContent(ibin_tmp + ibin + 1, v_mean_vfd_fluence.first[ibin]); //Binning starts at 1
				}
				else //1 histo per subdet
				{
                    v_h[idet]->SetBinContent(ibin_tmp + ibin + 1, v_mean_vfd_fluence.first[ibin]); //Binning starts at 1

                    if(idet == 1 || (idet == 3 && ibin > 3) ) {h_color->SetBinContent(ibin_tmp + ibin + 1, v_mean_vfd_fluence.first[ibin]);} //Only color few TOB/TEC bins
				}
				// cout<<"SetBinContent bin "<<ibin_tmp + ibin + 1<<", v_mean_vfd_fluence.first[ibin] = "<<v_mean_vfd_fluence.first[ibin]<<endl;
			}

			ibin_tmp+= v_mean_vfd_fluence.first.size();
		}
	}

	//text labels
	const Int_t nx = 20;
	// const char *labels[nx]  = {"L1","L2","L3","L4","R1","R2","R3","L1","L2","L3","L4","L5","L6","R1","R2","R3","R4","R5","R6","R7"};
	const char *labels[nx]  = {"TIB L1","TIB L2","TIB L3","TIB L4","TOB L1","TOB L2","TOB L3","TOB L4","TOB L5","TOB L6","TID R1","TID R2","TID R3","TEC R1","TEC R2","TEC R3","TEC R4","TEC R5","TEC R6","TEC R7"};

	for(int i=1;i<=nx;i++) v_h[0]->GetXaxis()->SetBinLabel(i,labels[i-1]);

	v_h[0]->GetXaxis()->LabelsOption("v");

	int nof_iter = 0; //need to loop either on runs (if there are >1) or on subdets
	if(v_runs.size() > 1) {nof_iter = v_runs.size();}
	else {nof_iter = v_subdet.size();}

	//Set y-axis maximum (leave space for legend)
	v_h[0]->SetMaximum(v_h[0]->GetMaximum() * 1.2);

	for(int iter=0; iter<nof_iter; iter++)
	{
		// if(v_runs.size() > 1) {leg->AddEntry(v_h[iter], v_runs[iter], "F");}
		// else {leg->AddEntry(v_h[iter], v_subdet[iter], "F");}
		// v_h[iter]->SetFillColor(v_colors[iter]);
		// v_h[iter]->SetLineColor(kBlack);
		// v_h[iter]->GetXaxis()->SetTitle("Barrel Layers / End Cap Rings");
	    // v_h[iter]->GetXaxis()->SetTitleSize(.04);
		//v_h[iter]->GetXaxis()->SetLabelSize(.05);
		// v_h[iter]->GetXaxis()->SetTitleOffset(1.2);

		v_h[iter]->GetYaxis()->SetTitle("Full depletion voltage drop [V]");
		v_h[iter]->GetYaxis()->SetTitleSize(.04);
		v_h[iter]->GetYaxis()->SetTitleOffset(1.4);

		v_h[iter]->SetMinimum(0.);

		v_h[iter]->Draw("hist same");
	}

    h_color->SetFillColor(kGray);
    h_color->Draw("hist same");

    TGaxis *axis = 0;
	if(plot_fluence)
	{
		//scale hint1 to the pad coordinates
		Float_t rightmax = 1.25 * h_flu->GetMaximum();
		Float_t scale = v_h[0]->GetMaximum()/rightmax;
		h_flu->Scale(scale);
		// Float_t rightmax = h_flu->GetMaximum();
		// h_flu->Scale(scale * 0.8);
        h_flu->SetLineColor(kRed);
        h_flu->SetLineWidth(3);
        h_flu->SetLineStyle(2); //dashed

		// cout<<"h_flu->GetMaximum() "<<h_flu->GetMaximum()<<endl;

		h_flu->Draw("hist same");

        //draw an axis on the right side
		axis = new TGaxis(20, 0, 20, v_h[0]->GetMaximum(), 0, rightmax * pow(10, -12), 510,"+L");
		// axis = new TGaxis(20, 0, 20, h_flu->GetMaximum(), 0, rightmax * pow(10, -12), 510,"+L");

		axis->SetTickLength(0.03);
	    axis->SetLabelSize(0.035);
		axis->SetLabelFont(42);
		axis->SetTitleFont(42);
	    axis->SetNoExponent(kTRUE);
	    // axis->SetLabelOffset(-0.008);

		axis->SetTitle("Simulated fluence [10^{12} MeV neutron equivalent . cm^{-2}]");
		axis->SetTitleSize(.035);
		axis->SetTitleOffset(1.3);

        axis->SetLineColor(kRed);
        axis->SetLabelColor(kRed);
        axis->Draw("same");
	}


	//Legend
	leg->AddEntry(v_h[0], "320 #mum sensors", "F");
	leg->AddEntry(h_color, "500 #mum sensors", "F");

	//superimpose lines at the xbins positions
	const Int_t nxbins = 20;
	Double_t xbins[nxbins+1];
	for(int i=0; i<nxbins+1; i++)
	{
		xbins[i] = i;
	}
	TLine l;
	c1->Update();
	Double_t ymin = c1->GetUymin();
	Double_t ymax = c1->GetUymax();
	l.SetLineStyle(2);
	for (Int_t ibin=1;ibin<nxbins;ibin++)
	{
		if(v_runs.size() > 1) {ymax = v_h[0]->GetBinContent(ibin);} //plot lines according to histo of a single run
		else //if 1 histo per subdet, need to loop on all subdets to draw proper lines
		{
			//-- HARD-CODED -- order of layers / rings
			if(ibin > 0 && ibin < 5) {ymax = v_h[0]->GetBinContent(ibin);} //TIB histo
			else if(ibin > 4 && ibin < 11) {ymax = v_h[1]->GetBinContent(ibin);} //TOB histo
			else if(ibin > 10 && ibin < 14) {ymax = v_h[2]->GetBinContent(ibin);} //TID histo
			else if(ibin > 13) {ymax = v_h[3]->GetBinContent(ibin);} //TEC histo
		}

		l.DrawLine(xbins[ibin],ymin,xbins[ibin],ymax);
	}

	leg->Draw("same");


//----------------
	// CAPTIONS //
//----------------

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c1->GetLeftMargin(),0.93,cmsText);

	bool writeExtraText = false;
	TString extraText   = "Preliminary";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c1->GetLeftMargin() + 0.1, 0.932, extraText);

    TString lumi_text   = "29.45 fb^{-1} (Run 1) + 70.55 fb^{-1} (Run 2)";
	latex.SetTextFont(42);
	latex.SetTextSize(0.03);
	latex.DrawLatex(0.60, 0.93, lumi_text);

	c1->SaveAs("Vfd_Drop_Per_Layer.png");
	// c1->SaveAs("Vfd_Drop_Per_Layer.pdf");

	delete c1; c1 = NULL;
	for(int i=0; i<v_h.size(); i++)
	{
		delete v_h[i]; v_h[i] = NULL;
	}

	delete h_flu;
    delete h_color;
	delete leg;
    if(plot_fluence) {delete axis;}
	// delete axis;

	return;
}





//--------------------------------------------
// ##     ## ##     ## ##       ######## ####     ######   ######     ###    ##    ##  ######
// ###   ### ##     ## ##          ##     ##     ##    ## ##    ##   ## ##   ###   ## ##    ##
// #### #### ##     ## ##          ##     ##     ##       ##        ##   ##  ####  ## ##
// ## ### ## ##     ## ##          ##     ##      ######  ##       ##     ## ## ## ##  ######
// ##     ## ##     ## ##          ##     ##           ## ##       ######### ##  ####       ##
// ##     ## ##     ## ##          ##     ##     ##    ## ##    ## ##     ## ##   ### ##    ##
// ##     ##  #######  ########    ##    ####     ######   ######  ##     ## ##    ##  ######
//--------------------------------------------

void Plot_Mean_Vfd_Drop_Per_Layer_MultipleScans(TString dirname, TString antype="ClusterWidth")
{
	bool use_fluence = true; // false <-> do not superimpose fluence for each layer //faster
	bool read_flu_from_file = true;

	// ofstream file_out("fluence_per_layer.txt"); //write the mean fluences to txt file //NB : comment to avoid overwrite
	ifstream file_in("Fluence_perLayer_MultiScans.txt"); //read the mean fluences from txt file


	vector<TString> v_subdet;
	v_subdet.push_back("TIB");
	v_subdet.push_back("TOB");
	v_subdet.push_back("TEC");
	v_subdet.push_back("TID");

	//If want to plot multiple runs at once, remove 'run' arg and list the runs here instead !
	vector<TString> v_runs; vector<double> lumis;

	// v_runs.push_back("170000");lumis.push_back(1.44);
	v_runs.push_back("190459");lumis.push_back(6.15);
	v_runs.push_back("193928");lumis.push_back(7.41);
	v_runs.push_back("246963");lumis.push_back(0.001+29.46); //Full, 0T
	v_runs.push_back("271056");lumis.push_back(4.26+29.46); //Full, No B field
	v_runs.push_back("295376");lumis.push_back(45.71+29.46); //-- FULL
	v_runs.push_back("303824");lumis.push_back(70.55+29.46); //-- FULL (~100fb-1)
	v_runs.push_back("314574");lumis.push_back(97.37+29.46); //-- FULL (-20°)

	TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
	c1->SetTopMargin(0.1);
	c1->SetBottomMargin(0.1);
	c1->SetRightMargin(0.15);

	// TLegend* leg = new TLegend(0.73, 0.75, 0.95, 0.88);
	TLegend* leg = 0;
	// leg = new TLegend(0.70, 0.50, 0.90, 0.88);
	leg = new TLegend(0.86, 0.20, 1.06, 0.88);
	leg->SetBorderSize(0);
	gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.03);

	vector<int> v_color; //Store colors, according on the subdets which are booked
	for(int idet=0; idet<v_subdet.size(); idet++)
	{
		if(v_subdet[idet] == "TIB")
		{
			v_color.push_back(kRed);
			v_color.push_back(kPink+10);
			v_color.push_back(kOrange);
			v_color.push_back(kRed+2);
		}
		else if(v_subdet[idet] == "TOB")
		{
			v_color.push_back(kBlue+4);
			v_color.push_back(kBlue-2);
			v_color.push_back(kBlue+1);
            v_color.push_back(kBlue-4);
			v_color.push_back(kBlue-7);
			v_color.push_back(kBlue-10);
		}
		else if(v_subdet[idet] == "TEC")
		{
			v_color.push_back(kGreen-7);
			v_color.push_back(kGreen);
			v_color.push_back(kGreen+1);
			v_color.push_back(kGreen+2);
			v_color.push_back(kYellow+1);
			v_color.push_back(kYellow+2);
			v_color.push_back(kYellow-6);
		}
		else if(v_subdet[idet] == "TID")
		{
			v_color.push_back(kMagenta-7);
			v_color.push_back(kMagenta);
			v_color.push_back(kMagenta+2);
		}
	}



	vector<TGraph*> v_graphs(20); //1 tgraph per layer
	for(int ig=0; ig<v_graphs.size(); ig++)
	{
		v_graphs[ig] = new TGraph(v_runs.size() );
	}

	vector<TString> v_legend(20); //Associated legend to each layer

	//Can store 1 value (<-> mean Vfd drop) for each layer of each subdet
	vector<vector<double> > v_meanVfdDrop_PerLayer_PerSubdet(v_subdet.size() );
	vector<vector<double> > v_meanFluence_PerLayer_PerSubdet(v_subdet.size() );

	for(int irun=0; irun<v_runs.size(); irun++) //For each scan
	{
		int index_layer = 0; //need to relate layer <-> tgraph

		for(int idet=0; idet<v_subdet.size(); idet++) //For each subdet
		{
			pair<vector<double>, vector<double> > v_mean_vfd_fluence = Compute_Mean_Vfd_Drop_Per_Layer(dirname, v_subdet[idet], antype, v_runs[irun], lumis[irun], !read_flu_from_file);

			v_meanVfdDrop_PerLayer_PerSubdet[idet] = v_mean_vfd_fluence.first; //Get mean Vfd drop of each layer
			v_meanFluence_PerLayer_PerSubdet[idet] = v_mean_vfd_fluence.second; //Get mean fluence of each layer

			//For each layer
			for(int ilayer=0; ilayer<v_meanVfdDrop_PerLayer_PerSubdet[idet].size(); ilayer++)
			{
				//-- Can select specific layers here
				// if(v_subdet[idet] == "TOB" && ilayer+1 > 2) {continue;}
				// if(v_subdet[idet] == "TIB" && ilayer+1 > 2) {continue;}

				double x = 0;
				if(!use_fluence)
				{
					x = lumis[irun];
				}
				else if(read_flu_from_file)
				{
					string line;
					getline(file_in, line);
					stringstream ss(line); int run; string det;
					ss>>run>>det>>x; //read mean flu
				}
				else
				{
					x = v_meanFluence_PerLayer_PerSubdet[idet][ilayer];
					// file_out<<v_runs[irun]<<" "<<v_subdet[idet]<<" "<<x<<endl; //write mean flu
				}

				v_graphs[index_layer]->SetPoint(irun, x, -v_meanVfdDrop_PerLayer_PerSubdet[idet][ilayer]);

				cout<<"-- Graph "<<index_layer<<" / ipt "<<irun<<" / x = "<<x<<" / y = "<<v_meanVfdDrop_PerLayer_PerSubdet[idet][ilayer]<<endl;


				if(!irun) //Add legend entries only once
				{
					if(v_subdet[idet]=="TIB")
					{
						v_graphs[index_layer]->SetMarkerStyle(kFullCircle);
					}
					else if(v_subdet[idet]=="TOB")
					{
						v_graphs[index_layer]->SetMarkerStyle(kFullSquare);
					}
					else if(v_subdet[idet]=="TEC")
					{
						v_graphs[index_layer]->SetMarkerStyle(kFullTriangleUp);
						v_graphs[index_layer]->SetMarkerSize(2);
					}
					else if(v_subdet[idet]=="TID")
					{
						v_graphs[index_layer]->SetMarkerStyle(kFullDiamond);
						v_graphs[index_layer]->SetMarkerSize(2);
					}
					v_graphs[index_layer]->SetMarkerColor(v_color[index_layer]);
					v_graphs[index_layer]->SetLineColor(v_color[index_layer]);


					// if(v_subdet[idet]=="TIB") {v_graphs[index_layer]->SetMarkerColor(kOrange + ilayer); v_graphs[index_layer]->SetLineColor(kOrange + ilayer);}
					// else if(v_subdet[idet]=="TOB") {v_graphs[index_layer]->SetMarkerColor(kAzure + ilayer); v_graphs[index_layer]->SetLineColor(kAzure + ilayer);}
					// else if(v_subdet[idet]=="TEC") {v_graphs[index_layer]->SetMarkerColor(kGreen + ilayer);}
					// else if(v_subdet[idet]=="TID") {v_graphs[index_layer]->SetMarkerColor(kMagenta + ilayer);}


					v_legend[index_layer] = v_subdet[idet] + " ";
					if(v_subdet[idet] == "TEC" || v_subdet[idet] == "TID") {v_legend[index_layer]+= "R";}
					else {v_legend[index_layer]+= "L";}
					v_legend[index_layer]+= Convert_Number_To_TString(ilayer+1);

					leg->AddEntry(v_graphs[index_layer], v_legend[index_layer], "P");
				}

				index_layer++;
			} //ilayer loop
		} //idet loop
	} //irun loop

	TH1F* h = v_graphs[0]->GetHistogram();
	// h->SetMinimum(0.);
	h->SetMaximum(0.);
	h->GetYaxis()->SetTitle("Full depletion voltage drop [V]");
	if(use_fluence) {h->GetXaxis()->SetTitle("Simulated fluence [1 MeV neutron equivalent . cm^{-2}]");}
	else {h->GetXaxis()->SetTitle("Integrated luminosity [fb^{-1}]");}
    h->GetXaxis()->SetTitleSize(.04);
    h->GetXaxis()->SetTitleOffset(1.18);
	h->GetYaxis()->SetTitleSize(.04);
	h->GetYaxis()->SetTitleOffset(1.4);

	h->Draw();

	for(int ig=0; ig<v_graphs.size(); ig++)
	{
		v_graphs[ig]->Draw("same PL");
	}

	leg->Draw("same");

//----------------
	// CAPTIONS //
//----------------

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c1->GetLeftMargin(),0.93,cmsText);

	bool writeExtraText = false;
	TString extraText   = "Preliminary";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c1->GetLeftMargin() + 0.1, 0.932, extraText);

    TString lumi_text   = "29.45 fb^{-1} (Run 1) + 70.55 fb^{-1} (Run 2)";
	latex.SetTextFont(42);
	latex.SetTextSize(0.03);
	latex.DrawLatex(0.60, 0.93, lumi_text);

	c1->SaveAs("Vfd_Drop_Per_Layer_MultiScans.png");
	// c1->SaveAs("Vfd_Drop_Per_Layer_MultiScans.pdf");


	TFile* f_output = new TFile("TGraphs_VFDevol.root", "RECREATE");
	f_output->cd();
	for(int ig=0; ig<v_graphs.size(); ig++)
	{
		v_graphs[ig]->Write(v_legend[ig]);
	}

	for(int i=0; i<v_graphs.size(); i++)
	{
		delete v_graphs[i]; v_graphs[i] = NULL;
	}
	delete leg;
	delete c1; c1 = NULL;

	delete f_output; f_output = NULL;

	return;
}








/**
 * Idea : check 2D plot CW@300V vs VFD to see if clear correlations between the 2 (-> not found ; but can see what are the populations of Vfd)
 */
/*
void Plot_CW300V_VS_Vfd(TString dirname, TString subdet, TString antype, TString run)
{
	// TString inputfile_name = dirname+"/all_modules/DECO_allModules_"+antype+"_"+subdet+"_line_"+run+".root";
	TString inputfile_name = "./DECO_allModules_"+antype+"_"+subdet+"_line_"+run+".root";

    TFile *f = TFile::Open(inputfile_name);
    cout<<endl<<endl<<"File "<<inputfile_name<<" opened"<<endl;

	TTree* tr = (TTree*) f->Get("tout");
	cout<<"Tree opened" <<endl;


	ULong64_t odetid;
	int olayer;
	double odepvolt;
	double oerrdepvolt;
	double CW300V;

    tr->SetBranchAddress("DETID",&odetid);
    tr->SetBranchAddress("LAYER",&olayer);
    tr->SetBranchAddress("DEPVOLT",&odepvolt);
	tr->SetBranchAddress("ERRDEPVOLT",&oerrdepvolt);
	tr->SetBranchAddress("CW300V",&CW300V); // CW value at 300V

	UInt_t nentries = tr->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	int nlayers = 4;
	for(int ilayer=1; ilayer<nlayers+1; ilayer++)
	{
		TH2F* h = new TH2F("", "", 15, 0, 150, 10, 1.5, 4.5);

	    for(UInt_t ie = 0; ie <nentries; ie++)
	    {
			tr->GetEntry(ie);

			if(olayer != ilayer) {continue;}

			h->Fill(odepvolt, CW300V);
		}

		h->GetYaxis()->SetTitle("CW at 300V");
		h->GetYaxis()->SetTitleOffset(0.8);
		h->GetXaxis()->SetTitle("Vfd");

		TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
		c1->SetRightMargin(0.15);
		h->Draw("colz");

		c1->SaveAs("CW300V_VS_Vfd_TIBL"+Convert_Number_To_TString(ilayer)+".png");

		delete c1; c1 = NULL;
		delete h; h = NULL;
	}

	f->Close();

	return;
}


 //Idea Eric :  plot CW vs Seeding strip for 1 partition / 1 scan. Check if border effects, and if changes at different Vfds
void Test()
{
	// TString inputfile_name = dirname+"/all_modules/DECO_allModules_"+antype+"_"+subdet+"_line_"+run+".root";
	TString inputfile_name = "./test.root";
    TFile *f = TFile::Open(inputfile_name);
    cout<<endl<<endl<<"File "<<inputfile_name<<" opened"<<endl;

	TTree* tr = (TTree*) f->Get("T");
	cout<<"Tree opened" <<endl;

	TH2F* h = new TH2F("", "", 20, 0, 128*6, 7, 2, 8);

	TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);

	ULong64_t odetid = 0;
	ULong64_t seed = 0;
	double voltage = 0, cw = 0;

    tr->SetBranchAddress("DetID",&odetid);
    tr->SetBranchAddress("seed",&seed);
	tr->SetBranchAddress("voltage",&voltage);
	tr->SetBranchAddress("cw",&cw);

	UInt_t nentries = tr->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	for(int ientry=0; ientry<nentries; ientry++)
	{
		tr->GetEntry(ientry);
		h->Fill(seed, cw);
		// cout<<"seed "<<seed<<" / cw "<<cw<<endl;

	}

	h->Draw("colz");

	c1->SaveAs("test.png");

	delete h;
	delete f;
	delete c1;

	return;
}
*/









//--------------------------------------------
// ##     ##    ###    #### ##    ##
// ###   ###   ## ##    ##  ###   ##
// #### ####  ##   ##   ##  ####  ##
// ## ### ## ##     ##  ##  ## ## ##
// ##     ## #########  ##  ##  ####
// ##     ## ##     ##  ##  ##   ###
// ##     ## ##     ## #### ##    ##
//--------------------------------------------

int main(int argc, char *argv[])
{
  Modified_tdr_style();

  string dirname="./DECO_files";

  bool use_curvature = false; //true-->kink ; false-->lines


  bool usefluence = true; //Draw fluence axis
  bool superimpose_simu = true; //Superimpose simulation curves (only TIB for now)
  bool draw_vdep_lab = true; //Draw lab measurement of initial Vfd
  bool draw_fit = false; //Draw linear fit of vfd evolution



//-- ACTIONS --//
  bool draw_vfd_evolution_plots = true; //Vfd evol plots
  bool draw_vfd_relative_evolution_plots = false; //Vfd relative evol plots
  bool draw_vfd_relative_evolution_superimposed_plots = false; //Vfd relative evol plots with both observables drawn
  bool compute_mean_drop = false; //Compute mean Vfd drop for each Layer
  bool compute_mean_drop_multipleScans = false; //Compute mean Vfd drop for each Layer, for several scans
  bool plot_cw_vs_vfd = false;


//-- Choose the observables
  vector<string> v_analysis;
  // v_analysis.push_back("Signal");
  v_analysis.push_back("ClusterWidth");

//-- Choose the subdet
  vector<string> v_subdet;
  v_subdet.push_back("TIB");
  // v_subdet.push_back("TOB");
  // v_subdet.push_back("TEC");





//--------------------------------------------
//--- Automatized from here
//--- Can remove/add below scans if desired
	std::cout << endl << "Starting DrawKinkVsLumi.exe" << std::endl;

	for(int i=0; i<v_analysis.size(); i++)
	{
		for(int j=0; j<v_subdet.size(); j++)
		{
			if(v_subdet[j] != "TIB") {superimpose_simu = false;} //Only available for TIB yet

			//FIll vectors here in order to read vector values and get rid of faulty runs accordingly
			vector<string> runs;
			vector<float> lumis;  //NB : Lumi Run I = 29.46 fb-1
			//Old runs (10)
			//runs.push_back("160497");lumis.push_back(0.045); //Full
			runs.push_back("170000");lumis.push_back(1.44); //Full
			runs.push_back("190459");lumis.push_back(6.15); //Full
			runs.push_back("193541");lumis.push_back(7.19);
			runs.push_back("193928");lumis.push_back(7.41); //Full
			runs.push_back("199832");lumis.push_back(15.14);
			//runs.push_back("200786");lumis.push_back(16.98); //Full
			if(v_subdet[j] != "TOB" || v_analysis[i] != "Signal")
			{runs.push_back("203832");lumis.push_back(21.18);}
			runs.push_back("208339");lumis.push_back(28.57);
			runs.push_back("211797");lumis.push_back(29.45);

			//2015 (4) -- Run 2
			runs.push_back("246963");lumis.push_back(0.001+29.46); //Full, 0T
			runs.push_back("254790");lumis.push_back(0.17+29.46);
			if(v_subdet[j] != "TOB" || v_analysis[i] != "Signal")
			{runs.push_back("258443");lumis.push_back(2.09+29.46);} //Mostly bad curves for TOB
			runs.push_back("262254");lumis.push_back(4.23+29.46);

			//2016 (5)
			runs.push_back("271056");lumis.push_back(4.26+29.46); //Full, No B field
			runs.push_back("274969");lumis.push_back(7.58+29.46);
			runs.push_back("276437");lumis.push_back(14.48+29.46);
			// runs.push_back("276453");lumis.push_back(14.84+29.46); //Full, not completed (15v-195v)
			runs.push_back("278167");lumis.push_back(23.64+29.46);
			// runs.push_back("279865");lumis.push_back(32.12+29.46); //FUll, not completed (225-350V)
			runs.push_back("280385");lumis.push_back(35.06+29.46);
			//runs.push_back("285371");lumis.push_back(45.70+29.46); //P-pb collisions ; not shown in final results (~ outlier)

			//2017 (5)
			// runs.push_back("295324");lumis.push_back(45.71+29.46); //-- FULL -- some pixel FEDs missing, use 295376 instead
			runs.push_back("295376");lumis.push_back(45.71+29.46); //-- FULL
			runs.push_back("298996");lumis.push_back(52.18+29.46);
			runs.push_back("302131");lumis.push_back(65.84+29.46);
			runs.push_back("303824");lumis.push_back(70.55+29.46); //-- FULL (~100fb-1)
			// if(v_subdet[j] != "TOB") {runs.push_back("305862");lumis.push_back(91.65+29.46);} //Low stat TOB -- (ALCARECO issue?)

			//2018
			// runs.push_back("314574");lumis.push_back(97.37+29.46); //-- FULL (-20°)
			// runs.push_back("314755");lumis.push_back(97.37+29.46); //-- FULL (-10)
			// runs.push_back("314756");lumis.push_back(97.37+29.46); //-- FULL (-10) -- few previously excluded PGs
			// runs.push_back("317182");lumis.push_back(113.01+29.46); //-- (ALCARECO issue)
            // runs.push_back("317683");lumis.push_back(119.21+29.46);
			// runs.push_back("320674");lumis.push_back(127.08+29.46);
			// runs.push_back("323374");lumis.push_back(152.45+29.46); //FULL

			//--------------------------------------------
			if(draw_vfd_evolution_plots) {DrawKinkVsLumi(dirname, v_subdet[j], v_analysis[i], runs, lumis, usefluence, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit);} //VFD EVOLUTION, SINGLE MODULES
			if(compute_mean_drop)
			{
				Plot_Mean_Vfd_Drop_Per_Layer(dirname, v_analysis[i], "303824", 70.55+29.46);
				break; //Don't need to repeat for each subdet
			}
			// if(plot_cw_vs_vfd) {Plot_CW300V_VS_Vfd(dirname, v_subdet[j], v_analysis[i], "314574");}


			TString ref = "170000";
			if(v_subdet[j] == "TEC" || v_subdet[j] == "TOB") {ref = "190459";} //170000 "bad scan"

			if(draw_vfd_relative_evolution_plots) {DrawDiffModules_SmallScan(dirname, v_subdet[j], v_analysis[i], ref.Data(), runs.size(), runs, lumis, false, use_curvature, 0, draw_fit);}

			for(int k=3; k<8; k++) //Relative plots for all TEC layers
			{
				if(draw_vfd_relative_evolution_plots) {DrawDiffModules_SmallScan(dirname, v_subdet[j], v_analysis[i], ref.Data(), runs.size(), runs, lumis, false, use_curvature, k, draw_fit);} //RELATIVE VFD EVOLUTION, AVERAGED OVER MODULES
				if(draw_vfd_relative_evolution_superimposed_plots) {Superimpose_DrawDiffModules_SmallScan(dirname, v_subdet[j], ref.Data(), runs.size(), runs, lumis, false, use_curvature, draw_fit);} //RELATIVE VFD EVOL, SUPERIMPOSED FOR BOTH OBSERVABLES
				if(v_subdet[j] != "TEC") {break;}
			}
			//--------------------------------------------
		}
	}

	if(compute_mean_drop_multipleScans) {Plot_Mean_Vfd_Drop_Per_Layer_MultipleScans(dirname, "ClusterWidth");}

	//--------------------------------------------

	return 0;
}

