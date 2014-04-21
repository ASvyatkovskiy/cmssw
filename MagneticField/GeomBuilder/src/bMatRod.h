#ifndef bMatRod_H
#define bMatRod_H

/** \class MagGeoBuilderFromDDD::bMatRod
 *  A rod of volumes in a barrel sector.
 *  A rod is made of several "slabs".
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bMatSlab.h"

class MatBRod;

class MatGeoBuilderFromDDD::bMatRod {
public:
  /// Constructor from list of volumes
  bMatRod(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bMatRod();
 
  /// Distance from center along sector normal.
  const float RN() const {
    return volumes.front()->RN();
  }

  /// Construct the MatBRod upon request.
  MatBRod* buildMatBRod() const;

private:
  std::vector<bMatSlab> slabs;
  handles volumes; // pointers to all volumes in the rod
  mutable MatBRod* mrod;
};

#endif
