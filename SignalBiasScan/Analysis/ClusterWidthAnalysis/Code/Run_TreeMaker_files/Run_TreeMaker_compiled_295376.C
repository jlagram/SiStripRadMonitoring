#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias1_v1/ZeroBias1/crab_HVscan_20170527_run295376_ZeroBias1_v1/180413_122404/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias2_v1/ZeroBias2/crab_HVscan_20170527_run295376_ZeroBias2_v1/180413_133157/0000/clustersTree*.root/demo/ttree"); //NOT COMPLETED
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias3_v2/ZeroBias3/crab_HVscan_20170527_run295376_ZeroBias3_v2/180413_121925/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias4_v1/ZeroBias4/crab_HVscan_20170527_run295376_ZeroBias4_v1/180413_133336/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias5_v1/ZeroBias5/crab_HVscan_20170527_run295376_ZeroBias5_v1/180413_153322/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias6_v1/ZeroBias6/crab_HVscan_20170527_run295376_ZeroBias6_v1/180413_134348/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias7_v1/ZeroBias7/crab_HVscan_20170527_run295376_ZeroBias7_v1/180413_134833/0000/clustersTree*.root/demo/ttree"); //NOT COMPLETED
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias8_v1/ZeroBias8/crab_HVscan_20170527_run295376_ZeroBias8_v1/180413_135910/0000/clustersTree*.root/demo/ttree");
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias9_v1/ZeroBias9/crab_HVscan_20170527_run295376_ZeroBias9_v1/180413_140600/0000/clustersTree*.root/demo/ttree"); //NOT COMPLETED
  //c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20170527_run295376_ZeroBias10_v1/ZeroBias10/crab_HVscan_20170527_run295376_ZeroBias10_v1/180413_133135/0000/clustersTree*.root/demo/ttree");
  
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20170527_run295376.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

