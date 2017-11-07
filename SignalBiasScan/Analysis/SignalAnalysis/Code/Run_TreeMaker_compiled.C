#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  

  //TChain c("ttree");
  TChain c("demo/ttree");
  
  
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias1/crab_HVscan_20170924_run303824_v1/170928_130801/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias2/crab_HVscan_20170924_run303824_dataset2_v1/171002_111156/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias3/crab_HVscan_20170924_run303824_dataset3_v1/171002_111212/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias4/crab_HVscan_20170924_run303824_datase4_v1/171002_162608/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias5/crab_HVscan_20170924_run303824_dataset5_v1/171002_162620/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias6/crab_HVscan_20170924_run303824_dataset6_v1/171002_162632/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias7/crab_HVscan_20170924_run303824_dataset7_v1/171002_162644/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170924_run303824_v1/ZeroBias8/crab_HVscan_20170924_run303824_dataset8_v1/171002_162655/0000/clustersTree*.root/demo/ttree");
  
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC

  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip

  
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20170924_run303824.txt", subdet, usetimestamp, angle, width, onstrip);

  t->Loop();

}

