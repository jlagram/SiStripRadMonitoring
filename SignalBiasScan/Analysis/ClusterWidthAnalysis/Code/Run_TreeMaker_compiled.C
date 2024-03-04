#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias0_v1/EphemeralZeroBias0/crab_HVscan_20230407_run365843_EphemeralZeroBias0_v1/230427_113822/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias1_v1/EphemeralZeroBias1/crab_HVscan_20230407_run365843_EphemeralZeroBias1_v1/230427_114804/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias2_v1/EphemeralZeroBias2/crab_HVscan_20230407_run365843_EphemeralZeroBias2_v1/230427_114820/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias3_v1/EphemeralZeroBias3/crab_HVscan_20230407_run365843_EphemeralZeroBias3_v1/230427_114836/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias4_v1/EphemeralZeroBias4/crab_HVscan_20230407_run365843_EphemeralZeroBias4_v1/230427_114854/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias5_v1/EphemeralZeroBias5/crab_HVscan_20230407_run365843_EphemeralZeroBias5_v1/230427_114928/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias6_v1/EphemeralZeroBias6/crab_HVscan_20230407_run365843_EphemeralZeroBias6_v1/230427_114938/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias7_v1/EphemeralZeroBias7/crab_HVscan_20230407_run365843_EphemeralZeroBias7_v1/230427_114951/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias8_v1/EphemeralZeroBias8/crab_HVscan_20230407_run365843_EphemeralZeroBias8_v1/230427_115012/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias9_v1/EphemeralZeroBias9/crab_HVscan_20230407_run365843_EphemeralZeroBias9_v1/230427_115023/0000/clustersTree*.root/demo/ttree");
  //---------------------------------------10--------------------------------------------------//
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias10_v1/EphemeralZeroBias10/crab_HVscan_20230407_run365843_EphemeralZeroBias10_v1/230427_115038/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias11_v1/EphemeralZeroBias11/crab_HVscan_20230407_run365843_EphemeralZeroBias11_v1/230427_115226/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias12_v1/EphemeralZeroBias12/crab_HVscan_20230407_run365843_EphemeralZeroBias12_v1/230427_115242/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias13_v1/EphemeralZeroBias13/crab_HVscan_20230407_run365843_EphemeralZeroBias13_v1/230427_115255/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias14_v1/EphemeralZeroBias14/crab_HVscan_20230407_run365843_EphemeralZeroBias14_v1/230427_115307/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias15_v1/EphemeralZeroBias15/crab_HVscan_20230407_run365843_EphemeralZeroBias15_v1/230427_115318/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias16_v1/EphemeralZeroBias16/crab_HVscan_20230407_run365843_EphemeralZeroBias16_v1/230427_115330/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias17_v1/EphemeralZeroBias17/crab_HVscan_20230407_run365843_EphemeralZeroBias17_v1/230427_115341/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias18_v1/EphemeralZeroBias18/crab_HVscan_20230407_run365843_EphemeralZeroBias18_v1/230427_115351/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVscan_20230407_run365843_EphemeralZeroBias19_v1/EphemeralZeroBias19/crab_HVscan_20230407_run365843_EphemeralZeroBias19_v1/230427_115400/0000/clustersTree*.root/demo/ttree");
 //---------------------------------------20--------------------------------------------------//

  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC 
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20230407_run365843.txt", subdet, usetimestamp, angle, width, onstrip);

  t->Loop();

}

