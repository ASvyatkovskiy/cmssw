#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedMatNav.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

#include <iostream>
using namespace std;

VolumeBasedMatNav::VolumeBasedMatNav(const edm::ParameterSet& config,
                                                    std::vector<MatBLayer *> theBLayers,
						    std::vector<MatESector *> theESectors,
						    std::vector<MatVolume6Faces*> theBVolumes,
						    std::vector<MatVolume6Faces*> theEVolumes, 
						    float rMax, float zMax) :
  field(new MatGeometry(config,theBLayers,theESectors,theBVolumes,theEVolumes)), 
  maxR(rMax),
  maxZ(zMax)
{ 
  cout << "Calling VolumeBasedMatNav::VolumeBasedMatNav constructor" << endl;
}

VolumeBasedMatNav::VolumeBasedMatNav(const VolumeBasedMatNav& vbf) : 
  field(vbf.field),
  maxR(vbf.maxR),
  maxZ(vbf.maxZ) {
}

VolumeBasedMatNav::~VolumeBasedMatNav(){
}

const MatVolume * VolumeBasedMatNav::findVolume(const GlobalPoint & gp) const
{
  return field->findVolume(gp);
}

bool VolumeBasedMatNav::isDefined(const GlobalPoint& gp) const {
  return (fabs(gp.z()) < maxZ && gp.perp() < maxR);
}


bool VolumeBasedMatNav::isZSymmetric() const {
  return field->isZSymmetric();
}
