#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  //TChain c("ttree");
  TChain c("demo/ttree");

  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVscan_20231025_run375658/StreamHIExpress/crab_HVscan_20231025_run375658/231030_063938/0000/clustersTree*.root/demo/ttree");
  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC 
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20230907_run373060.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

