#include <TFile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <vector>
#include <iostream>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TColor.h>

// ---------------------------------------------------------
// This macro allows to compare the leakage current of a given detid for different runs directly (and not by comparing the plots by eye)
//

void combineTGraphErrors( std::vector<TString> RUNS) {

    const int NRUNS = RUNS.size();

    std::vector<TString> FileNames;
    for (unsigned int i = 0; i < NRUNS; i++) {
        // FileNames.push_back("/eos/user/j/jlagram/SiStripRadMonitoring/LeakageCurrentCorrections/Corrections/wDCUcur/LeakCurCorr_TIB_" + RUNS[i] + ".root");
        FileNames.push_back("LeakCurCorr_TIB_" + RUNS[i] + ".root");
    }

    std::vector<TGraphErrors*> graphs_381;
    std::vector<TGraphErrors*> graphs_386;

    Float_t r1 = 0.246;
    Float_t g1 = 0.563;
    Float_t b1 = 0.852;
    TColor color1 = TColor(301,r1, g1, b1);
    // color1.SetRGB(r1, g1, b1);
    Int_t ColorBlue = color1.GetNumber();

    Float_t r2 = 1.000;
    Float_t g2 = 0.661;
    Float_t b2 = 0.055;
    TColor color2 = TColor(302,r2, g2, b2);
    // color2.SetRGB(r2, g2, b2);
    Int_t ColorOrange = color2.GetNumber();

    Float_t r3 = 0.739;
    Float_t g3 = 0.122;
    Float_t b3 = 0.004;
    TColor color3 = TColor(303,r3, g3, b3);
    Int_t ColorRed = color3.GetNumber();

    Float_t r4 = 0.578;
    Float_t g4 = 0.641;
    Float_t b4 = 0.635;
    TColor color4 = TColor(304,r4, g4, b4);
    Int_t ColorGrey = color4.GetNumber();

    Float_t r5 = 0.513;
    Float_t g5 = 0.176;
    Float_t b5 = 0.713;
    TColor color5 = TColor(305,r5, g5, b5);
    Int_t ColorDarkPurple = color5.GetNumber();

    Float_t r6 = 0.661;
    Float_t g6 = 0.418;
    Float_t b6 = 0.348;
    TColor color6 = TColor(306,r6, g6, b6);
    Int_t ColorBrown = color6.GetNumber();

    Float_t r7 = 0.905;
    Float_t g7 = 0.387;
    Float_t b7 = 0.000;
    TColor color7 = TColor(307,r7, g7, b7);
    Int_t ColorDarkOrange = color7.GetNumber();

    Float_t r8 = 0.723;
    Float_t g8 = 0.672;
    Float_t b8 = 0.438;
    TColor color8 = TColor(308,r8, g8, b8);
    Int_t ColorNeutral = color8.GetNumber();

    Float_t r9 = 0.441;
    Float_t g9 = 0.457;
    Float_t b9 = 0.504;
    TColor color9 = TColor(309,r9, g9, b9);
    Int_t ColorDarkGrey = color9.GetNumber();

    Float_t r10 = 0.571;
    Float_t g10 = 0.852;
    Float_t b10 = 0.867;
    TColor color10 = TColor(310,r10, g10, b10);
    // color10.SetRGB(r10, g10, b10);
    Int_t ColorLightBlue = color10.GetNumber();

    std::vector<Int_t> colors;

    colors.push_back(ColorBlue);
    colors.push_back(ColorOrange);
    colors.push_back(ColorRed);
    colors.push_back(ColorGrey);
    colors.push_back(ColorDarkPurple);
    colors.push_back(ColorBrown);
    colors.push_back(ColorDarkOrange);
    colors.push_back(ColorNeutral);
    colors.push_back(ColorDarkGrey);
    colors.push_back(ColorLightBlue);


    for (unsigned int i = 0; i < NRUNS; i++) {
        std::cout<<"Opening file: "<<FileNames[i]<<std::endl;
        TFile* file = TFile::Open(FileNames[i]);
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file: " << FileNames[i] << std::endl;
            continue;
        }

        // file->ls();
        file->cd();
        TGraphErrors* graph_381 = (TGraphErrors*)file->Get("vdrop_369121381");
        TGraphErrors* graph_386 = (TGraphErrors*)file->Get("vdrop_369121386");

        if (graph_381) {
            graph_381->SetLineColor(colors[i]);
            graph_381->SetMarkerColor(colors[i]);
            graph_381->SetMarkerStyle(20);
            graphs_381.push_back(graph_381);
        } else {
            std::cerr << "vdrop_369121381 not found in file: " << FileNames[i] << std::endl;
        }

        if (graph_386) {
            graph_386->SetLineColor(colors[i]);
            graph_386->SetMarkerColor(colors[i]);
            graph_386->SetMarkerStyle(20);
            graphs_386.push_back(graph_386);
        } else {
            std::cerr << "vdrop_369121386 not found in file: " << FileNames[i] << std::endl;
        }

        file->Close();
    }


    // Combine the TGraphErrors    

    TCanvas* canvas = new TCanvas("canvas", "Combined TGraphErrors",0,0,1600,1000);
    canvas->Divide(2, 1);

    // --------------381----------------
    canvas->cd(1);
    TLegend* legend_381 = new TLegend(0.1, 0.7, 0.4, 0.9);
        legend_381->SetTextSize(0.02); 

    TMultiGraph* multiGraph_381 = new TMultiGraph();
    for (size_t i = 0; i < graphs_381.size(); ++i) {
        multiGraph_381->Add(graphs_381[i]);
        legend_381->AddEntry(graphs_381[i], RUNS[i], "lp");
    }
    multiGraph_381->SetTitle("TIB Detid 369121381");
    multiGraph_381->GetXaxis()->SetTitle("V_{biais} [V]");
    multiGraph_381->GetYaxis()->SetTitle("I_{Leak} [mA]");
    multiGraph_381->Draw("AP");
    legend_381->Draw();

    //------Start of Copy Paste-----------------------------//
TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = true;
TString extraText   = "";
float extraTextFont = 52;  // default is helvetica-italics

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
float lumiTextSize     = 0.6;
float lumiTextOffset   = 0.2;
float cmsTextSize      = 0.75;
float cmsTextOffset    = 0.1;  // only used in outOfFrame version

float relPosX    = 0.045;
float relPosY    = 0.035;
float relExtraDY = 1.2;

// ratio of "CMS" and extra text size
float extraOverCmsTextSize  = 0.76;

TString lumi_13TeV = "";//137 fb^{-1}
TString lumi_sqrtS = "TIBL1";
TString lumiText = lumi_13TeV+lumi_sqrtS;
  float H = canvas->GetWh();
  float W = canvas->GetWw();
  float l = canvas->GetLeftMargin();
  float t = canvas->GetTopMargin();
  float r = canvas->GetRightMargin();
  float b = canvas->GetBottomMargin();

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  float extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+0.005,lumiText);

      latex.SetTextFont(cmsTextFont);
      latex.SetTextAlign(11); 
      latex.SetTextSize(cmsTextSize*t);    
      latex.DrawLatex(l,1-t+0.005,cmsText);

float posX_=0;
  float posY_=0;
  int iPosX = 3;
  if( iPosX%10<=1 )
    {
      posX_ =   l + relPosX*(1-l-r);
    }
  else if( iPosX%10==2 )
    {
      posX_ =  l + 0.5*(1-l-r);
    }
  else if( iPosX%10==3 )
    {
      posX_ =  1-r - relPosX*(1-l-r);
    }
   posY_ = 1-t - relPosY*(1-t-b);
  	  if( writeExtraText ) 
	    {
         posX_ =   l +  relPosX*(1-l-r);
         posY_ =   1-t+lumiTextOffset*t;
        int alignY_=3;
         int alignX_=2;
         if( iPosX/10==0 ) alignX_=1;
         if( iPosX==0    ) alignX_=1;
         if( iPosX==0    ) alignY_=1;
         if( iPosX/10==1 ) alignX_=1;
         if( iPosX/10==2 ) alignX_=2;
         if( iPosX/10==3 ) alignX_=3;
         //if( iPosX == 0  ) relPosX = 0.12;
         int align_ = 10*alignX_ + alignY_;
      latex.SetTextFont(extraTextFont);
      latex.SetTextSize(extraTextSize*t);
      latex.SetTextAlign(11);
      latex.DrawLatex(posX_+0.18,1-t+0.005, extraText);
	    }

    // -------------386 ----------------
    canvas->cd(2);
    TLegend* legend_386 = new TLegend(0.1, 0.7, 0.4, 0.9);
    legend_386->SetTextSize(0.02); 
    TMultiGraph* multiGraph_386 = new TMultiGraph();
    for (size_t i = 0; i < graphs_386.size(); ++i) {
        multiGraph_386->Add(graphs_386[i]);
        legend_386->AddEntry(graphs_386[i], RUNS[i], "lp");
    }
    multiGraph_386->SetTitle("TIB Detid 369121386");
    multiGraph_386->GetXaxis()->SetTitle("V_{biais} [V]");
    multiGraph_386->GetYaxis()->SetTitle("I_{Leak} [mA]");
    multiGraph_386->Draw("AP");
    legend_386->Draw();
 //------Start of Copy Paste-----------------------------//

