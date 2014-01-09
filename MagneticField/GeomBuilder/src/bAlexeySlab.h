#ifndef bAlexeySlab_H
#define bAlexeySlab_H

/** \class MagGeoBuilderFromDDD::bAlexeySlab
 *  One or more slabs constitute a barrel rod.
 *  In most cases, a slab is a single volume, but in few cases it consists
 *  in several volumes contiguous in phi.
 *
 *  $Date: 2005/09/27 15:15:52 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "DataFormats/GeometryVector/interface/Pi.h"

class AlexeyBSlab;

class AlexeyGeoBuilderFromDDD::bAlexeySlab {
public:
  /// Constructor from list of volumes
  bAlexeySlab(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bAlexeySlab();
 
  /// Distance from center along sector normal.
  const float RN() const {
    return volumes.front()->RN();
  }

  /// Boundary in phi.
  // FIXME: use volumeHandle [max|min]Phi, which returns phi at median of
  // phi plane (not absolute limits). Used by: bRod ctor (only for dphi)
  Geom::Phi<float> minPhi() const;

  /// Boundary in phi.
  Geom::Phi<float> maxPhi() const;  

  /// Construct the AlexeyBSlab upon request.
  AlexeyBSlab* buildAlexeyBSlab() const;

private:
  handles volumes; // pointers to all volumes in the slab
  mutable AlexeyBSlab* mslab;
};

#endif
