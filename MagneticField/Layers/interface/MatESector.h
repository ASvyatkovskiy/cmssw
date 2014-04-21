#ifndef MatESector_H
#define MatESector_H

/** \class MatESector
 *  A sector of volumes in the endcap.
 *  One sector is composed of several layers (MatELayer)
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include <vector>

class MatVolume6Faces;
class MatELayer; 

class MatESector {
public:
  /// Constructor
  MatESector(std::vector<MatELayer*>& layers, Geom::Phi<float> phiMin);

  /// Destructor
  virtual ~MatESector();

  /// Find the volume containing a point, with a given tolerance
  MatVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Phi of sector start
  Geom::Phi<float> minPhi() const {return thePhiMin;}

private:
  std::vector<MatELayer*> theLayers;
  Geom::Phi<float> thePhiMin;
};
#endif

