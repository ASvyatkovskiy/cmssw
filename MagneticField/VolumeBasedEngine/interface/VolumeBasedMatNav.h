#ifndef MagneticField_VolumeBasedMatNav_h
#define MagneticField_VolumeBasedMatNav_h

/** \class VolumeBasedMagneticField
 */

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/VolumeBasedEngine/interface/MatGeometry.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"


class VolumeBasedMatNav : public MagneticField {
 public:
  VolumeBasedMatNav(const edm::ParameterSet& config,
                            std::vector<MatBLayer *> theBLayers,
			    std::vector<MatESector *> theESectors,
			    std::vector<MatVolume6Faces*> theBVolumes,
			    std::vector<MatVolume6Faces*> theEVolumes,
			    float rMax, float zMax);

  ~VolumeBasedMatNav();

  /// Copy constructor implement a shallow copy (ie no ownership of actual engines)
  VolumeBasedMatNav(const VolumeBasedMatNav& vbf);

  GlobalVector inTesla ( const GlobalPoint& g) const {return GlobalVector(0.,0.,0.);}

  const MatVolume * findVolume(const GlobalPoint & gp) const;

  bool isDefined(const GlobalPoint& gp) const;

  bool isZSymmetric() const;

 private:
  const MatGeometry* field;
  float maxR;
  float maxZ;
};

#endif
