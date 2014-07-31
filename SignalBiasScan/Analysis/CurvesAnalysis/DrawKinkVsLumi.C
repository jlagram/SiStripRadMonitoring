#include "../CommonTools/CurvesFunctions.C"
#include "../../../LeakageCurrentBiasScan/Analysis/GetKink.C"
#include "../CommonTools/VdeplRef.h"
#include "../CommonTools/ModFluence.h"

#include <sstream>
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TProfile.h" 
#include "TProfile2D.h"
#include "TMarker.h"
#include "TLine.h"


// Correct for the difference of total inelastic cross section when center of mass energy change
double CorrectForCME(double lumi)
{
  double lumi_8TeV = lumi<6.11 ? 0 : lumi - 6.11;
  double lumi_7TeV = lumi - lumi_8TeV;
  return lumi_7TeV + lumi_8TeV*73500./68000;
}


void DrawOneModule(string dirname, string subdet, string antype, string ref, const int NF, string* runs, float* lumis, ULong64_t detid, bool useflu=false)
{
  if(subdet!="TIB" && subdet!="TOB" && subdet!="TID") 
  {cout<<"Subdet '"<<subdet<<"' not allowed."<<endl; return;}

  string filename = "DECO_"+antype+"_"+subdet;


  // Fluence
  ModFluence DetFluence;
  DetFluence.loadFile();
 

  ULong64_t odetid;
  int olayer;
  double odepvolt;
  double oerrdepvolt;
  double oplateau;
  double ofitchisquare;
  int ofitstatus;
  double olastpty;
  double ochi2;

  TCanvas *c1 = new TCanvas();
  TGraphErrors *g = new TGraphErrors();
  int ipt=0;

  for(int i=0; i<NF; i++)
  {
    TFile *f = TFile::Open((dirname+"/"+filename+runs[i]+".root").c_str());
	if(!f) { cout<<"No file : "<<(dirname+"/"+filename+runs[i]+".root")<<endl; continue;}
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
    double fluence=0;
    if(useflu) fluence = DetFluence.GetFluence(detid);
	UInt_t nentries = tr->GetEntries();
    for(UInt_t ie = 0; ie <nentries; ie++){
      tr->GetEntry(ie);
	  if(odetid==detid && odepvolt>=0) {
	    lumi = lumis[i];
		if(useflu) lumi = CorrectForCME(lumi) * fluence;
		g->SetPoint(ipt, lumi, odepvolt);
		g->SetPointError(ipt, 0, oerrdepvolt);
		ipt++; 
		break;
	  }
	}
  }
  
  TH1F* h = g->GetHistogram();
  if(useflu) h->GetXaxis()->SetTitle("fluence [cm-2]");
  else h->GetXaxis()->SetTitle("L_{int} [fb-1]");
  h->GetYaxis()->SetTitle("V_{fd} [V]");
  h->SetTitle(Form("Detid %llu - %s L%i", detid, subdet.c_str(), olayer));
  g->SetMarkerStyle(20);
  g->Draw("APL");
  

  VdeplRef SubdetRef;
  SubdetRef.loadFile(subdet);

  // Draw Ref measurement;
  TMarker *pref = new TMarker( 0, SubdetRef.GetVdepl(detid), 21);
  pref->SetMarkerColor(2);
  pref->Draw();
 
  // Print values
  cout<<"DetID "<<detid;
  cout<<" lab. meas. : "<<SubdetRef.GetVdepl(detid)<<endl;
  double lumi;
  double Vfd;
  for(unsigned int ip=0; ip<(unsigned) g->GetN(); ip++)
  {
    g->GetPoint(ip, lumi, Vfd);
	cout<<lumi<<" "<<Vfd<<endl;
  }
  cout<<endl;
  
  c1->Modified();
  c1->Update();
  //c1->Print(Form("KinkVsLumi_detid_%llu.eps", detid));
  getchar();
}

void CheckFits(string dirname, string subdet, const int NF, string* runs, ULong64_t detid)
{
  for(int i=0; i<NF; i++)
  {
    ShowBestCurve(dirname, subdet, runs[i], detid, true, false);
  }
}

