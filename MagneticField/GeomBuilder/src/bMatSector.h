#ifndef bMatSector_H
#define bMatSector_H

/** \class MagGeoBuilderFromDDD::bMatSector
 *  A sector of volumes in a barrel layer (i.e. only 1 element in R)
 *  One sector is composed of 1 or more rods.
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bMatRod.h"

class MatBSector;

class MatGeoBuilderFromDDD::bMatSector {
public:
  /// Default ctor is needed to have arrays.
  bMatSector();

  /// Constructor from list of volumes
  bMatSector(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bMatSector();

  /// Distance  from center along normal of sectors.
  const float RN() const {
    return volumes.front()->RN();
  }

  /// Return all volumes in this sector
  const handles & getVolumes() const {return volumes;}

  /// Construct the MatBSector upon request.
  MatBSector* buildMatBSector() const;

private:
  std::vector<bMatRod> rods; // the rods in this layer
  handles volumes;   // pointers to all volumes in the sector
  mutable MatBSector* msector;
};
#endif
