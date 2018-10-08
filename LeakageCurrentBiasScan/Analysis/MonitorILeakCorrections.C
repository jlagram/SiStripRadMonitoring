#include <iostream>
#include <sstream>
#include <fstream>
#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCollection.h"
#include "TKey.h"
#include "TObject.h"
#include "TClass.h"
#include "TGraphErrors.h"
#include "TStyle.h"


//gROOT->LoadMacro("MonitorILeakCorrections.C+")
//ShowChi2Histo()
void ShowChi2Histo(std::string filename="LeakCurCorr_TIB_20120812_run200786.root", float chi2_thresh=20)
{
  //std::string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/";
  std::string dirname="./";
  std::string filepath=dirname+filename;
  // open file with corrections
  TFile* f = new TFile(filepath.c_str(), "read");
  if(!f) {std::cout<<"No leakage current correction file : "<<filepath<<std::endl; return ;}

  gStyle->SetOptStat(111111);
  TCanvas* c1 = new TCanvas();
  TH1F* h1 = (TH1F*) f->Get("hchi2");
  h1->Draw();
  cout<<"N modules > "<<chi2_thresh<<" : "<<h1->Integral(h1->FindBin(chi2_thresh), h1->GetNbinsX()+1)<<endl;
}

//gROOT->LoadMacro("MonitorILeakCorrections.C+")
//ShowBadChi2Fits()
void ShowBadChi2Fits(std::string filename="LeakCurCorr_TOB_20120812_run200786.root", float chi2_thresh=20)
{

  //std::string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/";
  std::string dirname="./";
  std::string filepath=dirname+filename;
  // open file with corrections
  TFile* f = new TFile(filepath.c_str(), "read");
  if(!f) {std::cout<<"No leakage current correction file : "<<filepath<<std::endl; return ;}

  TCanvas* c1 = new TCanvas();
  float chi2=0;

 TIter nextkey( f->GetListOfKeys() );
 TKey *key;
 while( (key = (TKey*)nextkey()))
 {
   TObject *obj = key->ReadObj();
   if( obj->IsA()->InheritsFrom("TGraphErrors") )
   {
     TGraphErrors *g1 = (TGraphErrors*)obj;
	 TF1* fit = (TF1*) g1->GetListOfFunctions()->First();
	 if(fit) chi2 = fit->GetChisquare()/fit->GetNDF();
	 else chi2 = 0;
	 
	 if(chi2>chi2_thresh)
	 {	 
       cout<<"Graph "<<g1->GetName()<<endl;
	   cout<<"  "<<fit->GetChisquare()/fit->GetNDF()<<endl;
       g1->Draw("AP");

	   c1->Modified();
       c1->Update();
       getchar();
	 }
	 
   }
 }

}

void ShowFit(std::string filename="LeakCurCorr_TOB_20120812_run200786.root", unsigned long long  detid=369125870)
{

  //std::string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/";
  std::string dirname="./";
  std::string filepath=dirname+filename;
  // open file with corrections
  TFile* f = new TFile(filepath.c_str(), "read");
  if(!f) {std::cout<<"No leakage current correction file : "<<filepath<<std::endl; return ;}

  TCanvas* c1 = new TCanvas();
  
  TGraphErrors *g1 = (TGraphErrors*) f->Get(Form("vdrop_%llu", detid));
  /*TF1* fit = (TF1*) g1->GetListOfFunctions()->First();
  if(fit) chi2 = fit->GetChisquare()/fit->GetNDF();
  else chi2 = 0;*/
  TF1* fit = (TF1*) f->Get(Form("fit_%llu", detid));
  if(g1){
    cout<<"Graph "<<g1->GetName()<<endl;
    g1->Draw("AP");
  }
  if(fit){
    if(fit->GetNDF()) cout<<"  "<<fit->GetChisquare()/fit->GetNDF()<<endl;
    if(g1) fit->Draw("same");
    else fit->Draw();
  }
  c1->Print(Form("LeakCurCorr_detid%llu.png", detid));
  c1->Modified();
  c1->Update();
  getchar();

}