H = canvas->GetWh();
   W = canvas->GetWw();
   l = canvas->GetLeftMargin();
   t = canvas->GetTopMargin();
   r = canvas->GetRightMargin();
   b = canvas->GetBottomMargin();
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+0.005,lumiText);

      latex.SetTextFont(cmsTextFont);
      latex.SetTextAlign(11); 
      latex.SetTextSize(cmsTextSize*t);    
      latex.DrawLatex(l,1-t+0.005,cmsText);

  posX_=0;
  posY_=0;
  iPosX = 3;
  if( iPosX%10<=1 )
    {
      posX_ =   l + relPosX*(1-l-r);
    }
  else if( iPosX%10==2 )
    {
      posX_ =  l + 0.5*(1-l-r);
    }
  else if( iPosX%10==3 )
    {
      posX_ =  1-r - relPosX*(1-l-r);
    }
   posY_ = 1-t - relPosY*(1-t-b);
  	  if( writeExtraText ) 
	    {
         posX_ =   l +  relPosX*(1-l-r);
         posY_ =   1-t+lumiTextOffset*t;
        int alignY_=3;
         int alignX_=2;
         if( iPosX/10==0 ) alignX_=1;
         if( iPosX==0    ) alignX_=1;
         if( iPosX==0    ) alignY_=1;
         if( iPosX/10==1 ) alignX_=1;
         if( iPosX/10==2 ) alignX_=2;
         if( iPosX/10==3 ) alignX_=3;
         //if( iPosX == 0  ) relPosX = 0.12;
         int align_ = 10*alignX_ + alignY_;
      latex.SetTextFont(extraTextFont);
      latex.SetTextSize(extraTextSize*t);
      latex.SetTextAlign(11);
      latex.DrawLatex(posX_+0.18,1-t+0.005, extraText);
	    }

    canvas->SaveAs("CombinedTGraphErrors.pdf");

    // Cleanup
    delete multiGraph_381;
    delete multiGraph_386;
    delete legend_381;
    delete legend_386;
    delete canvas;
}

int main()
    {
        std::vector<TString> RUNS;

        RUNS.push_back("20230407_run365843");
        RUNS.push_back("20230907_run373060");
        RUNS.push_back("20231025_run375658");
        RUNS.push_back("20240321_run378238");
        RUNS.push_back("20240702_run382655");
        RUNS.push_back("20240910_run385515");
        RUNS.push_back("20241012_run386863");

        combineTGraphErrors(RUNS);
    }