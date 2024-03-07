#include <iostream>
#include <TROOT.h>
#include "TVectorD.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TEfficiency.h" 
#include "TMath.h" 

void plot()
{
    bool Use3Files = true;
    TString SUB = "TOB"; // Only TIB or TOB at the moment // TID is not possible because no TIB modules in small scans and you may lack stats for TEC
    TFile* file = new TFile("./Delta"+SUB+"_L1SMALLSCAN.root", "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << file << std::endl;
        return 0;
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
    TFile* file2 = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_13_2_2/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/CW/relative/"+SUB+"_kink_diffClusterWidth.root", "READ");

    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error opening file: " << file2 << std::endl;
        return 0;
    }

    // Assume the data is stored in a TCanvas named "canvas"
    TCanvas* canvas2 = dynamic_cast<TCanvas*>(file2->Get("c1"));
    if (!canvas2) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file2->Close();
        return;
    }

    TList* primitives = canvas2->GetListOfPrimitives();
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
    //-----------------------------//

            TFile* file3 = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_13_2_2/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/CW/relative/"+SUB+"_line_diffClusterWidth.root", "READ");

            if (!file3 || file3->IsZombie()) {
                std::cerr << "Error opening file: " << file3 << std::endl;
                return 0;
            }

            // Assume the data is stored in a TCanvas named "canvas"
            TCanvas* canvas3 = dynamic_cast<TCanvas*>(file3->Get("c1"));
            if (!canvas3) {
                std::cerr << "Error: TCanvas not found in the file." << std::endl;
                file3->Close();
                return;
            }

            TList* primitive3 = canvas3->GetListOfPrimitives();
                // Find the first TH1F in the list
            TGraphErrors* TGE3 = nullptr;
            TIter next3(primitive3);
            TObject* obj3 = nullptr;
            while ((obj3 = next3())) {
                if (obj3->IsA()->InheritsFrom(TGraphErrors::Class())) {
                    TGE3 = dynamic_cast<TGraphErrors*>(obj3);
                    break;
                }
            }

//-------------------------//

    //-----------------------------//

            TFile* file4 = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_13_2_2/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/signal/relative/"+SUB+"_kink_diffSignal.root", "READ");

            if (!file4 || file4->IsZombie()) {
                std::cerr << "Error opening file: " << file4 << std::endl;
                return 0;
            }

            // Assume the data is stored in a TCanvas named "canvas"
            TCanvas* canvas4 = dynamic_cast<TCanvas*>(file4->Get("c1"));
            if (!canvas4) {
                std::cerr << "Error: TCanvas not found in the file." << std::endl;
                file4->Close();
                return;
            }

            TList* primitive4 = canvas4->GetListOfPrimitives();
                // Find the first TH1F in the list
            TGraphErrors* TGE4 = nullptr;
            TIter next4(primitive4);
            TObject* obj4 = nullptr;
            while ((obj4 = next4())) {
                if (obj4->IsA()->InheritsFrom(TGraphErrors::Class())) {
                    TGE4 = dynamic_cast<TGraphErrors*>(obj4);
                    break;
                }
            }


    //-----------------------------//

            TFile* file5 = new TFile("/afs/cern.ch/user/p/pvaucell/CMSSW_13_2_2/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/plots/signal/relative/"+SUB+"_line_diffSignal.root", "READ");

            if (!file5 || file5->IsZombie()) {
                std::cerr << "Error opening file: " << file5 << std::endl;
                return 0;
            }

            // Assume the data is stored in a TCanvas named "canvas"
            TCanvas* canvas5 = dynamic_cast<TCanvas*>(file5->Get("c1"));
            if (!canvas5) {
                std::cerr << "Error: TCanvas not found in the file." << std::endl;
                file5->Close();
                return;
            }

            TList* primitive5 = canvas5->GetListOfPrimitives();
                // Find the first TH1F in the list
            TGraphErrors* TGE5 = nullptr;
            TIter next5(primitive5);
            TObject* obj5 = nullptr;
            while ((obj5 = next5())) {
                if (obj5->IsA()->InheritsFrom(TGraphErrors::Class())) {
                    TGE5 = dynamic_cast<TGraphErrors*>(obj5);
                    break;
                }
            }


//-------------------------//
    canvas->Update();
    TGE->Draw("SAME");
    TGE->SetLineColor(2);
    TGE->SetLineWidth(2);

    TGE3->Draw("SAME");
    TGE3->SetLineColor(1);
    TGE3->SetLineWidth(2);

    TGE4->Draw("SAME");
    TGE4->SetLineColor(3);
    TGE4->SetLineWidth(2);

    TGE5->Draw("SAME");
    TGE5->SetLineColor(4);
    TGE5->SetLineWidth(2);

    TLegend *leg = new TLegend(0.55, 0.6, 0.9, 0.7);
    leg->SetTextFont(42);
    // leg->SetHeader("Fitting Methods");
    leg->AddEntry(TGE, "CW kink", "l");
    leg->AddEntry(TGE3, "CW line", "l");
    leg->AddEntry(TGE4, "Signal kink", "l");
    leg->AddEntry(TGE5, "Signal line", "l");
    leg->Draw("SAME");
    canvas->Update();

// // *****************************************************************************
 
//  c1->Update();
 canvas->SaveAs(SUB+"L1DeltaVFDGeneral.pdf");
//  delete canvas;
 file->Close();
 file2->Close();
 file3->Close();
 file4->Close();
 file5->Close();
}
