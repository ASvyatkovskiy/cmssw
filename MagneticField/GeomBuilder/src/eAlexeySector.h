#ifndef eAlexeySector_H
#define eAlexeySector_H

/** \class MagGeoBuilderFromDDD::eAlexeySector
 *  A sector of volumes in the endcap.
 *  One sector is composed of several layers (eAlexeyLayer)
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/eAlexeyLayer.h"

class AlexeyESector;

class AlexeyGeoBuilderFromDDD::eAlexeySector {
public:
  /// Constructor from list of volumes
  eAlexeySector(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~eAlexeySector();

//   /// Return all volumes in this sector
//   const handles & getVolumes() const {return volumes;}

  /// Construct the AlexeyESector upon request.
  AlexeyESector* buildAlexeyESector() const;

private:
  std::vector<eAlexeyLayer> layers; // the layers in this sectors
  handles theVolumes;       // pointers to all volumes in the sector
  mutable AlexeyESector* msector;
};
#endif
