#ifndef bMatLayer_H
#define bMatLayer_H

/** \class MagGeoBuilderFromDDD::bMatLayer
 *  A layer of barrel volumes. Holds a list of volumes and 12 sectors.
 *  It is assumed that the geometry is 12-fold periodic in phi!
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bMatSector.h"

class MatBLayer;

class MatGeoBuilderFromDDD::bMatLayer {
public:
  /// Constructor from list of volumes
  bMatLayer(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bMatLayer();

  /// Distance  from center along normal of sectors.
  const float RN() const {
    return theVolumes.front()->RN();
  }

  /// Return the list of all volumes.
  const handles & volumes() const {return theVolumes;}

  /// Return sector at i (handling periodicity)
  //   const bSector & sector(int i) const;

  /// Min R (conservative guess).
  double minR() const;

  // Depends on volumeHandle::maxR(), which actually returns max RN.
  // (should be changed?)
  // double maxR() const;

  /// Construct the MagBLayer upon request.
  MatBLayer * buildMatBLayer() const;

private:
  int size; //< the number of volumes

  // Check periodicity;
  int bin(int i) const;

  std::vector<bMatSector> sectors; // the sectors in this layer
  handles theVolumes;  // pointer to all volumes in this layer

  mutable MatBLayer * mlayer;
};
#endif

