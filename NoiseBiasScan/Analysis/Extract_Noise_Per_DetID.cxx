/**
Nicolas Tonon, IPHC
12/2017

- Modified by JL Agram 10/2019
*/

#include "Helper_Functions.h"


#define StripsPerApv 128

//--- CHOOSE here
bool use_only_strips_lowest_noise = false; //-- use only few "best" strips, supposed to be free of some noise sources
int nstrips_to_keep = 20; //How many strips with lowest

int counter_control_plots = 0; //Global var, to decide how many control plots we want
double detid_control = -1; //Make sure we will make control plots always for the same detid

using namespace std;




// ------------------------------
// ------------------------------


void Count_Events_EachStep_In1Profile(TString path, TString run)
{
	cout<<endl<<FYEL("#######################")<<endl;
	cout<<FYEL("--- Count events for each Vstep, for 1 given TProfile (2 APVs) ---")<<endl;
	cout<<FYEL("#######################")<<endl<<endl;

	TString output_filename = "nentries_run"+run+".txt";
	ofstream file_out(output_filename.Data() );
	file_out<<"Nof entries for each Vstep / Run "<<run<<endl<<endl;

	double min=pow(10, 7), max=0, mean=0;

	for(int istep=0; istep<step_list.size(); istep++)
	{
		cout<<endl<<"--- Vstep = "<<step_list[istep]<<endl;

		TString file_path = Find_Path_EOS_File(path, run, step_list[istep]); //Get filepath for current Vstep, from EOS
		if(file_path == "") {cout<<FRED("FilePath not found ! Continue")<<endl; continue;}

		//Input file from grid, containing noise and pedestal histograms (for 1 step)
		TString noisefile_name = "root://eoscms//eos/cms" + file_path;
		cout<<FBLU("--- Opening TFile : "<<noisefile_name<<"")<<endl;
		TFile* f_input = TFile::Open(noisefile_name, "READ");
                if(!f_input) {cout<<FRED("File not available")<<endl; continue;}

		//Hard-coded path to 1 random TProfile (path obtained from looking at TBrowser)
		TString dirname = "/DQMData/SiStrip/ControlView/FecCrate1/FecSlot10/FecRing1/CcuAddr119/CcuChan26";
		// f_input->cd(dirname);

		TString hname = "ExpertHisto_Pedestals_FedKey0x00028e08_LldChannel2_Noise";
		// if(!f_input ->GetListOfKeys()->Contains(hname) ) {cout<<hname<<" not found !"<<endl;  return;}
		TH1F* h = 0;
		h = (TH1F*) f_input->Get(dirname + "/" + hname);
                if(!h) {cout<<FRED("histo not found")<<endl; continue;}

		double nentries = h->GetEntries();

		cout<<"----> "<<nentries/256<<" events"<<endl;
		file_out<<step_list[istep]<<"V ----> "<<nentries/256<<" events"<<endl;

		if(nentries < min) min = nentries;
		else if(nentries > max) max = nentries;
		mean+= nentries;

		f_input->Close();
	}

	mean/= step_list.size();

	file_out<<endl<<endl;
	file_out<<"Min = "<<min/256<<endl;
	file_out<<"Max = "<<max/256<<endl;
	file_out<<"Mean = "<<mean/256<<endl;

	return;
}






// ------------------------------
// ###### # #####    #####  #####   ####  ###### # #      ######
// #      #   #      #    # #    # #    # #      # #      #
// #####  #   #      #    # #    # #    # #####  # #      #####
// #      #   #      #####  #####  #    # #      # #      #
// #      #   #      #      #   #  #    # #      # #      #
// #      #   #      #      #    #  ####  #      # ###### ######
// ------------------------------

/**
 * For both APV, produce noise histograms (noises from all strips), perform gaussian fit, return info from fit (mean, error, chi2)
 * @param profile        pointer to profile of noise for 2 APVs
 * @param fed_key_string fedkey given as a TString
 */
