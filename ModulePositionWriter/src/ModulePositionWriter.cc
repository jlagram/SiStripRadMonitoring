// -*- C++ -*-
//
// Package:    ModulePositionWriter
// Class:      ModulePositionWriter
// 
/**\class ModulePositionWriter ModulePositionWriter.cc SiStripRadMonitoring/ModulePositionWriter/src/ModulePositionWriter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jean-Laurent Agram,40 4-B12,+41227671530,
//         Created:  Tue Apr 22 17:27:38 CEST 2014
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"

#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"
#include "DataFormats/Common/interface/EDProductfwd.h"
#include "DataFormats/DetId/interface/DetId.h"

//
// class declaration
//

class ModulePositionWriter : public edm::EDAnalyzer {
   public:
      explicit ModulePositionWriter(const edm::ParameterSet&);
      ~ModulePositionWriter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------

   TTree *smalltree;
   edm::Service<TFileService> fs;
   
   unsigned int detId;
   int subdetId;
   double R;
   double Z;

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
ModulePositionWriter::ModulePositionWriter(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

  smalltree = fs->make<TTree>("ttree", "Modules positions");
  
  smalltree->Branch("detId", &detId, "detId/I");
  smalltree->Branch("subdetId", &subdetId, "subdetId/I");
  smalltree->Branch("R", &R, "R/D");
  smalltree->Branch("Z", &Z, "Z/D");
  
}


ModulePositionWriter::~ModulePositionWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ModulePositionWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;

   ESHandle<TrackerGeometry> tracker;
   iSetup.get<TrackerDigiGeometryRecord>().get(tracker);

   vector< DetId > detUnitIds = tracker->detUnitIds();
   map< int, float > modStripLength;
 
   for(unsigned int idet=0; idet<detUnitIds.size(); idet++)
   {
       if(idet%1000==0) cout<<idet<<" mod read"<<endl;
       DetId detID = detUnitIds[idet];
	   subdetId = detID.subdetId();
	   
	   if(subdetId<3) continue; // Remove Pixels
       detId = detID.rawId();
       const GeomDetUnit* gdu = static_cast<const GeomDetUnit*>(tracker->idToDet(detID));
	   R = gdu->position().perp();
	   Z = gdu->position().z();
	   smalltree->Fill();
	   //cout<<"Mod "<<subdetId<<" "<<detId<<" "<<R<<" "<<Z<<endl;

       // Look also at stripLength
       // enum ModuleGeometry {UNKNOWNGEOMETRY, IB1, IB2, OB1, OB2, W1A, W2A, W3A, W1B, W2B, W3B, W4, W5, W6, W7};
       int modGeom = SiStripDetId(detID).moduleGeometry();
       const StripTopology& topo = (const StripTopology&) gdu->topology();
       //modStripLength[ modGeom ] = 
       cout<<idet<<" subdet : "<<subdetId<<" type "<<modGeom<<" l "
         <<topo.stripLength()<<endl;
   }
}


// ------------ method called once each job just before starting event loop  ------------
void 
ModulePositionWriter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ModulePositionWriter::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
ModulePositionWriter::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
ModulePositionWriter::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
ModulePositionWriter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
ModulePositionWriter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ModulePositionWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ModulePositionWriter);
