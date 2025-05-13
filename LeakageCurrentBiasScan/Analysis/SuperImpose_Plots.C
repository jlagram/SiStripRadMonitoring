#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TLine.h>
#include <iostream>
#include <vector>

// !!
#include <algorithm>
#include <limits>


void plot()
{

    TCanvas *cfinal = new TCanvas("cfinal", "plots",200,0,1500,1000);
cfinal->SetFillColor(10);
cfinal->SetFillStyle(4000);
cfinal->SetBorderSize(2);

cfinal->cd();
TPad* pad1 = new TPad("pad1","This is pad1",0.01,0.01,0.99,1,21);
pad1->SetFillColor(0);
pad1->SetBorderMode(0);
pad1->SetFrameFillColor(10);
pad1->Draw();
// pad1->SetLogy(logy);
pad1->SetTopMargin(0.1);
pad1->SetBottomMargin(0.15);
pad1->SetRightMargin(0.05);
pad1->SetLeftMargin(0.1);
pad1->cd();

    bool DELTA = false;
    bool UseOnlyFullScan = false ;
    TString SUB = "TIB"; // Only TIB or TOB at the moment // TID is not possible because no TIB modules in small scans and you may lack stats for TEC
    TString lay = "1";
    TString CMSSW = "CMSSW_14_0_14";
    TString SCAN = "SMALLSCAN";

    TString FILENAME = "/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/LeakageCurrentBiasScan/Analysis/SummaryPlot_"+SCAN+SUB+"L"+lay+".root";
    TString FILENAME2 = "superimpose_Methods_FullScan_"+SUB+"_Signal_AllScans.root";
    TString FILENAME3 = "superimpose_Methods_FullScan_"+SUB+"_ClusterWidth_AllScans.root";
    if (UseOnlyFullScan) 
        {
            FILENAME2 = "superimpose_Methods_FullScan_"+SUB+"_Signal_FullScan.root";
            FILENAME3 = "superimpose_Methods_FullScan_"+SUB+"_ClusterWidth_FullScan.root";
        }
    if (DELTA) 
        {
            FILENAME = "/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/LeakageCurrentBiasScan/Analysis/Delta"+SUB+"_L"+lay+SCAN+".root";
            FILENAME2 = "superimpose_diffMethods_FullScan_"+SUB+"_Signal_AllScans.root";
            FILENAME3 = "superimpose_diffMethods_FullScan_"+SUB+"_ClusterWidth_AllScans.root";
            if (UseOnlyFullScan) 
                {
                    FILENAME2 = "superimpose_diffMethods_FullScan_"+SUB+"_Signal_FullScan.root";
                    FILENAME3 = "superimpose_diffMethods_FullScan_"+SUB+"_ClusterWidth_FullScan.root";
                }
        }




    //---------------//
        //-------------------------//
    TFile* file = new TFile(FILENAME, "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << file << std::endl;
        return;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas = dynamic_cast<TCanvas*>(file->Get("c"));//c1
    if (!canvas) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file->Close();
        return;
    }

    TList* primitiveA = canvas->GetListOfPrimitives();
        // Find the first TH1F in the list
    TGraph* TGEA = nullptr;
    TIter nextA(primitiveA);
    TObject* objA = nullptr;
    int countA = 0;
    while ((objA = nextA())) {
        if (objA->IsA()->InheritsFrom(TGraph::Class())) {
            if (countA == 0) {TGEA = dynamic_cast<TGraph*>(objA);countA += 1;}
            // else if (countA >= 1) {TGEA = dynamic_cast<TGraphErrors*>(obj3);countA += 1; break;}
            else {countA += 1;break;}
        }
    }
    // -- Get Text CMS , preliminay and layer
    TIter nextB(primitiveA);
    TObject* objAB = nullptr;
    for (int i = 0; i < primitiveA->GetSize(); i++) {
        TObject *obj = primitiveA->At(i);
        if (obj->InheritsFrom(TLatex::Class())) {
            TLatex *latex = (TLatex*) obj;
            if (DELTA)
                {
                    latex->SetY(205);
                }
            else
                {
                    latex->SetY(405);
                }
           
        }
    }
    // canvas->Draw();
    pad1->cd();
    TGEA->SetTitle("");
    TGEA->SetLineColor(kGray + 2);
    TGEA->SetLineColorAlpha(kGray + 2, 0.35);
    TGEA->Draw("");
    TGraphErrors* TGEAClone = (TGraphErrors*)TGEA->Clone("TGEAClone");


    TGraph* gr = (TGraph*) canvas->FindObject("lumigr_TIB_L1"); // nom du TGraph (ou TGraphErrors)
    if (!gr) {
        std::cerr << "TGraph 'gr' non trouvé dans le canvas." << std::endl;
        file->Close();
        return;
    }
    gr->SetLineColor(kRed);  // Couleur de la courbe
    gr->SetLineWidth(2);
    gr->Draw("L SAME");

    TGEAClone->SetFillColorAlpha(kGray + 2, 0.35);  // Couleur grise avec transparence
    TGEAClone->Draw("3SAME");  // "3" pour tracer la zone hachurée
    // canvas->ls();
    TH1* hframe = TGEA->GetHistogram(); // histogramme de cadre
    if (DELTA)
        {
                TGEA->GetYaxis()->SetRangeUser(-300, 200);
        }
    else
        {    
            if (hframe) {
                std::cout<<" hframe dound "<<std::endl;
                hframe->GetXaxis()->SetRangeUser(0, 410);   // range X
                hframe->GetYaxis()->SetRangeUser(0, 450);   // range Y
                hframe->GetYaxis()->SetTitleOffset(1.1);
            }
            // TGEA->GetYaxis()->SetRangeUser(0, 450);
        }

    // canvas->ls();
    // !! --------------------
    // TCanvas* c_final = new TCanvas("c_final", "Superposition Graphs", 800, 600);
    TFile* file2 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/superimpose_curves/"+FILENAME2, "READ");

    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error opening file: " << file2 << std::endl;
        return;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas2 = dynamic_cast<TCanvas*>(file2->Get("c1"));
    if (!canvas2) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file2->Close();
        return;
    }
    // canvas2->Draw();
    TList* primitives = canvas2->GetListOfPrimitives();
    // canvas2->ls();
        // Find the first TH1F in the list
    TGraphErrors* TGE = nullptr;
    TGraphErrors* TGE2 = nullptr;
    TIter next(primitives);
    TObject* obj = nullptr;
    int count = 0;
    while ((obj = next())) {
        if (obj->IsA()->InheritsFrom(TGraphErrors::Class())) {
            if (count == 0) {TGE = dynamic_cast<TGraphErrors*>(obj);count += 1; }
            else if (count == 1) {TGE2 = dynamic_cast<TGraphErrors*>(obj);count += 1; break;}
            else {count += 1;break;}
        }
    }
    // c_final->cd();

    TFile* file2A = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_signal_kink.root", "READ");
    if (!file2A || file2A->IsZombie()) {
        std::cerr << "Error opening file: " << file2A << std::endl;
        return;
    }
    TGE = dynamic_cast<TGraphErrors*>(file2A->Get("graph_moyenne"));

    TFile* file2B = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_signal_line.root", "READ");
    if (!file2B || file2B->IsZombie()) {
        std::cerr << "Error opening file: " << file2B << std::endl;
        return;
    }
    TGE = dynamic_cast<TGraphErrors*>(file2A->Get("graph_moyenne"));
    TGE2 = dynamic_cast<TGraphErrors*>(file2B->Get("graph_moyenne"));


    pad1->cd();
    TGE->Draw("LP SAME");
    TGE->SetLineColor(4);
    TGE->SetLineWidth(2);
    TGE->SetMarkerStyle(20);
    TGE->SetMarkerColor(4);
    TGE->GetYaxis()->SetRangeUser(-300, 25);
    TGE->SetTitle("");
    TGE->SetLineStyle(9);
    // std::cout<<"TGE->GetN(); "<<TGE->GetN()<<std::endl; 
    TGE2->Draw("LP SAME");
    TGE2->SetMarkerStyle(20);
    TGE2->SetMarkerColor(4);
    TGE2->SetLineColor(4);
    TGE2->SetLineWidth(1);
    TGE2->SetLineStyle(1);
    //  std::cout<<"TGE2->GetN(); "<<TGE2->GetN()<<std::endl;
    // !! -----------------------------//



            TFile* file3 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/superimpose_curves/"+FILENAME3, "READ");

            if (!file3 || file3->IsZombie()) {
                std::cerr << "Error opening file: " << file3 << std::endl;
                return;
            }

            // Assume the data is stored in a TCanvas named "canvas"
            TCanvas* canvas3 = dynamic_cast<TCanvas*>(file3->Get("c1"));
            if (!canvas3) {
                std::cerr << "Error: TCanvas not found in the file." << std::endl;
                file3->Close();
                return;
            }
        // canvas3->ls();
        // canvas3->Draw();
            TList* primitive3 = canvas3->GetListOfPrimitives();
                // Find the first TH1F in the list
            TGraphErrors* TGE3 = nullptr;
            TGraphErrors* TGE4 = nullptr;
            TIter next3(primitive3);
            TObject* obj3 = nullptr;
            int count2 = 0;
            while ((obj3 = next3())) {
                if (obj3->IsA()->InheritsFrom(TGraphErrors::Class())) {
                    if (count2 == 0) {TGE3 = dynamic_cast<TGraphErrors*>(obj3);count2 += 1;}
                    else if (count2 == 1) {TGE4 = dynamic_cast<TGraphErrors*>(obj3);count2 += 1; break;}
                    else {count2 += 1;break;}
                }
            }

    // /afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_CW_kink.root
    // /afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_CW_line.root

    TFile* file3A = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_CW_kink.root", "READ");
    if (!file3A || file3A->IsZombie()) {
        std::cerr << "Error opening file: " << file3A << std::endl;
        return;
    }


    TFile* file3B = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_14_0_14/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/Graph_TIB_CW_line.root", "READ");
    if (!file3B || file3B->IsZombie()) {
        std::cerr << "Error opening file: " << file3B << std::endl;
        return;
    }
    TGE3 = dynamic_cast<TGraphErrors*>(file3A->Get("graph_moyenne"));
    TGE4 = dynamic_cast<TGraphErrors*>(file3B->Get("graph_moyenne"));