void Fit_Profile(TProfile* profile, double fed_key, vector< pair<double, double> > &v_results, TFile* const f_control)
{
	// cout<<"--- Entering Fit_Profile()"<<endl;

	//Store results in pairs of double (2 APVs) -- store 6 vars
	if(v_results.size()<6) { cout<<FRED("Length of results container too small. Increase it.") << endl; return;}

	//loop on 2 APVs per profile
	for(int iAPV=0; iAPV<2; iAPV++)
	{
		if(mapFedkeyToDeviceid.find(fed_key)== mapFedkeyToDeviceid.end() )
		{
			cout<<FRED("Error: unknown fed key: " << fed_key << " ") << endl;
			return;
		}

		//1 histo made with values from all strips, 1 made only with values within 3 sigmas ('trimmed')
		TH1F* h_dist = new TH1F( ("testDistribution_APV" + Convert_Number_To_TString(iAPV + 1)), "test distribution", 200, 0, 10); //CHANGED
		TH1F* h_dist_trimmed = new TH1F( ("testDistribution_APV" + Convert_Number_To_TString(iAPV + 1) + "_trimmed"), "test trimmed distribution", 100, 0, 10);

		TF1* gaussian_fit = new TF1("gaus", "gaus", h_dist->GetXaxis()->GetXmin(), h_dist->GetXaxis()->GetXmax()); //CHANGED
		TF1* gaussian_fit_trimmed = gaussian_fit_trimmed = new TF1("gaus_trimmed", "gaus", h_dist_trimmed->GetXaxis()->GetXmin(), h_dist_trimmed->GetXaxis()->GetXmax()); //fit on trimmed distrib. (values within 3sigmas)


		//Compute first and last strip numbers, for loop
		unsigned int first_strip=1+(iAPV*StripsPerApv); //Either 1 (APV1) or 129 (APV2)
	    unsigned int last_strip=StripsPerApv+first_strip; //Either 129 (APV1) or 257 (APV2)
		// cout<<"First strip = "<<first_strip<<" / last strip = "<<last_strip<<endl;

	    //For each strip, add noise in histogram
		for(int istrip=first_strip; istrip<last_strip; istrip++)
		{
			h_dist->Fill(profile->GetBinContent(istrip) );
		}

		//Fit the histograms with gaussian
		h_dist->Fit(gaussian_fit, "q0"); //Quiet, don't draw fit
		double mean_tmp = gaussian_fit->GetParameter(1), sigma_tmp = gaussian_fit->GetParameter(2);


		//Store noise values of all strips (after removing extrema), then sort them from lowest to highest noise
		vector<double> v_lowest_noise_values;
		if(use_only_strips_lowest_noise)
		{
			for(int istrip=first_strip; istrip<last_strip; istrip++)
			{
				//Remove extreme values +-3sigmas first !
				if(profile->GetBinContent(istrip) < mean_tmp + 3. * sigma_tmp && profile->GetBinContent(istrip) > mean_tmp - 3. * sigma_tmp )
				{
					v_lowest_noise_values.push_back( profile->GetBinContent(istrip) ); //Store noise values for all strips
				}
			}

			std::sort(v_lowest_noise_values.begin(), v_lowest_noise_values.end() ); //Sorting algo
		}

		//Idem for trimmed histo -- remove points above 3 sigmas from gaussian mean
		if(!use_only_strips_lowest_noise) //Use all strips within 3 sigmas
		{
			for(int istrip=first_strip; istrip<last_strip; istrip++)
			{
				//Trimmed histo : remove values > 3 sigmas
				if(profile->GetBinContent(istrip) < mean_tmp + 3. * sigma_tmp && profile->GetBinContent(istrip) > mean_tmp - 3. * sigma_tmp )
				{
					h_dist_trimmed->Fill(profile->GetBinContent(istrip) );
				}
			}
		}
		else //keep only the "best" nstrips for fit
		{
			for(int istrip=0; istrip<nstrips_to_keep; istrip++)
			{
				if(istrip >= v_lowest_noise_values.size() ) {break;} //Could happen that less than 'nstrips' are within 3 sigmas

				h_dist_trimmed->Fill(v_lowest_noise_values[istrip] );
			}
		}

		if(h_dist_trimmed->GetEntries() == 0)
		{
			delete h_dist; delete h_dist_trimmed; h_dist = NULL;  h_dist_trimmed = NULL; continue;
		}

		//Fit the histograms with gaussian
		h_dist_trimmed->Fit(gaussian_fit_trimmed, "q0"); //Quiet, don't draw fit

		double chi2_norm = gaussian_fit_trimmed->GetChisquare() / gaussian_fit_trimmed->GetNDF(); //compute chi-2

		//Need to distinguish the 2 APVs --> Add a number
		double detid = 0;
		if(iAPV==0) {detid = mapFedkeyToDeviceid[fed_key] * 10 + mapFedkeyAPV0[fed_key];}
		else if(iAPV==1) {detid = mapFedkeyToDeviceid[fed_key] * 10 + mapFedkeyAPV1[fed_key];}

		if(detid_control == -1) {detid_control = detid;} //Make all control plots for 1 given (random detid)

		//Store results of fit into vector of pairs (2 APVs)
		if(iAPV == 0)
		{
			if(!use_only_strips_lowest_noise) //Use fit info if all stripŝ are used
			{
				v_results[0].first = detid;
				v_results[1].first = gaussian_fit_trimmed->GetParameter(1);
				v_results[2].first = gaussian_fit_trimmed->GetParError(1);
				v_results[3].first = chi2_norm;
				v_results[4].first = h_dist_trimmed->GetRMS();
				v_results[5].first = gaussian_fit->GetProb();
			}
			else //Else, distrib. not gaussian => Use mean & error
			{
				v_results[0].first = detid;
				v_results[1].first = h_dist_trimmed->GetMean();
				v_results[2].first = h_dist_trimmed->GetMeanError();
				v_results[3].first = chi2_norm;
				v_results[4].first = h_dist_trimmed->GetRMS();
				v_results[5].first = gaussian_fit_trimmed->GetProb();
			}

			// cout<<"iAPV==0"<<endl;
			// cout<<v_results[0].first<<endl;
			// cout<<v_results[1].first<<endl;
			// cout<<v_results[2].first<<endl;
			// cout<<v_results[3].first<<endl;
		}
		else if(iAPV == 1)
		{
			if(!use_only_strips_lowest_noise) //Use fit info if all stripŝ are used
			{
				v_results[0].second = detid;
				v_results[1].second = gaussian_fit_trimmed->GetParameter(1);
				v_results[2].second = gaussian_fit_trimmed->GetParError(1);
				v_results[3].second = chi2_norm;
				v_results[4].second = h_dist_trimmed->GetRMS();
				v_results[5].second = gaussian_fit_trimmed->GetProb();
			}
			else //Else, distrib. not gaussian => Use mean & error
			{
				v_results[0].second = detid;
				v_results[1].second = h_dist_trimmed->GetMean();
				v_results[2].second = h_dist_trimmed->GetMeanError();
				v_results[3].second = chi2_norm;
				v_results[4].second = h_dist_trimmed->GetRMS();
				v_results[5].second = gaussian_fit_trimmed->GetProb();
			}

			// cout<<"iAPV==1"<<endl;
			// cout<<v_results[0].second<<endl;
			// cout<<v_results[1].second<<endl;
			// cout<<v_results[2].second<<endl;
			// cout<<v_results[3].second<<endl;
		}

		// fit_chi2norm->Fill(chi2_norm);


		//Make control plots/histos
		if(counter_control_plots < 30 && detid == detid_control)
		{
			// cout << FBLU("--- Producing control plots") << endl;

			f_control->cd();

			// TCanvas *c = new TCanvas("c", "", 1000, 800);
			// c->SetFillColor(10);
			// c->SetFillStyle(4000);
			// c->SetBorderSize(2);;
			// c->cd();

			h_dist_trimmed->Draw();
			h_dist_trimmed->Fit(gaussian_fit_trimmed,"q"); //Quiet

			// c->SaveAs("control_profile_fit_"+Convert_Number_To_TString(fed_key)+".png");

			// h_dist->Write("noise_"+Convert_Number_To_TString(detid) );
			h_dist_trimmed->Write("noise_trimmed_"+Convert_Number_To_TString(detid) );

			// delete c;
			counter_control_plots++;
		}

		delete h_dist; delete h_dist_trimmed; h_dist = NULL;  h_dist_trimmed = NULL;

		delete gaussian_fit; delete gaussian_fit_trimmed;
		gaussian_fit = NULL; gaussian_fit_trimmed = NULL;
	}


	// delete gaussian_fit; delete gaussian_fit_trimmed;
	// gaussian_fit = NULL; gaussian_fit_trimmed = NULL;

	return;
}








