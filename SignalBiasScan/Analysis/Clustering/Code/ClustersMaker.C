#define ClustersMaker_cxx
#include "ClustersMaker.h"

#include <iostream>
#include <cmath>

void ClustersMaker::PrintCluster(TreeFullHit *hit)
{
  unsigned int nStrips = hit->stripCharges.size();
  unsigned int seedIndex = 999;
  int max=-1;
  float clusterNoise = 0;
  int numberStripsOverThreshold = 0;
  
  std::cout<<"nStrips "<<nStrips<<" charge "<<hit->chargeAngleCorr<<" noise "<<hit->noise<<std::endl;
  std::cout<<" seed charge "<<hit->seedChargeAngleCorr<<std::endl;
  std::cout<<"S : ";
  for(unsigned int istr=0; istr<nStrips; istr++)
  {
	if(max<hit->stripCharges[istr]) {max = hit->stripCharges[istr]; seedIndex = istr;} 
    cout<<(int) hit->stripCharges[istr]<<" ";
  }
  std::cout<<std::endl;
  std::cout<<"G : ";
  for(unsigned int istr=0; istr<nStrips; istr++)
    std::cout<<hit->stripGains[istr]<<" ";
  std::cout<<std::endl;
  std::cout<<"N : ";
  for(unsigned int istr=0; istr<nStrips; istr++)
  {
     if( hit->stripNoises[istr]!=0 ) 
    { clusterNoise += pow( hit->stripNoises[istr]/hit->stripGains[istr] , 2);
      numberStripsOverThreshold++;
    }
    std::cout<<hit->stripNoises[istr]<<" ";
  }
  std::cout<<std::endl;
  clusterNoise = sqrt(clusterNoise/numberStripsOverThreshold);

  std::cout<<"Q : ";
  for(unsigned int istr=0; istr<nStrips; istr++)
    std::cout<<hit->stripQualitiesBad[istr]<<" ";
  std::cout<<std::endl;

  std::cout<<"SoN : ";
  for(unsigned int istr=0; istr<nStrips; istr++)
    std::cout<< hit->stripCharges[istr]/hit->stripNoises[istr]*hit->stripGains[istr] <<" ";
  std::cout<<std::endl;
  
  std::cout<<" seed index "<<seedIndex<<" "<<std::endl;
  //max_element(hit->stripCharges.begin(), hit->stripCharges.end()) - hit->stripCharges.begin();
  std::cout<<" cluster noise "<<clusterNoise<<std::endl;
}

//----------------------------------------------------------------------

bool ClustersMaker::CandidateEnded(unsigned int istr){

  unsigned int maxSequentialHoles = 0;
  unsigned int holes = istr - lastStrip - 1;
  if(!ADCs.empty() && 
    holes > maxSequentialHoles)// &&
    //(holes > MaxSequentialBad || !allBadBetween(firstStrip, itstr)))
	return true;
  else return false;
}

bool ClustersMaker::IsGoodCandidate(float clusThresh){

  bool isGood = false;
  float signal = 0;

  for(unsigned int is=0; is<ADCs.size(); is++)
	signal += ADCs[is];
  if( !candidateLacksSeed && signal >= sqrt(noiseSquared)*clusThresh )
    isGood = true;
	
  //if(!isGood) std::cout<<"seed "<<!candidateLacksSeed<<" "<<signal-sqrt(noiseSquared)*clusThresh<<std::endl;

  return isGood;
}

vector< unsigned int > ClustersMaker::CreateCluster(unsigned int istr){

  vector< unsigned int > stripsIndex;
    
  for(unsigned int is=0; is<ADCs.size(); is++)
  	stripsIndex.push_back(Index[is]);
	
  Initialize();
  lastStrip = istr;

  return stripsIndex;
}


vector< vector< unsigned int > > ClustersMaker::ReClusterize(TreeFullHit *hit, float signalEff, 
       float seedThresh, float stripThresh, float clusThresh)
{
  vector< vector< unsigned int > > clustersStripsIndex;
  
  Initialize();
  
  //append strips
  unsigned int nStrips = hit->stripCharges.size();
  //cout<<"initial cluster size : "<<nStrips<<", charge : "<<hit->chargeAngleCorr/fabs(cos(hit->angleForCorr))<<endl;
  for(unsigned int istr=0; istr<nStrips; istr++)
  {
	// check if cluster ended
	if( CandidateEnded(istr) )
	if( IsGoodCandidate(clusThresh) )
	  clustersStripsIndex.push_back( CreateCluster(istr) );

    // check strip
	if(hit->stripQualitiesBad[istr] || 
	  hit->stripCharges[istr]*signalEff < static_cast<unsigned int>( static_cast<unsigned int>(hit->stripNoises[istr]*stripThresh)/hit->stripGains[istr] + 0.5 )) continue;

    // check seed
	if(candidateLacksSeed) candidateLacksSeed = 
	  hit->stripCharges[istr]*signalEff < static_cast<unsigned int>( static_cast<unsigned int>(hit->stripNoises[istr]*seedThresh)/hit->stripGains[istr] + 0.5 );
	
	// add holes
	if(!ADCs.empty())
	for(unsigned int ihol=lastStrip+1; ihol<istr; ihol++) { ADCs.push_back(ihol); ADCs.push_back(0);}

    // add strip
	Index.push_back( istr );
	ADCs.push_back( hit->stripCharges[istr]*signalEff );
	noiseSquared += pow(hit->stripNoises[istr]/hit->stripGains[istr], 2);
	lastStrip = istr;

  }
  
  // Check for last cluster candidate
  if( IsGoodCandidate(clusThresh) )
	clustersStripsIndex.push_back( CreateCluster(lastStrip+1) );

  
  // Print new clusters
/*  if(clustersStripsIndex.size()>1 || (clustersStripsIndex.size()==1 && clustersStripsIndex[0].size()!=nStrips))
  {
    std::cout<<"initial cluster size : "<<nStrips<<", charge : "<<hit->chargeAngleCorr/fabs(cos(hit->angleForCorr))<<std::endl;
    PrintCluster(hit);
	if(clustersStripsIndex.size()==0)  cout<<"no re-clusterized cluster"<<std::endl;
	for(unsigned int icl=0; icl<clustersStripsIndex.size(); icl++)
	{
	  std::cout<<"new cluster with "<<clustersStripsIndex[icl].size()<<" strips"<<std::endl;
	  std::cout<<" first strip : "<<clustersStripsIndex[icl][0]<<" last strip : "<<clustersStripsIndex[icl][clustersStripsIndex[icl].size()-1]<<std::endl;
	}
	std::cout<<endl;
  }
*/  
  return clustersStripsIndex;
  
}

// In checks :
// - qualitiesBad seems always at 0
// - very few cluster with last strip < 2*stripNoise
// - few cluster not pass 5*clusNoise due to truncation of ADC stored
