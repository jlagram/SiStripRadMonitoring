/**
Nicolas Tonon, IPHC
12/2017
*/

#include <map>
#include "TRandom3.h"
#include "TGraphErrors.h"

#include "Helper_Functions.h"

#define model_choice 1 //Choose here fitting model to use
#define transformation_choice 0 //0=normal, 1=log, 2=1/x2

using namespace std;



//-------------------------------------------------------

//  ######  ######## ##    ## ##       ########    ##     ## ####  ######  ########  #######
// ##    ##    ##     ##  ##  ##       ##          ##     ##  ##  ##    ##    ##    ##     ##
// ##          ##      ####   ##       ##          ##     ##  ##  ##          ##    ##     ##
//  ######     ##       ##    ##       ######      #########  ##   ######     ##    ##     ##
//       ##    ##       ##    ##       ##          ##     ##  ##        ##    ##    ##     ##
// ##    ##    ##       ##    ##       ##          ##     ##  ##  ##    ##    ##    ##     ##
//  ######     ##       ##    ######## ########    ##     ## ####  ######     ##     #######
//-------------------------------------------------------

void Style_Control_Histo(TH1F* h)
{
	h->SetMarkerStyle(8);
	h->SetMarkerSize(1.5);
	h->SetMarkerColor(1);

	h->GetXaxis()->SetTitle("Bias voltage [V]");
	h->GetXaxis()->SetTitleSize(.05);

	h->GetYaxis()->SetTitle("Noise [a.u.]");
	h->GetYaxis()->SetTitleSize(.05);
	h->GetYaxis()->SetTitleOffset(0.95);

	return;
}

void Style_Control_TGraph(TGraphErrors* g)
{
	g->SetMarkerStyle(8);
	g->SetMarkerSize(1.5);
	g->SetMarkerColor(1);

	g->SetLineWidth(2);

        g->GetXaxis()->SetRangeUser(0., 350 );
	//g->GetXaxis()->SetRangeUser(0., Convert_TString_To_Number(step_list[step_list.size() -1]) );

	g->GetXaxis()->SetTitle("Bias voltage [V]");
	g->GetXaxis()->SetTitleSize(.05);

	g->GetYaxis()->SetTitle("Noise [a.u.]");
	g->GetYaxis()->SetTitleSize(.05);
	g->GetYaxis()->SetTitleOffset(0.95);

	return;
}


void Draw_ExtraText(TCanvas* c)
{
	c->cd();

	TString cmsText     = "CMS";
	TLatex latex;
	latex.SetNDC();
	latex.SetTextAngle(0);
	latex.SetTextColor(kBlack);
	latex.SetTextFont(61);
	latex.SetTextAlign(11);
	latex.SetTextSize(0.05);
	latex.DrawLatex(c->GetLeftMargin(),0.93,cmsText);


	bool writeExtraText = false;
	TString extraText   = "Preliminary";
	latex.SetTextFont(52);
	latex.SetTextSize(0.04);
	latex.DrawLatex(c->GetLeftMargin() + 0.1, 0.932, extraText);

	return;
}







//-------------------------------------------------------
//  ######  ########  ########    ###    ######## ########    ##     ##    ###    ########
// ##    ## ##     ## ##         ## ##      ##    ##          ###   ###   ## ##   ##     ##
// ##       ##     ## ##        ##   ##     ##    ##          #### ####  ##   ##  ##     ##
// ##       ########  ######   ##     ##    ##    ######      ## ### ## ##     ## ########
// ##       ##   ##   ##       #########    ##    ##          ##     ## ######### ##
// ##    ## ##    ##  ##       ##     ##    ##    ##          ##     ## ##     ## ##
//  ######  ##     ## ######## ##     ##    ##    ########    ##     ## ##     ## ##
//-------------------------------------------------------


 //   #
 //  ##      #    #  ####  ##### ###### #####
 // # #      #    # #        #   #      #    #
 //   #      #    #  ####    #   #####  #    #
 //   #      #    #      #   #   #      #####
 //   #       #  #  #    #   #   #      #
 // #####      ##    ####    #   ###### #

/**
 * For given Vstep, reads associated file containing noise infos for all det ids (from code Extract_Noise_Per_DetID.cxx) and returns a map linking each detid to its noise infos (for this Vstep)
 * @param step -- Bias voltage step
 * @param run  -- Run number
 */
map< double, vector<double> > Read_Noise_For1Step_And_Fill_Map(TString step, TString run)
{
	//Input files, produced by code "Extract_Noise_Per_DetID.cxx"
	TString filename = "./Noise_All_DetIDs_files/run_"+run+"/Noise_All_DetIDs_run"+run+"_"+step+"V.root";
	if(!Check_File_Existence(filename) ) {cout<<FRED("File "<<filename<<" not found ! Continue")<<endl;}

	TFile* f_input = TFile::Open(filename);
	TTree* t_input = (TTree*) f_input->Get("Tree");

	double detid=0, noise=0, error=0, chi2=0, noise_rms = 0;

	//To read input branches
	t_input->SetBranchAddress("detid", &detid);
	t_input->SetBranchAddress("noise_mean", &noise);
	t_input->SetBranchAddress("noise_err", &error);
	t_input->SetBranchAddress("noise_chi2", &chi2);
	t_input->SetBranchAddress("noise_rms", &noise_rms);

	map< double, vector<double> > map_detid_noise; //Will map detid <-> noise infos (for given Vstep)

	vector<double> v_tmp(4);
	//For all detids, read input file, create map : detid <-> noise infos
	for(int ientry=0; ientry<t_input->GetEntries(); ientry++)
	{
		//-- don't process all entries for now
		// if(ientry>10) {break;} //FIXME

		v_tmp.clear(); v_tmp.resize(4);
		detid=0, noise=0, error=0, chi2=0, noise_rms=0;

		t_input->GetEntry(ientry);

		v_tmp[0] = noise; v_tmp[1] = error; v_tmp[2] = noise_rms; v_tmp[3] = chi2;

		map_detid_noise.insert(pair< double, vector<double> > (detid, v_tmp) );
	}

	f_input->Close();

	return map_detid_noise;
}







 //   ##   #      #         #    #  ####  ##### ###### #####   ####
 //  #  #  #      #         #    # #        #   #      #    # #
 // #    # #      #         #    #  ####    #   #####  #    #  ####
 // ###### #      #         #    #      #   #   #      #####       #
 // #    # #      #          #  #  #    #   #   #      #      #    #
 // #    # ###### ######      ##    ####    #   ###### #       ####

