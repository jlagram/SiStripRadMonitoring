{

  // Histo with fluence map
  TFile* fmap = TFile::Open("fluenceMap.root");
  if(!fmap) { cout<<"No file : "<<"fluenceMap.root"<<endl; continue;}
  TH2F* hmap = (TH2F*) fmap->Get("hfluence");

  // Tree with modules positions
  TFile *fpos = TFile::Open("modulesPositions.root");
  if(!fpos) { cout<<"No file : "<<"modulesPositions.root"<<endl; continue;}
  TTree* trpos = (TTree*) fpos->FindObjectAny("ttree");
  
  int detid;
  int subdetid;
  double R;
  double Z;
  
  trpos->SetBranchAddress("detId",&detid);
  trpos->SetBranchAddress("subdetId",&subdetid);
  trpos->SetBranchAddress("R",&R);
  trpos->SetBranchAddress("Z",&Z);
  
  // Tree with modules fluence
  TFile* fflu = new TFile("modulesFluence.root", "recreate");
  TTree* tflu = new TTree("tflu","modules fluence");
  
  double fluence;
  
  tflu->Branch("detId",&detid, "detId/I");
  tflu->Branch("subdetId",&subdetid, "subdetId/I");
  tflu->Branch("R",&R, "R/D");
  tflu->Branch("Z",&Z, "Z/D");
  tflu->Branch("fluence", &fluence, "fluence/D");
  
  UInt_t nentries = trpos->GetEntries();
  for(UInt_t ie = 0; ie <nentries; ie++)
  {
    trpos->GetEntry(ie);

    // Get right bin in fluence map
    //   TH2F* h = new TH2F("hfluence", "", 136, -274, 270, 65, 0, 130);
    int Rbin = R/2+1;
    int Zbin = (Z+274)/4+1;
	if(Zbin==137) Zbin=136; // ~50 modules at Z~271cm 
	fluence = hmap->GetBinContent(Zbin, Rbin);
	tflu->Fill();
	
    //cout<<detid<<" "<<R<<" "<<Z<<" "<<fluence<<endl;
  }

  tflu->Write();
  fflu->Close();

}
