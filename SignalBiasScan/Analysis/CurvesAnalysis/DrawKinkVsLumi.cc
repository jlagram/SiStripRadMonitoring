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
#include<vector>
#include "TGaxis.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TLatex.h"



//---------------------------


// Correct for the difference of total inelastic crosfs section when center of mass energy change
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


	return fluence; //Implicit calls to destructors of creates instances
}

/*
// Un-correct for CME --> needed to get the lumi value from fluence (which has been corrected for CME)
//Don't consider this for now (minor correction)
double UncorrectForCME(double lumi)
{
	if(lumi>29.46) return lumi;
	else
	{
 		double factor = 74700./73500;

 		return (lumi + 6.12*(factor-1) ) / factor;
	}
}

double GetLumiFromFluence(double fluence, ULong64_t detid)
{
	double lumi_Run1 = 29.46;
	double lumi = -1;

	// Fluence files
	ModFluence DetFluenceRun1;
	DetFluenceRun1.loadFile("../CommonTools/modulesFluence_3500_sigmaTotem.root");
	ModFluence DetFluenceRun2;
	DetFluenceRun2.loadFile("../CommonTools/modulesFluence_7000.root");

	double fluenceRun1 = DetFluenceRun1.GetFluence(detid);
	double fluenceRun2 = DetFluenceRun2.GetFluence(detid);

	double fluence_lumiRun1 = ComputeFluence(lumi_Run1, detid);

	if(fluence < fluence_lumiRun1)
	{
		lumi = fluence / fluenceRun1;
	}
	else
	{
		lumi = (fluence + lumi_Run1*fluenceRun2) / (fluenceRun1 + fluenceRun2);
	}

	return lumi;
}
*/

//Get Luminosity approximate value from fluence
//NB : not possible to get the exact lumi value, since the relation we use b/w lumi & fluence is not linear (cf. ComputeFluence : need to know fraction of lumi collected @ 8TeV, not possible from fluence value)
//---> Approximation
double GetLumiFromFluence(double fluence, ULong64_t detid, TString subdet)
{
	double flu_tmp = 0, lumi=0, mindiff=pow(10,15), step=4.; int i_tmp=0;
	
	if(subdet != "TIB" && subdet != "TOB" && subdet != "TEC" && subdet != "TID") {cout<<"Wrong argument 'subdet' ! Abort"<<endl; return 0;}

	//First, determine approximate lumi within an interval of width "step"
	do
	{
		lumi+= step;
		flu_tmp = ComputeFluence(lumi, detid, subdet);
	}
	while(flu_tmp < fluence);


	lumi-=step; //Go back to beginning of interval, to determine lumi more precisely

	double lumi_tmp=lumi;

	int n_substeps=40; //Divide interval of width "step" into n_substeps intervals
	//---> Check which interval contains the lumi value associated to input fluence
	//---> Return beginning value of interval
	for(int i=0; i<n_substeps; i++)
	{
		flu_tmp = ComputeFluence(lumi_tmp, detid, subdet);
		if(fabs(fluence-flu_tmp) < mindiff) {i_tmp=i; mindiff=fabs(fluence-flu_tmp);}
		else {break;}
		lumi_tmp+=step/n_substeps;
	}

	lumi+= i_tmp*(step/n_substeps);

	return lumi;
}

bool Is_Scan_Bad(TString subdet, TString run, TString observable, ULong64_t detid)
{
    bool isBad = false;

	if(subdet == "TOB" || subdet=="TEC") //low stat, ...
	{
		if(run=="170000" || run=="178367" || run=="180076" || run=="193541" || run=="193928" || run=="254790" || run=="258443" || run=="262254" || run=="178367" || run=="180076" ) {isBad = true;} //Scans w/ low stat or else --> remove for TOB

		else if(run == "208339" && subdet == "TEC" ) {isBad = true;} //Cf. TEC curves: this run makes no sense		
	}
	else if(subdet=="TIB" && run=="302131" && observable=="Signal" && detid==369125862) {isBad=true;} //Line algo failing for this detector
	
	return isBad;
}


//----------------------

