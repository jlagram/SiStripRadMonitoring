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
    TString TYPE = "CW"; //signal or CW
    TString TYPEEXTENDED = "ClusterWidth";//Signal
    TString METHOD = "kink"; //kink or line
    TString PATH = "./plots/"+TYPE+"/"+METHOD+"/";
    TString SUB = "TOB"; // Only TIB or TOB at the moment // TID is not possible because no TIB modules in small scans and you may lack stats for TEC
    
    int nDETID = 18 ; 

    TString TIBL1_DETIDS[18] = {"369121381","369121382","369121385","369121386","369121389","369121390",
                                "369121605","369121606","369121609","369121610","369121613","369121614",
                                "369125861","369125862","369125865","369125866","369125869","369125870"}

    
    TString TOBL4_DETIDS[6]= {"436281508", "436281512", "436281516", "436281520", "436281524", "436281528"}; 
    TString TOBL1_DETIDS[12]= {"436232901", "436232902", "436232905", "436232906", "436232909", "436232910", 
                                "436232913", "436232914", "436232917", "436232918", "436232921", "436232922" };
    
    TCanvas *CAN1 = new TCanvas("CAN1", "plots",200,0,1000,1100);
    CAN1->SetFillColor(10);
    CAN1->SetFillStyle(4000);
    CAN1->SetBorderSize(2);

    TPad* pad1 = new TPad("pad1","This is pad1",0.01,0.01,0.99,1,21);
    pad1->SetFillColor(0);
    pad1->SetBorderMode(0);
    pad1->SetFrameFillColor(10);
    pad1->Draw();
    pad1->SetLogy(0);
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.1);
    pad1->SetRightMargin(0.1);
    pad1->SetLeftMargin(0.1);

    

    for (unsigned int i = 0 ; i < nDETID ; i++)
        {
            TFile* file = new TFile(PATH+"KinkVSLumi_CSI_"+SUB+"_"+METHOD+"detid"+TIBL1_DETIDS[i]+"_"+TYPEEXTENDED, "READ");
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

            TList* primitive = canvas->GetListOfPrimitives();
                // Find the first TH1F in the list
            TGraphErrors* TGE = nullptr;
            TIter next(primitive);
            TObject* obj = nullptr;
            while ((obj = next())) {
                if (obj->IsA()->InheritsFrom(TGraphErrors::Class())) {
                    TGE = dynamic_cast<TGraphErrors*>(obj);
                    break;
                }
            }

            TGE->Draw("SAME");
            TGE->SetLineColor(2);
            TGE->SetLineWidth(2);
            CAN1->Update();

            file->close();
        }
    
    



}
