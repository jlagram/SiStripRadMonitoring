#include "DCUCurrent.h"
using namespace std;
TGraph* ReadPSCurrentTxt(std::string filename="Data/PS_I_TIB_L1_20120405_run190459.txt", int detid=369121605, std::string bad_periods="")
{
  
  // Read bad periods
  std::vector< int > bad_periods_start;
  std::vector< int > bad_periods_end;
  if(bad_periods!="")
  {
    ReadBadPeriods(bad_periods, bad_periods_start, bad_periods_end);
	if(bad_periods_start.size() != bad_periods_end.size())
	{
	  std::cerr<<"Wrong definition of bad periods : size of starts != size of ends. Will not use them."<<endl;
	  bad_periods="";
	}
  }

  std::string line;
  std::ifstream fin(filename);
  std::string PS;
  std::string clob;
  std::string modids;
  int modid[8];
  std::string str_date;
  std::string str_time;
  std::string str_am_pm="";
  Int_t time=-1;
  float current=-1;

  TGraph *g = new TGraph();
  g->SetName("PS_Current");

  int i=0;
  if(fin.is_open())  {
    while( getline ( fin, line) )
        {
          if(fin.eof()) continue;
          current=-1;
          std::stringstream ss(line);
          ss >> PS >> clob >> modids >> str_date >> str_time >> current;
          // some format without (CLOB)
          if(current==-1)
          { 
            std::stringstream ss(line);
            ss >> PS >> modids >> str_date >> str_time >> current;
          }
          // some format without (CLOB) and with AM/PM for time
          if(str_time=="AM" || str_time=="PM")
          {
            std::stringstream ss(line);
            ss >> PS >> modids >> str_date >> str_time >> str_am_pm >> current;
          } 

          //cout<<PS<<"; "<<modids<<"; "<<str_date<<"; "<<str_time<<"; "<<str_am_pm<<"; "<<current<<endl;
          time = convertDate( str_date , str_time);
          if(str_am_pm=="PM") time += 12*3600;

          // Remove points during bad periods
          if(bad_periods!="")
		  {
		     for(unsigned int ip=0; ip<bad_periods_start.size(); ip++)
			   if(time>=bad_periods_start[ip] && time<=bad_periods_end[ip]) continue;
		  }

          //cout<<modids<<endl;
          bool detid_found=false;
          std::stringstream ss2;
          int nid = modids.size()/10+1;
          if(nid>8) 
          {
            std::cout<<"Warning : array limited to 8 modules/channel"<<std::endl;
            nid=8;
          }
          for(int idet=0; idet<nid; idet++)
          {
            ss2.clear(); ss2 << modids.substr(10*idet, 9); ss2 >> modid[idet];
            //cout<<modid[idet]<<" ";
            if( detid == modid[idet] ) detid_found=true;
          }
          //cout<<endl;

          if(detid_found) 
          {
            std::cout<<str_date<<" "<<str_time<<" "<<current<<std::endl;
            std::cout<<time<<std::endl;
            g->SetPoint(i, time, current);
            i++;
          }
        }
    fin.close();
  }

  TH1F* h = g->GetHistogram();
  h->GetXaxis()->SetTimeDisplay(1);
  h->GetXaxis()->SetTimeFormat("%H:%M");
  
  return g;

}

void ConvertPSCurrentTxtToRoot(std::string filename="Data/PS_I_TIB_L1_20120405_run190459.txt")
{
  std::cout<<"Converting file "<<filename<<" to root format."<<std::endl;
  
  // Create output file
  TString file(filename.c_str());
  int dot=file.Index(".txt");
  file.Replace(dot,4,".root");
  TFile fout(file.Data(), "recreate");

  // Create output tree
  char ps[50];
  int detid=-1;
  Long64_t time=0;
  float current=-1;
  int nmod=1;
  TTree* tree = new TTree("ps", "ps");
  tree->Branch("PS", &ps, "PS/C");
  tree->Branch("DETID", &detid, "DETID/I");
  tree->Branch("TIME", &time, "TIME/L");
  tree->Branch("CURRENT", &current, "CURRENT/F");
  tree->Branch("NMODFORCHANNEL", &nmod, "NMODFORCHANNEL/I");

  // Read input file
  std::string line;
  std::ifstream fin(filename);
  std::string str_ps;
  std::string str_clob;
  std::string str_modids;
  int modid[8];
  std::string str_date;
  std::string str_time;
  std::string str_am_pm="";
  std::string prev_str_ps;

  if(fin.is_open())  {
    while( getline ( fin, line) )
        {
          if(fin.eof()) continue;
          std::stringstream ss(line);
          current=-1;
          ss >> str_ps >> str_clob >> str_modids >> str_date >> str_time >> current;
          if(current==-1)
          {
            std::stringstream ss(line);
            ss >> str_ps >> str_modids >> str_date >> str_time >> current;
          }

          // some format without (CLOB) and with AM/PM for time
          if(str_time=="AM" || str_time=="PM")
          {
            std::stringstream ss(line);
            ss >> str_ps >> str_modids >> str_date >> str_time >> str_am_pm >> current;
          }
          
          sprintf(ps, "%s", str_ps.c_str());
          time = convertDate( str_date , str_time);
          if(str_am_pm=="PM") time += 12*3600;
          if(str_am_pm=="AM" && str_time.substr(0,3)=="12.") time -= 12*3600; // bug in txt files for run 200786


          //cout<<modids<<endl;
          bool detid_found=false;
          std::stringstream ss2;
          nmod = str_modids.size()/10+1;
          if(nmod>8)
          {
            std::cout<<"Warning : array limited to 8 modules/channel"<<std::endl;
            nmod=8;
          }
          for(int idet=0; idet<nmod; idet++)
          {
            ss2.clear(); ss2 << str_modids.substr(10*idet, 9); ss2 >> modid[idet];
            //cout<<modid[idet]<<" ";
            detid=modid[idet];
            tree->Fill();
          }
          //cout<<endl;


          if(str_ps!=prev_str_ps) std::cout<<str_ps<<"   "<<str_modids<<std::endl;
          
          prev_str_ps=str_ps;
        }
    fin.close();
  }

  // Save tree
  tree->Write();
  fout.Close();
  
}

TGraph* ReadPSCurrentRoot(std::string filename, int modid, int &nmodforchannel, 
// std::string filename="Data/PS_I_TIB_L1_20120405_run190459.root", int modid=369121605
 std::string bad_periods="", bool print=false)
{
  
  TGraph* g = ReadCurrentRoot(filename, modid, nmodforchannel, "ps", bad_periods, print); // Same tree format for DCU and PS currents
  
  g->SetMarkerStyle(23);
  g->SetMarkerColor(8);
  g->SetLineColor(8);
  
  return g;
  
}