void DrawOneModule(string dirname, string subdet, string antype, string ref, const int NF, vector<string> runs, vector<float> lumis, ULong64_t detid, bool useflu=false, bool print=false, bool use_curvature=true, bool superimpose_simu=false, bool draw_vdep_lab=true, bool draw_fit=false)
{
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
  	//if(antype=="ClusterWidth" && runs[i]=="258443") {continue;}

  	string inputfile_name = dirname+"/"+filename+"_"+runs[i]+".root";

    if(!Check_File_Existence(inputfile_name)) {cout<<FRED("No data file : ")<<inputfile_name<<endl; continue;}

    TFile *f = TFile::Open(inputfile_name.c_str());
    //cout<<"File "<<inputfile_name<<" opened"<<endl;

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
    	if( Is_Scan_Bad(subdet, runs[i], antype, odetid) == true) {continue;}
    }


//----------------

	//cout<<"odetid = "<<odetid<<endl;
	//if(odetid== 4701481960) {cout<<"!!! odepvolt = "<<odepvolt<<endl;}

	  if(odetid==detid && odepvolt>=0)
	  { //interested only in 1 module



	   	  lumi = lumis[i];

        //cout<<" --> Write point!";

		g->SetPoint(ipt, lumi, odepvolt);
		g->SetPointError(ipt, 0, oerrdepvolt);

		//cout<<"lumi = "<<lumi<<endl;

		ipt++;

		break; //module has been found --> next entry
	  }
	} //end entries loop

	//cout<<endl;

	//if(useflu) {cout<<lumi<<endl;}

	delete f;

  }//end NF loop


//--- DRAWING ---//

  TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);

  //In case of superposition with simu curve, g is drawn in second
  //Make sure that the y-axis covers all of g points
  double y_tmp = 0, x_tmp=0, y_max=0, y_min=9999;
  for(int i=0; i<g->GetN(); i++)
  {
  	g->GetPoint(i, x_tmp, y_tmp);

  	//if(superimpose_simu) g->SetPoint(i, ComputeFluence(x_tmp,detid), y_tmp, subdet); //only rpz fluence

  	if(y_tmp > y_max) {y_max = y_tmp;}
  	else if(y_tmp < y_min) {y_min = y_tmp;}
  }

  TGraph* g_simu = 0;

  //cout<<"--- Superimpose simulation : Fluence --> Lumi"<<endl;

  if(superimpose_simu)
  {
	if(Check_File_Existence(dirname+"/VdepGraphs_TIB.root") )
	{
		TFile* f_simu = TFile::Open( (dirname+"/VdepGraphs_TIB.root").c_str() );

		TString g_simu_name = "graph_" + Convert_Number_To_TString(detid);
		//cout<<g_simu_name.Data()<<endl;
	
		g_simu = (TGraph*) f_simu->Get(g_simu_name.Data());	
		if(g_simu == 0) {cout<<"Error : g_simu not found !"<<endl; return;}

		for(int i=0; i<g_simu->GetN(); i++)
		{
			g_simu->GetPoint(i, x_tmp, y_tmp);

			//cout<<"i =="<<i<<endl;

			g_simu->SetPoint(i, GetLumiFromFluence(x_tmp, detid, subdet), y_tmp);

			if(y_tmp > y_max) {y_max = y_tmp;}
			else if(y_tmp < y_min) {y_min = y_tmp;}
		}
		delete f_simu;
 	}
 	else {cout<<FRED(""<<dirname<<"/VdepGraphs_TIB.root not found !"<<)<<endl;}
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

  g->SetMinimum(y_min * 0.85);
  g->SetLineWidth(2);

  //if(superimpose_simu) h->GetXaxis()->SetTitle("Fluence [cm^{-2}]"); //single fluence axis
  //else h->GetXaxis()->SetTitle("L_{int} [fb^{-1} ]");
  h->GetXaxis()->SetTitle("L_{int} [fb^{-1} ]");
  h->GetXaxis()->SetTitleSize(.04);

  h->GetYaxis()->SetTitle("V_{FD} [V]");
  h->GetYaxis()->SetTitleSize(.04);
  h->GetYaxis()->SetTitleOffset(1.2);
  //h->GetYaxis()->SetLabelOffset(0.02);

  if(subdet == "TEC") h->SetTitle(Form("Detid %llu - %s R%i", detid, subdet.c_str(), olayer));
  else h->SetTitle(Form("Detid %llu - %s L%i", detid, subdet.c_str(), olayer));

  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.6);
  g->Draw("APL");

  TLatex* latex = new TLatex();
  latex->SetNDC(); // draw in NDC coordinates [0,1]
  latex->SetTextSize(0.03);
  latex->SetTextAlign(13);
  latex->DrawLatex(0.15, 0.86, "CMS Preliminary");



