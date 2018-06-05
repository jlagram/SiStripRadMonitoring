#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias1_v1/ZeroBias1/crab_HVscan_20180420_run314755_ZeroBias1_v1/180426_103850/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias2_v1/ZeroBias2/crab_HVscan_20180420_run314755_ZeroBias2_v1/180424_185422/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias3_v1/ZeroBias3/crab_HVscan_20180420_run314755_ZeroBias3_v1/180424_185717/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias4_v1/ZeroBias4/crab_HVscan_20180420_run314755_ZeroBias4_v1/180424_190314/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias5_v1/ZeroBias5/crab_HVscan_20180420_run314755_ZeroBias5_v1/180424_190349/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20180420_run314755_ZeroBias6_v1/ZeroBias6/crab_HVscan_20180420_run314755_ZeroBias6_v1/180426_103757/0000/clustersTree*.root/demo/ttree");
  
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20180420_run314755.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