void ShowILeakCorrectionsAt300V(std::string filename="LeakCurCorr_TIB_20120812_run200786.root", std::string subdet="TIB")
{
  TCanvas* c1 = new TCanvas();
  TH1F* h300 = new TH1F("h300", "Correction at 300V", 150, 0, 30);
  TH2F* h300vslayer = new TH2F("h300", "Correction at 300V", 10, 0, 10, 150, 0, 30);

  //std::string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/";
  std::string dirname="./";
  std::string filepath=dirname+filename;
  // open file with corrections
  TFile* f = new TFile(filepath.c_str(), "read");
  if(!f) {std::cout<<"No leakage current correction file : "<<filepath<<std::endl; return ;}
  
  int notfound=0;
  const int kSubdetOffset = 25;

  // input file with list of detids
  std::string detids_filename="Data/"+subdet+"_detids_sorted.txt";
  std::string line;
  std::ifstream fin(detids_filename);
  int idet=0;
  unsigned long long detid=0;

  if(fin.is_open()) {
    while( getline ( fin, line) && idet<6500)
	{
	  if(fin.eof()) continue;
	  std::stringstream ss(line);
	  ss >> detid;
	  idet++;
	  
	  if(idet%100==0) std::cout<<idet<<" mod"<<std::endl;
	  
	  // get correction
	  TF1* func = (TF1*) f->Get(Form("fit_%llu", detid));
	  if(!func) {notfound++; continue;}
	  
	  // get layer/ring
	  int isubdet = (detid>>25)&0x7;
	  int ilayer = 0;
	  if(isubdet==3 || isubdet==5) ilayer = (detid>>14)&0x7; //barrel
	  if(isubdet==4) ilayer = (detid>>9)&0x3; //TID
	  if(isubdet==6) ilayer = (detid>>5)&0x7; //TEC
	  
	  h300->Fill(func->Eval(300));
	  h300vslayer->Fill(ilayer, func->Eval(300));
	  //if(func->Eval(300)>3) std::cout<<"detid "<<detid<<" "<<func->Eval(300)<<std::endl;
	  //if(func->Eval(300)>3) {func->Draw(); c1->Modified(); c1->Update(); getchar();}

	}
	fin.close();
  }
  else {std::cout<<"File "<<detids_filename<<" not found."<<std::endl;}

  f->Close();
  f->Delete();
  
  std::cout<<std::endl<<idet<<" module read"<<std::endl;
  std::cout<<notfound<<" not found"<<std::endl;
  h300->Draw(); c1->Modified(); c1->Update(); getchar();
  h300vslayer->Draw("colz"); c1->Modified(); c1->Update(); getchar();
  
}

void MonitorILeakCorrections()
{
  /*ShowILeakCorrectionsAt300V("LeakCurCorr_TID_20120405_run190459.root", "TID");
  ShowILeakCorrectionsAt300V("LeakCurCorr_TID_20120510_run193928.root", "TID");
  ShowILeakCorrectionsAt300V("LeakCurCorr_TID_20120812_run200786.root", "TID");
  ShowILeakCorrectionsAt300V("LeakCurCorr_TID_20150603_run246963.root", "TID");
  ShowILeakCorrectionsAt300V("LeakCurCorr_TID_20160423_run271056.root", "TID");*/
  //ShowILeakCorrectionsAt300V("LeakCurCorr_TIB_20180418_run314574.root", "TIB");
  //ShowILeakCorrectionsAt300V("LeakCurCorr_TIB_20180923_run323370.root", "TIB");
  ShowChi2Histo("LeakCurCorr_TEC_noise_20180618_run317974.root", 10);
  ShowBadChi2Fits("LeakCurCorr_TEC_noise_20180618_run317974.root", 10);
  //ShowFit("LeakCurCorr_TIB_20170527_run295324.root", 369121381);
  //ShowBadChi2Fits("LeakCurCorr_TIB_20180418_run314574.root", 1);
}
