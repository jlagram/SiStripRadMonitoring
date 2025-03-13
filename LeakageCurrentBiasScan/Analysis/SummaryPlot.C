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
//!!
//the code is made such that, it can start from any point in lumi using the nPointsCorr varaible, for any number of scans using nPoints, but explecitely for 10 fits
// If you want to remove one fit method, good luck with it :D



void SummaryPlot() {
    // Ouvrir le premier fichier root qui contient les 10 TGraph
    TString subdet = "TEC";// TIB or TOB
    TString layer = "R5";//L1 or  L4
    TString extra = "5";
    TString Mode = ""; // Delta
    TString Scan = "FULLSCAN"; //SMALL, FULL
    TString file1Name = Mode+subdet + "_" + layer + Scan + ".root";   
    //-------------------------------------------------------------------
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
    //--- filemanes of the predictions made by Paul :D
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
    std::vector<double> xValues, minValues, maxValues;

    //-----------------------------------------------------
    // !! 
    //!! 1)  Seuils spécifiques à chaque std::vector<double>
    std::vector<double> minThresholds ;
    std::vector<double> maxThresholds ;

    for ( int i  = 0; i< nPoints ; i++)
        {
            if (Scan == "SMALLSCAN")
                {
                    if (i == nPoints-1 || i == nPoints-2 || i == nPoints-3)
                        {
                            if (subdet == "TIB")
                                {
                                    minThresholds.push_back(100);
                                    maxThresholds.push_back(350);
                                }
                            else if (subdet == "TOB" && layer == "L1")
                                {
                                    minThresholds.push_back(100);
                                    maxThresholds.push_back(300);
                                }
                            else if (subdet == "TOB" && layer == "L4")
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(300);    
                                }
                            else
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(350);
                                }
                        }
                    else
                        {
                                    if (subdet == "TIB")
                                        {
                                            if (i == 0)
                                                {
                                                    minThresholds.push_back(210);
                                                    maxThresholds.push_back(350);
                                                }
                                            else
                                                {
                                                    minThresholds.push_back(40);
                                                    maxThresholds.push_back(350);
                                                }
                                        }
                                    else if (subdet == "TOB" && layer == "L1")
                                        {
                                            minThresholds.push_back(40);
                                            maxThresholds.push_back(300);
                                        }
                                    else if (subdet == "TOB" && layer == "L4")
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(300);    
                                        }
                                    else
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(350);
                                        }
                        }
                }
            else //FULLSCAN
                {
                    if (i == nPoints-1 )
                        {
                            if (subdet == "TIB" && layer == "L1")
                                {
                                    minThresholds.push_back(160);
                                    maxThresholds.push_back(350);
                                }
                            else if (subdet == "TIB" && layer == "L4")
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(200);
                                }
                            else if (subdet == "TOB" && layer == "L1")
                                {
                                    minThresholds.push_back(100);
                                    maxThresholds.push_back(300);
                                }
                            else if (subdet == "TOB" && layer == "L4")
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(300);    
                                }
                            else if (subdet == "TEC" && layer == "R5")
                                {
                                    minThresholds.push_back(110);
                                    maxThresholds.push_back(200);    
                                }
                            else
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(350);
                                }
                        }
                    else
                        {
                            if (subdet == "TIB" && layer == "L1")
                                {

                                    if (i == 2)
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(200);
                                        }
                                    else if (i == 1)
                                        {
                                            minThresholds.push_back(100);
                                            maxThresholds.push_back(250);
                                        }
                                    else
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(350);
                                        }
                                }
                            else if (subdet == "TIB" && layer == "L4")
                                {
                                    if (i == 5)
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(150);
                                        }
                                    else if (i == 8)
                                        {
                                            minThresholds.push_back(40);
                                            maxThresholds.push_back(200);
                                        }
                                    else if (i == 4)
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(150);
                                        }
                                    else if (i == 3)
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(150);
                                        }
                                    else if (i == 2)
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(200);
                                        }
                                    else if (i == 1)
                                        {
                                            minThresholds.push_back(100);
                                            maxThresholds.push_back(250);
                                        }
                                    else if (i == 0)
                                        {
                                            minThresholds.push_back(100);
                                            maxThresholds.push_back(400);
                                        }
                                    else
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(350);
                                        }
                                }
                            else if (subdet == "TOB" && layer == "L1")
                                {
                                    if (i == 2)
                                        {
                                            minThresholds.push_back(30);
                                            maxThresholds.push_back(250);
                                        }
                                    else if (i == 0)
                                        {
                                            minThresholds.push_back(100);
                                            maxThresholds.push_back(400);
                                        }
                                    else
                                        {
                                            minThresholds.push_back(30);
                                            maxThresholds.push_back(400);
                                        }

                                }
                            else if (subdet == "TOB" && layer == "L4")
                                {
                                    if (i == 0)
                                        {
                                            minThresholds.push_back(100);
                                            maxThresholds.push_back(350);
                                        }
                                    else
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(300);
                                        }
    
                                }
                            else if (subdet == "TEC" && layer == "R5")
                                {
                                    if (i == 6)
                                        {
                                            minThresholds.push_back(0);
                                            maxThresholds.push_back(160);
                                        }
                                    else if ( i == 7)
                                        {
                                            minThresholds.push_back(60);
                                            maxThresholds.push_back(300);
                                        }
                                    else if (i == 8)
                                        {
                                            minThresholds.push_back(110);
                                            maxThresholds.push_back(200);
                                        }
                                    else
                                        {
                                            minThresholds.push_back(50);
                                            maxThresholds.push_back(300);
                                        }
    
                                }
                            else
                                {
                                    minThresholds.push_back(50);
                                    maxThresholds.push_back(350);
                                }
                        }
                }

        //  std::cout<<" i :"<<i<<" minThresholds :"<<minThresholds[i]<<" maxThresholds :"<<maxThresholds[i]<<std::endl;

        }
    //------------------------------------------------------

    // !! 
    // !! 2) Store graph points in vectors
    std::vector<std::vector<double>> data;//[nScan][10]

    for ( int i = 0 ; i < nPoints ; i++) 
        {
            if (subdet == "TOB" && layer == "L1" && i <= 4 && Scan == "SMALLSCAN")
                {
                    continue;
                }
            double x = 0;
            double y = 0;
            double lower_threshold = minThresholds[i];
            double upper_threshold = maxThresholds[i];
            std::vector<double> temp;

            for (int j = 0; j < 10; ++j) 
                {

                    graphs[j]->GetPoint(i, x, y);
                    if ( y > lower_threshold && y < upper_threshold)
                        {
                            temp.push_back(y);
                        }
                    // std::cout<<" x :"<<x<<" y :"<<y<<std::endl;
                }
            if (temp.size()==0)
                {
                    std::cout<<"No data registered for this run with the defined thresholds"<<std::endl;
                    break;
                }
            data.push_back(temp);
            xValues.push_back(x);
            std::cout<<" xi:"<<i<<std::endl;
        }

    //---------------------------------------------------

    // !!
    // !! 3) Trouver les min et max pour chaque std::vector<double> respectant les seuils
    // Vector pour stocker les min et max trouvés

    // Loop sur chaque vecteur interne et ses seuils
    for (size_t i = 0; i < data.size(); ++i) {
        const auto& vec = data[i];

        auto min_it = std::min_element(vec.begin(), vec.end());
        auto max_it = std::max_element(vec.begin(), vec.end());

        double min_value = *min_it; 
        double max_value = *max_it;

        minValues.push_back(min_value);
        maxValues.push_back(max_value);
        // std::cout<<" data_i:"<<i<<std::endl;

    }

    //---------------------------------------------------
    // Afficher les résultats
    // for (size_t i = 0; i < minValues.size(); ++i) {
    //     std::cout << " Vecteur " << i + 1 << " : "<< xValues[i]
    //               << " Min = " << minValues[i]
    //               << ", Max = " << maxValues[i] << std::endl;
    // }
    //---------------------------------------------------
    int nPointsCorr = data.size();

    // Créer les TGraph pour min et max
    TGraph *minGraph = new TGraph(nPointsCorr, &xValues[0], &minValues[0]);
    TGraph *maxGraph = new TGraph(nPointsCorr, &xValues[0], &maxValues[0]);

    // Créer un canvas
    TCanvas *c = new TCanvas("c", "Min-Max Area Plot", 800, 600);

    // Tracer l'aire hachurée entre min et max
    TH1F *frame = new TH1F("frame", "", 100, 0,400);//xValues[0], xValues[nPointsCorr-1] if you wan to focus on the data especially for smallscan TOBL1
    frame->GetYaxis()->SetRangeUser(0,400);
    // frame->GetXaxis()->SetRange(1,-1);
    frame->SetStats(0);
    frame->GetYaxis()->SetTitle("Full Depletion Voltage [V]");
    frame->GetXaxis()->SetTitle("Int. Lumi [fb^{-1}]");
    // frame->SetAxisRange(0, 300);
    frame->SetMinimum(0);
    frame->Draw();

    TGraphErrors *fillArea = new TGraphErrors(nPointsCorr);
    for (int i = 0; i < nPointsCorr; ++i) {
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

    TGraph *additionalGraph = (TGraph*)file2->Get("lumigr_"+subdet+"_"+layer+"");
    if  (subdet == "TEC" || subdet == "TID" )
        {
            additionalGraph = (TGraph*)file2->Get("lumigr_"+extra);
 
        }
    if (!additionalGraph) {
        std::cerr << "Erreur: impossible de trouver la courbe dans _lumigr_"+subdet+"_"+layer+".root" << std::endl;
        return;
    }
    additionalGraph->SetLineColor(kRed);  // Couleur de la courbe
    additionalGraph->SetLineWidth(2);
    additionalGraph->Draw("L SAME");  // Tracer la courbe sur le même canvas

    // Légende
    TLegend *legend = new TLegend(0.65, 0.7, 0.9, 0.9);
    legend->AddEntry(fillArea, "V_{fd} range", "f");
    legend->AddEntry(additionalGraph, "Simulation", "l");
    legend->Draw();


    TLine* lvdrop = new TLine(195.5,0,195.5,400);
    lvdrop->SetLineStyle(2);
    lvdrop->SetLineColor(1);//red
    lvdrop->SetLineWidth(2);
    lvdrop->Draw();

    TLine* lvdrop1 = new TLine(29.5,0,29.5,400);
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
TString lumi_sqrtS = subdet+layer;
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
    c->SaveAs("SummaryPlot_"+Scan+subdet+layer+".png");
    c->SaveAs("SummaryPlot_"+Scan+subdet+layer+".pdf");

}
