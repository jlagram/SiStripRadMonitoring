#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
   TChain c("ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_1_1_1Bc.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_2_1_YPk.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_3_1_LTT.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_4_1_qN6.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_5_1_r6H.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_6_1_siL.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_7_1_rli.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_8_1_16n.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_9_1_3gw.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_10_1_8A3.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_11_1_k4q.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_12_1_H7Z.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_13_1_s3y.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_14_1_TZu.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_15_1_R6N.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_16_1_k4w.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_17_1_CrM.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_18_1_wvS.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_19_1_efH.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_20_1_B50.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_21_1_vPj.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_22_1_OAQ.root/demo/ttree");
  
  int subdet=4; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20150603_run246963.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