void DrawModules_SmallScan(string dirname, string subdet, string antype, string ref, const int NF, string* runs, float* lumis)
{
 if(subdet!="TIB" && subdet!="TOB") {cout<<"Subdet '"<<subdet<<"' not allowed."<<endl; return;}

 if(subdet=="TIB")
 {
   //TIBminus_1_2_2_1
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121381);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121382);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121385);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121386);
   //TIBminus_1_4_2_5
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121605);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121606);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121609);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121610);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121613);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369121614);
   //TIBplus_1_6_2_5
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125861);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125862);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125866);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125869);
   DrawOneModule(dirname, "TIB", antype, ref, NF, runs, lumis, 369125870);
 }
 if(subdet=="TOB")
 {
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815121);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815122);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815281);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815282);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815081);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815082);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815241);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815242);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815201);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815202);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815161);
   DrawOneModule(dirname, "TOB", antype, ref, NF, runs, lumis, 4362815162); 
 } 
}


TH1F* DrawHistoDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, string run, bool show=true)
{
  if(subdet!="TIB" && subdet!="TOB") {cout<<"Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}
  
  const int Ndet_TIB=17;
  ULong64_t Detids_TIB[Ndet_TIB] = {369121381, 369121382, 369121385, 369121386, 369121389, 369121390, 369121605, 369121606, 369121609,
  369121610, 369121613, 369121614, 369125861, 369125862, 369125866, 369125869, 369125870};
  double vfd_ref_TIB[Ndet_TIB];
  
  const int Ndet_TOB=12;
  ULong64_t Detids_TOB[Ndet_TOB] = {4362815121, 4362815122, 4362815281, 4362815282,
  4362815081, 4362815082, 4362815241, 4362815242, 4362815201, 4362815202, 4362815161, 4362815162};
  double vfd_ref_TOB[Ndet_TOB];
  
  unsigned int Ndet;
  ULong64_t* Detids;
  double* vfd_ref;
  
  if(subdet=="TIB") {Ndet=Ndet_TIB; Detids=Detids_TIB; vfd_ref=vfd_ref_TIB;}
  if(subdet=="TOB") {Ndet=Ndet_TOB; Detids=Detids_TOB; vfd_ref=vfd_ref_TOB; ref="_160497_S_NEW_Sensors";}

  string filename = dirname+"/"+"DECO_"+antype+"_"+subdet;
  
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
  if(show) c1 = new TCanvas();
  TH1F* h = new TH1F("h", "", 150, -100, 200);

  TFile *fref = TFile::Open((filename+ref+".root").c_str());
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
  for(UInt_t ie = 0; ie <nentries; ie++){
	tref->GetEntry(ie);
    for(UInt_t idet = 0; idet < Ndet; idet++)
    {
      if(odetid==Detids[idet] && odepvolt>=0) vfd_ref[idet] = odepvolt;
    }
  }
  

  if(run!="labref")
  {
	TFile *f = TFile::Open((filename+run+".root").c_str());
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
	for(UInt_t ie = 0; ie <nentries; ie++){
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

  return h;
}

TGraphErrors* DrawDiffModules_SmallScan(string dirname, string subdet, string antype, string ref, const int NF, string* runs, float* lumis)
{

  TCanvas *c1 = new TCanvas();
  TGraphErrors *g = new TGraphErrors();
  int ipt=0;
  
  TH1F* href = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, "labref", false);

  for(int i=0; i<NF; i++)
  {
    TH1F* hdiff = DrawHistoDiffModules_SmallScan(dirname, subdet, antype, ref, runs[i], false);
	g->SetPoint(ipt, lumis[i], hdiff->GetMean()-href->GetMean());
	g->SetPointError(ipt, 0, hdiff->GetRMS());
	if(i==0) g->SetPointError(ipt, 0, href->GetRMS());
	//g->SetPointError(ipt, 0, hdiff->GetMeanError());
	cout<<ipt<<" mean "<<hdiff->GetMean()-href->GetMean()<<" rms "<<hdiff->GetRMS()
	    <<" rms_err "<<hdiff->GetRMSError()<<endl;
	ipt++;
  }
  
  TH1F* h = g->GetHistogram();
  h->GetXaxis()->SetTitle("L_{int} [fb-1]");
  h->GetYaxis()->SetTitle("V_{fd} [V]");
  g->SetMarkerStyle(20);
  g->Draw("APL");
  
  // Draw Ref measurement;
  TMarker *pref = new TMarker( 0, 0, 21);
  pref->SetMarkerColor(2);
  pref->Draw();
  TLine *lref = new TLine(0, -1*href->GetRMS(), 0, href->GetRMS());
  lref->SetLineColor(2);
  lref->Draw();

  // Fit with Hamburg model
  TF1* fit = new TF1("ham", "[3]-([0]*(1-exp(-[1]*x))+[2]*x)", 0, 30);
  //g->Fit("ham");
  fit->SetParameters(50, 0.1, 1, 250);
  //fit->Draw();
  
  c1->Modified();
  c1->Update();
  getchar();
  
  return g;
}


TGraphErrors* DrawDiffModules_FullScan(string dirname, string subdet, string antype, string ref, const int NF, string* runs, float* lumis, int layer=1, bool show=false, bool useflu=false)
{

  if(NF<1) { cout<<"Error : should give at list one run"<<endl; return 0;}

  // Fluence
  ModFluence DetFluence;
  DetFluence.loadFile("../CommonTools/modulesFluence.root");
  double fluence=0; // average fluence for the layer
  double tempfluence, tempR, tempZ;
  TProfile* pFluenceVsZ = new TProfile("pFZ","", -110,110,54);
  TProfile* pFluenceVsR = new TProfile("pFR","", 0,110,55);

  TH1F* href;
  TH1F* histos[NF];
  TProfile* profiles[NF];
  
  if(subdet!="TIB" && subdet!="T0B" && subdet!="TID") 
  {cout<<"Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}
  
  
 // Vdepl references from db
 //--------------------------
  
  VdeplRef SubdetRef;
  SubdetRef.loadFile(subdet);


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
  TFile *fscanref = TFile::Open((filename+ref+".root").c_str());
  if(!fscanref) { cout<<"No file : "<<filename<<endl; return 0;}
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
	if(odepvolt>=0 && olayer==layer)
           //&& SubdetRef.GetVdepl(odetid)>270  )
	{
      ScanRef_DetIDs[Nscanref] = odetid;
	  ScanRef_Vdepl[Nscanref] = odepvolt;
	  Nscanref++;
	  //tempfluence = DetFluence.GetFluence(odetid);
	  DetFluence.GetModFluenceInfos(odetid, tempfluence, tempR, tempZ);
	  fluence += tempfluence;
	  pFluenceVsZ->Fill(tempZ, tempfluence);
	  pFluenceVsR->Fill(tempR, tempfluence);
	}
  }
  fscanref->Close();
  fluence/=Nscanref;
  
  TCanvas *c1 = 0;
  TCanvas *c2 = 0;
  TCanvas *c3 = 0;
  if(show) c1 = new TCanvas();
  if(show) c2 = new TCanvas();
  if(show) c3 = new TCanvas();

  // Fill first histo
  for(unsigned int iscanref=0; iscanref<Nscanref; iscanref++)
  {
     href->Fill(SubdetRef.GetVdepl(ScanRef_DetIDs[iscanref])-ScanRef_Vdepl[iscanref]);
  }
  
  if(show)
  {
    c1->cd();
    href->Draw();
	c1->Modified();
	c1->Update();
	
	c3->Divide(1,2);
	c3->cd(1);
	pFluenceVsZ->Draw();
	c3->cd(2);
	pFluenceVsR->Draw();
	c3->cd();
	c3->Modified();
	c3->Update();
	getchar();
	
  }
  
  
  // Look for other scans
  //-----------------------
  //TH1F* hl = new TH1F("hl","",6, 0, 6);
  for(unsigned int ir=0; ir<NF; ir++)
  {
    histos[ir] = new TH1F(Form("h%i",ir), "", 100, -100, 100);
	profiles[ir] = new TProfile(Form("p%i",ir), "", 60, 0, 300);
    
	TFile *f = TFile::Open((filename+runs[ir]+".root").c_str());
	if(!f) { cout<<"No file : "<<filename<<endl; continue;}
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
    	if(odetid==ScanRef_DetIDs[iscanref] && odepvolt>=0 && olayer==layer )
		{  
           histos[ir]->Fill(odepvolt-ScanRef_Vdepl[iscanref]);
		   profiles[ir]->Fill(ScanRef_Vdepl[iscanref], odepvolt-ScanRef_Vdepl[iscanref]);
		   //if(odepvolt-ScanRef_Vdepl[iscanref]>15) hl->Fill(olayer);
		 } 
      }
	}
	f->Close();
  
	if(show)
	{
      c1->cd();
	  histos[ir]->Draw();
	  //h->Draw();
	  c1->Modified();
	  c1->Update();
	  c2->cd();
	  //profiles[ir]->SetMinimum(-20);
	  profiles[ir]->Draw();
	  c2->Modified();
	  c2->Update();
	  getchar();
	}

  }
 
  float ratio[5];
  ratio[0]=0;
  ratio[1]=1;
  ratio[2]=0.66;
  ratio[3]=0.47;
  ratio[4]=0.37;

  double lumi = 0;
  TGraphErrors *g = new TGraphErrors();
  int ipt=0;
  for(int i=0; i<NF; i++)
  {
	lumi = lumis[i];
	if(useflu) lumi = CorrectForCME(lumi) * fluence;
	//g->SetPoint(ipt, lumi*ratio[layer], histos[i]->GetMean());
	g->SetPoint(ipt, lumi, histos[i]->GetMean());//-href->GetMean());
	g->SetPointError(ipt, 0, histos[i]->GetRMS());
	if(i==0) g->SetPointError(ipt, 0, href->GetRMS());
	//g->SetPointError(ipt, 0, hdiff->GetMeanError());
	cout<<ipt<<" mean "<<histos[i]->GetMean()-href->GetMean()<<" rms "<<histos[i]->GetRMS()
	    <<" rms_err "<<histos[i]->GetRMSError()<<endl;
	ipt++;
  }
  
  int icolor = 1;
  if(layer==2) icolor = 2;
  if(layer==3) icolor = 4;
  if(layer==4) icolor = 8;
  g->SetMarkerColor(icolor);
  //g->SetLineColor(icolor);

  if(!c1) c1 = new TCanvas();

  TH1F* h = g->GetHistogram();
  if(useflu) h->GetXaxis()->SetTitle("fluence [cm-2]");
  else h->GetXaxis()->SetTitle("L_{int} [fb-1]");
  h->GetYaxis()->SetTitle("V_{fd} [V]");
  g->SetMarkerStyle(20);
  g->Draw("APL");
  
  // Draw Ref measurement;
  //TMarker *pref = new TMarker( 0, href->GetMean(), 21);
  TMarker *pref = new TMarker( 0, 0, 21);
  pref->SetMarkerColor(icolor);
  //pref->Draw();

  
  c1->Modified();
  c1->Update();
  //getchar();
  
  return g;
}

