#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");

  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias1_v2/ZeroBias1/crab_HVscan_20180923_run323374_ZeroBias1_v1/181003_145314/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias2_v2/ZeroBias2/crab_HVscan_20180923_run323374_ZeroBias2_v1/181003_145711/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias3_v2/ZeroBias3/crab_HVscan_20180923_run323374_ZeroBias3_v1/181003_150453/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias4_v2/ZeroBias4/crab_HVscan_20180923_run323374_ZeroBias4_v1/181003_150813/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias5_v2/ZeroBias5/crab_HVscan_20180923_run323374_ZeroBias5_v1/181003_150913/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias6_v2/ZeroBias6/crab_HVscan_20180923_run323374_ZeroBias6_v1/181003_151129/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias7_v2/ZeroBias7/crab_HVscan_20180923_run323374_ZeroBias7_v1/181003_151513/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180923_run323374_ZeroBias8_v2/ZeroBias8/crab_HVscan_20180923_run323374_ZeroBias8_v1/181003_152440/0000/clustersTree*.root/demo/ttree");

  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20180923_run323374.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