//--- Superimpose Vfd prediction curve from C. Barth
  TH1F* h_simu = 0;

  TLegend *leg = 0;
  if(superimpose_simu && g_simu != 0)
  {
 	  g_simu->SetMarkerColor(kRed); g_simu->SetLineColor(kRed);
	  g_simu->SetLineWidth(2);
	  g_simu->SetMarkerSize(1.6);
	  //g_simu->Draw("APL");
	  g_simu->Draw("PL");

	  leg = new TLegend(.65,.70,.85,.80);
	  gStyle->SetLegendTextSize(0.03);
	  leg->AddEntry(g_simu, "Simulation", "P");
	  leg->AddEntry(g, "Measurement", "P");
	  leg->Draw("same");
  }



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
  		
	double flu_Run1 = ComputeFluence(lumi_Run1, detid, subdet) / pow(10,12);

	double flu_max = ComputeFluence(h->GetXaxis()->GetXmax(), detid, subdet) / pow(10, 12);
	

   //NEED TO DRAW 2 X AXIS SIDE TO SIDE ! BECAUSE FLUENCE FILE DIFFER BETWEEN RUN 1 & 2 !! --> not linear
   TGaxis *axis = new TGaxis(0,h->GetMaximum(),lumi_7TeV,h->GetMaximum(),0,flu_7TeV, 1, "-"); // - : tick marks on negative side ; L : labels on left of tick mark; = : label on same side as marks
   double axis1_length = (lumi_8TeV) / h->GetXaxis()->GetXmax();
   axis->SetTickLength(0.06/axis1_length);
   axis->SetLabelSize(0.035);
   axis->SetNoExponent(kTRUE);
   axis->SetLabelOffset(0.01);
   axis->Draw();

   TGaxis *axis2 = new TGaxis(lumi_7TeV,h->GetMaximum(),lumi_Run1,h->GetMaximum(),flu_7TeV,flu_Run1, 503, "-"); // - : tick marks on negative side ; L : labels on left of tick mark; = : label on same side as marks
   double axis2_length = (lumi_Run1 - lumi_8TeV) / h->GetXaxis()->GetXmax();
   axis2->SetTickLength(0.06/axis2_length);
   axis2->SetLabelSize(0.035);
   axis2->SetNoExponent(kTRUE);
   axis2->SetLabelOffset(0.01);
   axis2->Draw();
   
   TGaxis *axis3 = new TGaxis(lumi_Run1,h->GetMaximum(),h->GetXaxis()->GetXmax(),h->GetMaximum(),flu_Run1,flu_max, 510, "-"); // - : tick marks on negative side ; L : labels on left of tick mark; = : label on same side as marks
   double axis3_length = (h->GetXaxis()->GetXmax() - lumi_Run1) / h->GetXaxis()->GetXmax();
   axis3->SetTitle("Fluence [10^{12} * cm^{-2}]");
   axis3->SetTitleSize(0.035);
   axis3->SetTitleOffset(-0.8);
   axis3->SetTickLength(0.06/axis3_length);
   axis3->SetLabelSize(0.035);
   axis3->SetNoExponent(kTRUE);
   //axis2->SetLabelOffset(-0.01);
   axis3->Draw();


/*
   TGaxis *axis = new TGaxis(h->GetXaxis()->GetXmin(),h->GetMaximum(),h->GetXaxis()->GetXmax(),h->GetMaximum(),flu_min,flu_max, 510, "-"); // - : tick marks on negative side ; L : labels on left of tick mark; = : label on same side as marks
   axis->SetLineColor(1);
   axis->SetTitle("Fluence [cm^{-2}]");
   axis->SetTitleSize(0.035);
   axis->SetTitleOffset(-0.65);
   axis->SetTickLength(0.03);
   axis->SetLabelSize(0.03);
   axis->SetLabelOffset(-0.01);
   axis->Draw(); //FIXME
*/

  // Draw Ref measurement (measured in lab.);
  TMarker *pref = new TMarker( 0, SubdetRef.GetVdepl(detid), 21);
  pref->SetMarkerColor(2);
  if(draw_vdep_lab) pref->Draw("same");

