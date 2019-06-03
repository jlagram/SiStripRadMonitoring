#ifndef _mapFedkeyToDeviceid_h_
#define _mapFedkeyToDeviceid_h_

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "fedKey.h"

std::map<fedKeyType, long> mapFedkeyToDeviceid;
std::map<fedKeyType, int> mapFedkeyAPV0;
std::map<fedKeyType, int> mapFedkeyAPV1;

using namespace std;

void loadMapFedkeyToDeviceid(string filename) {
  
  ifstream file(filename);
  string line;
  string key, id, apv, fec;
  long fedkey, detid;
  int apv0, apv1;
  
  if (file.is_open()){
	while(getline(file, line))
	{
      istringstream iss(line);
	  iss >> key >> fedkey >> id >> detid >> apv >> apv0 >> apv1 >> fec;
	  mapFedkeyToDeviceid[fedkey] = detid;
	  mapFedkeyAPV0[fedkey] = apv0-32;
	  mapFedkeyAPV1[fedkey] = apv1-32;
	  //cout<<fedkey<<" "<<detid<<endl;
	}
	file.close();
  }
  else {cerr<<"File "<<filename<<" not found."<<endl; return;}
}
#endif
