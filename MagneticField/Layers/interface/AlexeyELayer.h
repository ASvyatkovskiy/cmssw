#ifndef AlexeyELayer_H
#define AlexeyELayer_H

/** \class AlexeyELayer
 *  A layer of volumes in an endcap sector.
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include <vector>

class AlexeyVolume6Faces;

class AlexeyELayer {
public:
  /// Constructor
  AlexeyELayer(std::vector<AlexeyVolume6Faces*> volumes, double zMin, double zMax);

  /// Destructor
  virtual ~AlexeyELayer();

  /// Find the volume containing a point, with a given tolerance
  AlexeyVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Lower Z bound
  double minZ() const {return theZMin;}

  /// Upper Z bound
  double maxZ() const {return theZMax;}

private:
  std::vector<AlexeyVolume6Faces*> theVolumes;
  double theZMin;
  double theZMax;
};
#endif

