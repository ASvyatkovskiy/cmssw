#ifndef bMatSlab_H
#define bMatSlab_H

/** \class MagGeoBuilderFromDDD::bMatSlab
 *  One or more slabs constitute a barrel rod.
 *  In most cases, a slab is a single volume, but in few cases it consists
 *  in several volumes contiguous in phi.
 *
 *  $Date: 2005/09/27 15:15:52 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "DataFormats/GeometryVector/interface/Pi.h"

class MatBSlab;

class MatGeoBuilderFromDDD::bMatSlab {
public:
  /// Constructor from list of volumes
  bMatSlab(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bMatSlab();
 
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

  /// Construct the MatBSlab upon request.
  MatBSlab* buildMatBSlab() const;

private:
  handles volumes; // pointers to all volumes in the slab
  mutable MatBSlab* mslab;
};

#endif
