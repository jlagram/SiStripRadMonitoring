#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  //EGamma dataset
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_EGamma_v1/EGamma/crab_HVscan_20180801_run320674_EGamma_noDoubleCounting_v1/180918_092604/0000/clustersTree*.root/demo/ttree");
  
  //SingleMuon dataset -- no double counting
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_SingleMuon_v1/SingleMuon/crab_HVscan_20180801_run320674_SingleMuon_noDoubleCounting_v1/180918_091048/0000/clustersTree*.root/demo/ttree");
  
  //ZeroBias dataset -- no double counting -- error for now
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180801_run320674_v1/ZeroBias/crab_HVscan_20180801_run320674_v1/180808_100833/0000/clustersTree*.root/demo/ttree");
  
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20180801_run320674.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

