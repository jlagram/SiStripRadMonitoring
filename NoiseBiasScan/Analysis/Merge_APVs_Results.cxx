#include "TString.h"
#include "Helper_Functions.h"


/**
 * Read file with results for all APVs, determine best value per DetID
 * @param run -- run number
 */
void Merge_APVs_Results(TString run, TString date)
{
	map< double, vector<double> > APV_Vfd_Per_DetID;
	
	TString filename = "./Fit_Results_run"+run+".root";
	if(!Check_File_Existence(filename) ) {cout<<FRED("File "<<filename<<" not found ! Continue")<<endl; return;}

	TFile* f_input = TFile::Open(filename);
	TTree* t_input = (TTree*) f_input->Get("Tree");

	double detid=0, Vfd=0, Vfd_err=0, Fit_proba=0, Chi2_norm=0;

	//To read input branches
	t_input->SetBranchAddress("detid", &detid);
	t_input->SetBranchAddress("Vfd", &Vfd);
	t_input->SetBranchAddress("Vfd_err", &Vfd_err);
	t_input->SetBranchAddress("Fit_proba", &Fit_proba);
	t_input->SetBranchAddress("Chi2_norm", &Chi2_norm);

	double DetID, APV;
	for(int ientry=0; ientry<t_input->GetEntries(); ientry++)
	{
		t_input->GetEntry(ientry);
		DetID=long(detid/10);
		APV=long(detid)%10;
		
		if(APV_Vfd_Per_DetID.find(DetID)==APV_Vfd_Per_DetID.end())
		{
			vector<double> vect;
			APV_Vfd_Per_DetID[DetID] = vect;
		}

		// Exclude very bad fits
		if( Chi2_norm<10 && Vfd>0 ) APV_Vfd_Per_DetID[DetID].push_back(Vfd);
	}
	
	f_input->Close();


	//Output file, containing Vfd results for all detids
	TFile *f_results = new TFile("Fit_Results_Median_run"+run+".root","RECREATE");
	cout<<"File "<<f_results->GetName()<<" created"<<endl;
	TTree* tree = new TTree("tout", "Fit results");

	ULong64_t odetid;
	int olayer; 
	double odepvolt, oerrdepvolt;
	double oplateau=0;
	int ofitstatus=0;
	double ofitchisquare=0;
	double olastpty=0;
	double ochi2=0;

	tree->Branch("DETID",&odetid,"DETID/l");
	tree->Branch("LAYER",&olayer,"LAYER/I");
	tree->Branch("ERRDEPVOLT",&oerrdepvolt,"ERRDEPVOLT/D");
	tree->Branch("DEPVOLT",&odepvolt,"DEPVOLT/D");
	tree->Branch("PLATEAU",&oplateau,"PLATEAU/D"); // not used
	tree->Branch("FITCHI2",&ofitchisquare,"FITCHI2/D"); // not used
	tree->Branch("FITSTATUS",&ofitstatus,"FITSTATUS/I"); // used for # values with correct fit
	tree->Branch("LASTPOINTS",&olastpty,"LASTPOINTS/D"); // not used
	tree->Branch("CHI2",&ochi2,"CHI2/D"); // not used

	// Compute medians and save results
	map< double, vector<double> >::iterator iter_APV_Vfd_Per_DetID = APV_Vfd_Per_DetID.begin();
	cout<<" "<<APV_Vfd_Per_DetID.size()<<" modules written"<<endl;
	for( ; iter_APV_Vfd_Per_DetID!=APV_Vfd_Per_DetID.end(); iter_APV_Vfd_Per_DetID++)
	{
		if( iter_APV_Vfd_Per_DetID->second.size()>0)
		{
			odepvolt = GetMedian( iter_APV_Vfd_Per_DetID->second );
			odetid = iter_APV_Vfd_Per_DetID->first;
			olayer = GetLayer( odetid );
			oerrdepvolt = 0;
			ofitstatus = iter_APV_Vfd_Per_DetID->second.size();
			tree->Fill();
		}
	}
	
	tree->Write();
	f_results->Close();

	
	return;

}

int main()
{
	//Modified_tdr_style(); //Enforce official tdrStyle (slightly modified)
	TString run, date;

	//run = "203243"; date = "20120921";
	//run = "280667"; date = "20160914";
	//run = "303272"; date = "20170919";
	//run = "317974"; date = "20180618";
	//run = "323011"; date = "20180919";
	//run = "328691"; date = "20190321";
    run = "331595"; date = "20190920";
        //
	Merge_APVs_Results(run, date);
	
	return 0;
}

