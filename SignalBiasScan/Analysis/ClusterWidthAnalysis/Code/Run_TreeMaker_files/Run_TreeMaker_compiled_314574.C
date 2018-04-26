#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias1_v2/ZeroBias1/crab_HVscan_20180418_run314574_ZeroBias1_v2/180425_171600/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias2_v2/ZeroBias2/crab_HVscan_20180418_run314574_ZeroBias2_v2/180425_172209/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias3_v2/ZeroBias3/crab_HVscan_20180418_run314574_ZeroBias3_v2/180425_172318/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias4_v2/ZeroBias4/crab_HVscan_20180418_run314574_ZeroBias4_v2/180425_172349/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias5_v2/ZeroBias5/crab_HVscan_20180418_run314574_ZeroBias5_v2/180425_172411/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180418_run314574_ZeroBias6_v2/ZeroBias6/crab_HVscan_20180418_run314574_ZeroBias6_v2/180425_172432/0000/clustersTree*.root/demo/ttree");
  
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20180418_run314574.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

