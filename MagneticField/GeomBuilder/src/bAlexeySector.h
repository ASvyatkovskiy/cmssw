#ifndef bAlexeySector_H
#define bAlexeySector_H

/** \class MagGeoBuilderFromDDD::bAlexeySector
 *  A sector of volumes in a barrel layer (i.e. only 1 element in R)
 *  One sector is composed of 1 or more rods.
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bAlexeyRod.h"

class AlexeyBSector;

class AlexeyGeoBuilderFromDDD::bAlexeySector {
public:
  /// Default ctor is needed to have arrays.
  bAlexeySector();

  /// Constructor from list of volumes
  bAlexeySector(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bAlexeySector();

  /// Distance  from center along normal of sectors.
  const float RN() const {
    return volumes.front()->RN();
  }

  /// Return all volumes in this sector
  const handles & getVolumes() const {return volumes;}

  /// Construct the AlexeyBSector upon request.
  AlexeyBSector* buildAlexeyBSector() const;

private:
  std::vector<bAlexeyRod> rods; // the rods in this layer
  handles volumes;   // pointers to all volumes in the sector
  mutable AlexeyBSector* msector;
};
#endif
