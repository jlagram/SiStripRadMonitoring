#include "TChain.h"
#include "SignalAnalysisTreeMaker.h"

int main()
{
  
  TChain c("ttree");
for(unsigned irun=1; irun<27; irun++)
  c.Add(Form("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20160423_run271056_v1_3/ZeroBias1/crab_HVscan_20160423_run271056_v7/160523_115845/0000/clustersTree_%i.root/demo/ttree", irun));

  int subdet=0; // 0 all subdet, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true if input file uses timestamps
  int angle=0; // 0 all angles, 1 <20deg, ...
  int width=0; // 0 all width, 1 nstrips=1, ...
  int onstrip=0; // 0 not used, 1 on strip, 2 or -2 next to strip, 3 or -3 inter-strip
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../../VoltageSteps/Steps/Steps_20160423_run271056.txt", subdet, usetimestamp, angle, width, onstrip);
  t->Loop();

}

