#ifndef AlexeyESector_H
#define AlexeyESector_H

/** \class AlexeyESector
 *  A sector of volumes in the endcap.
 *  One sector is composed of several layers (AlexeyELayer)
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include <vector>

class AlexeyVolume6Faces;
class AlexeyELayer; 

class AlexeyESector {
public:
  /// Constructor
  AlexeyESector(std::vector<AlexeyELayer*>& layers, Geom::Phi<float> phiMin);

  /// Destructor
  virtual ~AlexeyESector();

  /// Find the volume containing a point, with a given tolerance
  AlexeyVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Phi of sector start
  Geom::Phi<float> minPhi() const {return thePhiMin;}

private:
  std::vector<AlexeyELayer*> theLayers;
  Geom::Phi<float> thePhiMin;
};
#endif

