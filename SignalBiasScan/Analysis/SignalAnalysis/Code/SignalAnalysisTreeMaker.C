/*Idea of the code :
- Loop over events > tracks > Hits
- Each detid is associated with a vector of TH1F*, and each TH1F* corresponds to 1 voltage step
- For a given event, for each good track of the event, for each good hit of the track : fill the hit.charge() value into the corresponding histogram (depending on detid, Vstep)
- At the end, fit all histograms (all detids, all Vsteps) ==> Obtain charge estimate for each detid/Vstep !

*/

#define SignalAnalysisTreeMaker_cxx

#include "SignalAnalysisTreeMaker.h"

#include <iostream>
//#include <time.h>
#include <sstream>
//#include <set>
//#include <utility>
//#include <fstream>
//#include <cmath>
//#include <exception>

#include <TString.h>
#include <TROOT.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH2F.h>
#include <TMath.h>
#include <TRint.h>
#include <TRandom3.h>

//FIXME
//int counter = 0;


// 1 = TIB // 2 = TOB // 3 = TID // 4 = TEC
// ****************************************

enum SubDet { All, TIB, TOB, TID, TEC};

using namespace std;

Double_t fitLandauGauss(Double_t *x, Double_t *par){

  Double_t value =  (1-par[3])*TMath::Landau(x[0], par[0], par[1]) + par[3]*TMath::Gaus(x[0], par[0], par[2]);
  return value;
}

  
// Main method //
//-------------//
void SignalAnalysisTreeMaker::Loop()
{
 
  std::cout<<"Initializing"<<std::endl;
  
  std::string output_file;
  
  // Read definition of voltage steps
  std::cout << "Setting Deco Voltage vector : " << std::endl;
  if(usetimestamp) std::cout << " using timestamp step definition " << std::endl;
  else std::cout << " using event number step definition" << std::endl;
  
  //if(usetimestamp) VSmaker.readVoltageSteps_timestamp(stepsfile.c_str());
  //else VSmaker.readVoltageSteps_evtnumber(stepsfile.c_str());
  VSmaker.readVoltageSteps(stepsfile.c_str(), usetimestamp);
  std::cout << " done" << std::endl;
  std::cout << " found " << VSmaker.getNVoltage() << " steps " << std::endl;
  

  VSmaker.Initialize();
  t_monitor_start = VSmaker.t_monitor_start;
  t_monitor_end = VSmaker.t_monitor_end;


  int nevent    = 0;
  int theVoltage=-1;
  

  if (fChain == 0) return;
  
  // Define histos
  std::vector< std::vector< TH1F* > > commonHistos;
  std::vector< std::vector< TH1F* > > emptyHistos;
  string subdetname[] = {"TIB", "TOB", "TID", "TEC"};
    
  for(unsigned int idet=0; idet<4; idet++)
  {
	std::vector< TH1F* > Histos;
	Histos.push_back(new TH1F(Form("hNtracks_%s", subdetname[idet].c_str()), "hNtracks", 1000, 0, 1000));
	Histos.push_back(new TH1F(Form("hTrackAngle_%s", subdetname[idet].c_str()), "hTrauckAngle", 90, -180, 180));
	Histos.push_back(new TH1F(Form("hTsosxMinusClusx_%s", subdetname[idet].c_str()), "hTsosxMinusClusx", 100, -0.01, 0.01));
	Histos.push_back(new TH1F(Form("hClusMinusSeed_%s", subdetname[idet].c_str()), "hClusMinusSeed", 150, -1.5, 1.5));
	Histos.push_back(new TH1F(Form("hNevtPerStep_%s", subdetname[idet].c_str()), "hNevtPerStep", 80, 0, 400));
    commonHistos.push_back(Histos);
  }
  

  // Define monitors
  Monitors_TIB.insert(std::pair< ULong64_t, TProfile* > (369121605, new TProfile("monitor_369121605", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TIB.insert(std::pair< ULong64_t, TProfile* > (369121390, new TProfile("monitor_369121390", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TIB.insert(std::pair< ULong64_t, TProfile* > (369125870, new TProfile("monitor_369125870", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TOB.insert(std::pair< ULong64_t, TProfile* > (436281512, new TProfile("monitor_436281512", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TID.insert(std::pair< ULong64_t, TProfile* > (402664070, new TProfile("monitor_402664070", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TEC.insert(std::pair< ULong64_t, TProfile* > (470148196, new TProfile("monitor_470148196", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));
  Monitors_TEC.insert(std::pair< ULong64_t, TProfile* > (470148300, new TProfile("monitor_470148300", "", (t_monitor_end-t_monitor_start)/30, 0, t_monitor_end-t_monitor_start)));

  

  //Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nentries = fChain->GetEntries();

  std::cout << "NUMBER OF ENTRIES = "<< nentries << std::endl; 


  //------------------//
  // Loop over events //
  //------------------//
  

  int nEntries_isPosV = 1;
  

  //MAIN LOOP
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {

    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
	fChain->GetEntry(jentry);
    
	if(jentry%5000 == 0) 
    { 
       std::cout << "Number of events : " << jentry <<"/"<<nentries<< std::endl;
    }   
    
    //if(event->run_nr != 295324) 
    
    // if(event->ev_nr%1000==0)
    // cout<<"run "<<event->run_nr<<" event "<<event->ev_nr<<" timestamp "<<event->ev_timestamp<<" V "<<theVoltage<<std::endl;
    
    if(theVoltage > 0) {nEntries_isPosV++;}    
    if(nEntries_isPosV % 5000 == 0)
    {
    	cout<<"--- "<<nEntries_isPosV<<" entries w/ positive V values"<<endl;
    }
    


    // Get voltage setting for this event
	theVoltage=-1;
  // std::cout<<"t_monitorstart"<<t_monitor_start<<"|| timsetamp"<< (int)(event->ev_timestamp)<<"|| tmonitorend"<<t_monitor_end<<std::endl;
	if(usetimestamp) theVoltage = VSmaker.getVoltage_timestamp((int)(event->ev_timestamp));//error with timestamp
	else theVoltage = VSmaker.getVoltage_evtnumber(event->run_nr, event->ev_nr, event->ev_timestamp);
	
	// cout<<"V = "<<theVoltage<<" -- Run : "<<event->run_nr<<" / Event : "<<event->ev_nr<<" / Timestamp : "<<event->ev_timestamp<<endl;
	//if(theVoltage != 20) {continue;} //FIXME
	
	if(theVoltage<0) 
	{
		//cout<<"Voltage step < 0 / Continue"<<endl; 
		
		continue; // skip event if not on a voltage step
	} 
	
    //cout<<endl<<"Voltage step = "<<theVoltage<<endl;

	int thePointNumber = VSmaker.getIndex(theVoltage);

	// check index
	if(thePointNumber < 0){
      std::cout<<" WARNING : point number out of range : "<<thePointNumber<<std::endl;
	  return;
	}


	for(unsigned int idet=0; idet<4; idet++)
	{
	  //commonHistos[idet][0]->Fill(event->tracks.size()); // N tracks per event
	  commonHistos[idet][0]->Fill(event->Ntracks); // adapted to v1.1 data format
	  commonHistos[idet][4]->Fill(theVoltage);
	}
	
	//cout<<event->tracks.size()<<" tracks"<<endl; //FIXME

    // Loop over tracks
    for(unsigned int itr=0; itr<event->tracks.size(); itr++)
	{
	  //cout<<"itr "<<itr<<endl; //FIXME
	
	  TreeTrack *track = &(event->tracks[itr]);
	  if(track->chi2>5) 
	  {
	  	//cout<<"Track "<<itr<<" : track->chi2>5 !! Skip track"<<endl;
	  	continue;	
	  }
	  //if(track->pT<5) continue;

	  // Get number of hits
      int nTIBhits, nTOBhits, nTIDhits, nTEChits;
	  
	  if(track->TIB_hits.size()) nTIBhits = track->TIB_hits.size();
	  else nTIBhits = track->TIB_fullHits.size(); // same hits with more infos
	  if(track->TOB_hits.size()) nTOBhits = track->TOB_hits.size();
	  else nTOBhits = track->TOB_fullHits.size();
	  if(track->TID_hits.size()) nTIDhits = track->TID_hits.size();
	  else nTIDhits = track->TID_fullHits.size();
	  if(track->TEC_hits.size()) nTEChits = track->TEC_hits.size();
	  else nTEChits = track->TEC_fullHits.size();

      // int nHitsTotal = nTIBhits + nTOBhits + nTIDhits + nTEChits;
      int nHitsTotal = track->Nhits; // adapted to v1.1 data format
      if(nHitsTotal < 5) 
      {
      	//cout<<"nHitsTotal < 5 !! Skip track"<<endl;
      	continue;
      } // remove tracks with less than 5 hits


	  // Fill histos
	  
	  //cout<<"---- Track "<<itr<<", Hits : "<<track->TIB_hits.size()<<" TIB / "<<track->TOB_hits.size()<<" TOB / "<<track->TEC_hits.size()<<" TEC / "<<track->TID_hits.size()<<" TID / "<<nHitsTotal<<" Total"<<endl; //FIXME
	  
	  // TIB
	  if((part==TIB || part==All) && !use_onstrip)
	  {
	    if(track->TIB_hits.size()) FillHistSoN(HistSoN_TIB, track->TIB_hits, thePointNumber, false, commonHistos[0], Monitors_TIB);
	    else if(track->TIB_fullHits.size()) FillHistSoN(HistSoN_TIB, track->TIB_fullHits, thePointNumber, false, commonHistos[0], Monitors_TIB);
      }

	  if((part==TIB || part==All) && use_onstrip!=0)
	  {
		use_onstrip=1;
		FillHistSoN(HistSoN_TIB_onstrip[0], track->TIB_fullHits, thePointNumber, false, commonHistos[0], Monitors_TIB);
		use_onstrip=2;
		FillHistSoN(HistSoN_TIB_onstrip[1], track->TIB_fullHits, thePointNumber, false, commonHistos[0], Monitors_TIB);
		use_onstrip=3;
		FillHistSoN(HistSoN_TIB_onstrip[2], track->TIB_fullHits, thePointNumber, false, commonHistos[0], Monitors_TIB);
		use_onstrip=1;
	  }

      // TOB
	  if(part==TOB || part==All)
	  {
	    if(track->TOB_hits.size()) FillHistSoN(HistSoN_TOB, track->TOB_hits, thePointNumber, true, commonHistos[1], Monitors_TOB);
	    else if(track->TOB_fullHits.size()) FillHistSoN(HistSoN_TOB, track->TOB_fullHits, thePointNumber, true, commonHistos[1], Monitors_TOB);
      }

      // TID
	  if((part==TID || part==All) && !use_onstrip)
	  {
	    if(track->TID_hits.size()) FillHistSoN(HistSoN_TID, track->TID_hits, thePointNumber, false, commonHistos[2], Monitors_TID);
	    else if(track->TID_fullHits.size()) FillHistSoN(HistSoN_TID, track->TID_fullHits, thePointNumber, false, commonHistos[2], Monitors_TID);	  
	  }
	  
	  if((part==TID || part==All) && use_onstrip!=0)
	  {
		use_onstrip=1;
		FillHistSoN(HistSoN_TID_onstrip[0], track->TID_fullHits, thePointNumber, false, commonHistos[2], Monitors_TID);
		use_onstrip=2;
		FillHistSoN(HistSoN_TID_onstrip[1], track->TID_fullHits, thePointNumber, false, commonHistos[2], Monitors_TID);
		use_onstrip=3;
		FillHistSoN(HistSoN_TID_onstrip[2], track->TID_fullHits, thePointNumber, false, commonHistos[2], Monitors_TID);
		use_onstrip=1;
	  }

      // TEC
	  if(part==TEC || part==All) 
	  {
	    if(track->TEC_hits.size()) FillHistSoN(HistSoN_TEC, track->TEC_hits, thePointNumber, false, commonHistos[3], Monitors_TEC);
	    else if(track->TEC_fullHits.size()) FillHistSoN(HistSoN_TEC, track->TEC_fullHits, thePointNumber, false, commonHistos[3], Monitors_TEC);
      }
	  

    } // End of loop over tracks

    
  } // End of loop over events

  

  // Fit of histos with Landau and store results
  std::cout << "Starting to perform fits " << std::endl;
  
  if((part==1 || part==0) && !use_onstrip) FitHistos(HistSoN_TIB, "TIB_output_DecoMode.root", commonHistos[0], Monitors_TIB);
  if((part==1 || part==0) && use_onstrip!=0) {
    FitHistos(HistSoN_TIB_onstrip[0], "TIB_output_DecoMode_onstrip_1.root", commonHistos[0], Monitors_TIB);
    FitHistos(HistSoN_TIB_onstrip[1], "TIB_output_DecoMode_onstrip_2.root", commonHistos[0], Monitors_TIB);
    FitHistos(HistSoN_TIB_onstrip[2], "TIB_output_DecoMode_onstrip_3.root", commonHistos[0], Monitors_TIB);
  }
  
  if((part==2 || part==0)) FitHistos(HistSoN_TOB, "TOB_output_DecoMode.root", commonHistos[1], Monitors_TOB);
  
  if((part==3 || part==0) && !use_onstrip) FitHistos(HistSoN_TID, "TID_output_DecoMode.root", commonHistos[2], Monitors_TID);
  if((part==3 || part==0) && use_onstrip!=0) {
    FitHistos(HistSoN_TID_onstrip[0], "TID_output_DecoMode_onstrip_1.root", commonHistos[2], Monitors_TID);
    FitHistos(HistSoN_TID_onstrip[1], "TID_output_DecoMode_onstrip_2.root", commonHistos[2], Monitors_TID);
    FitHistos(HistSoN_TID_onstrip[2], "TID_output_DecoMode_onstrip_3.root", commonHistos[2], Monitors_TID);
  }
  if((part==4 || part==0)) FitHistos(HistSoN_TEC, "TEC_output_DecoMode.root", commonHistos[3], Monitors_TEC);
  
  

  // If evtid selection used, print corresponding timestamp ranges
  if(!usetimestamp) VSmaker.printComputedSteps();
  std::cout<< "help, DEAR GOD OF PHYSICS "<<VSmaker.t_monitor_start<<"PLS :"<<VSmaker.t_monitor_end<<std::endl;
  if(reclusterize) std::cout<<"N_clus "<<N_clus<<" lost : "<<N_clus_lost<<" split : "<<N_clus_split<<std::endl;

}

//---------------------------------------------------------------------------------------------------------


void SignalAnalysisTreeMaker::FillHitInfo(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, TreeHit *hit, int thePointNumber, bool sensors, std::vector< TH1F* > commonHistos)
{
  FillHitInfo(HistSoN, hit, thePointNumber, sensors, commonHistos, 0, 0, 0);
}

void SignalAnalysisTreeMaker::FillHitInfo(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, TreeFullHit *hit, int thePointNumber, bool sensors, std::vector< TH1F* > commonHistos)
{
  if(reclusterize)
  {
    N_clus++;
	TreeHit newHit;
    vector< vector< unsigned int > > clustersStripsIndex = Cmaker.ReClusterize(hit, 1, 3, 2, 5);
	for(unsigned int icl=0; icl<clustersStripsIndex.size(); icl++)
	if(clustersStripsIndex[icl].size()>0)
	{
	  newHit.detId = hit->detId;
	  newHit.angle = hit->angle;
	  newHit.tsosy = hit->tsosy;
	  
	  for(unsigned int istr=0; istr<clustersStripsIndex[icl].size(); istr++)
	  {
	    unsigned int index = clustersStripsIndex[icl][istr];
	    newHit.chargeAngleCorr += (int) hit->stripCharges[index];
	    newHit.noise +=  pow(hit->stripNoises[index]/hit->stripGains[index],2);
	  }
	  newHit.chargeAngleCorr*=fabs(cos(hit->angleForCorr));
	  newHit.noise = sqrt(newHit.noise);
	  newHit.width = clustersStripsIndex[icl].size();
	  //std::cout<<"newHit  C :"<<newHit.chargeAngleCorr<<" N : "<<newHit.noise<<" w : "<<newHit.width<<endl;

      FillHitInfo(HistSoN, &newHit, thePointNumber, sensors, commonHistos, 0, 0, 0);
	  newHit.detId = 0;
	  newHit.angle = 0;
	  newHit.tsosy = 0;
	  newHit.chargeAngleCorr = 0;
	  newHit.noise = 0;
	  newHit.width = 0;
	}
    if(clustersStripsIndex.size()==0) N_clus_lost++;
    if(clustersStripsIndex.size()>1) N_clus_split++;
  
  }
  else FillHitInfo(HistSoN, dynamic_cast< TreeHit* >(hit), thePointNumber, sensors, commonHistos, hit->barycenter, hit->seed, hit->seedChargeAngleCorr);
}
  
void SignalAnalysisTreeMaker::FillHitInfo(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, TreeHit *hit, int thePointNumber, bool sensors, std::vector< TH1F* > commonHistos, float
barycenter, float seed, float seedChargeAngleCorr)
{
  if(thePointNumber<0) return;

  std::map<ULong64_t , std::vector<TH1F*> >::iterator iter;
  std::vector<TH1F*> detidvector;


  ULong64_t detid = hit->detId;
  
  //FIXME -- test
  //if(detid != 369120677) {return;}

  
//-------------  
  //TOB & TEC modules can have 2 sensors --> distinguish them via y coo.
  //FIXME change sensors range

  int firstsensor = 0;
  
  int subdetector = ((detid>>25)&0x7); //extract subdet info
  if(subdetector==6)
  {
  	int TECgeom = ((detid>>5)&0x7); //extract geometry info
  	
  	if(TECgeom==5) //exclusion zone width = 6cm (wider bc 2 faces --> angle ->...)
  	{
  		if(hit->tsosy > -0.59) {firstsensor = 2;}
  		else if(hit->tsosy < -1.19) {firstsensor = 1;}
  		else {return;} //exclusion zone b/w sensors
  	}  
  	else if(TECgeom==6) //exclusion zone width = 2cm
  	{
  		if(hit->tsosy > -0.46) {firstsensor = 2;}
  		else if(hit->tsosy < -0.66) {firstsensor = 1;}
  		else {return;} //exclusion zone b/w sensors
  	}  
  	else if(TECgeom==7) //exclusion zone width = 2cm
  	{
  		if(hit->tsosy > 0.7) {firstsensor = 2;}
  		else if(hit->tsosy < 0.5) {firstsensor = 1;}
  		else {return;} //exclusion zone b/w sensors
  	}  
  }
  
  if(sensors) //option activated for TOB
  {
  	firstsensor = hit->tsosy<0 ? 1 : 2;
  }
  
  
          
//--------          
  
  if(subdetector == 6 || sensors) detid = detid*10+firstsensor; //TEC & TOB -- 2 sensors
  //NB : for other TEC rings, add a 0 to detid!


  //std::cout << "Detid " << detid << " "<< hit->detId <<" "<<firstsensor<< std::endl;

 
  iter = HistSoN.find(detid);
  if(iter == HistSoN.end() ){
	//std::cout << "Detid " << detIds[i] << " not found yet, creating vector " << std::endl;
	detidvector.clear();
	TString histoname;
	histoname.Form("DetID_%llu",detid);
	if(use_onstrip!=0) histoname.Append(Form("_%i",use_onstrip)); 
        if(VSmaker.getNVoltage()>0) // protection for an exception in CMSSW_74X
	for(int j=0; j< VSmaker.getNVoltage(); j++){
	  std::string s;
	  std::stringstream out;
	  out << j;
	  s = out.str();
	  TString thestr = histoname+"_"+s;
	  detidvector.push_back( new TH1F(thestr.Data(), thestr.Data() , 90, 0, 300)  ); // 30, 0, 100 
	  // 60, 0, 200 // not far enought for TOB -> 90 0 300
//std::cout << thestr << std::endl;
	}

	HistSoN.insert(std::pair<ULong64_t,std::vector<TH1F*> >(detid,detidvector));
    iter = HistSoN.find(detid);
  }

  if( iter == HistSoN.end() ){
	std::cout << "Error could not find Histogram with DetID: " << detid << std::endl;
	return;
  }

  if(commonHistos.size()>2) commonHistos[2]->Fill(hit->tsosx-hit->clusx);
  // before using angle info, check that traj measurement close to cluster position
  if(use_angle && abs(hit->tsosx-hit->clusx)>0.001) 
  {
  	cout<<"hit->tsosx-hit->clusx)>0.001 !! Skip hit"<<endl;
  	return; // a verifier 
  }
  
  // cut choisi pour TIB //FIXME


  bool usehit=false;
  float angle_deg = hit->angle/3.141592*180;
  if(angle_deg>90) angle_deg = angle_deg-180;
  if(angle_deg<-90) angle_deg = angle_deg+180;
  if(commonHistos.size()>1) commonHistos[1]->Fill(angle_deg);
  angle_deg = TMath::Abs( angle_deg );

  if(use_angle==0) usehit=true; // default
  if(use_angle==1 && angle_deg<10) usehit=true;
  if(use_angle==2 && angle_deg>20 && angle_deg<40) usehit=true;
  if(use_angle==3 && angle_deg>40 && angle_deg<60) usehit=true;
  if(use_angle==4 && angle_deg>60 && angle_deg<80) usehit=true;
  if(commonHistos.size()>1) commonHistos[1]->Fill(angle_deg);

  if(use_width==1 && hit->width!=1 && usehit) usehit=false;
  if(use_width==2 && hit->width!=2 && usehit) usehit=false;
  if(use_width==3 && hit->width!=3 && usehit) usehit=false;
  if(use_width==4 && hit->width!=4 && usehit) usehit=false;
  if(use_width==5 && hit->width!=5 && usehit) usehit=false;


  // for TIB
  //float pitch = 1.;
  /*if(nstrips) {
    if(nstrips[i]==512) pitch = 0.012;
    if(nstrips[i]==768) pitch = 0.008;
  }*/
  //float offset = -3.072;
  //float cluspos = (clusx[i] - offset)/pitch -0.5; // prendre clusx ou tsosx ? 
  float cluspos = 0;
  if(barycenter) cluspos = barycenter -0.5; // ou barycenter
  cluspos -= floor(cluspos); if(cluspos>0.5) cluspos-=1;
  //if(seed) cluspos-=seed;

  if(commonHistos.size()>3 && seed && usehit) commonHistos[3]->Fill(cluspos);

  int clus_onStrip = 0;
  if(abs(cluspos)<0.125) clus_onStrip = 1;
  if(abs(cluspos)>0.125 && abs(cluspos)<0.375) clus_onStrip = (cluspos>0) ? 2 : -2;
  if(abs(cluspos)>0.375 && abs(cluspos)<0.625) clus_onStrip = (cluspos>0) ? 3 : -3;
  if(abs(cluspos)>0.625 && abs(cluspos)<0.875) clus_onStrip = (cluspos>0) ? 4 : -4;


  if(use_onstrip==1 && clus_onStrip!=1 && usehit) usehit=false;
  if(use_onstrip==2 && abs(clus_onStrip)!=2 && usehit) usehit=false;
  if(use_onstrip==3 && abs(clus_onStrip)!=3 && usehit) usehit=false;
  if(use_onstrip==4 && clus_onStrip!=4 && usehit) usehit=false;
  if(use_onstrip==-4 && clus_onStrip!=-4 && usehit) usehit=false;


  //FIXME
  //cout<<"**Good Hit"<<endl;
  //counter++;
  //cout<<"counter = "<<counter<<endl;

  if(hit->chargeAngleCorr > 0 && usehit) {iter->second.at(thePointNumber)->Fill(hit->chargeAngleCorr);}
  //if(seedChargeAngleCorr > 0 && usehit) {iter->second.at(thePointNumber)->Fill(seedChargeAngleCorr);} //--- USE SEED CHARGE INSTEAD OF TOTAL CHARGE	 !!
  
  
  //cout<<"----"<<endl;
  //cout<<"-- hit->chargeAngleCorr = "<<hit->chargeAngleCorr<<endl;
  //cout<<"-- seedChargeAngleCorr = "<<seedChargeAngleCorr<<endl;


}


//Hits
void SignalAnalysisTreeMaker::FillHistSoN(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, std::vector< TreeHit > &Hits, 
 int thePointNumber, bool sensors, std::vector< TH1F* > commonHistos, std::map<ULong64_t, TProfile* > Monitors)
{

  std::map<ULong64_t, TProfile* >::iterator itMon;
  for(unsigned int i=0; i<Hits.size(); i++)
  {
    FillHitInfo(HistSoN, &(Hits[i]), thePointNumber, sensors, commonHistos);
	for(itMon=Monitors.begin(); itMon!=Monitors.end(); itMon++)
	  if(itMon->first==Hits[i].detId) itMon->second->Fill(event->ev_timestamp - t_monitor_start, Hits[i].chargeAngleCorr);
  }

}


//FullHits
void SignalAnalysisTreeMaker::FillHistSoN(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, std::vector< TreeFullHit > &Hits, 
 int thePointNumber, bool sensors, std::vector< TH1F* > commonHistos, std::map<ULong64_t, TProfile* > Monitors)
{

  std::map<ULong64_t, TProfile* >::iterator itMon;
  for(unsigned int i=0; i<Hits.size(); i++)
  {
    FillHitInfo(HistSoN, &(Hits[i]), thePointNumber, sensors, commonHistos);
	for(itMon=Monitors.begin(); itMon!=Monitors.end(); itMon++)
	  if(itMon->first==Hits[i].detId) itMon->second->Fill(event->ev_timestamp - t_monitor_start, Hits[i].chargeAngleCorr);
  }

}

//---------------------------------------------------------------------------------------------------------


void SignalAnalysisTreeMaker::FitHistos(std::map<ULong64_t , std::vector<TH1F*> > &HistSoN, string output_file, 
 std::vector< TH1F* > commonHistos, std::map<ULong64_t, TProfile* > Monitors){

  TFile * myFile = new TFile(output_file.c_str(), "recreate");

  ULong64_t detid;
  double voltage;
  double errvoltage;
  double MPV;
  double errMPV;
  double Width;
  double errWidth;
  int index;
  double chi2overndf;
  int nhits;
  double pedestal_fraction;
  TTree *tree = new TTree("T", "summary information");

  tree->Branch("DetID",&detid, "DetID/l");
  tree->Branch("Voltage",&voltage,"Voltage/D");
  tree->Branch("Index",&index,"Index/I");
  tree->Branch("errVoltage",&errvoltage,"errVoltage/D");
  tree->Branch("MPV",&MPV,"MPV/D");
  tree->Branch("errMPV",&errMPV,"errMPV/D");
  tree->Branch("Width",&Width,"Width/D");
  tree->Branch("errWidth",&errWidth,"errWidth/D");
  tree->Branch("Chi2OverNdf",&chi2overndf,"Chi2OverNdf/D");
  tree->Branch("Nhits",&nhits,"Nhits/I");
  tree->Branch("PedFraction",&pedestal_fraction,"PedFraction/D");


  //TCanvas* c1 = new TCanvas();
  TH1F* hNhits = new TH1F("hNhits", "hNhits", 1000, 0,1000); // N hits per module
  TH1F* hChi2OverNDF = new TH1F("hChi2OverNDF", "hChi2OverNDF", 100, 0,100);
  TH2F* hChi2OverNDFvsstep = new TH2F("hChi2OverNDFvsstep", "hChi2OverNDFvsstep", 20, 0, 20, 100, 0,100);
  
  unsigned int nbadfits=0;
  unsigned int nnegpar0=0;
  unsigned int nnegpar1=0;
  unsigned int nfitrm=0;

  for(std::map<ULong64_t , std::vector<TH1F*> >::iterator iter = HistSoN.begin(); iter != HistSoN.end(); ++iter)
  {
    
	unsigned int i=0; // voltage index    
    std::set< int >::iterator itVolt;
	std::set< int > Voltage = VSmaker.getVoltageList();
    for( itVolt=Voltage.begin(); itVolt!=Voltage.end(); itVolt++)
    {
      
      //std::cout<<"going through the measurement: " << i << std::endl;
      
      TF1 *fitFunc = new TF1("fitFunc","landau", 0, 200);
      fitFunc->SetLineColor(1);
      
      TString thestring;
      thestring.Form("DetID_%llu_%u",iter->first,i);
 	  
	  
      //std::cout << "searching for " << thestring.Data() << std::endl;
      //TH1F*  SoNHisto= (TH1F*)gROOT->FindObject( thestring.Data() );
	  
	  if(i>=iter->second.size()) 
       { std::cout<<" Wrong number of voltage steps. "<<std::endl; i++; continue;}
 	  TH1F*  SoNHisto = iter->second[i];
	  
	  if(!SoNHisto) 
       { std::cout<<" Histo "<<thestring.Data()<<"_"<<i<<" not found."<<std::endl; i++; continue;}
 
      if(SoNHisto->GetEntries()) hNhits->Fill(SoNHisto->Integral());
      
	  
	  if(SoNHisto->Integral()<20) //0.1 //20 by default
	   { std::cout<<" Not enough entries for histo "<<thestring.Data()<<std::endl;
	     i++; continue;
	   }
 
	  //fitFunc->SetParameter(0, SoNHisto->Integral("w")); //JLA
	  //fitFunc->SetParameter(1, 10); //JLA
	  //fitFunc->SetParLimits(0, 0, 1e+10);
	  //fitFunc->SetParLimits(1, 0, 100);
	  Int_t fitStatus = SoNHisto->Fit("fitFunc","q");
	  Float_t chi2overndf_temp = 999;
	  if(fitFunc->GetNDF()>0) chi2overndf_temp = fitFunc->GetChisquare()/fitFunc->GetNDF();
	  
	  if( fitStatus!=0 || fitFunc->GetParameter(0)<0 || fitFunc->GetParameter(1)<0 ||
	   fitFunc->GetParameter(1)>200 || chi2overndf_temp > 100 )
	  {
		//fitFunc->SetParameter(0, SoNHisto->Integral());
		//fitFunc->SetParLimits(0, 0, 1e+10);
    	//fitFunc->SetParLimits(1, 0, 100);
		//fitFunc->SetParameter(1, SoNHisto->GetMean()); 
		fitFunc->SetRange(0, SoNHisto->GetMean()+3*SoNHisto->GetRMS());
		fitStatus = SoNHisto->Fit("fitFunc","rq");
	  }

	  chi2overndf = 999;
	  if(fitFunc->GetNDF()>0) chi2overndf = fitFunc->GetChisquare()/fitFunc->GetNDF();
	  hChi2OverNDF->Fill(chi2overndf);
	  hChi2OverNDFvsstep->Fill( i, chi2overndf);
	  detid = iter->first;
	  

	  bool rmfit=false;
	  if(fitStatus!=0 || fitFunc->GetParameter(0)<0 || fitFunc->GetParameter(1)<0 ||
	   fitFunc->GetParameter(1)>200 || chi2overndf > 100) rmfit=true;
	   

      //if(MPV<0 || (MPV>25 && k<15) || errMPV>1 || detid==369169740)
      //if(errMPV>5)
      /*if(detid==369121605 || detid==369121606 || detid==369121614 || detid==369121613 ||
      detid==369121610 || detid==369121609 || detid==369121437 || detid==369142077 ||
      detid==369121722 || detid==369125534 || detid==369137018 || detid==369121689 ||
      detid==369121765 || detid==369137045 || detid==369169740)*/
      if( rmfit || 
      // TIB modules
          // TIB - 1.4.2.5
      detid==369121605 || detid==369121606 || detid==369121614 || 
      detid==369121613 || detid==369121610 || detid==369121609 ||
          // TIB - 1.2.2.1
      detid==369121390 || detid==369121382 || detid==369121386 || 
      detid==369121385 || detid==369121389 || detid==369121381 ||
          // TIB + 1.6.2.5
      detid==369125870 || detid==369125862 || detid==369125866 ||
      detid==369125869 || detid==369125865 || detid==369125861 ||
          // others in TIB  
      detid==369121437 || detid==369142077 || detid==369121722 || 
      detid==369125534 || detid==369137018 || detid==369121689 ||
      detid==369121765 || detid==369137045 || detid==369169740 ||
      detid==369121689 || detid==369141941 || detid==369141946 ||
      detid==369141949 || detid==369141942 || detid==369141945 ||
      detid==369141950 ||
      // TOB modules 
	      // TOB + 4.3.3.8
      detid/10==436281512 || detid/10==436281528 || detid/10==436281508 ||
      detid/10==436281524 || detid/10==436281520 || detid/10==436281516 ||

      		// TOB + 1.3.1.6 //NEW
      detid/10==436232901 || detid/10==436232902 || detid/10==436232905 ||
      detid/10==436232906 || detid/10==436232909 || detid/10==436232910 ||
      detid/10==436232913 || detid/10==436232914 || detid/10==436232917 ||
      detid/10==436232918 || detid/10==436232921 || detid/10==436232922 ||

          // others in TOB  
      detid/10==436228249 || detid/10==436232694 || detid/10==436228805 ||
      detid/10==436244722 || detid/10==436245110 || detid/10==436249546 ||
      detid/10==436310808 || detid/10==436312136 || detid/10==436315600 ||
	      // without 'sensors' option 
      detid==436281512 || detid==436281528 || detid==436281508 ||
      detid==436281524 || detid==436281520 || detid==436281516 ||
      detid==436228249 || detid==436232694 || detid==436228805 ||
      detid==436244722 || detid==436245110 || detid==436249546 ||
      detid==436310808 || detid==436312136 || detid==436315600 || 
      // TID modules
      detid==402664070 || detid==402664110 ||
	  // TEC modules in small scans
      detid==470148196 || detid==470148200 || detid==470148204 ||
      detid==470148228 || detid==470148232 || detid==470148236 ||
      detid==470148240 || detid==470148261 || detid==470148262 ||
	  detid==470148265 || detid==470148266 || detid==470148292 ||
	  detid==470148296 || detid==470148300 || detid==470148304 ||
	  detid==470148324 || detid==470148328 || detid==470148332 ||
	  detid==470148336 || detid==470148340 )  { 
	    SoNHisto->Write();
        std::cout << " Saving histo : " << thestring.Data() << std::endl;
	    // if(fitStatus!=0) std::cout << " fit status : " << fitStatus << std::endl;
      }  


	  if(fitStatus!=0) nbadfits++;
	  if(fitFunc->GetParameter(0)<0) nnegpar0++;
	  if(fitFunc->GetParameter(1)<0) nnegpar1++;

	  if(rmfit) {nfitrm++; i++; continue;}

          int subdet = ((detid>>25)&0x7);
          int TECgeom=0;

          if(subdet==6) TECgeom = ((detid>>5)&0x7); //TEC -> extract geometry (ring 5, ring6, ...)
    TRandom3* Land = new TRandom3(0);
    Land->SetSeed(0);
      // save values
	  detid = iter->first;
	  voltage  = *itVolt;
	  index = i;
	  errvoltage = 2 ;
	  MPV = fitFunc->GetParameter(1);
    // MPV = fitFunc->GetMaximumX();//low stat
	  errMPV = fitFunc->GetParError(1);
    // errMPV = Land->Landau(4.247,0.5);//low stat
	  Width = fitFunc->GetParameter(2);
    //  Width = SoNHisto->GetStdDev();//low stat
	  errWidth = fitFunc->GetParError(2);
    	  // errWidth = SoNHisto->GetStdDevError();//low stat
	  // chi2overndf already set
	  nhits = (int) SoNHisto->Integral();
      if(subdet==3 || subdet==4 || (subdet==6 && TECgeom<5))
         pedestal_fraction = SoNHisto->Integral(0, 16)/SoNHisto->Integral(17, 91); // fraction of noise below 53.3 ADC counts for thin sensors
      else 
         pedestal_fraction = SoNHisto->Integral(0, 27)/SoNHisto->Integral(28, 91); // fraction of noise below 90 ADC counts for thick sensors
	  tree->Fill();
	    
	  i++;

    }   //iter voltage

  } //iter histos
  
  tree->Write();
  hNhits->Write();
  hChi2OverNDF->Write();
  hChi2OverNDFvsstep->Write();
  
  std::cout<<" N fits wo convergence : "<<nbadfits<<std::endl;
  std::cout<<" N fits with par0<0 : "<<nnegpar0<<std::endl;
  std::cout<<" N fits with par1<0 : "<<nnegpar1<<std::endl;
  std::cout<<" N fit results removed : "<<nfitrm<<std::endl;


  for(unsigned int ih=0; ih<commonHistos.size(); ih++) commonHistos[ih]->Write();

  std::map<ULong64_t, TProfile* >::iterator itMon;
  for(itMon=Monitors.begin(); itMon!=Monitors.end(); itMon++)
  {
    itMon->second->GetXaxis()->SetTimeDisplay(1);
	itMon->second->GetXaxis()->SetTimeFormat("%H:%M");
	itMon->second->GetXaxis()->SetTimeOffset(t_monitor_start);
	itMon->second->Write();
  }
  
  //// If you want to store all the individual detId histograms uncomments this line !!!!
  //myFile->Write();
  myFile->Close();

}
