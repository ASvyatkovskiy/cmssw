/** \file
 *
 *  $Date: 2010/04/20 03:23:47 $
 *  $Revision: 1.5 $
 */

#include "MagneticField/GeomBuilder/plugins/VolumeBasedAlexeyESProducer.h"
#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedMagneticField.h"
#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedAlexeyField.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

VolumeBasedAlexeyESProducer::VolumeBasedAlexeyESProducer(const edm::ParameterSet& iConfig) : pset(iConfig)
{
  setWhatProduced(this, pset.getUntrackedParameter<std::string>("label",""));
}


// ------------ method called to produce the data  ------------
std::auto_ptr<AlexeyField> VolumeBasedAlexeyESProducer::produce(const IdealMagneticFieldRecord & iRecord)
{
  bool debug = pset.getUntrackedParameter<bool>("debugBuilder", false);
  //if (debug) {
  cout << "VolumeBasedAlexeyESProducer::produce() " << pset.getParameter<std::string>("version") << endl;
  //}
  
  edm::ESTransientHandle<DDCompactView> cpv;
  iRecord.get("magfield",cpv );
  AlexeyGeoBuilderFromDDD builder(pset.getParameter<std::string>("version"),
 			       debug, 
 		       pset.getParameter<bool>("overrideMasterSector"));

  builder.build(*cpv);

  // Get slave field
  edm::ESHandle<AlexeyField> paramField;
  if (pset.getParameter<bool>("useParametrizedTrackerField")) {;
    iRecord.get(pset.getParameter<string>("paramLabel"),paramField);
  }
  std::auto_ptr<AlexeyField> s(new VolumeBasedAlexeyField(pset,builder.barrelLayers(), builder.endcapSectors(), builder.barrelVolumes(), builder.endcapVolumes(), builder.maxR(), builder.maxZ()));
  return s;
}
DEFINE_FWK_EVENTSETUP_MODULE(VolumeBasedAlexeyESProducer);
