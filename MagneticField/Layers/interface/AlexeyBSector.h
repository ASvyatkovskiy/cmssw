#ifndef AlexeyBSector_H
#define AlexeyBSector_H

/** \class AlexeyBSector
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

class AlexeyBRod;
class AlexeyVolume6Faces;
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class AlexeyBSector {
public:
  /// Constructor
  AlexeyBSector(std::vector<AlexeyBRod*>& rods, Geom::Phi<float> phiMin);

  /// Destructor
  virtual ~AlexeyBSector();  

  /// Find the volume containing a point, with a given tolerance
  AlexeyVolume6Faces * findVolume(const GlobalPoint & gp, double tolerance) const;

  /// Phi of sector start
  Geom::Phi<float> minPhi() const {return thePhiMin;}

private:
  std::vector<AlexeyBRod*> theRods;
  Geom::Phi<float> thePhiMin;
};
#endif

