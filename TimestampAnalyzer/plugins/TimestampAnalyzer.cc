// -*- C++ -*-
//
// Package:    SiStripRadMonitoring/TimestampAnalyzer
// Class:      TimestampAnalyzer
// 
/**\class TimestampAnalyzer TimestampAnalyzer.cc SiStripRadMonitoring/TimestampAnalyzer/plugins/TimestampAnalyzer.cc

 Description: Get the event numbers associated to given timestamps

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Nicolas Tonon
//         Created:  Wed, 03 Oct 2018 13:38:36 GMT
//
//


// system include files
#include <memory>
#include <iostream>
#include <fstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs. ==> DONE

using namespace std;

class TimestampAnalyzer : public edm::one::EDAnalyzer</*edm::one::SharedResources*/>  {
   public:
      explicit TimestampAnalyzer(const edm::ParameterSet&);
      ~TimestampAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TimestampAnalyzer::TimestampAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   //usesResource("TFileService");

}


TimestampAnalyzer::~TimestampAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TimestampAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	
	double timestamp = 0;

	// Get event infos
	cout<<"---------------------"<<endl;
    cout<<iEvent.id().run()<<endl;
    cout<<iEvent.id().event()<<endl;
    cout<<iEvent.time().unixTime()<<endl; 

	return;
}


// ------------ method called once each job just before starting event loop  ------------
void 
TimestampAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TimestampAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TimestampAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TimestampAnalyzer);