/**
 * Call above function for each Vstep --> Fill a vector of maps (1 map per Vstep)
 * @param run -- run number
 */
vector < map< double, vector<double> > > Get_Detid_Noise_Maps_For_All_Steps(TString run)
{
	vector < map< double, vector<double> > > v_maps_allSteps( step_list.size() );

	for(int istep=0; istep<step_list.size(); istep++)
	{
		if(!step_isUsed[istep]) {continue;}

		v_maps_allSteps[istep] = Read_Noise_For1Step_And_Fill_Map(step_list[istep], run);
	}

	return v_maps_allSteps;
}







//-------------------------------------------------------
// ###### # #####    #    #  ####  #####  ###### #
// #      #   #      ##  ## #    # #    # #      #
// #####  #   #      # ## # #    # #    # #####  #
// #      #   #      #    # #    # #    # #      #
// #      #   #      #    # #    # #    # #      #
// #      #   #      #    #  ####  #####  ###### ######
//-------------------------------------------------------


/**
 * Fitting model used to fit the noise VS bias plots and extract Vfd.
 * User-defined function used to create a TF1 object
 * @param  x   -- x[0] = Bias voltage (1D histo)
 * @param  par -- array of parameters : 0 = cst, 1 = beta, 2 = vfd
 */
double Fitting_Model(double* x, double *par) //Arguments by convention
{
	double xx = x[0]; //abcissa
	double result = 0;

	if(xx <= par[2])
	{
		if(model_choice == 0) result = par[0] + par[1] * (1./sqrt(xx) - 1./sqrt(par[2]) );
		else if(model_choice == 1)	result = par[0] + par[3] * (xx - par[2]) + par[1] * (1./sqrt(xx) - 1./sqrt(par[2]) );
		else if(model_choice == 2)	result = par[0] + par[3] * (xx - 285) + par[1] * (1./sqrt(xx) - 1./sqrt(par[2]) );
	}
	else
	{
		if(model_choice == 0) result = par[0];
		else if(model_choice == 1) result = par[0] + par[3] * (xx - par[2]); //c0 + c1 * (x-Vfd)
		else if(model_choice == 2) result = par[0] + par[3] * (xx - 285);
	}

	return result;
}











//-------------------------------------------------------
// ########  ##        #######  ########       ###    ##    ## ########     ######## #### ########
// ##     ## ##       ##     ##    ##         ## ##   ###   ## ##     ##    ##        ##     ##
// ##     ## ##       ##     ##    ##        ##   ##  ####  ## ##     ##    ##        ##     ##
// ########  ##       ##     ##    ##       ##     ## ## ## ## ##     ##    ######    ##     ##
// ##        ##       ##     ##    ##       ######### ##  #### ##     ##    ##        ##     ##
// ##        ##       ##     ##    ##       ##     ## ##   ### ##     ##    ##        ##     ##
// ##        ########  #######     ##       ##     ## ##    ## ########     ##       ####    ##
//-------------------------------------------------------


/**
 * Takes as input the Noise_vs_bias histo for given detid/run, and fits it with model ; returns the Vfd param and its error, and stores a few plots for control
 * @param graph_to_fit          -- TGraphErrors* noise_vs_bias to fit
 * @param counter_control_plots -- counter (don't store all control plots)
 * @param detid                 -- detid corresponding to the histo
 */
