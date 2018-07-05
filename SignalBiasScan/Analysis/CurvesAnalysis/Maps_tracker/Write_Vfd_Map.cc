#include "Fit.h"
#include "../CommonTools/VdeplRef.h"
// #include <vector>

using namespace std;

/**
 * Extract Vfd values for ALL modules (Full Scan), for given Run
 * Writes the values as "detid Vfd" in separate text files for each subdetector, and merges them in one single .Txt File
 * Output file to be given as input to ./TrackerMapMaker executable to produce Tk map
 * Can also use automatic script "Produce_Tracker_Map.sh"
 */
void Write_Vfd_FullScan_Allmodules()
{
	bool write_relative_vfd_initial = false;

	TString run = "314574"; //run
	TString observable = "ClusterWidth"; //observable
	TString method = "line"; //method

	VdeplRef SubdetRef;

	//Output text files, modid vs Vfd
	ofstream file_out_TIB("tracker_vfd_map_TIB.txt");
	ofstream file_out_TOB("tracker_vfd_map_TOB.txt");
	ofstream file_out_TEC("tracker_vfd_map_TEC.txt");
	ofstream file_out_TID("tracker_vfd_map_TID.txt");

	ULong64_t modid=-99; Double_t vfd=-99;

	TString dirname = "/afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_8_0_20_patch1/src/SiStripRadMonitoring/SignalBiasScan/Analysis/CurvesAnalysis/DECO_files/all_modules";

	TString filename = dirname + "/DECO_allModules_ClusterWidth_TIB_line_295324.root";
	if(!Check_File_Existence(filename) ) {cout<<BOLD(FRED("File "<<filename<<" not found ! Abort"))<<endl; return;}
	TFile* f = TFile::Open(filename);
	TTree* t = (TTree*) f->FindObjectAny("tout");
	int nentries = t->GetEntries();
	t->SetBranchAddress("DETID",&modid);
	t->SetBranchAddress("DEPVOLT",&vfd);

	SubdetRef.loadFile("TIB");

	for(int ientry=0; ientry<nentries; ientry++)
	{
		t->GetEntry(ientry);

		if(write_relative_vfd_initial) file_out_TIB<<modid<<" "<<SubdetRef.GetVdepl(modid) - vfd<<endl;
		else file_out_TIB<<modid<<" "<<vfd<<endl;
	}

	delete t;
	f->Close();


	filename = dirname + "/DECO_allModules_"+observable+"_"+subdet+"_"+method+"_"+run+".root";
	// filename = dirname + "/DECO_allModules_ClusterWidth_TOB_line_295324.root";
	if(!Check_File_Existence(filename) ) {cout<<BOLD(FRED("File "<<filename<<" not found ! Abort"))<<endl; return;}
	f = TFile::Open(filename);
	t = (TTree*) f->FindObjectAny("tout");
	nentries = t->GetEntries();
	t->SetBranchAddress("DETID",&modid);
	t->SetBranchAddress("DEPVOLT",&vfd);

	SubdetRef.loadFile("TOB");

	for(int ientry=0; ientry<nentries; ientry++)
	{
		t->GetEntry(ientry);

		if(write_relative_vfd_initial) file_out_TOB<<modid/10<<" "<<SubdetRef.GetVdepl(modid/10) - vfd<<endl;
		else file_out_TOB<<modid/10<<" "<<vfd<<endl;
	}


	delete t;
	f->Close();


	filename = dirname + "/DECO_allModules_ClusterWidth_TEC_line_295324.root";
	if(!Check_File_Existence(filename) ) {cout<<BOLD(FRED("File "<<filename<<" not found ! Abort"))<<endl; return;}
	f = TFile::Open(filename);
	t = (TTree*) f->FindObjectAny("tout");
	nentries = t->GetEntries();
	t->SetBranchAddress("DETID",&modid);
	t->SetBranchAddress("DEPVOLT",&vfd);

	SubdetRef.loadFile("TEC");

	for(int ientry=0; ientry<nentries; ientry++)
	{
		t->GetEntry(ientry);

		if(write_relative_vfd_initial) file_out_TEC<<modid/10<<" "<<SubdetRef.GetVdepl(modid/10) - vfd<<endl;
		else file_out_TEC<<modid/10<<" "<<vfd<<endl;
	}



	delete t;
	f->Close();


	filename = dirname + "/DECO_allModules_ClusterWidth_TID_line_295324.root";
	if(!Check_File_Existence(filename) ) {cout<<BOLD(FRED("File "<<filename<<" not found ! Abort"))<<endl; return;}
	f = TFile::Open(filename);
	t = (TTree*) f->FindObjectAny("tout");
	nentries = t->GetEntries();
	t->SetBranchAddress("DETID",&modid);
	t->SetBranchAddress("DEPVOLT",&vfd);

	SubdetRef.loadFile("TID");

	for(int ientry=0; ientry<nentries; ientry++)
	{
		t->GetEntry(ientry);

		if(write_relative_vfd_initial) file_out_TID<<modid<<" "<<SubdetRef.GetVdepl(modid) - vfd<<endl;
		else file_out_TID<<modid/10<<" "<<vfd<<endl;
	}



	delete t;
	f->Close();

	system("cat tracker_vfd_map_TIB.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TOB.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TEC.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TID.txt >> tracker_vfd_allModules.txt");


	return;
}


int main()
{
	Write_Vfd_FullScan_Allmodules();

	return 0;
}