// ------------------------------
 //  ####  #####  ####  #####  ######    # #    #    ##### ##### #####  ###### ######
 // #        #   #    # #    # #         # ##   #      #     #   #    # #      #
 //  ####    #   #    # #    # #####     # # #  #      #     #   #    # #####  #####
 //      #   #   #    # #####  #         # #  # #      #     #   #####  #      #
 // #    #   #   #    # #   #  #         # #   ##      #     #   #   #  #      #
 //  ####    #    ####  #    # ######    # #    #      #     #   #    # ###### ######
// ------------------------------

/**
 * Reads histograms names, calls Fit_Profile() function, fills results of fits into TTree
 * @param dir  directory where to find the histograms (this function is called only if directory contains only histos, not subdirs)
 * @param tree TTree to fill with results
 */
void Extract_Infos_From_Profile_Into_TTree(TDirectory* dir, TTree* tree, TFile* const f_control)
{
	TProfile* profile = NULL;
	TObject* obj_tmp = NULL;
	TList* list_of_keys = dir->GetListOfKeys();
	TIter* iter_list = new TIter(list_of_keys);

	//Tmp store results from profile fits
	vector< pair<double, double> > v_infos_from_profile(6);

	//Loop on all histograms from this directory (2 APVs, noise/pedestal/...)
	while(obj_tmp = iter_list->Next() )
	{
		TString ts = "", fed_key = "", laser_chan = "", plot_type = ""; 
		
		
		dir->GetObject(obj_tmp->GetTitle(), profile); //Get profile object

		if(profile == 0) {continue;}

		//Histo names are of the form : "ExpertHisto_Pedestals_FedKey0x%x_LldChannel%d_%s", &fedKey, &laserChannel, plotType
		int index_tmp = 0;

		ts = profile->GetTitle();
		// cout<<ts<<endl; //cout the histo name

		//-----------------

		//--- Extract fedkey from object name, using TString functions
		index_tmp = ts.Index("FedKey0x"); index_tmp+= ((string) "FedKey0x").size(); //find fedkey position
		ts = ts.Remove(0, index_tmp); //Remove all characters before fedkey
		index_tmp = ts.Index("_LldChannel"); //Find end of fedkey string
		fed_key = ts(0, index_tmp); //extract fedkey
		int fed_key_decimal = Convert_Hexa_To_UnsignedLong(fed_key); //Fed keys stored in hexadecimal format, needs conversion


		//-- Just look few TIB APVs for now
		// cout<<Convert_Number_To_TString(mapFedkeyToDeviceid[fed_key_decimal])<<endl;
		// if( !Convert_Number_To_TString(mapFedkeyToDeviceid[fed_key_decimal]).Contains("36912") ) {delete iter_list; return;}

		//--- Extract laser channel from object name, using TString functions
		index_tmp+= ((string) "_LldChannel").size();
		ts = ts.Remove(0, index_tmp); //Remove all characters before laser_chan
		index_tmp = ts.Index("_"); //Find end of laser_chan string
		laser_chan = ts(0, index_tmp); //Extract laser_chan

		//--- Extract plot type (noise/pedestal) from object name, using TString functions
		ts = ts.Remove(0, index_tmp+1); //Remove all remaining characters but plot_type string
		plot_type = ts; //Extract plot_type

		// cout<<endl<<"fed_key = "<<fed_key<<"--> Decimal : "<<fed_key_decimal<<endl;
		// cout<<"laser_chan = "<<laser_chan<<endl;
		// cout<<"plot_type = "<<plot_type<<endl;


		//FIXME
		//--- FILTER ON PARTITION AND LAYER (decoding info from detid)
		if( mapFedkeyToDeviceid.find(fed_key_decimal)== mapFedkeyToDeviceid.end() ) continue;
		int partition = int( (mapFedkeyToDeviceid[fed_key_decimal]>>25)&0x7);
		int layer = int( (mapFedkeyToDeviceid[fed_key_decimal]>>14)&0x7);

		//if(partition != 3 || (layer != 1 && layer != 4) ) {continue;} //Only TIB L1 & L4 for now
		if(partition != 3) {continue;} //Only TIB

		//-----------------

		if((plot_type != "Noise" && plot_type != "NoiseProfile") || fed_key == "" || laser_chan == "")
		{
			continue;
		}

		//Get infos from fit to this profile
		Fit_Profile(profile, fed_key_decimal, v_infos_from_profile, f_control);

		//-----------------
		//--- Store infos from fit into TTree

		//APV1
		tree->SetBranchAddress("detid", &v_infos_from_profile[0].first);
		tree->SetBranchAddress("noise_mean", &v_infos_from_profile[1].first);
		tree->SetBranchAddress("noise_err", &v_infos_from_profile[2].first);
		tree->SetBranchAddress("noise_chi2", &v_infos_from_profile[3].first);
		tree->SetBranchAddress("noise_rms", &v_infos_from_profile[4].first);
		tree->SetBranchAddress("noise_fitProba", &v_infos_from_profile[5].first);
		// tree->SetBranchAddress("Vstep_nentries", &v_infos_from_profile[6].first);
		// tree->SetBranchAddress("pedestal_mean_err", &pedestal_mean_err); //alternative error

		tree->Fill();

		//APV2
		tree->SetBranchAddress("detid", &v_infos_from_profile[0].second);
		tree->SetBranchAddress("noise_mean", &v_infos_from_profile[1].second);
		tree->SetBranchAddress("noise_err", &v_infos_from_profile[2].second);
		tree->SetBranchAddress("noise_chi2", &v_infos_from_profile[3].second);
		tree->SetBranchAddress("noise_rms", &v_infos_from_profile[4].second);
		tree->SetBranchAddress("noise_fitProba", &v_infos_from_profile[5].second);
		// tree->SetBranchAddress("Vstep_nentries", &v_infos_from_profile[6].second);
		// tree->SetBranchAddress("pedestal_mean_err", &pedestal_mean_err);

		tree->Fill();
	}

	delete iter_list;
	profile = NULL; obj_tmp = NULL;

	return;
}







