#ifndef eAlexeyLayer_H
#define eAlexeyLayer_H

/** \class MagGeoBuilderFromDDD::eAlexeyLayer
 *  A layer of volumes in an endcap sector.
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bAlexeySector.h"

class AlexeyELayer;

class AlexeyGeoBuilderFromDDD::eAlexeyLayer {
public:
  /// Constructor from list of volumes
  eAlexeyLayer(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~eAlexeyLayer();

//   /// Return the list of all volumes.
//   const handles & volumes() const {return theVolumes;}

  /// Construct the AlexeyELayer upon request.
  AlexeyELayer * buildAlexeyELayer() const;

private:
  handles theVolumes;  // pointer to all volumes in this layer
  mutable AlexeyELayer * mlayer;
};
#endif

