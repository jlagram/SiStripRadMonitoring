void averageHistos() {
    TString Method = "CW"; // CW or signal
    TString kink = "kink"; // kink or line   
    TString path = Method+"/"+kink+"/simu/";
    TString extra = "Signal";
    if (Method == "CW") {
        extra = "ClusterWidth";
    }
    TString subdet = "TIB";

    std::vector<TString> fileNames;
    fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121381_"+extra+"_simu");
    fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121382_"+extra+"_simu");
    fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121385_"+extra+"_simu");
    fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121389_"+extra+"_simu");


    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121390_"+extra+"_simu");

    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121610_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121613_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121614_"+extra+"_simu");

    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125861_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125862_"+extra+"_simu");
    // // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125865_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125866_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125869_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369125870_"+extra+"_simu");

    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121605_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121606_"+extra+"_simu");
    // fileNames.push_back("KinkVSLumi_2024_"+subdet+"_"+kink+"_detid369121609_"+extra+"_simu");
    
    const TString histoName = "myHisto";

    TH1* hSum = nullptr;
    const int nFiles = fileNames.size();

    const TString canvasName = "c1";  // nom du TCanvas
    const TString graphName = "";  // nom du TGraphErrors à l’intérieur

    std::vector<TGraphErrors*> graphs;

    for (int i = 0; i < nFiles; ++i) {
        TFile* f = TFile::Open(path+fileNames[i]+".root");
        if (!f || f->IsZombie()) {
            std::cerr << "Erreur en ouvrant " << fileNames[i] << std::endl;
            continue;
        }

        TCanvas* c = dynamic_cast<TCanvas*>(f->Get(canvasName));
        if (!c) {
            std::cerr << "Canvas " << canvasName << " non trouvé dans " << fileNames[i] << std::endl;
            f->Close();
            continue;
        }

        TGraphErrors* gr = dynamic_cast<TGraphErrors*>(c->FindObject(graphName));
        if (!gr) {
            std::cerr << "TGraphErrors " << graphName << " non trouvé dans " << fileNames[i] << std::endl;
            f->Close();
            continue;
        }

        graphs.push_back((TGraphErrors*)gr->Clone());
        f->Close();
    }

    if (graphs.size() == 0) {
        std::cerr << "Aucun graph trouvé, rien à faire." << std::endl;
        return;
    }

    const int nPoints = graphs[0]->GetN();
  
    std::vector<double> x(nPoints, 0), y(nPoints, 0), ey(nPoints, 0);

    for (int i = 0; i < nPoints; ++i) {
        double xi = 0, yi = 0, eyi = 0;
          float renorm= 0.;
        for (auto& g : graphs) {
            
            double x_val=0, y_val=0;
            g->GetPoint(i, x_val, y_val);
            xi = x_val; // tous les x sont censés être alignés

            std::cout<<"point "<<i<<" x = "<<xi<<", y = "<<y_val<<std::endl;
            if (y_val <= 30)continue;
            yi += y_val;
            eyi += std::pow(g->GetErrorY(i), 2);
            renorm += 1.0;
        }
        std::cout<<"renorm = "<<renorm<<std::endl;
        std::cout<<"yi = "<<yi<<std::endl;
        if (renorm != graphs.size() && renorm != 0)
            {
                x[i]  = xi;
                // std::cout<<"yi v2 = "<<yi<<std::endl;
                // std::cout<<"renorm v2 = "<<renorm<<std::endl;
                y[i]  = yi / renorm;  ;//(renorm-1)
                // std::cout<<"y[i] = "<<y[i]<<std::endl;
                ey[i] = std::sqrt(eyi) /renorm; // erreur moyenne quadratique
            }
        else
            {
                x[i]  = xi;
                y[i]  = yi / graphs.size();  ;//(renorm-1)
                ey[i] = std::sqrt(eyi) / graphs.size(); // erreur moyenne quadratique
            }
    }

    TGraphErrors* gAvg = new TGraphErrors(nPoints, &x[0], &y[0], nullptr, &ey[0]);
    gAvg->SetName("graph_moyenne");
    gAvg->SetTitle("Moyenne des TGraphErrors");

    TFile* fout = new TFile("Graph_"+subdet+"_"+Method+"_"+kink+".root", "RECREATE");
    gAvg->Write();
    fout->Close();

    std::cout << "Graph moyenné sauvegardé dans graph_moyenne.root" << std::endl;
}