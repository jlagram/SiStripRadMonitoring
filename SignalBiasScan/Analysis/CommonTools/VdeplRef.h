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
  
  if(subdet!="TIB" && subdet!="TOB" && subdet!="TID" ) 
  {std::cout<<"Subdet '"<<subdet<<"' not allowed."<<std::endl; return;}

  std::cout<<"Loading "<<subdet<<" refs ..."<<std::endl; 
  //int sensor_diff = modid%10;
  //if(subdet=="TOB") modid/=10;
  //cout<<"Looking for "<<modid<<" "<<sensor_diff<<endl;
 
  string filename="/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/VdeplRef/depletion_"+subdet+".txt";
  ifstream fref(filename.c_str());
  std::string line;
  unsigned long objectid;
  std::string object;
  std::string type_description;
  unsigned long sensor;
  int sensor_nb;
  float vdepl=-1;
  std::string center;
  unsigned int detid;
  unsigned int dcuid;
  std::string status; // For TIB
  
  Nref=0;
  DetIDs = new ULong64_t[Nmax];
  Vdepl = new float[Nmax];

  if(fref.is_open())
  {
    while( getline ( fref, line) && Nref < Nmax)
	{
	  if(fref.eof()) continue;
	  if(Nref%1000==0) std::cout<<Nref<<" mod"<<std::endl;
	  std::stringstream ss(line);
	  ss >> objectid >> object >> type_description >> sensor >> sensor_nb >> vdepl >> center >> detid >>
	  dcuid >> status;

      if(subdet=="TOB") DetIDs[Nref] = detid*10+sensor_nb-1;
      else DetIDs[Nref] = detid;
	  Vdepl[Nref] = vdepl;
	  //if(Nref%1000==0) std::cout<<Nref<<" "<<DetIDs[Nref]<<" "<<Vdepl[Nref]<<std::endl;
	  Nref++;
	  
	}
	fref.close();
  }

  
  std::cout<<Nref<<" mod loaded."<<std::endl;
}


double VdeplRef::GetVdepl(ULong64_t modid)
{
  unsigned int id=0;
  ULong64_t detid=0;
  double vdepl=0;
  
  while(detid!=modid && id<Nref)
  {
    detid=DetIDs[id];
	vdepl=Vdepl[id];
	id++;
  }
  
  return vdepl;
}


#endif