TProfile* PrintDiffModules_FullScan(string dirname, string subdet, string antype, string ref, string run, int layer=0, bool print=false, bool show=false)
{

  TH1F* href;
  TH1F* histo;
  TProfile* profile;
  TProfile2D* pVfdTemp;
  gStyle->SetPalette(1);

  if(subdet!="TIB") {cout<<"Subdet '"<<subdet<<"' not allowed."<<endl; return 0;}
  
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
  TFile *fscanref = TFile::Open((filename+ref+".root").c_str());
  if(!fscanref) { cout<<"No file : "<<(filename+ref+".root")<<endl; return 0;}
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
  if(show) c1 = new TCanvas();
  if(show) c2 = new TCanvas();

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
    
	TFile *f = TFile::Open((filename+run+".root").c_str());
	if(!f) { cout<<"No file : "<<(filename+run+".root")<<endl; return 0;}
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

  return profile;
}


//#################################################################################################

// use it with Aclic : root -l DrawKinkVsLumi.C+

void DrawKinkVsLumi()
{
  
  string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/ClusterWidthCurves/";
  bool usefluence = true;

  gStyle->SetOptStat(111111);
  const int NF=12;
  string runs[NF];
  float lumis[NF];
  
  runs[0] = "_160497_preamp_corrected2_32ns";
  runs[1] = "_170000_newlandau_S_wchi2_wnhits";
  runs[2] = "_178367_newlandau_S_wchi2_wnhits";
  runs[3] = "_180076_newlandau_S_wchi2_wnhits";
  runs[4] = "_190459_timestamp";
  runs[5] = "_193541";
  runs[6] = "_193928_timestamp";
  runs[7] = "_199832_merged_steps";
  runs[8] = "_200786";
  runs[9] = "_203832_merged_steps";
  runs[10] = "_208339_merged_steps";
  runs[11] = "_211797";

  lumis[0] = 0.045;
  lumis[1] = 1.44;
  lumis[2] = 5.13;
  lumis[3] = 6.02;
  lumis[4] = 6.15;
  lumis[5] = 7.19;
  lumis[6] = 7.41;
  lumis[7] = 15.14;
  lumis[8] = 16.98;
  lumis[9] = 21.18;
  lumis[10] = 28.57;
  lumis[11] = 29.45;
  
  const int NF_full=5;
  string runs_full[NF];
  float lumis_full[NF];
  
  runs_full[0] = "_160497_preamp_corrected2_32ns";
  runs_full[1] = "_170000_newlandau_S_wchi2_wnhits";//_141865_S_NEW
  runs_full[2] = "_190459_timestamp";
  runs_full[3] = "_193928_timestamp";
  runs_full[4] = "_200786"; //_200786_wLeakCurCorr

  lumis_full[0] = 0.045;
  lumis_full[1] = 1.44;
  lumis_full[2] = 6.15;
  lumis_full[3] = 7.41;
  lumis_full[4] = 16.98;

  //runs_full[0] = "_200786";
  //lumis_full[0] = 16.98;

  const int NF_TOB=4;
  string runs_TOB[NF_TOB];
  float lumis_TOB[NF_TOB];
  
  runs_TOB[0] = "_160497_S_NEW_Sensors";
  runs_TOB[1] = "_190459_timestamp_Sensors";
  runs_TOB[2] = "_199832_merged_steps_Sensors";
  runs_TOB[3] = "_208339_merged_steps_Sensors";
  lumis_TOB[0] = 0.045;
  lumis_TOB[1] = 6.15;
  lumis_TOB[2] = 15.14;
  lumis_TOB[3] = 28.57;

  //DrawOneModule("TIB", "Kink", "_160497_preamp_corrected2_32ns", NF, runs, lumis, 369121606, usefluence);
  //CheckFits("~/work/public/SiStripRadMonitoring/SignalCurves", "TIB", NF, runs, 369121605);
  //DrawModules_SmallScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", NF, runs, lumis);
  //DrawHistoDiffModules_SmallScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "_208339_merged_steps");
  //DrawHistoDiffModules_SmallScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "labref");
//  TGraphErrors* gS = DrawDiffModules_SmallScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", NF, runs, lumis);
  
    
/*  TProfile* p1 = PrintDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "_200786", 1, false, false);
  TProfile* p2 = PrintDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "_200786", 2, false, false);
  TProfile* p3 = PrintDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "_200786", 3, false, false);
  TProfile* p4 = PrintDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns", "_200786", 4, false, false);
  p1->Draw();
  p2->SetLineColor(2);
  p2->Draw("same");
  p3->SetLineColor(4);
  p3->Draw("same");
  p4->SetLineColor(8);
  p4->Draw("same");
*/

/*
  string subdet="TID";
  TGraphErrors *g1 = DrawDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns",
   NF_full, runs_full, lumis_full, 1, false, usefluence);
  TGraphErrors *g2 = DrawDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns",
   NF_full, runs_full, lumis_full, 2, false, usefluence);
  TGraphErrors *g3 = DrawDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns",
   NF_full, runs_full, lumis_full, 3, false, usefluence);
  TGraphErrors *g4 = DrawDiffModules_FullScan(dirname, "TIB", "Kink", "_160497_preamp_corrected2_32ns",
   NF_full, runs_full, lumis_full, 4, false, usefluence);

  TH1F* hf = g1->GetHistogram();
  hf->SetMaximum(20);
  hf->SetMinimum(-30); 
 
  g1->Draw("AP");
  g2->Draw("P");
  g3->Draw("P");
  g4->Draw("P");
*/

  // Compare errors for full and small scans L1 modules
/*  TH1F* hf = g1->GetHistogram();
  hf->SetMinimum(-25);
  gS->SetMarkerColor(2);
  gS->SetLineColor(2);
  gS->Draw("P");
*/

  // TOB //
  //-----//
  
  //DrawModules_SmallScan(dirname, "TOB", NF_TOB, runs_TOB, lumis_TOB);
  //CheckFits("~/work/public/SiStripRadMonitoring/SignalCurves", "TOB", NF_TOB, runs_TOB, 4362815122);
  //ShowBestCurve("TOB", "_208339_merged_steps_Sensors", 4362815122, true, false);
  //DrawHistoDiffModules_SmallScan("dirname, TOB", "_208339_merged_steps_Sensors");
  //DrawDiffModules_SmallScan(dirname, "TOB", NF_TOB, runs_TOB, lumis_TOB);
  
  
  // ClusterWidth analysis
  //-----------------------
  
  
  const int NF_cw=11;
  string runs_cw[NF];
  float lumis_cw[NF];
  
  runs_cw[0] = "_160497";
  runs_cw[1] = "_170000";
  runs_cw[2] = "_180076";
  runs_cw[3] = "_190459_leakcor";
  runs_cw[4] = "_193541_leakcor";
  runs_cw[5] = "_193928_leakcor";
  runs_cw[6] = "_199832_leakcor";
  runs_cw[7] = "_200786_leakcor";
  runs_cw[8] = "_203832_leakcor";
  runs_cw[9] = "_208339_leakcor";
  runs_cw[10] = "_211797_leakcor";
  lumis_cw[0] = 0.045;
  lumis_cw[1] = 1.44;
  lumis_cw[2] = 6.02;
  lumis_cw[3] = 6.15;
  lumis_cw[4] = 7.19;
  lumis_cw[5] = 7.41;
  lumis_cw[6] = 15.14;
  lumis_cw[7] = 16.98;
  lumis_cw[8] = 21.18;
  lumis_cw[9] = 28.57;
  lumis_cw[10] = 29.45;

  const int NF_cw_f=5;
  string runs_cw_f[NF];
  float lumis_cw_f[NF];
  runs_cw_f[0] = "_160497";
  runs_cw_f[1] = "_170000";
  runs_cw_f[2] = "_190459";
  runs_cw_f[3] = "_193928";
  runs_cw_f[4] = "_200786";
  lumis_cw_f[0] = 0.045;
  lumis_cw_f[1] = 1.44;
  lumis_cw_f[2] = 6.15;
  lumis_cw_f[3] = 7.41;
  lumis_cw_f[4] = 16.98;

  //DrawHistoDiffModules_SmallScan(dirname, "TIB", "ClusterWidth_KinkThresh", "_160497", "_193928", true);
  //TGraphErrors* gCW = DrawDiffModules_SmallScan(dirname, "TIB", "ClusterWidth_KinkThresh", "_160497", NF_cw, runs_cw, lumis_cw);
  //DrawDiffModules_FullScan(dirname, "TIB", "ClusterWidth_Kink", "_160497", NF_cw_f, runs_cw_f, lumis_cw_f, 1, true);

  const int NF_cw_max=5;
  string runs_cw_max[NF];
  float lumis_cw_max[NF];
  
  runs_cw_max[0] = "_160497";
  runs_cw_max[1] = "_170000";
  runs_cw_max[2] = "_190459_leakcor";
  runs_cw_max[3] = "_199832_leakcor";
  runs_cw_max[4] = "_200786_leakcor";
  lumis_cw_max[0] = 0.045;
  lumis_cw_max[1] = 1.44;
  lumis_cw_max[2] = 6.15;
  lumis_cw_max[3] = 15.14;
  lumis_cw_max[4] = 16.98;
//  TGraphErrors* gCW_max = DrawDiffModules_SmallScan(dirname, "TIB", "ClusterWidth_Max", "_160497", NF_cw_max, runs_cw_max, lumis_cw_max);

  //gS->Draw("APL");
  //gCW->SetMarkerColor(4);
  //gCW->SetLineColor(4);
  //gCW->Draw("P");
  //gCW_max->SetMarkerColor(2);
  //gCW_max->SetLineColor(2);
  //gCW_max->Draw("P");
  
  
  string subdet="TID";
  TGraphErrors *gCW1 = DrawDiffModules_FullScan(dirname, subdet, "ClusterWidth_KinkThresh", "_160497",
   NF_cw_f, runs_cw_f, lumis_cw_f, 1, false, usefluence);
  TGraphErrors *gCW2 = DrawDiffModules_FullScan(dirname, subdet, "ClusterWidth_KinkThresh", "_160497",
   NF_cw_f, runs_cw_f, lumis_cw_f, 2, false, usefluence);
  TGraphErrors *gCW3 = DrawDiffModules_FullScan(dirname, subdet, "ClusterWidth_KinkThresh", "_160497",
   NF_cw_f, runs_cw_f, lumis_cw_f, 3, false, usefluence);
  TGraphErrors *gCW4;
  if(subdet=="TIB") gCW4 = DrawDiffModules_FullScan(dirname, subdet, "ClusterWidth_KinkThresh", "_160497",
   NF_cw_f, runs_cw_f, lumis_cw_f, 4, false, usefluence);

  TH1F* hCWf = gCW1->GetHistogram();
  hCWf->SetMaximum(60);
  hCWf->SetMinimum(-10); 
 
  gCW1->Draw("AP");
  gCW2->Draw("P");
  gCW3->Draw("P");
  if(subdet=="TIB") gCW4->Draw("P");

}


int main(int argc, char *argv[]) {
  std::cout << "Starting DrawKinkVsLumi.C" << std::endl;
  DrawKinkVsLumi();

  return 0;
}
