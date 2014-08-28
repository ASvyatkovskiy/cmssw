// -*- C++ -*-
//
// Package:    EventPicker
// Class:      EventPicker
// 
/**\class EventPicker EventPicker.cc EventPicker/EventPicker/src/EventPicker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Alex Svyatkovskiy
//         Created:  Fri Aug 12 22:26:35 CDT 2011
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class EventPicker : public edm::EDFilter {
   public:
      explicit EventPicker(const edm::ParameterSet&);
      ~EventPicker();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual bool beginRun(edm::Run&, edm::EventSetup const&);
      virtual bool endRun(edm::Run&, edm::EventSetup const&);
      virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

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
EventPicker::EventPicker(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


EventPicker::~EventPicker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
EventPicker::filter(edm::Event& event, const edm::EventSetup& iSetup)
{
   bool accepted = false;

//   if (event.id().run() == 1 && event.id().luminosityBlock() == 666670 && (event.id().event() == 405 || event.id().event() == 408 || event.id().event() == 417 || event.id().event() == 419 || event.id().event() == 424 || event.id().event() == 432 || event.id().event() == 440 || event.id().event() == 453 )) accepted = true;

//FIXME events of the first step
/*
   if ((event.id().luminosityBlock() == 666704 && event.id().event() == 3878) ||
(event.id().luminosityBlock() == 666728 && event.id().event() == 6217) ||
(event.id().luminosityBlock() == 666728 && event.id().event() == 6271) ||
(event.id().luminosityBlock() == 666732 && event.id().event() == 6654) ||
(event.id().luminosityBlock() == 666732 && event.id().event() == 6670) ||
(event.id().luminosityBlock() == 666732 && event.id().event() == 6689) ||
(event.id().luminosityBlock() == 666734 && event.id().event() == 6805) ||
(event.id().luminosityBlock() == 666734 && event.id().event() == 6868) ||
(event.id().luminosityBlock() == 666735 && event.id().event() == 6935) ||
(event.id().luminosityBlock() == 666735 && event.id().event() == 6954) ||
(event.id().luminosityBlock() == 666737 && event.id().event() == 7132) ||
(event.id().luminosityBlock() == 666738 && event.id().event() == 7280) ||
(event.id().luminosityBlock() == 666739 && event.id().event() == 7307) ||
(event.id().luminosityBlock() == 666739 && event.id().event() == 7317) ||
(event.id().luminosityBlock() == 666740 && event.id().event() == 7463) ||
(event.id().luminosityBlock() == 666740 && event.id().event() == 7464) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7519) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7523) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7581) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7593) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7600) ||
(event.id().luminosityBlock() == 666744 && event.id().event() == 7837) ||
(event.id().luminosityBlock() == 666744 && event.id().event() == 7845) ||
(event.id().luminosityBlock() == 666745 && event.id().event() == 7949) ||
(event.id().luminosityBlock() == 666745 && event.id().event() == 7986) ||
(event.id().luminosityBlock() == 666746 && event.id().event() == 8003) ||
(event.id().luminosityBlock() == 666746 && event.id().event() == 8025) ||
(event.id().luminosityBlock() == 666746 && event.id().event() == 8054) ||
(event.id().luminosityBlock() == 666747 && event.id().event() == 8175) ||
(event.id().luminosityBlock() == 666748 && event.id().event() == 8270) ||
(event.id().luminosityBlock() == 666748 && event.id().event() == 8298) ||
(event.id().luminosityBlock() == 666749 && event.id().event() == 8303) ||
(event.id().luminosityBlock() == 666749 && event.id().event() == 8345) ||
(event.id().luminosityBlock() == 666749 && event.id().event() == 8394) ||
(event.id().luminosityBlock() == 666750 && event.id().event() == 8493) ||
(event.id().luminosityBlock() == 666751 && event.id().event() == 8546) ||
(event.id().luminosityBlock() == 666751 && event.id().event() == 8552) ||
(event.id().luminosityBlock() == 666752 && event.id().event() == 8622) ||
(event.id().luminosityBlock() == 666752 && event.id().event() == 8682) ||
(event.id().luminosityBlock() == 666753 && event.id().event() == 8702) ||
(event.id().luminosityBlock() == 666753 && event.id().event() == 8764) ||
(event.id().luminosityBlock() == 666755 && event.id().event() == 8950) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 311) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 337) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 366) ||
(event.id().luminosityBlock() == 666671 && event.id().event() == 580) ||
(event.id().luminosityBlock() == 666672 && event.id().event() == 672) ||
(event.id().luminosityBlock() == 666672 && event.id().event() == 693) ||
(event.id().luminosityBlock() == 666673 && event.id().event() == 789) ||
(event.id().luminosityBlock() == 666674 && event.id().event() == 882) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1916) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1975) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1980) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1981) ||
(event.id().luminosityBlock() == 666686 && event.id().event() == 2061) ||
(event.id().luminosityBlock() == 666688 && event.id().event() == 2224) ||
(event.id().luminosityBlock() == 666692 && event.id().event() == 2612)) accepted = true;
*/

//FIXME events of the second step
if ((event.id().luminosityBlock() == 666728 && event.id().event() == 6217) ||
(event.id().luminosityBlock() == 666728 && event.id().event() == 6271) ||
(event.id().luminosityBlock() == 666732 && event.id().event() == 6654) ||
(event.id().luminosityBlock() == 666732 && event.id().event() == 6670) ||
(event.id().luminosityBlock() == 666735 && event.id().event() == 6935) ||
(event.id().luminosityBlock() == 666737 && event.id().event() == 7132) ||
(event.id().luminosityBlock() == 666738 && event.id().event() == 7280) ||
(event.id().luminosityBlock() == 666739 && event.id().event() == 7307) ||
(event.id().luminosityBlock() == 666739 && event.id().event() == 7317) ||
(event.id().luminosityBlock() == 666740 && event.id().event() == 7463) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7519) ||
(event.id().luminosityBlock() == 666741 && event.id().event() == 7600) ||
(event.id().luminosityBlock() == 666744 && event.id().event() == 7845) ||
(event.id().luminosityBlock() == 666745 && event.id().event() == 7986) ||
(event.id().luminosityBlock() == 666747 && event.id().event() == 8175) ||
(event.id().luminosityBlock() == 666748 && event.id().event() == 8270) ||
(event.id().luminosityBlock() == 666749 && event.id().event() == 8394) ||
(event.id().luminosityBlock() == 666750 && event.id().event() == 8493) ||
(event.id().luminosityBlock() == 666752 && event.id().event() == 8622) ||
(event.id().luminosityBlock() == 666752 && event.id().event() == 8682) ||
(event.id().luminosityBlock() == 666753 && event.id().event() == 8702) ||
(event.id().luminosityBlock() == 666755 && event.id().event() == 8950) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 311) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 337) ||
(event.id().luminosityBlock() == 666669 && event.id().event() == 366) ||
(event.id().luminosityBlock() == 666672 && event.id().event() == 693) ||
(event.id().luminosityBlock() == 666673 && event.id().event() == 789) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1980) ||
(event.id().luminosityBlock() == 666685 && event.id().event() == 1981)) accepted = true;


   return accepted;
}

// ------------ method called once each job just before starting event loop  ------------
void 
EventPicker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EventPicker::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
EventPicker::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
EventPicker::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
EventPicker::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
EventPicker::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EventPicker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(EventPicker);