//-------------------------//
    // c_final->cd();
    pad1->cd();
    TGE3->Draw("LP SAME");
    TGE3->SetLineColor(kOrange+2);
    TGE3->SetMarkerStyle(20);
    TGE3->SetMarkerColor(kOrange+2);
    TGE3->SetLineWidth(2);
    //   std::cout<<"TGE3->GetN(); "<<TGE3->GetN()<<std::endl;
    TGE3->SetLineStyle(9);

    TGE4->Draw("LP SAME");
    TGE4->SetMarkerStyle(20);
    TGE4->SetMarkerColor(kOrange+2);
    TGE4->SetMarkerSize(1);
    TGE4->SetLineColor(kOrange+2);
    TGE4->SetLineWidth(2);
    TGE4->SetLineStyle(1);
    //   std::cout<<"TGE4->GetN(); "<<TGE4->GetN()<<std::endl;


    // TList* primitiveA = canvas->GetListOfPrimitives();
        // Find the first TH1F in the list
    TLegend* leg2 = nullptr;
    TIter nextLeg(primitiveA);
    TObject* objLeg = nullptr;
    while ((objLeg = nextLeg())) {
        if (objLeg->IsA()->InheritsFrom(TLegend::Class())) {
            leg2 = dynamic_cast<TLegend*>(objLeg);
            break;

        }
    }
    leg2->Delete();
      
    TLegend *leg = new TLegend(0.65, 0.70, 0.95, 0.9);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    leg->SetHeader("Cluster Variables");
    leg->AddEntry(TGE3, "Cluster Width kink", "l");
    leg->AddEntry(TGE4, "Cluster Width line", "l");
    leg->AddEntry(TGE, "Cluster Charge kink", "l");
    leg->AddEntry(TGE2, "Cluster Charge line", "l");
    leg->Draw("SAME");

    leg = new TLegend(0.65, 0.60, 0.95, 0.7);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    // leg->SetHeader("Cluster Variables");
    leg->AddEntry(TGEAClone, "I.Leak", "f");
    leg->AddEntry(gr, "Hamburg Model", "l");
    // leg->AddEntry(TGE4, "CW line", "l");
    leg->Draw("SAME");

