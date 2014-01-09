#ifndef AlexeyBRod_H
#define AlexeyBRod_H

/** \class AlexeyBRod
 *
 *  A container of volumes in the barrel. It is part of the hierarchical 
 *  organisation of barrel volumes:
 *
 *  A barrel layer (AlexeyBLayer) groups volumes at the same distance to
 *  the origin. It consists of 12 sectors in phi (AlexeyBSector). 
 *  Each sector consists of one or more rods (AlexeyBRods) of equal width in phi.
 *  Rods consist of one or more slabs (AlexeyBSlab); each one consisting of one 
 *  or, in few cases, several volumes with the same lenght in Z.
 *
 *  $Date: 2007/02/03 16:08:50 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include <vector>

class AlexeyBSlab;
class AlexeyVolume6Faces;
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Layers/src/AlexeyBinFinders.h"

class AlexeyBRod {
public:
  /// Constructor
  AlexeyBRod(std::vector<AlexeyBSlab*>& slabs, Geom::Phi<float> phiMin);

  /// Destructor
  virtual ~AlexeyBRod();

  /// Find the volume containing a point, with a given tolerance
  AlexeyVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Phi of rod start
  Geom::Phi<float> minPhi() const {return thePhiMin;}

private:
  std::vector<AlexeyBSlab*> theSlabs;
  Geom::Phi<float> thePhiMin;
  AlexeyBinFinders::GeneralBinFinderInZ<double>* theBinFinder;

};
#endif


