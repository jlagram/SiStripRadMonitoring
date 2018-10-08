bool isUsedInSmallBiasScan ( int detid ) {
    
    // TIB - 1.4.2.5, until run ??
    if(detid==369121605 ||
       detid==369121606 ||
       detid==369121614 ||
       detid==369121613 ||
       detid==369121610 ||
       detid==369121609 ) return true;
    
    // TIB - 1.2.2.1, from run 193541
    if(detid==369121390 ||
       detid==369121382 ||
       detid==369121386 ||
       detid==369121385 ||
       detid==369121389 ||
       detid==369121381 ) return true;
    
    // TIB + 1.6.2.5
    if(detid==369125870 ||
       detid==369125862 ||
       detid==369125866 ||
       detid==369125869 ||
       detid==369125865 || // module not powered and masked
       detid==369125861 ) return true;
    
    // TOB + 1.3.1.6
    if(detid==436232901 ||
       detid==436232902 ||
       detid==436232905 ||
       detid==436232906 ||
       detid==436232909 ||
       detid==436232910 ||
       detid==436232913 ||
       detid==436232914 ||
       detid==436232917 ||
       detid==436232918 ||
       detid==436232921 ||
       detid==436232922 ) return true;
    
    // TOB + 4.3.3.8
    if(detid==436281512 ||
       detid==436281528 ||
       detid==436281508 ||
       detid==436281524 ||
       detid==436281520 ||
       detid==436281516 ) return true;
    
    // TEC
    if(detid==470148196 ||
       detid==470148200 ||
       detid==470148204 ||
       detid==470148228 ||
       detid==470148232 ||
       detid==470148236 ||
       detid==470148240 ||
       detid==470148261 ||
       detid==470148262 ||
       detid==470148265 ||
       detid==470148266 ||
       detid==470148292 ||
       detid==470148296 ||
       detid==470148300 ||
       detid==470148304 ||
       detid==470148324 ||
       detid==470148328 ||
       detid==470148332 ||
       detid==470148336 || 
       detid==470148340 ) return true; 
    
    
    return false;
}

// read input tree and keep only few branches
void CopyTree(const char* filename="DCU_TIBD_TEC+_TEC-_TOB_from_1440141900_to_1440149400_Aug2015.root", const char* run="20150821_run254790",
 bool isSmallBiasScan=false, int timeshiftinhour=0){
    
    // Open file and set branches
    TFile* oldfile = TFile::Open(filename);
    if(!oldfile) {std::cout<<"Error : file '"<<std::string(filename)<<"' not found."<<std::endl; return;}
    
    double Detid;
    double Dcutimestamp;
    double TemperatureSi;
    double Ileak;
    
    TTree* oldtree = (TTree*) oldfile->Get("outTree");
    if(!oldtree) {std::cout<<"Error : tree not found."<<std::endl; return;}
    oldtree->SetBranchAddress("Detid", &Detid);
    oldtree->SetBranchAddress("Dcutimestamp", &Dcutimestamp);
    oldtree->SetBranchAddress("TemperatureSi", &TemperatureSi);
    oldtree->SetBranchAddress("Ileak", &Ileak);
    oldtree->SetBranchStatus("*",0);
    oldtree->SetBranchStatus("Detid",1);
    oldtree->SetBranchStatus("Dcutimestamp",1);
    oldtree->SetBranchStatus("TemperatureSi",1);
    oldtree->SetBranchStatus("Ileak",1);
    Long64_t nentries = oldtree->GetEntries();

    TFile *newfile = new TFile(Form("DCU_I_%s.root", run),"recreate");
    TTree *newtree;
    
    if(!isSmallBiasScan) {
        newtree = oldtree->CloneTree();
        newtree->Print();
        newtree->Write();
    }
    // filter on module id
    else {
        newtree = oldtree->CloneTree(0);
        for (Long64_t i=0;i<nentries; i++) {
            oldtree->GetEntry(i);
            if (isUsedInSmallBiasScan(Detid)){ 
			    if(timeshiftinhour!=0) Dcutimestamp+=3600*timeshiftinhour;
			    newtree->Fill();
			}
        }
        newtree->Print();
        newtree->AutoSave();
    }
    
    
}


void FilterDCUInfos(){
/*    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1333635000_to_1333662000_Apr2012.root", "20120405_run190459", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1336315800_to_1336322700_May2012.root", "20120506_run193541", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1336684500_to_1336691700_May2012.root", "20120510_run193928", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1343488200_to_1343500200_Jul2012.root", "20120728_run199832", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1344814200_to_1344826200_Aug2012.root", "20120812_run200786", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1348851600_to_1348868700_Sep2012.root", "20120928_run203832", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1354303500_to_1354324500_Nov2012.root", "20121130_run208339", true);
	// 2013
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1360746600_to_1360751700_Feb2013.root", "20130213_run211797", true);
    // 2015
	CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1433364300_to_1433373300_Jun2015.root", "20150603_run246963_part1", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1433455260_to_1433462400_Jun2015.root", "20150603_run246963_part2", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1440141900_to_1440149400_Aug2015.root", "20150821_run254790", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1444206900_to_1444211100_Oct2015.root", "20151007_run258443", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1448100600_to_1448104800_Nov2015.root", "20151121_run262254", true);
    // 2016
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1461370800_to_1461377700_Apr2016.root", "20160423_run271056", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1465722000_to_1465726800_Jun2016.root", "20160612_run274969", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1467803400_to_1467808200_Jul2016.root", "20160706_run276437", true);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1467824700_to_1467828900_Jul2016_FullScan.root", "20160706_run276453", false);
    CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1470250800_to_1470255600_Aug2016.root", "20160803_run278167", true);
	CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1472749800_to_1472753700_Sep2016_FullScan.root", "20160901_run279865", false);
	CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1473415200_to_1473424200_Sep2016.root", "20160909_run280385", true);
	CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1479307200_to_1479312900_Nov2016.root", "20161116_run285371", true);
*/

     // 2017
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1509354000_to_1509358500_Oct2017.root", "20171030_run305862", true);

     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1505832000_to_1505839500_Sep2017_Noise1.root", "noise_20170919_run303272_part1", false);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1505883000_to_1505886000_Sep2017_Noise2.root", "noise_20170919_run303272_part2", false);

     // 2018
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1524017100_to_1524024600_Apr2018.root", "20180418_run314574", false);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1524095100_to_1524102300_Apr2018_-10C.root", "20180419_run314755", false);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1524189300_to_1524194400_Apr2018_-10C_missingPS.root", "20180419_run314756", false);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1527679200_to_1527685500_May2018.root", "20180530_run317182", true);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1528734600_to_1528739400_Jun2018.root", "20180611_run317683", true);
     CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1529315700_to_1529324700_Jun2018_Noise1.root", "noise_20180618_run317974_part1", true);
     CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1529419500_to_1529423700_Jun2018_Noise2.root", "noise_20180618_run317974_part2", true);
     //CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1533111900_to_1533119100_Aug2018.root", "20180801_run320674", true);    
     CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1537341300_to_1537349460_Sep2018_Noise.root", "noise_20180919_323011", false);
     CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1537707600_to_1537715700_Sep2018_Part1.root", "20180923_run323370_part1", false);
     CopyTree("~/work/DCU_TIBD_TEC+_TEC-_TOB_from_1537735200_to_1537737600_Part2.root", "20180923_run323370_part2", false);
}
