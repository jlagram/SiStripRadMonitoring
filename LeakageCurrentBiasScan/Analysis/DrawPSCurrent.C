#include "Code/PSCurrent.h"
#include "TCanvas.h"


void ConvertAllPSToRoot()
{
//  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20150821_run254790.txt");
/*  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120405_run190459.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120506_run193541.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120510_run193928.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120728_run199832.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120812_run200786.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120928_run203832.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20121130_run208339.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20130213_run211797.txt");

  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L2_20120405_run190459.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L2_20120506_run193541.txt");
*/

/*  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20120405_run190459.txt"); 
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20120510_run193928.txt"); 
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20120728_run199832.txt"); 
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20120812_run200786.txt"); 
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20120928_run203832.txt"); 
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20121130_run208339.txt");
  ConvertPSCurrentTxtToRoot("Data/PS_I_TOB_20130213_run211797.txt");
*/

  //ConvertPSCurrentTxtToRoot("/afs/cern.ch/work/j/jlagram/PS_TEC_I_20120812_run200786.txt");
}


// Utiliser avec 'root -l DrawPSCurrent.C+' pour convertir les fichiers Txt efficacement
void DrawPSCurrent()
{

  int nmod;
  //ConvertAllPSToRoot();
  //TGraph* g = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20150821_run254790.root", 369121381, nmod);
  TGraph* g = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20151007_run258443.root", 369125861, nmod);
  //TGraph* g = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20120506_run193541.root", 3691216005);
  //TGraph* g2 = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20120405_run190459.root", 369121605);
  
  //TGraph* g = ReadPSCurrentRoot("Data/PS_I_TOB_20120728_run199832.root", 436281516);
  g->Draw("APL");

  //ConvertPSCurrentTxtToRoot("Data/PS_I_TIB_L1_20120928_run203832.txt"); 
/*  TGraph* g = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20120928_run203832.root", 369121382, "", true);
  TGraph* g2 = ReadPSCurrentRoot("Data/PS_I_TIB_L1_20120928_run203832.root", 369125862, "", true);

  double x,y;
  g->GetPoint(0,x,y);
  cout<<x<<endl;
  double x2,y2;
  g2->GetPoint(0,x2,y2);
  cout<<x2<<endl;
  int offset=x-x2;
  for(int i=0; i<g2->GetN(); i++)
  {
    g2->GetPoint(i, x, y);
    g2->SetPoint(i, x+offset, y);
  }
  g2->SetMarkerColor(4);
  g2->SetLineColor(4);
  
  
  TCanvas c1;
  g->Draw("APL");
  g2->Draw("PL");
  c1.Modified();
  c1.Update();
  getchar();
*/
}
