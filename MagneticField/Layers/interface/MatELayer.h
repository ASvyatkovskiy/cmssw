#ifndef MatELayer_H
#define MatELayer_H

/** \class MatELayer
 *  A layer of volumes in an endcap sector.
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include <vector>

class MatVolume6Faces;

class MatELayer {
public:
  /// Constructor
  MatELayer(std::vector<MatVolume6Faces*> volumes, double zMin, double zMax);

  /// Destructor
  virtual ~MatELayer();

  /// Find the volume containing a point, with a given tolerance
  MatVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Lower Z bound
  double minZ() const {return theZMin;}

  /// Upper Z bound
  double maxZ() const {return theZMax;}

private:
  std::vector<MatVolume6Faces*> theVolumes;
  double theZMin;
  double theZMax;
};
#endif