// ------------------------------
//  #       ####   ####  #####     #####  # #####
//  #      #    # #    # #    #    #    # # #    #
//  #      #    # #    # #    #    #    # # #    #
//  #      #    # #    # #####     #    # # #####  ###
//  #      #    # #    # #         #    # # #   #  ###
//  ######  ####   ####  #         #####  # #    # ###
// ------------------------------

/**
 * Loop on all directories found in input file. If subdirs are found, function stops and we loop on the subdirs (by calling the function itself on subdirs). Else, other functions are called to find the noise profiles, fit them, and store results.
 * @param current_dir directory in which to look for (to verify if it contains subdirs or not) -- passed by pointer
 * @param tree        TTree in which the results of fit are stored, passed by pointer to other functions
 */
void Loop_On_All_Directories(TDirectory* current_dir, TTree* tree, TFile* const f_control, int& counter)
{
	TList* list_of_keys = current_dir->GetListOfKeys();
	TIter* iter_list = new TIter(list_of_keys);
	TObject* obj_tmp = NULL;
	TDirectory* next_directory = NULL;
	bool contains_subdirectory = false;

	counter++; if(counter%1000 == 0) {cout<<"--- "<<counter<<" directories checked"<<endl;}

	// if(counter>20) {delete iter_list; return;}


/// CHECK: skip first element in the list ?

	//Loop on all directories found in input file
	while (obj_tmp = iter_list->Next() )
	{
		// cout<<obj_tmp->GetTitle()<<endl;
		current_dir->GetObject(obj_tmp->GetTitle(), next_directory);

		//If a subdirectory is found : we re-start the function from this new directory, and stops with the current one
		if(next_directory)
		{
			Loop_On_All_Directories(next_directory, tree, f_control, counter);

			contains_subdirectory = true;
		}
	}

	if(contains_subdirectory) {delete iter_list; return;}

	//If dir. does NOT contain subdirs, we move to next steps : look for profiles, fit them, store infos into TTree
	// cout<<FYEL("SUBDIR. FOUND !")<<endl;
	Extract_Infos_From_Profile_Into_TTree(current_dir, tree, f_control);


	delete iter_list;

	return;
}






