#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_1_1_mtG.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_2_1_Pzh.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_3_1_emQ.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_4_1_VM0.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_5_1_eH2.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_6_1_QE4.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_7_1_shh.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_8_1_lgi.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_9_1_VHO.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_10_1_82f.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_11_1_zZB.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_12_1_g4d.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_13_1_afz.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_14_1_pqZ.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_15_1_coN.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_16_1_tjk.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_17_1_fVx.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_18_1_SQi.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_19_1_KGb.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20121130_run208339_v1_2/clustersTree_20_1_l0V.root/demo/ttree");


  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=false; // true if input file uses timestamps
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20121130_run208339_merged_steps.txt", subdet, usetimestamp);
  t->Loop();

}

