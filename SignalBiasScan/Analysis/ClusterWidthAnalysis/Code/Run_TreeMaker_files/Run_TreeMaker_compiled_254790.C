#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
  //TChain c("demo/ttree");
  
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_1_1_UA9.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_2_1_z6L.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_3_1_Sm8.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_4_1_9Mv.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_5_1_YGl.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_6_1_Sw3.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_7_1_ghm.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_8_2_JHp.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_9_1_cE1.root/demo/ttree");
	c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150821_run254790_v1_3/clustersTree_10_1_I3x.root/demo/ttree");
  
  int subdet=4; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20150821_run254790.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