void Fit_Noise_VS_Bias_Histogram_And_Make_Control_Plots(TFile* f_output_plots, vector<Double_t> &result, TGraphErrors* graph_to_fit, double detid, TString run, TString date, bool do_control_plot)
{
	bool print_correl_matrices = false;

	gStyle->SetOptFit(1);
	gStyle->SetFuncWidth(3);

	//--- FILTER ON PARTITION AND LAYER (decoding info from detid)
	int partition = int( ( (long) (detid/10)>>25)&0x7);
	int layer = int( ( (long) (detid/10)>>14)&0x7);

	TString subdet;
	if(partition == 3) {subdet = "TIB";}
	// else if(partition == ) {subdet = "";}
	else {cout<<FRED("Name of subdet not found from detid !")<<endl;}

	TString corr_name= "_" + subdet + "_" + date + "_run" + run;
	int corrected = CorrectGraphForLeakageCurrent(graph_to_fit, (long) detid/10, corr_name); //Need to trop last number of detid !
	// if(!corrected) cout<<FRED("No Leakage Current Correction found for run "<<run<<" !")<<endl;

	//Get X boundaries of TGraph
	double xmin, xmax, y_tmp, ymin, ymax;
	graph_to_fit->GetPoint(0, xmin, ymax);
	graph_to_fit->GetPoint(graph_to_fit->GetN()-1, xmax, ymin);
	// cout<<"xmin "<<xmin<<" / xmax "<<xmax<<endl;

	//Get y-mean from last 5 points to initialize the "Constant" param, since it seems sensitive to tuning
	int npts_constant_computation = 5;
	double constant_init = 0;
	graph_to_fit->GetPoint(graph_to_fit->GetN()-1, xmin, y_tmp);

	for(int ipt=graph_to_fit->GetN() -npts_constant_computation; ipt<graph_to_fit->GetN(); ipt++)
	{
		graph_to_fit->GetPoint(ipt, xmin, y_tmp);
		constant_init+= y_tmp;
	}
	constant_init/= (double) npts_constant_computation; //FIXME
	constant_init+= 0.2;


	//--- Write TGraph in output file
	TString output_name = "noiseVSbias_";
	if(partition==3) output_name+= "TIB";
	else if(partition==4) output_name+= "TOB";
	// else if(partition==5) output_name+= "TID";
	// else if(partition==6) output_name+= "TEC";
	output_name+= Convert_Number_To_TString(layer);
	output_name+= "_" + Convert_Number_To_TString(detid);


	if(do_control_plot)
	{
		f_output_plots->cd();
		graph_to_fit->Write(output_name);
	}


	//Fitting model --- (name, fcn, xmin, xmax, npar)
	TF1* noise_fit;
	if(model_choice==0)
	{
		noise_fit = new TF1("myfunc", Fitting_Model, 0.1, xmax, 3);
		noise_fit->SetParNames("Constant","#beta", "V_{FD}");

	}
	else if(model_choice==1 || model_choice==2)
	{
		noise_fit = new TF1("myfunc", Fitting_Model, 0.1, xmax, 4);
		noise_fit->SetParNames("Constant","#beta", "V_{FD}", "Slope");
	}

	//-- Init values
	noise_fit->SetParameters(constant_init, 3., 70);
	if(model_choice==1 || model_choice==2) noise_fit->SetParameter(3, -0.001); //-- slope

	//-- Bounds for parameters
	noise_fit->SetParLimits(0, constant_init-0.5, constant_init+0.5);
	// noise_fit->SetParLimits(0, 3, 7);
	noise_fit->SetParLimits(1, 1.5, 5.5);
	if(model_choice==1 || model_choice==2) 	noise_fit->SetParLimits(3, -0.01, 0);

	// noise_fit->SetParLimits(2, 50, 250);

	//--- From root doc :
	//Smart pointer to TFitResult, containing infos on fit (cov matrix, status, ...)
	// “S” The result of the fit is returned in the TFitResultPtr.
	// “E” Perform better errors estimation using the Minos technique
	// “M” Improve fit results, by using the IMPROVE algorithm of TMinuit.
	TFitResultPtr fit_result = graph_to_fit->Fit(noise_fit, "M B Q S"); //quiet //CHANGED
	// TMatrixDSym cov_matrix = fit_result->GetCovarianceMatrix();
	TMatrixDSym correl_matrix = fit_result->GetCorrelationMatrix();
	// cout<<"Correlation 0 vs 1 : "<<fit_result.Correlation(0,1)<<endl;
	if(print_correl_matrices) correl_matrix.Print();

	// vector<Double_t> result(3);
	result[0] = noise_fit->GetParameter(2);
	result[1] = noise_fit->GetParError(2);
	result[2] = noise_fit->GetProb();
	result[3] = noise_fit->GetChisquare() / noise_fit->GetNDF();
	// cout<<endl<<"noise_fit->GetParameter(0) = "<<noise_fit->GetParameter(0)<<" / error = "<<noise_fit->GetParError(0)<<endl;
	// cout<<"noise_fit->GetParameter(1) = "<<noise_fit->GetParameter(1)<<" / error = "<<noise_fit->GetParError(1)<<endl;
	// cout<<"noise_fit->GetParameter(2) = "<<noise_fit->GetParameter(2)<<" / error = "<<noise_fit->GetParError(2)<<endl;

	//--- Protection against unreasonable Vfds
	if(result[0] < 0 || result[0] > 300)
	{
		result[0] = -1;
		result[1] = -1;
		result[2] = -1;
		result[3] = -1;
	}

	if(do_control_plot)
	{
		output_name+= ".png";

		TCanvas* c = new TCanvas("c", "", 1000, 800); c->cd();
		c->SetTopMargin(0.1);
		c->SetBottomMargin(0.1);

		if(transformation_choice == 1)
		{
			c->SetLogy();
			c->SetLogx();
		}

		Style_Control_TGraph(graph_to_fit);
		if(corrected) graph_to_fit->SetMarkerColor(13); //Gray markers if corrected

		graph_to_fit->Draw("AP"); //A=draw axis, P=draw markers,
		// noise_fit->Draw("same");

		Draw_ExtraText(c);

		TLine *l_vfd = new TLine(noise_fit->GetParameter(2), ymin - 0.1, noise_fit->GetParameter(2), ymax - 0.1);

		l_vfd->SetLineStyle(2);
		l_vfd->Draw("same");

		//--- display fit proba on plot
		// cout<<"Fit proba : "<<noise_fit->GetProb()<<endl;
		TLatex latex;
		latex.SetNDC();
		latex.SetTextSize(0.03);
		TString tmp = "Fit->GetProb() = " + Convert_Number_To_TString(noise_fit->GetProb(), 3);
		// latex.DrawLatex(0.70, 0.60, tmp);

		TLegend *leg = new TLegend(0.70, 0.65, 0.85, 0.80);
		//leg->SetBorderSize(0.1);
		leg->SetTextFont(42);
		leg->SetHeader(" "+date(0,4)+" scan");
		leg->AddEntry(graph_to_fit, "Data", "P");
		leg->AddEntry(noise_fit, "Fit model", "l");
		leg->Draw("same");

		TString output_name = "noiseVSbias_";
		if(partition==3) output_name+= "TIB";
		else if(partition==4) output_name+= "TOB";
		// else if(partition==5) output_name+= "TID";
		// else if(partition==6) output_name+= "TEC";
		output_name+= Convert_Number_To_TString(layer);
		output_name+= "_" + Convert_Number_To_TString(detid)+".png";

		c->SaveAs( output_name );

		delete leg; leg = NULL;
		delete l_vfd; l_vfd = NULL;
		delete c; c = NULL;
	}

	delete noise_fit; noise_fit = NULL;
}




/*
███████ ██    ██ ██████  ███████ ██████  ██ ███    ███ ██████   ██████  ███████ ███████
██      ██    ██ ██   ██ ██      ██   ██ ██ ████  ████ ██   ██ ██    ██ ██      ██
███████ ██    ██ ██████  █████   ██████  ██ ██ ████ ██ ██████  ██    ██ ███████ █████
     ██ ██    ██ ██      ██      ██   ██ ██ ██  ██  ██ ██      ██    ██      ██ ██
███████  ██████  ██      ███████ ██   ██ ██ ██      ██ ██       ██████  ███████ ███████
*/

/**
 * Plot noise VS bias for the 6 APVs belonging to same module
 */
