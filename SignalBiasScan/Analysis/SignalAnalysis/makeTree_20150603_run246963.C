#include "Code/Dict.C"
{
  gROOT->Reset();

  gROOT->ProcessLine(".L ../../interface/TreeEvent.h+");  
  gROOT->ProcessLine(".L ../VoltageSteps/Code/VoltageStepsMaker.C+");
  gROOT->ProcessLine(".L ../Clustering/Code/ClustersMaker.C+");
  gROOT->ProcessLine(".L Code/SignalAnalysisTreeMaker.C+");

  if(gROOT->GetClass("TreeEvent") == 0) return;
  if(gROOT->GetClass("VoltageStepsMaker") == 0) return;
  if(gROOT->GetClass("ClustersMaker") == 0) return;
  if(gROOT->GetClass("SignalAnalysisTreeMaker") == 0) return;

  //#include "SignalAnalysisTreeMakerLinkDef.h"

  TChain c("ttree");
  c.Add("root://eoscms//eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20150603_run246963_v1_3/ZeroBias1/clustersTree_7_1_rli.root/demo/ttree");
  int subdet=0; // 0 all, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true -> uses timestamps for steps definition instead of event numbers
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../VoltageSteps/Steps/Steps_20150603_run246963.txt", subdet, usetimestamp);
  //t->Loop();

}