// ------------------------------
 // #    # #####  # ##### ######     ####  #    # ##### #####  #    # #####
 // #    # #    # #   #   #         #    # #    #   #   #    # #    #   #
 // #    # #    # #   #   #####     #    # #    #   #   #    # #    #   #
 // # ## # #####  #   #   #         #    # #    #   #   #####  #    #   #
 // ##  ## #   #  #   #   #         #    # #    #   #   #      #    #   #
 // #    # #    # #   #   ######     ####   ####    #   #       ####    #
// ------------------------------

/**
 * Open TFiles, calls all other functions. Once TTree has been filled, writes it into output file
 * @param run run_number for naming
 */
void Store_Noise_Infos_All_DetIDs(TString path, TString run, TString step, TFile* f_control, bool copy_gridfile_locally = true)
{
	int counter = 0; //Counts nof directories already checked

	bool use_local_file = false; //If true, use SiStripCommissioningSource_1.root file present in local dir --- For tests, if want to re-use same root files for several executions of program

	cout<<endl<<FYEL("#######################")<<endl;
	cout<<FYEL("--- Extract noise infos and store it in output file ---")<<endl;
	cout<<"Step = "<<step<<" V"<<endl;
	cout<<FYEL("#######################")<<endl<<endl;

	TString file_path = Find_Path_EOS_File(path, run, step); //Get filepath for current Vstep, from EOS
	if(file_path == "") {cout<<FRED("FilePath not found ! Abort")<<endl; return;}

	//Input file from grid, containing noise and pedestal histograms (for 1 step)
	TString noisefile_name = "";

	TString cmd;

	//First, make sure we will not use a previous/wrong file --> delete
	cmd = "rm ./SiStripCommissioningSource_1.root";
	if(Check_File_Existence("./SiStripCommissioningSource_1.root") && !use_local_file) {system(cmd.Data() );}


	if(copy_gridfile_locally && !use_local_file) //Copy locally in tmp file
	{
		cout<<FBLU("--- Copying TFile locally : "<<file_path<<" (faster processing)")<<endl;

		noisefile_name = "./SiStripCommissioningSource_1.root";

		TString cmd = "xrdcp -f root://eoscms//eos/cms" + file_path + " " + noisefile_name;
		system(cmd.Data() );

		if( !Check_File_Existence(noisefile_name) ) {cout<<BOLD(FRED(<<"FILE "<<noisefile_name<<" NOT FOUND !"))<<endl; return;}
	}
	else if(!use_local_file) //Don't copy locally (distant access)
	{
		noisefile_name = "root://eoscms//eos/cms" + file_path;
	}
	else {noisefile_name = "./SiStripCommissioningSource_1.root";}

	cout<<FBLU("--- Opening TFile : "<<noisefile_name<<"")<<endl;
	TFile* f_input = TFile::Open(noisefile_name, "READ");

	mkdir( "Noise_All_DetIDs_files/", 0777); //Create directory if inexistant
	mkdir( ("Noise_All_DetIDs_files/run_"+run).Data(), 0777); //Create directory if inexistant

	//Output file, in which to write the fit infos (gaussian mean, error, chi2, ...) for each APV
	TString outputfile_name = "./Noise_All_DetIDs_files/run_"+run+"/Noise_All_DetIDs_run"+run+"_"+step+"V.root";
	TFile* f_output = new TFile(outputfile_name, "RECREATE"); f_output->cd();

	//TTree* in which output infos are stored
	TTree* tree = new TTree("Tree", "Noise infos");
	TBranch *b_detid = tree->Branch("detid", 0, "detid/D");
	TBranch *b_noise_mean = tree->Branch("noise_mean", 0, "noise_mean/D");
	TBranch *b_noise_err = tree->Branch("noise_err", 0, "noise_err/D");
	// TBranch *b_pedestal_mean_err = tree->Branch("pedestal_mean_err", 0, "pedestal_mean_err/D"); //alternative error
	TBranch *b_noise_chi2 = tree->Branch("noise_chi2", 0, "noise_chi2/D");
	TBranch *b_noise_rms = tree->Branch("noise_rms", 0, "noise_rms/D");
	TBranch *b_noise_fitProba = tree->Branch("noise_fitProba", 0, "noise_fitProba/D");
	// TBranch *b_noise_nentries = tree->Branch("Vstep_nentries", 0, "Vstep_nentries/D");


	f_input->cd();
	f_input->cd("/DQMData/SiStrip/ControlView");
	TDirectory* baseDirectory = gDirectory;
	if(!baseDirectory)
	{
		cerr<<"Did not find the directory /DQMData/SiStrip/ControlView in input file ! Abort"<<endl;
		f_output->Close();
		f_input->Close();
		delete tree; tree = NULL; delete f_output; return;
	}



	//Calls all necessary function to read input and store infos
	cout<<endl<<FBLU("--- Starting loop over all directories --")<<endl;
	Loop_On_All_Directories(baseDirectory, tree, f_control, counter);

	f_output->cd();
	tree->Write();

	cout<<"--- Successful Write() of TTree with "<<tree->GetEntries()<<" entries !"<<endl;

	delete tree; tree = NULL;

	f_output->Close();
	f_input->Close();
	
	delete f_output;
	return;
}








