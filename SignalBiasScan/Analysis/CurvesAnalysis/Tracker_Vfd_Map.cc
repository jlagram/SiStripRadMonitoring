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
void Write_Vfd_FullScan_Allmodules(TString run, TString observable, TString method)
{
	bool write_relative_vfd_initial = false; //Tk Map values relatively to initial depletion values

	VdeplRef SubdetRef;

	vector<TString> v_subdet;
	v_subdet.push_back("TIB");
	v_subdet.push_back("TOB");
	v_subdet.push_back("TEC");
	v_subdet.push_back("TID");

	ULong64_t modid=-99; Double_t vfd=-99;

	TString dirname = "./DECO_files/all_modules";

	for(int isubdet=0; isubdet<v_subdet.size(); isubdet++)
	{
		cout<<"Subdet = "<<v_subdet[isubdet]<<endl;


		ofstream file_out_tmp("tracker_vfd_map_tmp.txt");

		TString filename = dirname + "/DECO_allModules_"+observable+"_"+v_subdet[isubdet]+"_"+method+"_"+run+".root";
		if(!Check_File_Existence(filename) ) {cout<<BOLD(FRED("File "<<filename<<" not found ! Abort"))<<endl; return;}

		TFile* f = TFile::Open(filename);
		TTree* t = (TTree*) f->FindObjectAny("tout");
		int nentries = t->GetEntries();

		t->SetBranchAddress("DETID",&modid);
		t->SetBranchAddress("DEPVOLT",&vfd);

		SubdetRef.loadFile(v_subdet[isubdet].Data());

		for(int ientry=0; ientry<nentries; ientry++)
		{
			modid=-99; vfd=-99;
			t->GetEntry(ientry);

			if(v_subdet[isubdet] == "TOB" || v_subdet[isubdet] == "TEC") {modid/=10;} //Remove sensor identifier

			if(write_relative_vfd_initial) file_out_tmp<<modid<<" "<<SubdetRef.GetVdepl(modid) - vfd<<endl;
			else file_out_tmp<<modid<<" "<<vfd<<endl;
		}

		f->Close();
		file_out_tmp.close();

		//Output text files, modid vs Vfd
		if(v_subdet[isubdet] == "TIB") {system("mv tracker_vfd_map_tmp.txt tracker_vfd_map_TIB.txt");}
		else if(v_subdet[isubdet] == "TOB") {system("mv tracker_vfd_map_tmp.txt tracker_vfd_map_TOB.txt");}
		else if(v_subdet[isubdet] == "TEC") {system("mv tracker_vfd_map_tmp.txt tracker_vfd_map_TEC.txt");}
		else if(v_subdet[isubdet] == "TID") {system("mv tracker_vfd_map_tmp.txt tracker_vfd_map_TID.txt");}

		cout<<"--- Done"<<endl;
	}


	//Concatenate all partitions in 1 single file
	system("rm tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TIB.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TOB.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TEC.txt >> tracker_vfd_allModules.txt");
	system("cat tracker_vfd_map_TID.txt >> tracker_vfd_allModules.txt");

	return;
}


int main()
{
	TString run = "295376"; //run
	TString observable = "ClusterWidth"; //observable
	TString method = "line"; //method

	Write_Vfd_FullScan_Allmodules(run, observable, method);

	return 0;
}

