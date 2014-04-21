#ifndef MatBSlab_H
#define MatBSlab_H

/** \class MatBSlab
 *  
 *  A  container of volumes in the barrel. It is part of the hierarchical 
 *  organisation of barrel volumes:
 *
 *  A barrel layer (MatBLayer) groups volumes at the same distance to
 *  the origin. It consists of 12 sectors in phi (MatBSector). 
 *  Each sector consists of one or more rods (MatBRods) of equal width in phi.
 *  Rods consist of one or more slabs (MatBSlab); each one consisting of one 
 *  or, in few cases, several volumes with the same lenght in Z.
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include <vector>

class MatVolume6Faces;
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class MatBSlab {
public:
  /// Constructor
  MatBSlab(std::vector<MatVolume6Faces*> volumes, double zMin);

  /// Destructor
  virtual ~MatBSlab();

  /// Find the volume containing a point, with a given tolerance
  MatVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Lower Z bound
  double minZ() const { return theZMin;}
  
private:
  std::vector<MatVolume6Faces*> theVolumes;
  double theZMin;
  
};
#endif

