#ifndef ModFluence_h
#define ModFluence_h

#include <iostream>
#include "TFile.h"
#include "TTree.h"
 
// Fluence in module
//-------------------

class ModFluence{
  public:
  
    ModFluence(){fflu=0; trflu=0;};
    void loadFile(string filename="modulesFluence.root");
	double GetFluence(ULong64_t modid);
	double GetR(ULong64_t modid);
	double GetZ(ULong64_t modid);
	void GetModFluenceInfos(ULong64_t modid, double & fluence, double & R, double & Z);
	
  private:
  
    TFile *fflu;
	TTree* trflu;
	
	int det;
	int subdetid;
	double R;
	double Z;
	double fluence;
	
};

void ModFluence::loadFile(string filename)
{
  fflu = TFile::Open(filename.c_str());
  if(!fflu) { cout<<"No file : "<<filename<<endl; return;}
  trflu = (TTree*) fflu->FindObjectAny("tflu");

  trflu->SetBranchAddress("detId",&det);
  trflu->SetBranchAddress("subdetId",&subdetid);
  trflu->SetBranchAddress("R",&R);
  trflu->SetBranchAddress("Z",&Z);
  trflu->SetBranchAddress("fluence",&fluence);
}

double ModFluence::GetFluence(ULong64_t modid)
{
    UInt_t nentries = trflu->GetEntries();
    UInt_t ie = 0;
	det = 0;
	fluence = -1;
	while(ie <nentries && det!=(int)modid){
      trflu->GetEntry(ie);
	  ie++;
	}
    return fluence;
}

double ModFluence::GetR(ULong64_t modid)
{
    UInt_t nentries = trflu->GetEntries();
    UInt_t ie = 0;
	det = 0;
	R = -1;
	while(ie <nentries && det!=(int)modid){
      trflu->GetEntry(ie);
	  ie++;
	}
    return R;
}

double ModFluence::GetZ(ULong64_t modid)
{
    UInt_t nentries = trflu->GetEntries();
    UInt_t ie = 0;
	det = 0;
	Z = -1;
	while(ie <nentries && det!=(int)modid){
      trflu->GetEntry(ie);
	  ie++;
	}
    return Z;
}

void ModFluence::GetModFluenceInfos(ULong64_t modid, double & tmpfluence, double & tmpR, double & tmpZ)
{
    UInt_t nentries = trflu->GetEntries();
    UInt_t ie = 0;
	det = 0;
	fluence = -1;
	R = -1;
	Z = -1;
 	while(ie <nentries && det!=(int)modid){
      trflu->GetEntry(ie);
 	  ie++;
	}
	tmpfluence = fluence;
	tmpR = R;
	tmpZ = Z;
    return ;
}
#endif
