void GetNumberOfEvents(TFile* f, int& ntot, int& n300, int& n325)
{
  f->cd();
  string histoname = "hNevtPerStep_TIB";
  TH1F* h = (TH1F*) f->Get(histoname.c_str());
  if(!h) h = (TH1F*) f->Get("hNevtPerStep");
  if(!h) { cout<<"Error : no histo '"<<histoname<<"'"<<endl; ntot=0; n300=0; n325=0; return;}
  ntot = h->GetEntries();

  int ibin300 = 61;
  int ibin = h->FindBin(300);
  if(ibin!=ibin300) { ibin300=ibin; cout<<"Warning : 300V bin found to be "<<ibin<<". Please check."<<endl;}
  n300 = h->GetBinContent(ibin300);
  int ibin325 = 66;
  ibin = h->FindBin(325);
  if(ibin!=ibin325) { ibin325=ibin; cout<<"Warning : 325V bin found to be "<<ibin<<". Please check"<<endl;}
  n325 = h->GetBinContent(ibin325);

  return;
}

void GetNumberOfTracks(TFile* f, int& ntot, int& overflow, float& mean, float& fbelow10)
{
  f->cd();
  string histoname = "hNtracks_TIB";
  TH1F* h = (TH1F*) f->Get(histoname.c_str());
  if(!h) h = (TH1F*) f->Get("hNtracks");
  if(!h) { cout<<"Error : no file '"<<f->GetName()<<"'"<<endl; ntot=0; overflow=0; mean=0; fbelow10=0; return;}
  mean = h->GetMean();

  // Should check binning : 0-1000, 1000 bins
  int nbins=1000;
  if(h->GetNbinsX()!=nbins) { nbins=h->GetNbinsX(); cout<<"Warrning : not standard binning for "<<histoname<<". Use : "<<nbins<<endl;}

  overflow = h->GetBinContent(nbins+1);
  for(int ibin=1; ibin<nbins+1; ibin++)
    ntot+=h->GetBinContent(ibin)*(ibin-1);

  int nbelow10=0;
  nbelow10 = h->Integral(1,10);
  if(ntot>0) fbelow10 = float(nbelow10)/h->GetEntries();
  else fbelow10 = 10;

  return;
}


TProfile* GetNHitsPerLayer(TFile* f, float Vstep=-1, string SubDet="TIB")
{
  f->cd();
  TTree *T  = (TTree*)f->Get("T");
  ULong64_t detid;
  // Warning DetID are modified for TOB : original detid * 10 + 1 or 2 depending on the sensor
  T->SetBranchAddress("DetID",&detid);
  int nhits;
  T->SetBranchAddress("Nhits",&nhits);
  double voltage;
  T->SetBranchAddress("Voltage",&voltage);

  Int_t subdet = 0;
  Int_t layer = 0;

  TProfile* hp = new TProfile("h2", "hits vs layer", 8, 0, 8);
  
  unsigned int nentries = T->GetEntries();
  for(unsigned int j=0; j<nentries; j++)
  {
    //if(j%10000==0) cout <<j<< " entries" <<endl;
    T->GetEntry(j);
    if(SubDet=="TOB") detid/=10; // For TOB, 2 sensors
    subdet = (detid>>25)&0x7;
    if(subdet==3 || subdet==5) layer = ((detid>>14) & 0x7);
    else layer = 8;

    //if(j%10000==0) cout<<detid<<" "<<subdet<<" "<<layer<<" "<<nhits<<endl;
    if(Vstep<0 || fabs(voltage-Vstep)<0.01)
      hp->Fill(layer, nhits);
  }

  return hp;
}

//----------------------------------------------------------------------------------

