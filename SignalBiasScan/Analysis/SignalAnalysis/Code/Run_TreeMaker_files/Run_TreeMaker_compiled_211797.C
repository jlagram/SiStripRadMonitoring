#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_1_1_jfz.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_2_1_0Dn.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_3_2_PYS.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_4_1_NK1.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_5_1_OyJ.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_6_1_8YF.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_7_1_l40.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_8_2_vvW.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_9_1_xFz.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_10_1_Gin.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_11_2_abB.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_12_1_i1K.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20130213_run211797_v1_2/clustersTree_13_2_AYT.root/demo/ttree");


  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=false; // true if input file uses timestamps
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20130213_run211797.txt", subdet, usetimestamp);
  t->Loop();

}

