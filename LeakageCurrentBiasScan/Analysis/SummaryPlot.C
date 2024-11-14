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

void plotArea() {
    // Ouvrir le premier fichier root qui contient les 10 TGraph
    TString subdet = "TIB";
    TString layer = "L1";
    TString Mode = ""; // Delta
    TString Scan = "SMALLSCAN"; //SMALL
    TString file1Name = Mode+subdet + "_" + layer + Scan + ".root";   
    TFile *file1 = TFile::Open(file1Name);

    if (!file1 || file1->IsZombie()) {
        std::cerr << "Erreur: impossible d'ouvrir file1.root" << std::endl;
        return;
    }

    TCanvas* canvas3 = dynamic_cast<TCanvas*>(file1->Get("c1"));
    if (!canvas3) {
        std::cerr << "Error: TCanvas not found in the file." << std::endl;
        file1->Close();
        return;
    }
    // canvas3->cd();

    // canvas3->ls();
    TString SimuIn[10] = {
        "lumigr_TIB_L1", "lumigr_TIB_L2", "lumigr_TIB_L3", "lumigr_TIB_L4",
        "lumigr_TOB_L1", "lumigr_TOB_L2", "lumigr_TOB_L3", "lumigr_TOB_L4","lumigr_TOB_L5", "lumigr_TOB_L6",
        };
    TString DataIn[10] = {
        "Full Depletion Voltage Average","Graph","Graph","Graph","Graph","Graph","Graph","Graph","Graph","Graph"
        };


    TList* primitive3 = canvas3->GetListOfPrimitives();
    int count = 0;
    // Find the first TH1F in the list
     std::vector<TGraph*> graphs;
    TIter next3(primitive3);
    TObject* obj3 = nullptr;
    while ((obj3 = next3())) {
        if (obj3->IsA()->InheritsFrom(TGraph::Class())) {
            // std::cout<<" i :"<<count<<std::endl;
            graphs.push_back(dynamic_cast<TGraph*>(obj3));
            count++;
            // break;
        }
        if (count == 10) break;
    }
    
    if (graphs.size() != 10) {
        std::cerr << "Erreur: impossible de trouver les 10 graphs : " <<graphs.size()<< std::endl;
        return;
    }

    // Créer deux TGraph pour stocker les valeurs min et max pour chaque bin
    int nPoints = graphs[0]->GetN();
    std::vector<double> xValues(nPoints), minValues(nPoints), maxValues(nPoints);

    for (int i = 0; i < nPoints; ++i) {
        double x, y;
        graphs[0]->GetPoint(i, x, y);
        double minY = y;
        // std::cout<<" x :"<<x<<" y :"<<y<<std::endl;
        double maxY = y;
        // if (i == nPoints-1) std::cout<<" x :"<<x<<" y :"<<y<<std::endl;
        // Boucle sur les autres graphs pour trouver le min et max à chaque bin
        for (int j = 1; j < 10; ++j) {
            double yTemp;

            if (x < 320) // First 14 bins have optimized parameters for each fit function
                {
                    graphs[j]->GetPoint(i, x, yTemp);
                    if (yTemp < minY && yTemp != 0) minY = yTemp;
                    if (yTemp > maxY) maxY = yTemp;
                }
            else
                {
                    graphs[j]->GetPoint(i, x, yTemp);
                    // if (i == nPoints-1) std::cout<<" x :"<<x<<" yTemp :"<<yTemp<<" minY :"<<minY<<std::endl;
                    // !! this conditions works somehow, but it is not the best way to do it sorry
                    if ( yTemp > 150  && minY>0) //&& yTemp<minY 
                        {
                            minY = yTemp;
                        }
                    else if (minY==0)
                        {
                            minY = yTemp;
                        }
                    if (yTemp > maxY) maxY = yTemp;

                }

        }

        xValues[i] = x;
        minValues[i] = minY;
        maxValues[i] = maxY;
    }

    // Créer les TGraph pour min et max
    TGraph *minGraph = new TGraph(nPoints, &xValues[0], &minValues[0]);
    TGraph *maxGraph = new TGraph(nPoints, &xValues[0], &maxValues[0]);

    // Créer un canvas
    TCanvas *c = new TCanvas("c", "Min-Max Area Plot", 800, 600);

    // Tracer l'aire hachurée entre min et max
    TH1F *frame = new TH1F("frame", "", 100, xValues[0], xValues[nPoints-1]);
    frame->GetYaxis()->SetRangeUser(*std::min_element(minValues.begin(), minValues.end()) - 10, 
                                    *std::max_element(maxValues.begin(), maxValues.end()) + 50);
    frame->SetStats(0);
    frame->GetYaxis()->SetTitle("Full Depletion Voltage [V]");
    frame->GetXaxis()->SetTitle("Int. Lumi [fb^{-1}]");
    // frame->SetAxisRange(0, 300);
    frame->SetMinimum(0);
    frame->Draw();

    TGraphErrors *fillArea = new TGraphErrors(nPoints);
    for (int i = 0; i < nPoints; ++i) {
        fillArea->SetPoint(i, xValues[i], (minValues[i] + maxValues[i]) / 2);  // Moyenne pour la position centrale
        fillArea->SetPointError(i, 0, (maxValues[i] - minValues[i]) / 2);      // Intervalle pour la hauteur
    }

    fillArea->SetFillColorAlpha(kGray + 2, 0.35);  // Couleur grise avec transparence
    fillArea->Draw("3");  // "3" pour tracer la zone hachurée

    // Ouvrir le deuxième fichier root et tracer une autre courbe
    TFile *file2 = TFile::Open("/afs/cern.ch/user/p/pvaucell/public/VdepEvo/VdepEvolution/Predictions/lumigr_"+subdet+"_"+layer+".root");
    if (!file2 || file2->IsZombie()) {
        std::cerr << "Erreur: impossible d'ouvrir lumigr_"+subdet+"_"+layer+".root" << std::endl;
        return;
    }

    TGraph *additionalGraph = (TGraph*)file2->Get("lumigr_"+subdet+"_"+layer+"");  // Assumer le nom du graph dans le deuxième fichier
    if (!additionalGraph) {
        std::cerr << "Erreur: impossible de trouver la courbe dans lumigr_lumigr_"+subdet+"_"+layer+".root" << std::endl;
        return;
    }
    additionalGraph->SetLineColor(kRed);  // Couleur de la courbe
    additionalGraph->SetLineWidth(2);
    additionalGraph->Draw("L SAME");  // Tracer la courbe sur le même canvas

    // Légende
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(fillArea, "V_{fd} range", "f");
    legend->AddEntry(additionalGraph, "Simulation", "l");
    legend->Draw();


    TLine* lvdrop = new TLine(195.5,0,195.5,347);
    lvdrop->SetLineStyle(2);
    lvdrop->SetLineColor(1);//red
    lvdrop->SetLineWidth(2);
    lvdrop->Draw();

    TLine* lvdrop1 = new TLine(29.5,0,29.5,347);
    lvdrop1->SetLineStyle(2);
    lvdrop1->SetLineColor(1);//red
    lvdrop1->SetLineWidth(2);
    lvdrop1->Draw();


 //------Start of Copy Paste
TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = true;
TString extraText   = "Preliminary";
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
TString lumi_sqrtS = "TIB L1";
TString lumiText = lumi_13TeV+lumi_sqrtS;
  float H = canvas3->GetWh();
  float W = canvas3->GetWw();
  float l = canvas3->GetLeftMargin();
  float t = canvas3->GetTopMargin();
  float r = canvas3->GetRightMargin();
  float b = canvas3->GetBottomMargin();

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  float extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r-0.005,1-t+lumiTextOffset*t,lumiText);

      latex.SetTextFont(cmsTextFont);
      latex.SetTextAlign(11); 
      latex.SetTextSize(cmsTextSize*t);    
      latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);

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
      latex.DrawLatex(posX_+0.09, posY_, extraText);
	    }


    // Afficher le canvas
    c->Update();
    c->SaveAs("SummaryPlot.png");
    c->SaveAs("SummaryPlot.pdf");
}
