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

  TChain c("ttree");
  c.Add("root://eoscms//eos/cms/store/group/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoHVScan_20120405_run190459_v1_2/clustersTree_*.root/demo/ttree");
  int subdet=0; // 0 all, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=true; // true -> uses timestamps for steps definition instead of event numbers
  SignalAnalysisTreeMaker *t = new SignalAnalysisTreeMaker(&c, "../VoltageSteps/Steps/Steps_20120405_run190459.txt", subdet, usetimestamp);
  t->Loop();

}

