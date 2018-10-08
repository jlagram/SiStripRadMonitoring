#include "Code/DCUCurrent.h"
#include "TCanvas.h"

void ConvertAllDCUToRoot()
{
  ConvertDCUCurrentTxtToRoot("Data/DCU_I_TIB_L1_20111012_run178367.txt");
  ConvertDCUCurrentTxtToRoot("Data/DCU_I_TIB_L1_20120405_run190459.txt");
  ConvertDCUCurrentTxtToRoot("Data/DCU_I_TIB_L1_20120506_run193541.txt");

  ConvertDCUCurrentTxtToRoot("Data/DCU_I_TIB_L2_20120405_run190459.txt");
  ConvertDCUCurrentTxtToRoot("Data/DCU_I_TIB_L2_20120506_run193541.txt");
}

void DrawDCUCurrent()
{
  //ConvertAllDCUToRoot();
  //TGraph* g = ReadDCUCurrentRoot("Data/DCU_I_TIB_L1_20120405_run190459.root", 369121605);
  //TGraph* g = ReadDCUCurrentFromGB("~/work/DCU_TIBD_TOB_from_1348837200_to_1348862400.root", 369125861, "", true);
  //TGraph* g = ReadDCUCurrentFromGB("~/work/DCU_from_1433347200_to_1433368500.root", 369125861, "", true);
  //TGraph* g = ReadDCUCurrentFromGB("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1344809400_to_1344859200.root", 369121605, "", true);
  //TGraph* g = ReadDCUCurrentFromGB("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1344809400_to_1344859200.root", 369121605, "", true);
  TGraph* g =
 
ReadDCUCurrentFromGB("/afs/cern.ch/user/j/jlagram/work/public/SiStripRadMonitoring/LeakageCurrentCorrections/Data/DCU/DCU_I_20171030_run305862.root",369121381, "", true);
 
  TCanvas c1;
  g->Draw("APL");
  c1.Modified();
  c1.Update();
  getchar();
}