if (DELTA)
    {  
        TLine* lvdrop = new TLine(195, -300, 195, 200);
        lvdrop->SetLineStyle(6);
        lvdrop->SetLineColor(1);
        lvdrop->Draw("SAME");

        TLine* lvdrop2 = new TLine(30, -300, 30, 200);
        lvdrop2->SetLineStyle(6);
        lvdrop2->SetLineColor(1);
        lvdrop2->Draw("SAME");
    }     
else 
    {
        TLine* lvdrop = new TLine(195, 0, 195, 450);
        lvdrop->SetLineStyle(6);
        lvdrop->SetLineColor(1);
        lvdrop->Draw("SAME");

        TLine* lvdrop2 = new TLine(30, 0, 30, 450);
        lvdrop2->SetLineStyle(6);
        lvdrop2->SetLineColor(1);
        lvdrop2->Draw("SAME");
    }


    cfinal->Update();
    // c_final->Update();

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    
//   float extraTextSize = extraOverCmsTextSize*cmsTextSize;
  latex.SetTextFont(61);
  latex.SetTextAlign(11); 
  latex.SetTextSize(0.05);    
  latex.DrawLatex(0.1,0.91,"CMS");

//   float extraTextSize = extraOverCmsTextSize*cmsTextSize;
  latex.SetTextFont(52);
  latex.SetTextAlign(11); 
  latex.SetTextSize(0.05*0.75);    
  latex.DrawLatex(0.18,0.91,"Preliminary");

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(0.05);    
  latex.DrawLatex(0.95,0.91,SUB+"L"+lay);
// // *****************************************************************************
 
//  c1->Update();
if (DELTA)
    {
        cfinal->SaveAs("SummaryPlotMethods_"+SUB+"L"+lay+"_DeltaVFDGeneral_"+SCAN+".pdf");
    }
else
    {
        cfinal->SaveAs("SummaryPlotMethods_"+SUB+"L"+lay+"_VFDGeneral_"+SCAN+".pdf");
    }
//  c_final->SaveAs("SummaryPlotMethods_"+SUB+"L"+lay+"_DeltaVFDGeneral_"+SCAN+".pdf");
 

//  delete canvas;
//  file->Close();
 file2->Close();
 file3->Close();

}
