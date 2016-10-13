void Draw(TCanvas* c1, string run="_251252", int layer=31)
{

  if(layer<30 && layer>70) {cout<<"Wrong layer number: "<<layer<<endl; return;}
  string subdet="";
  if(layer/10==3) subdet="TIB";
  if(layer/10==5) subdet="TOB";

  TFile* f = new TFile(Form("%s_output_DecoMode_prof%s.root", subdet.c_str(), run.c_str()));
  if(!f) {cout<<"File not found"<<endl; return;}
  TProfile* p1 = (TProfile*) f->Get(Form("DetID_%i0_prof_0", layer)); 
  TProfile* p2 = (TProfile*) f->Get(Form("DetID_%i1_prof_0", layer));

  c1->cd();
  p1->GetXaxis()->SetRangeUser(-2,2);
  TF1* fpol = p1->GetFunction("pol");
  if(!fpol) fpol->Delete();
  p1->Draw();
  p2->SetLineColor(8);
  fpol = p2->GetFunction("pol");
  if(!fpol) fpol->Delete();
  p2->Draw("same");

  c1->Modified();
  c1->Update();

}

void Draw(TCanvas* c1, string run1="_190459_300V", string run2="_251252", int layer=310, bool print=false)
{

  if(layer<300 && layer>700) {cout<<"Wrong layer number: "<<layer<<endl; return;}
  string subdet="";
  if(layer/100==3) subdet="TIB";
  if(layer/100==5) subdet="TOB";

  TFile* f1 = new TFile(Form("%s_output_DecoMode_prof%s.root", subdet.c_str(), run1.c_str()));
  if(!f1) {cout<<"File not found"<<endl; return;}
  TProfile* p1 = (TProfile*) f1->Get(Form("DetID_%i_prof_0", layer)); 
  TFile* f2 = new TFile(Form("%s_output_DecoMode_prof%s.root", subdet.c_str(), run2.c_str()));
  if(!f2) {cout<<"File not found"<<endl; return;}
  TProfile* p2 = (TProfile*) f2->Get(Form("DetID_%i_prof_0", layer));

  c1->cd();
  p1->GetXaxis()->SetRangeUser(-2,2);
  p1->GetFunction("pol")->SetBit(TF1::kNotDraw);
  p1->SetLineWidth(2);
  p1->Draw();
  p2->SetLineColor(8);
  p2->GetFunction("pol")->SetBit(TF1::kNotDraw);
  p2->SetLineWidth(2);
  p2->Draw("same");

  c1->Modified();
  c1->Update();
  if(print) c1->Print(Form("ClusterSizevsTanTheta%s%s_layer_%i.eps",run1.c_str(), run2.c_str(), layer));

}

void DrawSym(TCanvas* c1, string run="_251252" ,int layer=310, float xaxis=0)
{

  if(layer<300 && layer>700) {cout<<"Wrong layer number: "<<layer<<endl; return;}
  string subdet="";
  if(layer/100==3) subdet="TIB";
  if(layer/100==5) subdet="TOB";
 
  TFile* f = new TFile(Form("%s_output_DecoMode_prof%s.root", subdet.c_str(), run.c_str()));
  TProfile* p1 = (TProfile*) f->Get(Form("DetID_%i_prof_0", layer));
  c1->cd();

  TGraphErrors* gsym = new TGraphErrors();
  gsym->Clear();
  int ipt=0;
  for(int ibin=1; ibin<p1->GetNbinsX()+1; ibin++)
  {
    gsym->SetPoint(ipt, -1*(p1->GetBinCenter(ibin)-xaxis), p1->GetBinContent(ibin));
    //cout<<ipt<<" "<<p1->GetBinCenter(ibin)<<" "<<p1->GetBinContent(ibin)<<endl;
    gsym->SetPointError(ipt, p1->GetBinWidth(ibin)/2., p1->GetBinError(ibin));
    ipt++;
  }

  p1->GetXaxis()->SetRangeUser(-1.5,1.5);
  p1->GetFunction("pol")->SetBit(TF1::kNotDraw);
  p1->Draw();
  gsym->SetLineColor(2);
  gsym->Draw("PL");

  c1->Modified();
  c1->Update();

}

void AnalyzeLA()
{

  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas();
  //Draw(c1, 251252, 31);

  const int nlay=18;
  int layers[nlay] = {310, 311, 320, 321, 330, 331, 340, 341, 510, 511, 520, 521, 530, 531, 540, 541,
  550, 551};

  //DrawSym(c1, 310, -0.12);
  //DrawSym(c1, 311, 0.14);
  //DrawSym(c1, 320, -0.13);
  //DrawSym(c1, 321, 0.13);
  //DrawSym(c1, 330, -0.12);
  //DrawSym(c1, 331, 0.13);
  //DrawSym(c1, 340, -0.13);
  //DrawSym(c1, 341, 0.14);  
  
  //DrawSym(c1, 510, -0.13);
  //DrawSym(c1, 511, 0.08);
  //DrawSym(c1, 520, -0.10);
  //DrawSym(c1, 521, 0.12);
  //DrawSym(c1, 530, -0.12);
  //DrawSym(c1, 531, 0.10);
  //DrawSym(c1, 540, -0.10);
  //DrawSym(c1, 541, 0.10);
  //DrawSym(c1, 550, -0.15);
  //DrawSym(c1, 551, 0.15);
  //DrawSym(c1, 560, -0.15);
  //DrawSym(c1, 561, 0.15);

  // 190459
  //DrawSym(c1, "_190459_300V", 310, -0.13);
  //DrawSym(c1, "_190459_300V", 311, 0.15);
  //DrawSym(c1, "_190459_300V", 320, -0.13);
  //DrawSym(c1, "_190459_300V", 321, 0.12);


  //Draw(c1, "_190459_300V", "_251252", 310);
  //Draw(c1, "_190459_300V", "_251252", 311);

  for(int il=0; il<nlay; il++){
    Draw(c1, "_190459_300V", "_251252", layers[il], true);
    getchar();
  }
  //Draw(c1, "_190459_300V", "_251252", 510);
  //Draw(c1, "_190459_300V", "_251252", 511);
  //Draw(c1, "_190459_300V", "_251252", 550);
  //Draw(c1, "_190459_300V", "_251252", 551);

 
  getchar();
}
