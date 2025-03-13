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
    TString SCAN = "SMALLSCAN";

    TString FILENAME = "./"+SUB+"_L"+lay+SCAN+".root";
    TString FILENAME2 = "TIB_line_Signal.root";
    TString FILENAME3 = "TIB_kink_Signal.root";
    TString FILENAME4 = "TIB_line_ClusterWidth.root";
    TString FILENAME5 = "TIB_kink_ClusterWidth.root";

    if (UseOnlyFullScan) 
        {
            FILENAME2 = "TIB_line_Signal_fullscan.root";
            FILENAME3 = "TIB_kink_Signal_fullscan.root";
            FILENAME4 = "TIB_line_ClusterWidth_fullscan.root";
            FILENAME5 = "TIB_kink_ClusterWidth_fullscan.root";
        }
    if (DELTA) 
        {
            FILENAME = "./Delta"+SUB+"_L"+lay+".root";
            FILENAME2 = "TIB_line_diffSignal.root";
            FILENAME3 = "TIB_kink_diffSignal.root";
            FILENAME4 = "TIB_line_diffClusterWidth.root";
            FILENAME5 = "TIB_kink_diffClusterWidth.root";
            if (UseOnlyFullScan) 
                {
                    FILENAME2 = "TIB_line_diffSignal_fullscan.root";
                    FILENAME3 = "TIB_kink_diffSignal_fullscan.root";
                    FILENAME4 = "TIB_line_diffClusterWidth_fullscan.root";
                    FILENAME5 = "TIB_kink_diffClusterWidth_fullscan.root";
                }
        }

    // TFile* file = new TFile(FILENAME, "READ");

    // if (!file || file->IsZombie()) {
    //     std::cerr << "Error opening file: " << file << std::endl;
    //     return;
    // }

    // // Assume the data is stored in a TCanvas named "canvas"
    // TCanvas* canvas = dynamic_cast<TCanvas*>(file->Get("c1"));
    // if (!canvas) {
    //     std::cerr << "Error: TCanvas not found in the file." << std::endl;
    //     file->Close();
    //     return;
    // }

    // canvas->Draw();

//-----------------------------//
    TFile* file2 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/signal/relative/"+FILENAME2, "READ");

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
    canvas2->Draw();
    TList* primitives = canvas2->GetListOfPrimitives();
    // canvas2->ls();
        // Find the first TH1F in the list
    TGraphErrors* TGE = nullptr;

    TIter next(primitives);
    TObject* obj = nullptr;
    while ((obj = next())) {
        if (obj->IsA()->InheritsFrom(TGraphErrors::Class())) {
            TGE = dynamic_cast<TGraphErrors*>(obj);
            break;
        }
    }
    TGE->Draw("");
    TGE->SetLineColor(2);
    TGE->SetLineWidth(4);

//-----------------------------//
    TGraphErrors* TGE2 = nullptr;
    TFile* file3 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/signal/relative/"+FILENAME3, "READ");

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
    // canvas->Draw();
    TList* primitives3 = canvas3->GetListOfPrimitives();

    TIter next3(primitives3);
    TObject* obj3 = nullptr;

    while ((obj3 = next3())) {
        if (obj3->IsA()->InheritsFrom(TGraphErrors::Class())) {
            TGE2 = dynamic_cast<TGraphErrors*>(obj3);
            break;
        }
    }
    TGE2->Draw("SAME");
    TGE2->SetLineColor(2);
    TGE2->SetLineWidth(4);

    std::cout<<"TGE->GetN(); "<<TGE->GetN()<<std::endl; 
    TGE2->Draw("SAME");
    TGE2->SetLineColor(3);
    TGE2->SetLineWidth(4);
     std::cout<<"TGE2->GetN(); "<<TGE2->GetN()<<std::endl;
//-----------------------------//

    TFile* file4 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/CW/relative/"+FILENAME4, "READ");

    if (!file4 || file4->IsZombie()) {
        std::cerr << "Error opening file: " << file4 << std::endl;
        return;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas4 = dynamic_cast<TCanvas*>(file4->Get("c1"));
    if (!canvas4) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file4->Close();
        return;
    }
// canvas3->ls();
// canvas3->Draw();
    TList* primitive4 = canvas4->GetListOfPrimitives();
    TGraphErrors* TGE3 = nullptr;
    TIter next4(primitive4);
    TObject* obj4 = nullptr;

    while ((obj4 = next4())) {
        if (obj4->IsA()->InheritsFrom(TGraphErrors::Class())) {
            TGE3 = dynamic_cast<TGraphErrors*>(obj3);
        }
    }

    TGE3->Draw("SAME");
    TGE3->SetLineColor(1);
    TGE3->SetLineWidth(4);
    std::cout<<"TGE3->GetN(); "<<TGE3->GetN()<<std::endl;
//-------------------------//



//-----------------------------//

    TFile* file5 = new TFile("/afs/cern.ch/user/p/pvaucell/"+CMSSW+"/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/CW/relative/"+FILENAME5, "READ");

    if (!file5 || file5->IsZombie()) {
        std::cerr << "Error opening file: " << file5 << std::endl;
        return;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas5 = dynamic_cast<TCanvas*>(file5->Get("c1"));
    if (!canvas5) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file5->Close();
        return;
    }
// canvas3->ls();
// canvas3->Draw();
    TList* primitive5 = canvas5->GetListOfPrimitives();
    TGraphErrors* TGE4 = nullptr;
    TIter next5(primitive5);
    TObject* obj5 = nullptr;

    while ((obj5 = next5())) {
        if (obj5->IsA()->InheritsFrom(TGraphErrors::Class())) {
            TGE4 = dynamic_cast<TGraphErrors*>(obj5);
        }
    }


    TGE4->Draw("SAME");
    TGE4->SetLineColor(4);
    TGE4->SetLineWidth(4);
      std::cout<<"TGE4->GetN(); "<<TGE4->GetN()<<std::endl;

//-----------------------------//


    TLegend *leg = new TLegend(0.55, 0.6, 0.9, 0.7);
    leg->SetTextFont(42);
    // leg->SetHeader("Fitting Methods");
    leg->AddEntry(TGE, "Signal kink", "l");
    leg->AddEntry(TGE2, "Signal line", "l");
    leg->AddEntry(TGE3, "CW kink", "l");
    leg->AddEntry(TGE4, "CW line", "l");

    leg->Draw("SAME");
    // canvas->Update();
    canvas2->Update();

// // *****************************************************************************
 
//  c1->Update();
//  canvas->SaveAs("SummaryPlotMethodsv2_"+SUB+"L"+lay+"_DeltaVFDGeneral_"+SCAN+"_v2.pdf");
 canvas2->SaveAs("SummaryPlotMethodsv2_"+SUB+"L"+lay+"_DeltaVFDGeneral_"+SCAN+".pdf");
//  delete canvas;
//  file->Close();
 file2->Close();
 file3->Close();
 file4->Close();
 file5->Close();

}