//------




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
		  	
  	output_name+= "KinkVSLumi_"+subdet;

   	if(use_curvature) {output_name+="_kink";}
  	else {output_name+="_line";}

    TString ts_detid = Convert_Number_To_TString(detid);
    output_name+="_detid"+ts_detid+"_"+antype;
    if(superimpose_simu) {output_name+="_simu";}
    output_name+=+".png";
    //output_name+=+".pdf";

    // c1->Print(output_name.Data()); //FIXME
    c1->Print("test.png");

    //c1->Print(Form("KinkVsLumi_%s_detid_%llu_%s.eps", subdet.c_str(), detid, antype.c_str()));

  }

  //getchar(); //Waits for user to press enter to continue
  delete axis; delete axis2; delete axis3;
  delete pref; delete latex;
  if(superimpose_simu && g_simu != 0) {delete leg;}
  if(!superimpose_simu && draw_fit) {delete fit;}
  delete g;
  delete c1;
}

void CheckFits(string dirname, string subdet, const int NF, vector<string> runs, ULong64_t detid)
{
  for(int i=0; i<NF; i++)
  {
    ShowBestCurve(dirname, subdet, runs[i], detid, true, false);
  }
}

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


TH1F* DrawHistoDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, string run, bool show=true, bool use_curvature=true, int layer=0)
{
  if(subdet!="TIB" && subdet!="TOB" && subdet!="TEC") {cout<<__LINE__<<" : Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}

  const int Ndet_TIB=7;
  //ULong64_t Detids_TIB[Ndet_TIB] = {369121381, 369121382, 369121385, 369121386, 369121389, 369121390, 369121605, 369121606, 369121609, 369121610, 369121613, 369121614, 369125861, 369125862, 369125866, 369125869, 369125870};
  ULong64_t Detids_TIB[Ndet_TIB] = {369121381, 369121382, 369121385, 369121386, 369125862, 369125866, 369125870};
  double vfd_ref_TIB[Ndet_TIB];

  const int Ndet_TOB=12;
  ULong64_t Detids_TOB[Ndet_TOB] = {4362815121, 4362815122, 4362815281, 4362815282,
  4362815081, 4362815082, 4362815241, 4362815242, 4362815201, 4362815202, 4362815161, 4362815162};
  double vfd_ref_TOB[Ndet_TOB];

  const int Ndet_TEC=31;
  ULong64_t Detids_TEC[Ndet_TEC] = {4701481960, 4701482280, 4701482400, 4701482000, 4701482320, 4701482040, 4701482360, 4701482651, 4701482961, 4701483241, 4701483361, 4701482611, 4701482661, 4701483001, 4701483281, 4701483401, 4701482621, 4701482921, 4701483321, 4701482652, 4701482962, 4701483242, 4701483362, 4701482612, 4701482662, 4701483002, 4701483282, 4701483402, 4701482622, 4701482922, 4701483322};
  double vfd_ref_TEC[Ndet_TEC];


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
  for(UInt_t ie = 0; ie <nentries; ie++)
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
    	if( Is_Scan_Bad(subdet, run, antype, odetid) == true) {cout<<FRED("1 Point ignored (Is_Scan_Bad==true for module "<<odetid<<")")<<endl; return 0;}
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
      for(UInt_t idet  = 0; idet < Ndet; idet++)
      {
    	if(odetid==Detids[idet] && odepvolt>=0)
		{
		  h->Fill(odepvolt-vfd_ref[idet]);

		  //cout<<Detids[idet]<<" "<<odepvolt<<"-"<<vfd_ref[idet]<<"= "<<odepvolt-vfd_ref[idet]<<endl;
		}
      }
	}
	
	//for(int k=0; k<h->GetNbinsX(); k++)
	{
		//cout<<"bin "<<k<<" / "<<h->GetBinContent(k)<<endl;
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


TGraphErrors* DrawDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, const int NF, vector<string> runs, vector<float> lumis, bool useflu=false, bool use_curvature=true, int layer=0)
{

  //if(layer != "inner" && layer != "outer" && layer != "") {cout<<FRED("Wrong argument 'layer' !")<<endl;}

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
  int ipt=0;

  TH1F* href = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, "labref", useflu, use_curvature, layer); //Last arg : TEC layer we consider //FIXME //Verify error bars "manually" to see if correct
  if(!href) {cout<<FRED("hdiff is null! Abort")<<endl; return 0;}

  double lumi=0;
  double lumi_Run1=29.46;
  for(int i=0; i<NF; i++)
  {
	//if(antype=="ClusterWidth" && runs[i]=="258443") {continue;} //DECO file not available
	
    
    //------------------

    TH1F* hdiff = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, runs[i], useflu, use_curvature, layer);
    
    if(!hdiff) {cout<<FRED("hdiff is null! Continue")<<endl; continue;}

    lumi = lumis[i];


	//cout<<"run "<<runs[i]<<" / hdiff->GetMean() = "<<hdiff->GetMean()<<endl;
	//cout<<"Entries = "<<hdiff->GetEntries()<<endl;

	g->SetPoint(ipt, lumi, hdiff->GetMean());//-href->GetMean());


	if(useRmsAsErrors) g->SetPointError(ipt, 0, hdiff->GetRMS());
    else g->SetPointError(ipt, 0, hdiff->GetMeanError());


	if(i==0)
	{
        if(useRmsAsErrors) g->SetPointError(ipt, 0, href->GetRMS());
        else g->SetPointError(ipt, 0, href->GetMeanError());
    }


	cout<<ipt<<" mean "<<hdiff->GetMean()-href->GetMean();
	if(useRmsAsErrors) cout<<" / rms = "<<hdiff->GetRMS()<<endl;
	else cout<<" / Mean error = "<<hdiff->GetMeanError()<<endl;

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
  if(useflu) h->GetXaxis()->SetTitle("fluence [cm-2]");
  else h->GetXaxis()->SetTitle("L_{int} [fb-1]");
  h->GetYaxis()->SetTitle("V_{fd} [V]");
  g->SetMarkerStyle(20);
  TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
  g->Draw("APL");

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
  TF1* fit = new TF1("ham", "[3]-([0]*(1-exp(-[1]*x))+[2]*x)", 0, 30);
  //g->Fit("ham");
  fit->SetParameters(50, 0.1, 1, 250);
  //fit->Draw();

  c1->Modified();
  c1->Update();

  Create_Plot_Directories();

  string name = "plots/";
  if(antype=="Signal") name+= "signal/relative/";
  else name+="CW/relative/";
  
  if(!use_curvature) {name += subdet + "_line_diff"+antype;}
  else {name += subdet + "_kink_diff"+antype;}
  name+=".png";
  //name+=".pdf";
  c1->SaveAs(name.c_str());
  //getchar();
  
  delete c1; delete fit; delete href;

  return g;
}