void Plot_Superposed_APVs_SameModule(vector<TGraphErrors*> v_graphs_sameModule, double current_detid)
{
	bool superimpose_plateaus = true;

	if(superimpose_plateaus)
	{
		double y_first=0, y_tmp=0;
		double x, y;
		v_graphs_sameModule[0]->GetPoint(v_graphs_sameModule[0]->GetN()-1, x, y); y_first+= y;
		v_graphs_sameModule[0]->GetPoint(v_graphs_sameModule[0]->GetN()-2, x, y); y_first+= y;
		v_graphs_sameModule[0]->GetPoint(v_graphs_sameModule[0]->GetN()-3, x, y); y_first+= y;
		y_first/= 3;

		for(int iapv=1; iapv<v_graphs_sameModule.size(); iapv++)
		{
			y_tmp=0;

			v_graphs_sameModule[iapv]->GetPoint(v_graphs_sameModule[iapv]->GetN()-1, x, y); y_tmp+= y;
			v_graphs_sameModule[iapv]->GetPoint(v_graphs_sameModule[iapv]->GetN()-2, x, y); y_tmp+= y;
			v_graphs_sameModule[iapv]->GetPoint(v_graphs_sameModule[iapv]->GetN()-3, x, y); y_tmp+= y;
			y_tmp/= 3;

			double scale_factor = y_first / y_tmp;

			for(int ipt=0; ipt<v_graphs_sameModule[iapv]->GetN(); ipt++)
			{
				v_graphs_sameModule[iapv]->GetPoint(ipt, x, y);
				v_graphs_sameModule[iapv]->SetPoint(ipt, x, y*scale_factor);
			}
		}
	}

	//Find absolute min and max in y-axis
	double ymin=999, ymax=-999;
	for(int iapv=0; iapv<v_graphs_sameModule.size(); iapv++)
	{
		if(TMath::MinElement(v_graphs_sameModule[iapv]->GetN(), v_graphs_sameModule[iapv]->GetY()) < ymin) {ymin = TMath::MinElement(v_graphs_sameModule[iapv]->GetN(), v_graphs_sameModule[iapv]->GetY());}

		if(TMath::MaxElement(v_graphs_sameModule[iapv]->GetN(),v_graphs_sameModule[iapv]->GetY()) > ymax) {ymax = TMath::MaxElement(v_graphs_sameModule[iapv]->GetN(),v_graphs_sameModule[iapv]->GetY());}

		Style_Control_TGraph(v_graphs_sameModule[iapv]);
	}

	//Set ymin and ymax
	v_graphs_sameModule[0]->SetMinimum(ymin -0.1);
	v_graphs_sameModule[0]->SetMaximum(ymax +0.1);

	//Coloring
	v_graphs_sameModule[0]->SetLineColor(kBlack);
	v_graphs_sameModule[1]->SetLineColor(kRed);
	v_graphs_sameModule[2]->SetLineColor(kBlue);
	v_graphs_sameModule[3]->SetLineColor(kOrange);
	v_graphs_sameModule[4]->SetLineColor(kMagenta);
	v_graphs_sameModule[5]->SetLineColor(kGreen);

	TCanvas* c = new TCanvas("c", "", 1000, 800); c->cd();
	c->SetTopMargin(0.1);
	c->SetBottomMargin(0.1);

	//Draw
	v_graphs_sameModule[0]->Draw("AL"); //A=draw axis, P=draw markers, L =lines
	v_graphs_sameModule[1]->Draw("L same"); //A=draw axis, P=draw markers,
	v_graphs_sameModule[2]->Draw("L same"); //A=draw axis, P=draw markers,
	v_graphs_sameModule[3]->Draw("L same"); //A=draw axis, P=draw markers,
	v_graphs_sameModule[4]->Draw("L same"); //A=draw axis, P=draw markers,
	v_graphs_sameModule[5]->Draw("L same"); //A=draw axis, P=draw markers,

	Draw_ExtraText(c);

	TLegend *leg = new TLegend(0.90, 0.75, 1., 1.);
	leg->SetTextFont(42);
	for(int iapv=0; iapv<v_graphs_sameModule.size(); iapv++)
	{
		leg->AddEntry(v_graphs_sameModule[iapv], "APV "+Convert_Number_To_TString(iapv), "L");
	}
	leg->Draw("same");

	TString output_name = "Superpose_"+Convert_Number_To_TString( (long) current_detid/10, 15);
	if(superimpose_plateaus) output_name+= "_plateau";
	output_name+=".png";
	c->SaveAs(output_name);

	// delete leg; leg = NULL;
	delete c; c = NULL;

	return;
}







/*
██████  ██    ██ ███    ██      ██████  ███    ██      █████  ██      ██           █████  ██████  ██    ██ ███████
██   ██ ██    ██ ████   ██     ██    ██ ████   ██     ██   ██ ██      ██          ██   ██ ██   ██ ██    ██ ██
██████  ██    ██ ██ ██  ██     ██    ██ ██ ██  ██     ███████ ██      ██          ███████ ██████  ██    ██ ███████
██   ██ ██    ██ ██  ██ ██     ██    ██ ██  ██ ██     ██   ██ ██      ██          ██   ██ ██       ██  ██       ██
██   ██  ██████  ██   ████      ██████  ██   ████     ██   ██ ███████ ███████     ██   ██ ██        ████   ███████
*/

/**
 * Main function. For all detids, it will collect the fit results and store the Vfd values into an output TTree
 * @param run -- run number
 * @parel plots_only -- if TRUE, produce control plots but not the output tree with fit results
 */
