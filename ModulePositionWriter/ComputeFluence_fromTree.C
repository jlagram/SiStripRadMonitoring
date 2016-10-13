{

  float sigma_7TeV=79.9e12; //in fb
  // 7TeV
  // From CMS: 68mb
  // From Totem: 73.5mb

  // 14TeV
  // 79.9mb

  // Tree with fluence map
  TFile* fmap = TFile::Open("fluenceTree_7000.root");
  if(!fmap) { cout<<"No file : "<<"fluenceTree_7000.root"<<endl; return;}
  TTree* trfluIn = (TTree*) fmap->FindObjectAny("fluence");

  float Z_flu;
  float R_flu;
  float fluence_flu;
  float fluence_err_flu;

  trfluIn->SetBranchAddress("Z",&Z_flu);
  trfluIn->SetBranchAddress("R",&R_flu);
  trfluIn->SetBranchAddress("fluence",&fluence_flu);


  // Tree with modules positions
  TFile *fposition = TFile::Open("modulesPositions.root");
  if(!fposition) { cout<<"No file : "<<"modulesPositions.root"<<endl; return;}
  TTree* trposition = (TTree*) fposition->FindObjectAny("ttree");
  
  int detid;
  int subdetid;
  double R;
  double Z;
  
  trposition->SetBranchAddress("detId",&detid);
  trposition->SetBranchAddress("subdetId",&subdetid);
  trposition->SetBranchAddress("R",&R);
  trposition->SetBranchAddress("Z",&Z);
  
  // Tree with modules fluence
  TFile* fflu = new TFile("modulesFluence_7000.root", "recreate");
  TTree* tflu = new TTree("tflu","modules fluence");
  
  double fluence;
  
  tflu->Branch("detId",&detid, "detId/I");
  tflu->Branch("subdetId",&subdetid, "subdetId/I");
  tflu->Branch("R",&R, "R/D");
  tflu->Branch("Z",&Z, "Z/D");
  tflu->Branch("fluence", &fluence, "fluence/D");
  
  UInt_t nentries = trposition->GetEntries();
  UInt_t nentflu = trfluIn->GetEntries();
  for(UInt_t ie = 0; ie <nentries; ie++)
  {
    trposition->GetEntry(ie);

	UInt_t iflu=0;
	float DZ=0;
	float DR=0;
	bool found=false;
	// step of 2.5cm in R and ~1.6cm in Z
	while(iflu<nentflu && !found)
	{
	    trfluIn->GetEntry(iflu);
		DZ=TMath::Abs(Z-Z_flu);
		DR=TMath::Abs(R-R_flu);
	    if(DR<2.5 && DZ<2.) {found=true; fluence=fluence_flu*sigma_7TeV;}
		iflu++;
	}
	
	tflu->Fill();
	
    //cout<<detid<<" "<<R<<"-"<<R_flu<<" "<<Z<<"-"<<Z_flu<<" "<<fluence<<endl;
  }

  tflu->Write();
  fflu->Close();

}
