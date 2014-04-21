#ifndef MatBRod_H
#define MatBRod_H

/** \class MatBRod
 *
 *  A container of volumes in the barrel. It is part of the hierarchical 
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

class MatBSlab;
class MatVolume6Faces;
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Layers/src/MatBinFinders.h"

class MatBRod {
public:
  /// Constructor
  MatBRod(std::vector<MatBSlab*>& slabs, Geom::Phi<float> phiMin);

  /// Destructor
  virtual ~MatBRod();

  /// Find the volume containing a point, with a given tolerance
  MatVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Phi of rod start
  Geom::Phi<float> minPhi() const {return thePhiMin;}

private:
  std::vector<MatBSlab*> theSlabs;
  Geom::Phi<float> thePhiMin;
  MatBinFinders::GeneralBinFinderInZ<double>* theBinFinder;

};
#endif