// ------------------------------
//    #                     #     #
//   # #   #      #         #     #     ####  ##### ###### #####   ####
//  #   #  #      #         #     #    #        #   #      #    # #
// #     # #      #         #     #     ####    #   #####  #    #  ####
// ####### #      #          #   #          #   #   #      #####       #
// #     # #      #           # #      #    #   #   #      #      #    #
// #     # ###### ######       #        ####    #   ###### #       ####
// ------------------------------

/**
 * Run program for all voltage steps (automation)
 */
void Produce_Files_For_All_Steps(TString path, TString run, TFile* f_control)
{
	bool copy_gridfile_locally = true; //Copy rootfiles locally to run faster

	for(int istep=0; istep<step_list.size(); istep++) //step_list from Helper_Functions.h
	{
		if(!step_isUsed[istep]) {continue;}

		Store_Noise_Infos_All_DetIDs(path, run, step_list[istep], f_control, copy_gridfile_locally);
	}

	return;
}








// ------------------------------
// ##     ##    ###    #### ##    ##
// ###   ###   ## ##    ##  ###   ##
// #### ####  ##   ##   ##  ####  ##
// ## ### ## ##     ##  ##  ## ## ##
// ##     ## #########  ##  ##  ####
// ##     ## ##     ##  ##  ##   ###
// ##     ## ##     ## #### ##    ##
// ------------------------------


