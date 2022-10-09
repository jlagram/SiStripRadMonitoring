#ifndef Radiation_Signal_BiasScanSignal_TreeEvent
#define Radiation_Signal_BiasScanSignal_TreeEvent

#include <vector>

//------------------//
// DATA FORMAT v1.3 //
//------------------//


// v1.0 new data format with classes
// v1.1 added nb of tracks and of hits
// v1.2 added some tracks info (p, pt, chi2)
// v1.3 added strips infos + angle used to correct charge


// Standard hit informations
/////////////////////////////

class TreeHit {
 public: 
  
  TreeHit()
  {
	detId 			= 0;
	
	noise 			= 0;
	chargeAngleCorr = 0;
	angle	 		= 0;
	width 			= 0;

	tsosx			= 0;
	tsosy			= 0;
	clusx			= 0;
	
  };
  ~TreeHit(){};
  
  
  // data member

  unsigned int detId;
  
  float noise;
  float chargeAngleCorr;
  float angle;
  float width;
  
  float tsosx;
  float tsosy;
  float clusx;

};


// Hit extended informations
/////////////////////////////

class TreeFullHit : public TreeHit
{
 public: 

 TreeFullHit() : TreeHit()
  {
	seedChargeAngleCorr = 0;
	angleForCorr	= 0;

	barycenter 		= 0;
	seed			= 0;
	
    locBy			= 0;

  };
  ~TreeFullHit(){};
  
  void reset()
  {
	stripCharges.clear();
	stripGains.clear();
	stripNoises.clear();
	stripQualitiesBad.clear();
  }


  // data member

  float seedChargeAngleCorr;
  float angleForCorr;

  float barycenter;
  float seed;
  
  float locBy;
  
  
  // strips info
  std::vector< unsigned char > stripCharges;
  std::vector< float > stripGains;
  std::vector< float > stripNoises;
  std::vector< bool > stripQualitiesBad;
  
};


// Track hits container
/////////////////////////////

class TreeTrack {

 public:
 
  TreeTrack(){ charge=0; pT=0; p=0; chi2=0; Nhits=0; };
  ~TreeTrack(){};
  
  void reset()
  {
    Nhits=0;
	
	TIB_hits.clear();
    TID_hits.clear();
    TOB_hits.clear();
    TEC_hits.clear();

    TIB_fullHits.clear();
    TID_fullHits.clear();
    TOB_fullHits.clear();
    TEC_fullHits.clear();
  }
  
  
  // data member
  
  short charge;
  float pT;
  float p;
  float chi2;
  unsigned int Nhits; // number of hits of the track
  
  // Fill either hits or fullHits
  std::vector< TreeHit > TIB_hits;
  std::vector< TreeHit > TID_hits;
  std::vector< TreeHit > TOB_hits;
  std::vector< TreeHit > TEC_hits;

  std::vector< TreeFullHit > TIB_fullHits;
  std::vector< TreeFullHit > TID_fullHits;
  std::vector< TreeFullHit > TOB_fullHits;
  std::vector< TreeFullHit > TEC_fullHits;

};


// Event informations
/////////////////////////////

class TreeEvent {

 public:
 
  TreeEvent()
  {
    run_nr = 0;
	ev_nr = 0;
	ev_timestamp = 0;
	
	Ntracks = 0;
  };
  ~TreeEvent(){};
  
  void reset() {
    std::vector< TreeTrack >::iterator itTrack;
    for(itTrack=tracks.begin(); itTrack!=tracks.end(); itTrack++)
      itTrack->reset();
    tracks.clear(); 
    Ntracks = 0; 
  }
  
  
  // data member

  unsigned int run_nr;
  unsigned int ev_nr;
  unsigned int ev_timestamp;
  
  unsigned int Ntracks; // number of tracks with normalized chi2 < 5
  
  std::vector< TreeTrack > tracks; 
  
};


#endif
