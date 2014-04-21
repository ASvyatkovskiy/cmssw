#ifndef eMatSector_H
#define eMatSector_H

/** \class MagGeoBuilderFromDDD::eMatSector
 *  A sector of volumes in the endcap.
 *  One sector is composed of several layers (eMatLayer)
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/eMatLayer.h"

class MatESector;

class MatGeoBuilderFromDDD::eMatSector {
public:
  /// Constructor from list of volumes
  eMatSector(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~eMatSector();

//   /// Return all volumes in this sector
//   const handles & getVolumes() const {return volumes;}

  /// Construct the MatESector upon request.
  MatESector* buildMatESector() const;

private:
  std::vector<eMatLayer> layers; // the layers in this sectors
  handles theVolumes;       // pointers to all volumes in the sector
  mutable MatESector* msector;
};
#endif
