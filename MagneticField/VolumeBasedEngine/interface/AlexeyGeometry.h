#ifndef AlexeyGeometry_H
#define AlexeyGeometry_H

/** \class AlexeyGeometry
 *  Entry point to the geometry of magnetic volumes.
 *
 *  $Date: 2009/03/19 11:02:07 $
 *  $Revision: 1.10 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometrySurface/interface/BoundPlane.h"
#include "MagneticField/Layers/src/AlexeyBinFinders.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include <vector>

class AlexeyBLayer;
class AlexeyESector;
class AlexeyVolume;
class AlexeyVolume6Faces;
template <class T> class PeriodicBinFinderInPhi;
namespace edm {class ParameterSet;}

class AlexeyGeometry {
public:

  typedef Surface::GlobalVector   GlobalVector;
  typedef Surface::GlobalPoint    GlobalPoint;

  /// Constructor
  AlexeyGeometry(const edm::ParameterSet& config, std::vector<AlexeyBLayer *> ,
			     std::vector<AlexeyESector *> ,
			     std::vector<AlexeyVolume6Faces*> ,
			     std::vector<AlexeyVolume6Faces*> );

  /// Destructor
  ~AlexeyGeometry();

  /// Find a volume
  AlexeyVolume * findVolume(const GlobalPoint & gp, double tolerance=0.) const;

  bool isZSymmetric() const;

  const std::vector<AlexeyVolume6Faces*> & barrelVolumes() const {return theBVolumes;}
  const std::vector<AlexeyVolume6Faces*> & endcapVolumes() const {return theEVolumes;}

private:

  // Linear search (for debug purposes only)
  AlexeyVolume * findVolume1(const GlobalPoint & gp, double tolerance=0.) const;

  bool inBarrel(const GlobalPoint& gp) const;

  mutable AlexeyVolume * lastVolume; // Cache last volume found

  std::vector<AlexeyBLayer *> theBLayers;
  std::vector<AlexeyESector *> theESectors;

  std::vector<AlexeyVolume6Faces*> theBVolumes;
  std::vector<AlexeyVolume6Faces*> theEVolumes;

  AlexeyBinFinders::GeneralBinFinderInR<double>* theBarrelBinFinder;
  PeriodicBinFinderInPhi<float> * theEndcapBinFinder;

  bool cacheLastVolume;
  bool v_85l;
};
#endif