int main()
{
	loadMapFedkeyToDeviceid("fedkeys.txt"); //Load fedkey <-> detid map

	//Output control file, in which we store some control histos
	//-- need to be global so not re-written at each function call
	TFile* f_control = new TFile("Noise_Control_Plots.root", "RECREATE"); //Store some control histos, ... (via Fit_Profile() )
//--------------------------
	//Path of files : root://eoscms.cern.ch//eos/cms...

         //TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/Jul2011/TIB";
         //TString run = "153517";

	 TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/Sep2012/TIB";
	 TString run = "203243";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2016/VRRandom0to3";
	//TString run = "280667";

	// TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2017/VRRandom0and1";
	// TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2017/VRRandom0_linBaseline";
	// TString run = "303272";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2017Dec/TIB";
	//TString run = "307585";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/Jun2018/VRRandom0_part1";
	//TString run = "317974";

    //TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/Jun2018/VRRandom0_part2";
    //TString run = "318024";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/Sep2018/VRRandom0to7";
	//TString run = "323011";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2019Mar";
	//TString run = "328691";

	//TString path = "/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/NoiseBiasScan/2019Sep/VRRandom0";
	//TString run = "331595";


//-----------------------------
	Fill_Step_List_Vector(run); //Load Voltage steps

        //Count_Events_EachStep_In1Profile(path, run);
        
        Produce_Files_For_All_Steps(path, run, f_control);

	f_control->Close(); delete f_control; f_control = NULL;
	return 0;
}

