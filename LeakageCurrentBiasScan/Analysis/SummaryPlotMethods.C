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


void SummaryPlotMethods()
{
    bool DELTA = true;
    bool UseOnlyFullScan = false ;
    TString SUB = "TIB"; // Only TIB or TOB at the moment // TID is not possible because no TIB modules in small scans and you may lack stats for TEC
    TString lay = "1";
    TString CMSSW = "CMSSW_14_0_14";
    TString SCAN = "FULLSCAN";

    TString FILENAME = "./"+SUB+"_L"+lay+SCAN+".root";
    TString FILENAME2 = "superimpose_Methods_FullScan_"+SUB+"_Signal_AllScans.root";
    TString FILENAME3 = "superimpose_Methods_FullScan_"+SUB+"_ClusterWidth_AllScans.root";
    if (UseOnlyFullScan) 
        {
            FILENAME2 = "superimpose_Methods_FullScan_"+SUB+"_Signal_FullScans.root";
            FILENAME3 = "superimpose_Methods_FullScan_"+SUB+"_ClusterWidth_FullScans.root";
        }
    if (DELTA) 
        {
            FILENAME = "./Delta"+SUB+"_L"+lay+SCAN+".root";
            FILENAME2 = "superimpose_diffMethods_FullScan_"+SUB+"_Signal_AllScans.root";
            FILENAME3 = "superimpose_diffMethods_FullScan_"+SUB+"_ClusterWidth_AllScans.root";
            if (UseOnlyFullScan) 
                {
                    FILENAME2 = "superimpose_diffMethods_FullScan_"+SUB+"_Signal_FullScan.root";
                    FILENAME3 = "superimpose_diffMethods_FullScan_"+SUB+"_ClusterWidth_FullScan.root";
                }
        }

    TFile* file = new TFile(FILENAME, "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << file << std::endl;
        return;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas = dynamic_cast<TCanvas*>(file->Get("c1"));
    if (!canvas) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file->Close();
        return;
    }

    canvas->Draw();


    //---------------//
    
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
    // canvas->Draw();
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
            if (count == 0) {TGE = dynamic_cast<TGraphErrors*>(obj);count += 1; std::cout<<" here 1 "<<std::endl;}
            else if (count == 1) {TGE2 = dynamic_cast<TGraphErrors*>(obj);count += 1; std::cout<<" here 2 "<<std::endl;break;}
            else {count += 1;break;}
        }
    }
    TGE->Draw("SAME");
    TGE->SetLineColor(2);
    TGE->SetLineWidth(4);

    std::cout<<"TGE->GetN(); "<<TGE->GetN()<<std::endl; 
    TGE2->Draw("SAME");
    TGE2->SetLineColor(3);
    TGE2->SetLineWidth(4);
     std::cout<<"TGE2->GetN(); "<<TGE2->GetN()<<std::endl;
    //-----------------------------//

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
                    if (count2 == 0) {TGE3 = dynamic_cast<TGraphErrors*>(obj3);count2 += 1; std::cout<<" here 3 "<<std::endl;}
                    else if (count2 == 1) {TGE4 = dynamic_cast<TGraphErrors*>(obj3);count2 += 1; std::cout<<" here 4 "<<std::endl;break;}
                    else {count2 += 1;break;}
                }
            }

//-------------------------//

    TGE3->Draw("SAME");
    TGE3->SetLineColor(1);
    TGE3->SetLineWidth(4);
      std::cout<<"TGE3->GetN(); "<<TGE3->GetN()<<std::endl;
    TGE4->Draw("SAME");
    TGE4->SetLineColor(4);
    TGE4->SetLineWidth(4);
      std::cout<<"TGE4->GetN(); "<<TGE4->GetN()<<std::endl;
    TLegend *leg = new TLegend(0.55, 0.6, 0.9, 0.7);
    leg->SetTextFont(42);
    // leg->SetHeader("Fitting Methods");
    leg->AddEntry(TGE, "Signal kink", "l");
    leg->AddEntry(TGE2, "Signal line", "l");
    leg->AddEntry(TGE3, "CW kink", "l");
    leg->AddEntry(TGE4, "CW line", "l");

    leg->Draw("SAME");
    canvas->Update();

// // *****************************************************************************
 
//  c1->Update();
 canvas->SaveAs("SummaryPlotMethods_"+SUB+"L"+lay+"_DeltaVFDGeneral_"+SCAN+".pdf");
//  delete canvas;
 file->Close();
 file2->Close();
 file3->Close();

}
