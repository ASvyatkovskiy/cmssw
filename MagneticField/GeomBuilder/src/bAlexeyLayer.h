#ifndef bAlexeyLayer_H
#define bAlexeyLayer_H

/** \class MagGeoBuilderFromDDD::bAlexeyLayer
 *  A layer of barrel volumes. Holds a list of volumes and 12 sectors.
 *  It is assumed that the geometry is 12-fold periodic in phi!
 *
 *  $Date: 2005/09/06 15:48:28 $
 *  $Revision: 1.1 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bAlexeySector.h"

class AlexeyBLayer;

class AlexeyGeoBuilderFromDDD::bAlexeyLayer {
public:
  /// Constructor from list of volumes
  bAlexeyLayer(handles::const_iterator begin, handles::const_iterator end);

  /// Destructor
  ~bAlexeyLayer();

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
  AlexeyBLayer * buildAlexeyBLayer() const;

private:
  int size; //< the number of volumes

  // Check periodicity;
  int bin(int i) const;

  std::vector<bAlexeySector> sectors; // the sectors in this layer
  handles theVolumes;  // pointer to all volumes in this layer

  mutable AlexeyBLayer * mlayer;
};
#endif

