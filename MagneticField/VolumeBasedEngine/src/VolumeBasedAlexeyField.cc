#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedAlexeyField.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

VolumeBasedAlexeyField::VolumeBasedAlexeyField( const edm::ParameterSet& config,
						    std::vector<AlexeyBLayer *> theBLayers,
						    std::vector<AlexeyESector *> theESectors,
						    std::vector<AlexeyVolume6Faces*> theBVolumes,
						    std::vector<AlexeyVolume6Faces*> theEVolumes, 
						    float rMax, float zMax) :
  field(new AlexeyGeometry(config,theBLayers,theESectors,theBVolumes,theEVolumes)), 
  maxR(rMax),
  maxZ(zMax)
{ 
}

VolumeBasedAlexeyField::VolumeBasedAlexeyField(const VolumeBasedAlexeyField& vbf) : 
  field(vbf.field),
  maxR(vbf.maxR),
  maxZ(vbf.maxZ) {
}

VolumeBasedAlexeyField::~VolumeBasedAlexeyField(){
}

const AlexeyVolume * VolumeBasedAlexeyField::findVolume(const GlobalPoint & gp) const
{
  return field->findVolume(gp);
}


bool VolumeBasedAlexeyField::isDefined(const GlobalPoint& gp) const {
  return (fabs(gp.z()) < maxZ && gp.perp() < maxR);
}


bool VolumeBasedAlexeyField::isZSymmetric() const {
  return field->isZSymmetric();
}
