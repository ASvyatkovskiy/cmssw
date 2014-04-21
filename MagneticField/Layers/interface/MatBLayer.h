#ifndef MatBLayer_H
#define MatBLayer_H

/** \class MatBLayer
 *  
 *  A barrel layer (MatBLayer) groups volumes at the same distance to
 *  the origin. It consists of either 1 single volume (a cylinder) or
 *  12 sectors in phi (MatBSector). 
 *  Each sector consists of one or more rods (MatBRods) of equal width in phi.
 *  Rods consist of one or more slabs (MatBSlab); each one consisting of one 
 *  or, in few cases, several volumes with the same lenght in Z.
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include <vector>

class MatBSector;
class MatVolume6Faces;
template <class T> class PeriodicBinFinderInPhi;

class MatBLayer {
public:
  /// Constructor
  MatBLayer(std::vector<MatBSector*>& sectors, double rMin);

  /// Constructor for a trivial layer consisting of one single volume.
  MatBLayer(MatVolume6Faces* aVolume, double rMin);

  /// Destructor
  virtual ~MatBLayer();

  /// Find the volume containing a point, with a given tolerance
  MatVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;
  
  /// Lowest radius of the layer
  double minR() const {return theRMin;}

private:
  // To support either the case of a simple one-volume layer or a
  // composite structure we have both theSectors or theSingleVolume.
  // Only one can be active at a time; not very elegant, but acceptable.
  std::vector<MatBSector*> theSectors;
  MatVolume6Faces* theSingleVolume; 
  double theRMin;

  PeriodicBinFinderInPhi<float> * theBinFinder;

};
#endif

