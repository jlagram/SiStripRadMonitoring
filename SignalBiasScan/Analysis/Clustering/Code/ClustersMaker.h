#ifndef ClustersMaker_h
#define ClustersMaker_h

#include "../../../interface/TreeEvent.h"
#include <vector>

using namespace std;

class ClustersMaker {

 public :

   ClustersMaker(){ }

   void PrintCluster(TreeFullHit *);
   vector< vector< unsigned int > > ReClusterize(TreeFullHit *hit, float signalEff=1, 
       float seedThresh=3, float stripThresh=2, float clusThresh=5);

 private :
 
   vector<unsigned int> Index;
   vector<unsigned int> ADCs;
   bool candidateLacksSeed;
   float noiseSquared;
   int lastStrip;
   
   void Initialize(){ Index.clear(); ADCs.clear(); candidateLacksSeed=true; noiseSquared=0; lastStrip=0; };

   bool CandidateEnded(unsigned int istr);
   bool IsGoodCandidate(float clusThresh);
   vector< unsigned int > CreateCluster(unsigned int istr);
   
};

#endif 
