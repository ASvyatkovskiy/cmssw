#ifndef eMatLayer_H
#define eMatLayer_H

/** \class MagGeoBuilderFromDDD::eMatLayer
 *  A layer of volumes in an endcap sector.
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bMatSector.h"

class MatELayer;

class MatGeoBuilderFromDDD::eMatLayer {
public:
  /// Constructor from list of volumes
  eMatLayer(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~eMatLayer();

//   /// Return the list of all volumes.
//   const handles & volumes() const {return theVolumes;}

  /// Construct the MatELayer upon request.
  MatELayer * buildMatELayer() const;

private:
  handles theVolumes;  // pointer to all volumes in this layer
  mutable MatELayer * mlayer;
};
#endif