void GetTreeStat(string run="", string suffix="")
{
  string dirname = "~/work/public/SiStripRadMonitoring/SignalCurves";
  //string dirname= "./SiStripRadMonitoring/SignalBiasScan/Analysis/SignalAnalysis";
  //string dirname = "./Code";
  //string filename = "output_DecoMode";
  char file[100];
  sprintf(file, "%s/TIB_%s%s%s.root", dirname.c_str(), filename.c_str(), run.c_str(), suffix.c_str()); 
  //char file[200] = "~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/TIB_output_DecoMode_160497_preamp_corrected2_32ns.root";
  //char file[200] = "~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/TIB_output_DecoMode_170000_newlandau_S_wchi2_wnhits.root";
  //char file[200] = "~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/TIB_output_DecoMode_190459_timestamp.root";
  //char file[200] = "~/scratch0/CMSSW_4_2_5/src/Radiation_Signal/BiasScanSignal/test/TIB_output_DecoMode_193928_timestamp.root";
  TFile *f = TFile::Open(file);
  if(!f) cout<<"Error : no file '"<<file<<"'"<<endl;

  // Print results
  cout<<"Run "<<run<<" file "<<file<<endl;

  // Events
  int ntot, n300, n325;
  GetNumberOfEvents(f, ntot, n300, n325);
  cout<<"  #events"<<endl;
  cout<<"    total : "<<ntot<<endl;   
  cout<<"    @300V : "<<n300<<endl;
  cout<<"    @350V : "<<n325<<endl;

  // Tracks
  int ntracks, overflow;
  float mean, fbelow10;
  GetNumberOfTracks(f, ntracks, overflow, mean, fbelow10);  
  cout<<"  #tracks"<<endl;
  cout<<"    total : "<<ntracks<<" (overflow excluded)"<<endl;
  cout<<"    overflow : "<<overflow<<" evts"<<endl;
  cout<<"    mean : "<<mean<<endl;
  cout<<"    frac evt <10 : "<<fbelow10*100<<"%"<<endl;
  cout<<"    ntracks at 300V : "<<mean*n300<<endl<<endl;

  // Nhits in TIB
  TProfile *pf = GetNHitsPerLayer(f, 300);
  cout<<"  #hits/module in TIB at 300V"<<endl;
  cout<<"    L1 : "<<pf->GetBinContent(2)<<", rms: "<<pf->GetBinError(2)<<endl;
  cout<<"    L2 : "<<pf->GetBinContent(3)<<", rms: "<<pf->GetBinError(3)<<endl;
  cout<<"    L3 : "<<pf->GetBinContent(4)<<", rms: "<<pf->GetBinError(4)<<endl;
  cout<<"    L4 : "<<pf->GetBinContent(5)<<", rms: "<<pf->GetBinError(5)<<endl<<endl;

  // Nhits in TOB
  char fileTOB[200];
  sprintf(fileTOB, "%s/TOB_%s%s%s.root", dirname.c_str(), filename.c_str(), run.c_str(), suffix.c_str());
  TFile* fTOB = TFile::Open(fileTOB);
  TProfile *pfTOB = GetNHitsPerLayer(fTOB, 300, "TOB");
  cout<<"  #hits/module in TOB at 300V"<<endl;
  cout<<"    L1 : "<<pfTOB->GetBinContent(2)<<", rms: "<<pfTOB->GetBinError(2)<<endl;
  cout<<"    L2 : "<<pfTOB->GetBinContent(3)<<", rms: "<<pfTOB->GetBinError(3)<<endl;
  cout<<"    L3 : "<<pfTOB->GetBinContent(4)<<", rms: "<<pfTOB->GetBinError(4)<<endl;
  cout<<"    L4 : "<<pfTOB->GetBinContent(5)<<", rms: "<<pfTOB->GetBinError(5)<<endl;
  cout<<"    L5 : "<<pfTOB->GetBinContent(6)<<", rms: "<<pfTOB->GetBinError(6)<<endl;
  cout<<"    L6 : "<<pfTOB->GetBinContent(7)<<", rms: "<<pfTOB->GetBinError(7)<<endl;

  // Should save plots

}
