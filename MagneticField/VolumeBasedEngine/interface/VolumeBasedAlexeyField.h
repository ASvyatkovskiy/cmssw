#ifndef MagneticField_VolumeBasedAlexeyField_h
#define MagneticField_VolumeBasedAlexeyField_h

/** \class VolumeBasedMagneticField
 *
 *  Field engine providing interpolation within the full CMS region.
 *
 *  $Date: 2009/05/23 22:21:34 $
 *  $Revision: 1.9 $
 *  \author N. Amapane - CERN
 */

#include "MagneticField/Engine/interface/AlexeyField.h"
#include "MagneticField/VolumeBasedEngine/interface/AlexeyGeometry.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"


class VolumeBasedAlexeyField : public AlexeyField {
 public:
  VolumeBasedAlexeyField( const edm::ParameterSet& config,
			    std::vector<AlexeyBLayer *> theBLayers,
			    std::vector<AlexeyESector *> theESectors,
			    std::vector<AlexeyVolume6Faces*> theBVolumes,
			    std::vector<AlexeyVolume6Faces*> theEVolumes,
			    float rMax, float zMax);

  ~VolumeBasedAlexeyField();

  /// Copy constructor implement a shallow copy (ie no ownership of actual engines)
  VolumeBasedAlexeyField(const VolumeBasedAlexeyField& vbf);

  const AlexeyVolume * findVolume(const GlobalPoint & gp) const;

  bool isDefined(const GlobalPoint& gp) const;

  bool isZSymmetric() const;

 private:
  const AlexeyGeometry* field;
  float maxR;
  float maxZ;
};

#endif
