#ifndef VolumeBasedAlexeyESProducer_h
#define VolumeBasedAlexeyESProducer_h

/** \class VolumeBasedAlexeyESProducer
 *
 *  Producer for the VolumeBasedMagneticField.
 *
 *  $Date: 2006/05/31 13:52:10 $
 *  $Revision: 1.1 $
 */

#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include "MagneticField/Engine/interface/AlexeyField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class VolumeBasedAlexeyESProducer : public edm::ESProducer {
public:
  VolumeBasedAlexeyESProducer(const edm::ParameterSet& iConfig);

  std::auto_ptr<AlexeyField> produce(const IdealMagneticFieldRecord & iRecord);

private:
  // forbid copy ctor and assignment op.
  VolumeBasedAlexeyESProducer(const VolumeBasedAlexeyESProducer&);
  const VolumeBasedAlexeyESProducer& operator=(const VolumeBasedAlexeyESProducer&);

  edm::ParameterSet pset;
};
#endif