//Superimpose 2 "diff kink" curves (to have Signal & Clusterwidth on same plot)
void Superimpose_DrawDiffModules_SmallScan(string dirname, string subdet, string ref, const int NF, vector<string> runs, vector<float> lumis, bool useflu, bool use_curvature)
{
	TGraphErrors* g_sig = DrawDiffModules_SmallScan(dirname, subdet, "Signal", ref, NF, runs, lumis, useflu, use_curvature, 0);
	TGraphErrors* g_cluster = DrawDiffModules_SmallScan(dirname, subdet, "ClusterWidth", ref, NF, runs, lumis, useflu, use_curvature, 0);

	if(!g_sig || !g_cluster) {cout<<"Null TGraph ! Abort !"<<endl; return;}

	TCanvas *c1 = new TCanvas("c1","c1", 1000, 800);
	

  TH1F* h1 = g_cluster->GetHistogram(); //Access g content via TH1F*
  if(useflu) h1->GetXaxis()->SetTitle("Fluence [cm^{-2}]");
  else h1->GetXaxis()->SetTitle("L_{int} [fb^{-1}]");
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

  TString name = "plots/superpose_CW_signal/superimpose_diff_";
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


//#################################################################################################

// use it with Aclic : root -l g.C+

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

int main(int argc, char *argv[])
{
  //Load_Canvas_Style();i

  bool usefluence = true;

  string dirname="/afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_8_0_20_patch1/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/DECO_files";

  vector<string> v_analysis;
  //v_analysis.push_back("Signal");
  v_analysis.push_back("ClusterWidth");;

  bool use_curvature=false; //true-->kink ; false-->lines

  bool superimpose_simu = false;
  bool draw_vdep_lab = true; //Draw lab measurement of initial Vfd
  bool draw_fit = true; //Draw linear fit of vfd evolution

  vector<string> v_subdet;
  v_subdet.push_back("TIB");
  //v_subdet.push_back("TOB");
  //v_subdet.push_back("TEC");
  //v_subdet.push_back("TID");


//--- Scans
  std::cout << "Starting DrawKinkVsLumi.C" << std::endl;

  for(int i=0; i<v_analysis.size(); i++)
  {
  	for(int j=0; j<v_subdet.size(); j++)
  	{
		if(v_subdet[j] != "TIB") {superimpose_simu = false;} //Only available for TIB yet

  	  //FIll vectors here in order to read vector values and get rid of faulty runs accordingly
  	  vector<string> runs;
	  vector<float> lumis;  //NB : Lumi Run I = 29.46 fb-1
	  //Old runs (10)
	  //runs.push_back("160497");lumis.push_back(0.045);
	  runs.push_back("170000");lumis.push_back(1.44);
	  runs.push_back("190459");lumis.push_back(6.15);
	  runs.push_back("193541");lumis.push_back(7.19);
	  runs.push_back("193928");lumis.push_back(7.41);
	  runs.push_back("199832");lumis.push_back(15.14);
	  //runs.push_back("200786");lumis.push_back(16.98);
	  if(v_subdet[j] != "TOB" || v_analysis[i] != "Signal")
	  {runs.push_back("203832");lumis.push_back(21.18);}
	  runs.push_back("208339");lumis.push_back(28.57);
	  runs.push_back("211797");lumis.push_back(29.45);

	  //2015 (4) -- Run 2
	  runs.push_back("246963");lumis.push_back(0.001+29.46);
	  runs.push_back("254790");lumis.push_back(0.17+29.46);
	  if(v_subdet[j] != "TOB" || v_analysis[i] != "Signal")
	  {runs.push_back("258443");lumis.push_back(2.09+29.46);} //Mostly bad curves for TOB
	  runs.push_back("262254");lumis.push_back(4.23+29.46);

	  //2016 (5)
	  runs.push_back("271056");lumis.push_back(4.26+29.46); //No B field
	  runs.push_back("274969");lumis.push_back(7.58+29.46);
	  runs.push_back("276437");lumis.push_back(14.48+29.46);
	  runs.push_back("278167");lumis.push_back(23.64+29.46);
	  runs.push_back("280385");lumis.push_back(35.06+29.46);
	  runs.push_back("285371");lumis.push_back(45.70+29.46);

	  //2017 (2)
	  runs.push_back("295324");lumis.push_back(45.71+29.46); //-- FULL
	  runs.push_back("298996");lumis.push_back(52.18+29.46);
  	  runs.push_back("302131");lumis.push_back(65.84+29.46);
  	  if(v_subdet[j] != "TEC") runs.push_back("303824");lumis.push_back(70.55+29.46); //-- FULL (not enough stat. yet for TEC)


  	  DrawKinkVsLumi(dirname, v_subdet[j], v_analysis[i], runs, lumis, usefluence, use_curvature, superimpose_simu, draw_vdep_lab, draw_fit); //VFD EVOLUTION, SINGLE MODULES
  	  
  	  
  	  TString ref = "170000";
  	  if(v_subdet[j] == "TEC" || v_subdet[j] == "TOB") {ref = "190459";}
  	  //DrawDiffModules_SmallScan(dirname, v_subdet[j], v_analysis[i], ref.Data(), runs.size(), runs, lumis, false, use_curvature, ""); //RELATIVE VFD EVOLUTION, AVERAGED OVER MODULES
  	  
  	  //Superimpose_DrawDiffModules_SmallScan(dirname, v_subdet[j], ref.Data(), runs.size(), runs, lumis, false, use_curvature); //RELATIVE VFD EVOL, SUPERIMPOSED FOR BOTH OBSERVABLES
	}
  }


/*
    cout<<"Lumi = 5 for TEC modid 4701481960 --> fluence = "<<ComputeFluence(5, 4701481960, "TEC")<<endl;
    cout<<"Lumi = 5 for TEC modid 4701483241 --> fluence = "<<ComputeFluence(5, 4701483241, "TEC")<<endl;
    cout<<"Lumi = 100 for TEC modid 4701481960 --> fluence = "<<ComputeFluence(100, 4701481960, "TEC")<<endl;
    cout<<"Lumi = 100 for TEC modid 4701483241 --> fluence = "<<ComputeFluence(100, 4701483241, "TEC")<<endl;   
    cout<<"Fluence 1.20395e+12 --> lumi ) "<<GetLumiFromFluence( 1.20395*pow(10,12), 369121381, "")<<endl;
*/

  return 0;
}

