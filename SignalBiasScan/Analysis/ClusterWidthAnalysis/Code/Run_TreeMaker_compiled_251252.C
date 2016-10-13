#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150708_run251252_v1_3/clustersTree_1_1_7EN.root/demo/ttree");
c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150708_run251252_v1_3/clustersTree_2_1_opX.root/demo/ttree");

  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20150708_run251252.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

