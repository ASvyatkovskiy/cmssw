#ifndef MatGeometry_H
#define MatGeometry_H

/** \class MatGeometry
 *  Entry point to the geometry of magnetic volumes.
 *
 *  $Date: 2009/03/19 11:02:07 $
 *  $Revision: 1.10 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometrySurface/interface/BoundPlane.h"
#include "MagneticField/Layers/src/MatBinFinders.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include <vector>

class MatBLayer;
class MatESector;
class MatVolume;
class MatVolume6Faces;
template <class T> class PeriodicBinFinderInPhi;
namespace edm {class ParameterSet;}

class MatGeometry {
public:

  typedef Surface::GlobalVector   GlobalVector;
  typedef Surface::GlobalPoint    GlobalPoint;

  /// Constructor
  MatGeometry(const edm::ParameterSet& config, std::vector<MatBLayer *> ,
			     std::vector<MatESector *> ,
			     std::vector<MatVolume6Faces*> ,
			     std::vector<MatVolume6Faces*> );

  /// Destructor
  ~MatGeometry();

  /// Find a volume
  MatVolume * findVolume(const GlobalPoint & gp, double tolerance=0.) const;

  bool isZSymmetric() const;

  const std::vector<MatVolume6Faces*> & barrelVolumes() const {return theBVolumes;}
  const std::vector<MatVolume6Faces*> & endcapVolumes() const {return theEVolumes;}

private:

  // Linear search (for debug purposes only)
  MatVolume * findVolume1(const GlobalPoint & gp, double tolerance=0.) const;

  bool inBarrel(const GlobalPoint& gp) const;

  mutable MatVolume * lastVolume; // Cache last volume found

  std::vector<MatBLayer *> theBLayers;
  std::vector<MatESector *> theESectors;

  std::vector<MatVolume6Faces*> theBVolumes;
  std::vector<MatVolume6Faces*> theEVolumes;

  MatBinFinders::GeneralBinFinderInR<double>* theBarrelBinFinder;
  PeriodicBinFinderInPhi<float> * theEndcapBinFinder;

  bool cacheLastVolume;
  bool v_85l;
};
#endif

