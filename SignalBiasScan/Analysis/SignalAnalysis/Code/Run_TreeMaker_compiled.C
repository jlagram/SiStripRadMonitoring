#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
c.Add("root://eoscms//eos/cms/store/group/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20110315_run160497_v1_2/clustersTree*.root/demo/ttree");
  int subdet=1; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20110315_run160497.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

