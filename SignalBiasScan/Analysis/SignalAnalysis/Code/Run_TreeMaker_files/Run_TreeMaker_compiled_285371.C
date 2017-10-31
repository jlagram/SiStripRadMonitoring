#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias1/crab_HVscan_20161116_run285371_v1_PAMinimumBias1/170914_160307/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias2/crab_HVscan_20161116_run285371_v1_PAMinimumBias2/170914_160333/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias3/crab_HVscan_20161116_run285371_v1_PAMinimumBias3/170914_160352/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias4/crab_HVscan_20161116_run285371_v1_PAMinimumBias4/170920_105433/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias5/crab_HVscan_20161116_run285371_v1_PAMinimumBias5/170920_105451/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias6/crab_HVscan_20161116_run285371_v1_PAMinimumBias6/170920_105523/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias7/crab_HVscan_20161116_run285371_v1_PAMinimumBias7/170920_105605/0000/clustersTree*.root/demo/ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20161116_run285371_v1/PAMinimumBias8/crab_HVscan_20161116_run285371_v1_PAMinimumBias8/170920_105630/0000/clustersTree*.root/demo/ttree");



  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20161116_run285371.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