void Produce_Noise_VS_Bias_Histograms_For_All_Detids_And_Store_Results(TString run, TString date, bool plots_only)
{
	cout<<endl<<FYEL("#######################")<<endl;
	cout<<FYEL("--- Produce Noise VS Bias plots, Fit them, Store results and control plots")<<endl;
	cout<<"RUN "<<run<<" / Model "<<model_choice<<endl;
	cout<<FYEL("#######################")<<endl<<endl;

	bool plot_all_APVs_sameModule = true;

	//Get maps detid <-> noise infos, for all Vsteps
	vector < map< double, vector<double> > > v_maps_allSteps = Get_Detid_Noise_Maps_For_All_Steps(run); //Fill the vector of maps (1 map per Vstep)

	vector<Double_t> fit_results(4); double detid;

	TFile* f_output_plots = new TFile("NoiseVSvolt_graphs_run"+run+".root", "RECREATE"); //Will contain output TGraphs (then plot, superimpose, ...)

	//Output file, containing Vfd results for all detids
	TFile *f_fit_results = new TFile("Fit_Results_run"+run+".root","RECREATE");
	TTree* tree = new TTree("Tree", "Fit results");

	TBranch *b_detid = tree->Branch("detid", &detid, "detid/D");
	TBranch *b_Vfd = tree->Branch("Vfd", &fit_results[0], "Vfd/D");
	TBranch *b_Vfd_err = tree->Branch("Vfd_err", &fit_results[1], "Vfd_err/D");
	TBranch *v_Fit_proba = tree->Branch("Fit_proba", &fit_results[2], "Fit_proba/D");
	TBranch *v_Fit_chi2 = tree->Branch("Chi2_norm", &fit_results[3], "Chi2_norm/D");


	vector<TGraphErrors*> v_graphs_sameModule(6);
	double detid_currentModule;

	int n_control_plots_TIBL1=0; //Don't plot too many detids from TIBL1
	int n_control_plots_TIBL4=0; //Don't plot too many detids from TIBL4
	int n_control_plots_TIBL1_superimpose=0; //Don't plot too many detids from TIBL1
	int n_control_plots_TIBL4_superimpose=0; //Don't plot too many detids from TIBL4

	//Iterate over all the detids, taken as the list of keys stored in 1 map (e.g. the map corresponding to first Vstep)
	for(map< double, vector<double> >::iterator it = v_maps_allSteps[2].begin(); it != v_maps_allSteps[0].end(); it++)
	{
		detid = 0;
		for(int k=0; k<fit_results.size(); k++)
		{
			fit_results[k] = 0;
		}


		double current_detid = it->first; //if(current_detid == 0) {cout<<__LINE__<<FRED(" : Null detid -- correct map initialization ? ")<<endl;}
		// cout<<"current_detid = "<< setprecision(15) <<current_detid << endl;

		// cout<<Convert_Number_To_TString(current_detid)<<endl;
		// if( !Convert_Number_To_TString(current_detid).Contains("36912138") && !Convert_Number_To_TString(current_detid).Contains("369121390") && !Convert_Number_To_TString(current_detid).Contains("36912586") && !Convert_Number_To_TString(current_detid).Contains("369125870") ) {continue;}

		//--- FILTER ON PARTITION AND LAYER (decoding info from detid)
		int partition = int( ( (long) (current_detid/10)>>25)&0x7);
		int layer = int( ( (long) (current_detid/10)>>14)&0x7);
		if(partition != 3 || (layer != 1 && layer != 4) ) {continue;} //Only TIB L1 & L4 for now

		//-- Only plot few DetIDs
		bool do_control_plot=false;
		if((current_detid/10)==369174804) do_control_plot = true;
		if((current_detid/10)==369121381) do_control_plot = true;
		if((current_detid/10)==369125861) do_control_plot = true;
		if((current_detid/10)==436281512) do_control_plot = true;
		if(n_control_plots_TIBL1 <= 18 && layer == 1) {do_control_plot = true;}
		else if(n_control_plots_TIBL4 <= 18 && layer == 4) {do_control_plot = true;}

		bool do_superimposed_plot=false;
		if(n_control_plots_TIBL1_superimpose <= 30 && layer == 1) {do_superimposed_plot = true;}
		else if(n_control_plots_TIBL4_superimpose <= 30 && layer == 4) {do_superimposed_plot = true;}

		if(plots_only && !do_control_plot && !do_superimposed_plot) {continue;}

		TGraphErrors* g_noise_vs_bias_currentDetid = new TGraphErrors( nsteps_used );

		double ymin = 999; //There are empty bins messing with y-axis --> will show only bins with y > ymin instead

		//For each detid, we loop on all the maps (<-> all the Vsteps) and look for noise values for all Vsteps
		int ipt = 0;
		for(int istep=0; istep<step_list.size(); istep++)
		{
			if(!step_isUsed[istep]) {continue;}

			//Iterate over all detids (keys) of map, and look for the detid we're interested in
			map< double, vector<double> >::iterator it = v_maps_allSteps[istep].begin();

			while(it->first != current_detid && it != v_maps_allSteps[istep].end()) {it++;} //Loop on all detids (keys) of the map to find current detid

			if(it == v_maps_allSteps[istep].end() ) //If detid not found
			{
				cout<<FMAG("Detid "<<Convert_Number_To_TString(current_detid)<<" not found for Vstep "<<step_list[istep]<< " !")<<endl;
				continue;
			}

			// cout<<"istep "<<istep<<" / step "<<Convert_TString_To_Number(step_list[istep])<<" / y "<<it->second[0]<<endl;

			//Fill noise_vs_bias histo with noise info for current Vstep


			if(transformation_choice==0) {g_noise_vs_bias_currentDetid->SetPoint(ipt, Convert_TString_To_Number(step_list[istep]), it->second[0]);} //normal
			else if(transformation_choice==2) {g_noise_vs_bias_currentDetid->SetPoint(ipt, Convert_TString_To_Number(step_list[istep]), 1./pow(it->second[0], 1) );}

			g_noise_vs_bias_currentDetid->SetPointError(ipt, 0., it->second[1]); //ipt, e_x, e_y
			if(transformation_choice==2)  {g_noise_vs_bias_currentDetid->SetPointError(ipt, 0., 0.);} //ipt, e_x, e_y}
			// g_noise_vs_bias_currentDetid->SetPoint(istep, Convert_TString_To_Number(step_list[istep]), log(it->second[0]) );

			if(it->second[0] < ymin && it->second[0] != 0) {ymin = it->second[0];} //Compute ymin (for plotting)

			ipt++;
		}
		if(g_noise_vs_bias_currentDetid->GetN() == 0) {cout<<FRED("Noise VS Bias histo empty for detid "+Convert_Number_To_TString(current_detid)<<" !")<<endl;}

		//--- Clean TGraphs by removing missing points
		double x,y;
		for(int ipt=0; ipt<g_noise_vs_bias_currentDetid->GetN(); ipt++)
		{
			g_noise_vs_bias_currentDetid->GetPoint(ipt, x, y);
			if(x==0 && y==0) g_noise_vs_bias_currentDetid->RemovePoint(ipt);
		}


		//Once noise_vs_bias histo is done, fit it and extract results
		if(transformation_choice==0) {g_noise_vs_bias_currentDetid->SetMinimum(ymin - 0.2);} //Set custom ymin because there are empty bins

		if(plot_all_APVs_sameModule && do_superimposed_plot)
		{
			//If current APV is not from same module as previous ones, don't plot !
			if(v_graphs_sameModule[0] != 0 && (long) current_detid/10 != detid_currentModule)
			{
				for(int iapv=0; iapv<v_graphs_sameModule.size(); iapv++)
				{
					v_graphs_sameModule[iapv] = NULL;
				}
			}

			for(int iapv=0; iapv<v_graphs_sameModule.size(); iapv++)
			{
				if(v_graphs_sameModule[iapv] == 0)
				{
					if(iapv==0) {detid_currentModule = (long) current_detid/10;}
					v_graphs_sameModule[iapv] = (TGraphErrors*) g_noise_vs_bias_currentDetid->Clone();
					break;
				}
			}


			if(v_graphs_sameModule[5] != 0)
			{
				Plot_Superposed_APVs_SameModule(v_graphs_sameModule, current_detid);

				for(int iapv=0; iapv<v_graphs_sameModule.size(); iapv++)
				{
					v_graphs_sameModule[iapv] = NULL;
				}
			}
		}


		Fit_Noise_VS_Bias_Histogram_And_Make_Control_Plots(f_output_plots, fit_results, g_noise_vs_bias_currentDetid, current_detid, run, date, do_control_plot);
		detid = current_detid;

		// for(int k=0; k<fit_results.size(); k++)
		// {
			// cout<<"fit_results[i] "<<fit_results[k]<<endl;
		// }
		// cout<<"Vfd "<<fit_results[0]<<endl;

		//Fill the output TTree with fit result for current detid
		tree->Fill();


		delete g_noise_vs_bias_currentDetid; g_noise_vs_bias_currentDetid = NULL;

		if(do_control_plot && layer == 1) {n_control_plots_TIBL1++;}
		else if(do_control_plot && layer == 4) {n_control_plots_TIBL4++;}
		if(do_superimposed_plot && layer == 1) {n_control_plots_TIBL1_superimpose++;}
		else if(do_superimposed_plot && layer == 4) {n_control_plots_TIBL4_superimpose++;}
	}

	f_fit_results->cd();
	tree->Write();

	delete tree; tree = NULL;
	f_fit_results->Close(); delete f_fit_results; f_fit_results = NULL;

	f_output_plots->Close();

	return;
}







