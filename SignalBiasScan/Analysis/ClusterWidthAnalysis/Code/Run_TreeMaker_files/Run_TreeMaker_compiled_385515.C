#include "TChain.h"
#include "ClusterWidthAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");
  
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVscan_20240910_run385515_v1/StreamExpress/crab_HVscan_20240910_run385515_v1/240917_060524/0000/clustersTree*.root/demo/ttree");

  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC 
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20240910_run385515.txt", subdet, usetimestamp, angle, width, onstrip);

  t->Loop();

}

