TH2F* DrawCorrelationPlot(TTree* tr1, TTree* tr2, int layer=0)
{

	TH2F* h = new TH2F("h","", 60, 0, 300, 60, 0, 300);
	
	ULong64_t detid1, detid2;
	int layer1, layer2;
	double depvolt1, errdepvolt1, depvolt2, errdepvolt2;
	
	tr1->SetBranchAddress("DETID",&detid1);
    tr1->SetBranchAddress("LAYER",&layer1);
    tr1->SetBranchAddress("DEPVOLT",&depvolt1);
    tr1->SetBranchAddress("ERRDEPVOLT",&errdepvolt1);

	tr2->SetBranchAddress("DETID",&detid2);
    tr2->SetBranchAddress("LAYER",&layer2);
    tr2->SetBranchAddress("DEPVOLT",&depvolt2);
    tr2->SetBranchAddress("ERRDEPVOLT",&errdepvolt2);
	
	// store in memory tr2 infos
	map< ULong64_t, double > Vdep_list;
	for(int ientry=0; ientry<tr2->GetEntries(); ientry++)
	{
		tr2->GetEntry(ientry);
		Vdep_list.insert( pair<ULong64_t, double>(detid2, depvolt2));
	}
	
	cout<<tr1->GetEntries()<<" modules in tree1"<<endl;;
	cout<<tr2->GetEntries()<<" modules in tree2"<<endl;;
	
	for(int ientry=0; ientry<tr1->GetEntries(); ientry++)
	{
		tr1->GetEntry(ientry);
		if(layer!=0 && layer1!=layer) continue;
		
		// Get same detid in second tree
		if(Vdep_list.find(detid1)!=Vdep_list.end())
		{
			h->Fill(depvolt1, Vdep_list[detid1]);
		}
	}

	return h;
}


void CompareSignalNoiseRuns(string signalrun, string noiserun, int layer=0)
{
	// Signal bias scan
	string subdet="TIB";
	string antype="ClusterWidth";
	string dirname="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/ntonon/DECO_files/all_modules/";
	string filename = "DECO_allModules_"+antype+"_"+subdet+"_line_";
	TFile *f1 = TFile::Open((dirname+"/"+filename+signalrun+".root").c_str());
	if(!f1) { cout<<"No file : "<<(dirname+"/"+filename+signalrun+".root")<<endl; return;}
	TTree* tr1 = (TTree*) f1->FindObjectAny("tout");
	
	// Noise bias scan
	TFile *f2 = TFile::Open(("./Fit_Results_Median_run"+noiserun+".root").c_str());
	if(!f2) { cout<<"No file : "<<("./Fit_Results_Median_run"+noiserun+".root")<<endl; return;}
	TTree* tr2 = (TTree*) f2->FindObjectAny("tout");
	
	TH2F* h = DrawCorrelationPlot(tr1, tr2, layer);
	TCanvas* c = new TCanvas;
	h->Draw("colz");
	h->SetTitle(subdet.c_str());
	if(layer!=0) h->SetTitle((subdet+" L"+string(Form("%i",layer))).c_str());
	h->GetXaxis()->SetTitle("V_{fd} from signal bias scan [V]");
	h->GetYaxis()->SetTitle("V_{fd} from noise bias scan [V]");
	gStyle->SetOptStat(0);
	c->Print(("BiasScanComparison_"+signalrun+"-"+noiserun+"_"+subdet+"_layer"+to_string(layer)+".png").c_str());
}

void CompareNoiseRuns(string run1, string run2, int layer=0)
{
	// first scan
	string subdet="TIB";
	TFile *f1 = TFile::Open(("./Fit_Results_Median_run"+run1+".root").c_str());
	if(!f1) { cout<<"No file : "<<("./Fit_Results_Median_run"+run1+".root")<<endl; return;}
	TTree* tr1 = (TTree*) f1->FindObjectAny("tout");
	
	// second scan
	TFile *f2 = TFile::Open(("./Fit_Results_Median_run"+run2+".root").c_str());
	if(!f2) { cout<<"No file : "<<("./Fit_Results_Median_run"+run2+".root")<<endl; return;}
	TTree* tr2 = (TTree*) f2->FindObjectAny("tout");
	
	TH2F* h = DrawCorrelationPlot(tr1, tr2, layer);
	TCanvas* c = new TCanvas;
	h->Draw("colz");
	h->SetTitle(subdet.c_str());
	if(layer!=0) h->SetTitle((subdet+" L"+string(Form("%i",layer))).c_str());
	h->GetXaxis()->SetTitle(Form("V_{fd} from run %s [V]", run1.c_str()));
	h->GetYaxis()->SetTitle(Form("V_{fd} from run %s [V]", run2.c_str()));
	gStyle->SetOptStat(0);
	c->Print(("BiasScanComparison_"+run1+"-"+run2+"_"+subdet+"_layer"+to_string(layer)+".png").c_str());
}

void CompareRuns()
{
	//2012
	//CompareSignalNoiseRuns("193928", "203243", 0);
	//2016
	//CompareSignalNoiseRuns("271056", "280667", 0);
	//2017
	//CompareSignalNoiseRuns("303824", "303272", 0);
	CompareSignalNoiseRuns("303824", "307585", 3);
	//2018
	//CompareSignalNoiseRuns("323374", "323011", 0);
	//2019
	//CompareSignalNoiseRuns("323374", "328691", 0);
	//CompareNoiseRuns("323011", "328691", 0);
	//CompareSignalNoiseRuns("323374", "331595", 0);
	//CompareNoiseRuns("328691", "331595", 0);
}
