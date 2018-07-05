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
	bool write_relative_vfd_initial = true; //Tk Map values relatively to initial depletion values

	VdeplRef SubdetRef;

	vector<TString> v_subdet;
	v_subdet.push_back("TIB");
	v_subdet.push_back("TOB");
	v_subdet.push_back("TEC");
	v_subdet.push_back("TID");

	ULong64_t modid=-99; Double_t vfd=-99; Double_t ref_val = -99;

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

		ULong64_t modid_tmp = -99;
		for(int ientry=0; ientry<nentries; ientry++)
		{
			modid=-99; vfd=-99; ref_val = -99;
			t->GetEntry(ientry);

			ref_val = SubdetRef.GetVdepl(modid);

			// cout<<"modid = "<<modid<<" / modid_tmp = "<<modid_tmp<<endl;
			// if(v_subdet[isubdet] == "TOB" || v_subdet[isubdet] == "TEC") {modid/=10;} //obsolete

			//For double-sensor modules, only use first sensor
			if(modid/10 == modid_tmp/10 && (v_subdet[isubdet] == "TOB" || v_subdet[isubdet] == "TEC") ) {modid_tmp = modid; continue;}
			modid_tmp = modid;

			if(v_subdet[isubdet] == "TOB" || v_subdet[isubdet] == "TEC") {modid/= 10;}

			// cout<<"Modid = "<<modid<<" ";
			// if(write_relative_vfd_initial) {cout<<" / labRef = "<<ref_val<<" ";}
			// cout<<" / Vfd = "<<vfd<<endl;

			if(write_relative_vfd_initial)
			{
				if(ref_val != 0) {file_out_tmp<<modid<<" "<<ref_val - vfd<<endl;} //else modid not found
			}
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
	for(int isubdet=0; isubdet<v_subdet.size(); isubdet++)
	{
		system( ("cat tracker_vfd_map_"+v_subdet[isubdet]+".txt >> tracker_vfd_allModules.txt").Data() );
	}

	cout<<"--- Copying file 'tracker_vfd_allModules.txt' to : "<<endl;
	cout<<" -> /afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_9_2_10/src/SiStripRadMonitoring/TrackerMapMaker/test"<<endl<<endl;
	system("cp tracker_vfd_allModules.txt /afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_9_2_10/src/SiStripRadMonitoring/TrackerMapMaker/test");

	return;
}


int main()
{
	TString run = "314574"; //run
	TString observable = "ClusterWidth"; //observable
	TString method = "line"; //method

	Write_Vfd_FullScan_Allmodules(run, observable, method);

	return 0;
}

