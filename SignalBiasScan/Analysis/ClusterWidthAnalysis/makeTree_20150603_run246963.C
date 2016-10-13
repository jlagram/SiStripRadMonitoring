{

  gROOT->Reset();

  gROOT->ProcessLine(".L ../../interface/TreeEvent.h+");  
  gROOT->ProcessLine(".L ../VoltageSteps/Code/VoltageStepsMaker.C+");
  gROOT->ProcessLine(".L ../Clustering/Code/ClustersMaker.C+");
  gROOT->ProcessLine(".L Code/ClusterWidthAnalysisTreeMaker.C+");

  if(gROOT->GetClass("TreeEvent") == 0) return;
  if(gROOT->GetClass("VoltageStepsMaker") == 0) return;
  if(gROOT->GetClass("ClustersMaker") == 0) return;
  if(gROOT->GetClass("ClusterWidthAnalysisTreeMaker") == 0) return;

  TChain c("ttree");
  c.Add("root://eoscms//eos/cms/store/group/comm_tracker/Strip/RadMonitoring/SignalBiasScan/ClustersTrees/DecoSmallHVScan_20150603_run246963_v1_3/clustersTree_*.root/demo/ttree");
  int subdet=0; // 0 all, 1 TIB, 2 TOB, 3 TID, 4 TEC
  bool usetimestamp=false; // true -> uses timestamps for steps definition instead of event numbers
  ClusterWidthAnalysisTreeMaker *t = new ClusterWidthAnalysisTreeMaker(&c, "../VoltageSteps/Steps/Steps_20150603_run246963.txt", subdet, usetimestamp);
  t->Loop();

}