//--------------------------------------
//  ######  ######## ##     ## ########  #### ########  ######
// ##    ##    ##    ##     ## ##     ##  ##  ##       ##    ##
// ##          ##    ##     ## ##     ##  ##  ##       ##
//  ######     ##    ##     ## ##     ##  ##  ######    ######
//       ##    ##    ##     ## ##     ##  ##  ##             ##
// ##    ##    ##    ##     ## ##     ##  ##  ##       ##    ##
//  ######     ##     #######  ########  #### ########  ######
//--------------------------------------

double Get_Vfd_Pull_From_MCToys_And_Return_Mean(int ntoys, double error)
{
	bool print_correl_matrices = true;

	// create a random number generator
	// gRandom = new TRandom3();
	gRandom->SetSeed(0);

	//--- WARNING : dis-activates warnings !! and does not display the result of 10k toys... ?!
	//--- (To avoid Minos "new minimum" messages)
	// gPrintViaErrorHandler = kTRUE;
	// gErrorIgnoreLevel = kWarning;

	double xmin = Convert_TString_To_Number(step_list[0]), xmax = Convert_TString_To_Number(step_list[step_list.size() -1]);
	TH1F* h_vfd_toys = new TH1F("", "", 100, -0, 500);

	TF1* noise_fit; double* par;

	if(model_choice==0)
	{
		noise_fit = new TF1("myfunc", Fitting_Model, xmin, xmax, 3);
		noise_fit->SetParNames("Constant","#beta", "V_{FD}");
		par = new double[3];
	}
	else if(model_choice==1 || model_choice==2)
	{
		noise_fit = new TF1("myfunc", Fitting_Model, xmin, xmax, 4);
		noise_fit->SetParNames("Constant","#beta", "V_{FD}", "Slope");
		par = new double[4];
	}

	double* x = new double[1];

	for(int itoy=0; itoy<ntoys; itoy++)
	{
		//--- TRUE PARAMS VALUES
		par[0] = 5.5; par[1] = 2.8; par[2] = 120;
		if(model_choice==1 || model_choice==2) {par[3] = -0.0005;} //Slope

		// else if(model_choice==1 || model_choice==2) {par[0] = 5.; par[1] = 6; par[2] = 80.; par[3] = -0.0005;}


		//--- INIT FIT PARAMS
		noise_fit->SetParameters(4., 2., 70);
		if(model_choice==1 || model_choice==2) {noise_fit->SetParameter(3, -0.001);} //Slope

		noise_fit->SetParLimits(1, 1.5, 5.5);
		if(model_choice==1 || model_choice==2) noise_fit->SetParLimits(3, -0.01, 0);

		// noise_fit->SetParLimits(0, 3, 7);
		// noise_fit->SetParLimits(2, 50, 250);

		TGraphErrors* g_1toy = new TGraphErrors( step_list.size() );

		double vfd = 0;

		for(int istep=0; istep<step_list.size(); istep++)
		{
			x[0] = Convert_TString_To_Number(step_list[istep]);

			g_1toy->SetPoint(istep, x[0], gRandom->Gaus(Fitting_Model(x, par), error) ) ;
			g_1toy->SetPointError(istep, 0, error) ;
		}

		double constant_init = 0; double xmin=0, y_tmp=0; int n_pts_cst = 10;
		for(int ipt=g_1toy->GetN() - n_pts_cst; ipt<g_1toy->GetN(); ipt++)
		{
			g_1toy->GetPoint(ipt, xmin, y_tmp);
			constant_init+= y_tmp;
		}
		constant_init/= n_pts_cst;
		noise_fit->SetParameter(0, constant_init);
		noise_fit->SetParLimits(0, constant_init-0.1, constant_init+0.1);
		// noise_fit->FixParameter(0, constant_init);
		// cout<<"constant_init = "<<constant_init<<endl;

		//--- From root doc :
		//Smart pointer to TFitResult, containing infos on fit (cov matrix, status, ...)
		// “S” The result of the fit is returned in the TFitResultPtr.
		// “E” Perform better errors estimation using the Minos technique
		// “M” Improve fit results, by using the IMPROVE algorithm of TMinuit.
		TFitResultPtr fit_result = g_1toy->Fit(noise_fit, "M B Q S"); //quiet //CHANGED
		// TMatrixDSym cov_matrix = fit_result->GetCovarianceMatrix();
		TMatrixDSym correl_matrix = fit_result->GetCorrelationMatrix();

		vfd = noise_fit->GetParameter(2);

		h_vfd_toys->Fill(vfd);
		if(itoy % 1000 == 0)
		{
			cout<<"Vfd toy "<<itoy<<" = "<<vfd<<endl;
			if(print_correl_matrices) {correl_matrix.Print(); cout<<endl;}

			// cout<<"Fit proba : "<<noise_fit->GetProb()<<endl;

			if(itoy==0)
			{
				TCanvas* c_control = new TCanvas("c_control", "", 1000, 800); c_control->cd();
				c_control->SetTopMargin(0.1);
				c_control->SetBottomMargin(0.1);

				//Find absolute min and max in y-axis
				// double ymin=999, ymax=-999;
				// double x, y;
				// int npts = g_1toy->GetN();
				// for(int i=0; i<npts; i++)
				// {
				// 	g_1toy->GetPoint(i, x, y);
				// 	if(y<ymin) ymin = y;
				// 	else if(y>ymax) ymax=y;
				// }

				// h_1step->SetMinimum(par[0] - 0.5); h_1step->SetMaximum(par[0] + 1);

				Style_Control_TGraph(g_1toy);

				g_1toy->Draw("AP");
				Draw_ExtraText(c_control);
				c_control->SaveAs("toy_control.png");
				delete c_control; c_control = NULL;
			}
		}

		delete g_1toy; g_1toy = NULL;
	}

	TF1* gaus_fit = new TF1("myfunc", "gaus", h_vfd_toys->GetXaxis()->GetXmin(), h_vfd_toys->GetXaxis()->GetXmax() );
	// TF1* gaus_fit = new TF1("myfunc", "landau", h_vfd_toys->GetXaxis()->GetXmin(), h_vfd_toys->GetXaxis()->GetXmax() );
	h_vfd_toys->Fit(gaus_fit, "q");
	double mean_pull = gaus_fit->GetParameter(0);


	TCanvas* c = new TCanvas("c", "", 1000, 800); c->cd();
	c->SetTopMargin(0.1);
	c->SetBottomMargin(0.1);
	h_vfd_toys->Draw();
	Draw_ExtraText(c);

	// c->SaveAs("vfd_"+Convert_Number_To_TString(ntoys)+"toys.png" );
	c->SaveAs("vfd_toys.png" );

	delete noise_fit; noise_fit = NULL;
	delete gaus_fit; gaus_fit = NULL;
	delete h_vfd_toys; h_vfd_toys = NULL;
	delete c; c = NULL;
	delete[] par; par = NULL;
	delete[] x; x = NULL;

	return mean_pull;
}


