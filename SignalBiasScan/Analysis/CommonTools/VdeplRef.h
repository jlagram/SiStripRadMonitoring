#ifndef VdeplRef_h
#define VdeplRef_h

#include <iostream>
#include <fstream>
#include <sstream>
 
// Vdepl references from db
//--------------------------

class VdeplRef{
  public:
  
    VdeplRef():Nmax(11000){}
    void loadFile(string subdet);
	double GetVdepl(ULong64_t modid);
	
  private:
    
	const unsigned int Nmax;
	ULong64_t* DetIDs;
	float* Vdepl;
    unsigned int Nref;

};

void VdeplRef::loadFile(string subdet)
{
  

  if(subdet!="TIB" && subdet!="TOB" && subdet!="TID" && subdet!="TEC") 
  {std::cout<<"VdeplRef.h : Subdet '"<<subdet<<"' not allowed."<<std::endl; return;}

  //std::cout<<"Loading "<<subdet<<" refs ..."<<std::endl; 
  

  //int sensor_diff = modid%10;
  //if(subdet=="TOB") modid/=10;
  //cout<<"Looking for "<<modid<<" "<<sensor_diff<<endl;
 
  string filename="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/VdeplRef/depletion_"+subdet+".txt";
  ifstream fref(filename.c_str());
  std::string line;

  ULong64_t objectid;

  std::string object;
  std::string type_description;
  unsigned long sensor;
  int sensor_nb;
  float vdepl=-1;
  std::string center;

  ULong64_t detid;
  ULong64_t dcuid;

  std::string status; // For TIB
  
  Nref=0;
  DetIDs = new ULong64_t[Nmax];
  Vdepl = new float[Nmax];

  
  vector<bool> v_TEC_isDoubleSensor;
  
  ULong64_t detid_tmp = 0;
  
  
  //TEC -> added a number at end of detid : 0 if single sensor, 1 & 2 if double
  //Notice that in TEC file, if double sensor --> 2 consecutive lines (one for each sensor), if single -> only one line
  //First read file to check if single or double sensor ! Store info in vector
  if(subdet == "TEC" && fref.is_open())
  {
  	int iter = 0;
	getline ( fref, line); //1rst line
	
    while( getline ( fref, line))
	{
	  if(fref.eof()) continue;
	  //if(Nref%1000==0) std::cout<<Nref<<" mod"<<std::endl;
	  std::stringstream ss(line);
	  ss >> objectid >> object >> type_description >> sensor >> sensor_nb >> vdepl >> center >> detid >>
	  dcuid >> status;
      
      //First line of file
      //if(iter==0) {detid_tmp = detid; v_TEC_isDoubleSensor.push_back(false); continue;}      

	  //If same detid as previous line --> double sensor
	  if(detid == detid_tmp)
	  {
	  	v_TEC_isDoubleSensor[v_TEC_isDoubleSensor.size()-1] = true;
	  	v_TEC_isDoubleSensor.push_back(true);
	  }
	  //Else, single sensor
	  else {v_TEC_isDoubleSensor.push_back(false);}
      	
      detid_tmp = detid;	
      iter++;  
	}
	
	//go back to beginning of file
	fref.clear();
    fref.seekg(0, ios::beg);
  }

  getline ( fref, line); //1rst line
  if(fref.is_open())
  {
    while( getline ( fref, line) && Nref < Nmax)
	{
	  if(fref.eof()) continue;

	  //if(Nref%1000==0) std::cout<<Nref<<" mod"<<std::endl;
	  std::stringstream ss(line);
	  ss >> objectid >> object >> type_description >> sensor >> sensor_nb >> vdepl >> center >> detid >>
	  dcuid >> status;
	  
	    
	  //cout<<"detid = "<<detid<<endl;


      //--- create array of Vdep_ref values, adding a sensor ID for TOB & (some) TEC modules (different sensors have different ref values!)
      
      if(subdet=="TOB") {DetIDs[Nref] = detid*10+sensor_nb-1;}
      else if(subdet == "TEC")
      {
      	//cout<<"Nref = "<<Nref<<" / v_TEC_isDoubleSensor[Nref] = "<<v_TEC_isDoubleSensor[Nref]<<endl;	
      
      	if(v_TEC_isDoubleSensor[Nref] == true) {DetIDs[Nref] = detid*10+sensor_nb-1;} //Double sensor
      	else DetIDs[Nref] = detid*10; //Single sensor
      }    
      else DetIDs[Nref] = detid; //single sensor
     
      
      //cout<<"DetIDs[Nref] = "<<DetIDs[Nref]<<endl;

 
	  Vdepl[Nref] = vdepl;
	  //if(Nref%1000==0) std::cout<<Nref<<" "<<DetIDs[Nref]<<" "<<Vdepl[Nref]<<std::endl;
	  Nref++;
	  
	}
	fref.close();
  }

  

  //std::cout<<Nref<<" mod loaded."<<std::endl;

}


double VdeplRef::GetVdepl(ULong64_t modid)
{

  ULong64_t id=0;
  ULong64_t detid=0;
  double vdepl=0;


  
  while(detid!=modid && id<Nref)
  {
    detid=DetIDs[id];
	vdepl=Vdepl[id];
	id++;
  }
  
  if(detid != modid) {cout<<"Modid "<<modid<<" not found in VdeplRef file ! Return 0"<<endl; return 0;}
  //if(detid != modid) {return 0;}
  
  return vdepl;
}


#endif
