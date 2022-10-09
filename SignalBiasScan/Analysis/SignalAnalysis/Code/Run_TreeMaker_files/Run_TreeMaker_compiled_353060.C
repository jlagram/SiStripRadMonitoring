#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias1_v1/ZeroBias1/crab_HVscan_20220605_run353060_ZeroBias1_v1/220701_103148/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias2_v1/ZeroBias2/crab_HVscan_20220605_run353060_ZeroBias2_v1/220701_120527/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias3_v1/ZeroBias3/crab_HVscan_20220605_run353060_ZeroBias3_v1/220701_120545/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias4_v1/ZeroBias4/crab_HVscan_20220605_run353060_ZeroBias4_v1/220701_120602/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias5_v1/ZeroBias5/crab_HVscan_20220605_run353060_ZeroBias5_v1/220701_120620/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias6_v1/ZeroBias6/crab_HVscan_20220605_run353060_ZeroBias6_v1/220701_152020/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias7_v1/ZeroBias7/crab_HVscan_20220605_run353060_ZeroBias7_v1/220701_140529/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias8_v1/ZeroBias8/crab_HVscan_20220605_run353060_ZeroBias8_v1/220701_153852/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias9_v1/ZeroBias9/crab_HVscan_20220605_run353060_ZeroBias9_v1/220701_140733/0000/clustersTree*.root/demo/ttree");

  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias10_v1/ZeroBias10/crab_HVscan_20220605_run353060_ZeroBias10_v1/220703_154600/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias11_v1/ZeroBias11/crab_HVscan_20220605_run353060_ZeroBias11_v1/220703_171344/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias12_v1/ZeroBias12/crab_HVscan_20220605_run353060_ZeroBias12_v1/220703_171352/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias13_v1/ZeroBias13/crab_HVscan_20220605_run353060_ZeroBias13_v1/220703_171401/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias14_v1/ZeroBias14/crab_HVscan_20220605_run353060_ZeroBias14_v1/220703_171409/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias15_v1/ZeroBias15/crab_HVscan_20220605_run353060_ZeroBias15_v1/220704_070955/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias16_v1/ZeroBias16/crab_HVscan_20220605_run353060_ZeroBias16_v1/220703_171427/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias17_v1/ZeroBias17/crab_HVscan_20220605_run353060_ZeroBias17_v1/220704_071006/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias18_v1/ZeroBias18/crab_HVscan_20220605_run353060_ZeroBias18_v1/220703_171442/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/HVscan_20220605_run353060_ZeroBias19_v1/ZeroBias19/crab_HVscan_20220605_run353060_ZeroBias19_v1/220703_171449/0000/clustersTree*.root/demo/ttree");


  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC 
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20220605_run353060.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

