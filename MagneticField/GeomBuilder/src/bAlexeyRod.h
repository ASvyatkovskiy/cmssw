#ifndef bAlexeyRod_H
#define bAlexeyRod_H

/** \class MagGeoBuilderFromDDD::bAlexeyRod
 *  A rod of volumes in a barrel sector.
 *  A rod is made of several "slabs".
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bAlexeySlab.h"

class AlexeyBRod;

class AlexeyGeoBuilderFromDDD::bAlexeyRod {
public:
  /// Constructor from list of volumes
  bAlexeyRod(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bAlexeyRod();
 
  /// Distance from center along sector normal.
  const float RN() const {
    return volumes.front()->RN();
  }

  /// Construct the AlexeyBRod upon request.
  AlexeyBRod* buildAlexeyBRod() const;

private:
  std::vector<bAlexeySlab> slabs;
  handles volumes; // pointers to all volumes in the rod
  mutable AlexeyBRod* mrod;
};

#endif