void ToyStudy_Change_Scan_Procedure(int ntoys, double error, int scan_choice)
{
	if(scan_choice == 0)
	{
		step_list.push_back("275");
		step_list.push_back("280");
		step_list.push_back("290");
		step_list.push_back("295");
	}
	else if(scan_choice == 1)
	{
		step_list.push_back("5");
		step_list.push_back("15");
		step_list.push_back("25");
		step_list.push_back("35");
	}
	else if(scan_choice == 2)
	{
		step_list.push_back("85");
		step_list.push_back("95");
		step_list.push_back("105");
		step_list.push_back("115");
	}
	else {return;}

	Get_Vfd_Pull_From_MCToys_And_Return_Mean(ntoys, error);

	system( ("cp vfd_toys.png vfd_toys_scan"+Convert_Number_To_TString(scan_choice)+".png").Data() );

	return;
}






//FIXME

void Superimpose_Curves_From_Two_Scans(vector<TString> list_scans, vector<TString> v_dates, bool superimpose_plateaus)
{
	if(list_scans.size() < 2 || list_scans.size() > 5) {cout<<"Number of scans must be between 2 & 5! Abort"<<endl; return;}

	TFile* f = TFile::Open(("./NoiseVSvolt_graphs_run"+list_scans[0]+".root").Data() );

	// TString gname = "noiseVSbias_TIB1_3691202770";

	TGraphErrors* g_tmp = NULL;
	TObject* obj = NULL;
	TList* list_of_keys = f->GetListOfKeys();
	if (!list_of_keys) {cout<<FRED("No keys found in file !")<<endl; return;}
	TIter* iter_list = new TIter(list_of_keys);

	// while(key = (TKey*)next() )
	while(obj = iter_list->Next() )
	{
		f->GetObject(obj->GetTitle(), g_tmp); //Get profile object
	    // obj = key->ReadObj();
	    // if( !obj->InheritsFrom("TH1") ) {cout<<FRED("Obj doesn't inherit from TH1! Skip !")<<endl; continue;}

		TString tmp_name = obj->GetName();

		TCanvas* c = new TCanvas("c", "", 1000, 800); c->cd();
		c->SetTopMargin(0.1);
		c->SetBottomMargin(0.1);

		vector<TGraphErrors*> v_graphs(list_scans.size() );

		v_graphs[0] = (TGraphErrors*) f->Get(tmp_name);

		for(int i=1; i<list_scans.size(); i++)
		{
			TFile* f_tmp = TFile::Open("./NoiseVSvolt_graphs_run"+list_scans[i]+".root");

			v_graphs[i] = (TGraphErrors*) f_tmp->Get(tmp_name);

			f_tmp->Close(); f_tmp = NULL;
		}

		//Rescale to superimpose plateaus
		if(superimpose_plateaus)
		{
			double y_first=0, y_tmp=0;
			double x, y;
			v_graphs[0]->GetPoint(v_graphs[0]->GetN()-1, x, y); y_first+= y;
			v_graphs[0]->GetPoint(v_graphs[0]->GetN()-2, x, y); y_first+= y;
			v_graphs[0]->GetPoint(v_graphs[0]->GetN()-3, x, y); y_first+= y;
			y_first/= 3;

			for(int j=1; j<v_graphs.size(); j++)
			{
                                if(!v_graphs[j]) continue;
				y_tmp=0;

				v_graphs[j]->GetPoint(v_graphs[j]->GetN()-1, x, y); y_tmp+= y;
				v_graphs[j]->GetPoint(v_graphs[j]->GetN()-2, x, y); y_tmp+= y;
				v_graphs[j]->GetPoint(v_graphs[j]->GetN()-3, x, y); y_tmp+= y;
				y_tmp/= 3;

				double scale_factor = y_first / y_tmp;

				for(int ipt=0; ipt<v_graphs[j]->GetN(); ipt++)
				{
					v_graphs[j]->GetPoint(ipt, x, y);
					v_graphs[j]->SetPoint(ipt, x, y*scale_factor);
				}
			}
		}

		double ymin = 999, ymax = 0;
		for(int i=0; i<list_scans.size(); i++)
		{
                        if(!v_graphs[i]) continue;
			Style_Control_TGraph(v_graphs[i]);
			v_graphs[i]->SetLineColor(i+1);
			if(i==2) v_graphs[i]->SetLineColor(8); // change light green to dark
			if(i==4) v_graphs[i]->SetLineColor(kOrange); // change yellow to orange

			if(TMath::MinElement(v_graphs[i]->GetN(), v_graphs[i]->GetY()) < ymin) {ymin = TMath::MinElement(v_graphs[i]->GetN(), v_graphs[i]->GetY());}
			if(TMath::MaxElement(v_graphs[i]->GetN(), v_graphs[i]->GetY()) > ymax) {ymax = TMath::MaxElement(v_graphs[i]->GetN(), v_graphs[i]->GetY());}
		}

		v_graphs[0]->SetMaximum(ymax*1.05);
		v_graphs[0]->SetMinimum(ymin*0.95);

		Draw_ExtraText(c);

		TLegend *leg = new TLegend(0.70, 0.65, 0.85, 0.85);
		//leg->SetBorderSize(0.1);
		leg->SetTextFont(42);

		for(int i=0; i<v_graphs.size(); i++)
		{
                        if(!v_graphs[i]) continue;
			if(i==0) v_graphs[0]->Draw("AL"); //A=draw axis, P=draw markers,
			else v_graphs[i]->Draw("L same");

			// leg->AddEntry(v_graphs[i], list_scans[i], "L");
			leg->AddEntry(v_graphs[i], v_dates[i], "L");
		}
		leg->Draw("same");
		leg->SetHeader("Noise scans");

		TLine *l_vfd = new TLine(30, ymin*0.95, 30, ymax*1.05); //vertical line @ 30V
		l_vfd->SetLineStyle(2);
		l_vfd->Draw("same");

		int index_tmp = tmp_name.Index("_");
		tmp_name = tmp_name.Remove(0, index_tmp+1); //Remove all characters before first underscore
		index_tmp = tmp_name.Index("_");
		tmp_name = tmp_name.Remove(0, index_tmp+1); //Remove all characters from beginning to 2nd underscore (included)
		double detid = Convert_TString_To_Number(tmp_name); // The remaining TString corresponds to the detid
		// cout<<"detid "<<detid<<endl;
		//--- FILTER ON PARTITION AND LAYER (decoding info from detid)
		int partition = int( ( (long) (detid/10)>>25)&0x7);
		int layer = int( ( (long) (detid/10)>>14)&0x7);

		TString output_name = "noiseVSbias_";
		if(partition==3) output_name+= "TIB";
		else if(partition==4) output_name+= "TOB";
		// else if(partition==5) output_name+= "TID";
		// else if(partition==6) output_name+= "TEC";
		output_name+= Convert_Number_To_TString(layer);
		output_name+= "_" + Convert_Number_To_TString(detid);
                if(superimpose_plateaus) output_name+= "_plateau";
                output_name+= "_superimposedRuns.png";

		c->SaveAs(output_name);
		// c->SaveAs("test.png");

		for(int i=0; i<v_graphs.size(); i++)
		{
			delete v_graphs[i]; v_graphs[i] = NULL;
		}
		delete leg; leg = NULL;
		delete c; c = NULL;
		delete l_vfd; l_vfd = NULL;
	}

	f->Close();
	delete iter_list;

	return;
}




//-------------------------------
// ##     ##    ###    #### ##    ##
// ###   ###   ## ##    ##  ###   ##
// #### ####  ##   ##   ##  ####  ##
// ## ### ## ##     ##  ##  ## ## ##
// ##     ## #########  ##  ##  ####
// ##     ## ##     ##  ##  ##   ###
// ##     ## ##     ## #### ##    ##
//-------------------------------

int main()
{
	Modified_tdr_style(); //Enforce official tdrStyle (slightly modified)
	TString run, date;

	//run = "203243"; date = "20120921";
	//run = "280667"; date = "20160914";
	//run = "303272"; date = "20170919";
	run = "317974"; date = "20180618";
        //run = "318024"; date = "20180618";
	//run = "323011"; date = "20180919";
	//run = "328691"; date = "20190321";

	Fill_Step_List_Vector(run);

	Produce_Noise_VS_Bias_Histograms_For_All_Detids_And_Store_Results(run, date, false);

	//--- TOYS STUDIES
	// Get_Vfd_Pull_From_MCToys_And_Return_Mean(10000, 0.015);
	// ToyStudy_Change_Scan_Procedure(10000, 0.01, 2);

	vector<TString> list_scans; vector<TString> v_dates;
	list_scans.push_back("203243"); v_dates.push_back("Sep 2012");
	list_scans.push_back("280667"); v_dates.push_back("Sep 2016");
	list_scans.push_back("303272"); v_dates.push_back("Sep 2017");
	list_scans.push_back("323011"); v_dates.push_back("Sep 2018");
	list_scans.push_back("328691"); v_dates.push_back("Mar 2019");
	//Superimpose_Curves_From_Two_Scans(list_scans, v_dates, false);

	return 0;
}

