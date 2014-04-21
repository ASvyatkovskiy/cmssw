// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2005/09/26 14:47:13 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/eMatLayer.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"
#include "MagneticField/Layers/interface/MatELayer.h"

#include "Utilities/General/interface/precomputed_value_sort.h"

using namespace SurfaceOrientation;
using namespace std;

//The ctor is in charge of finding sectors inside the layer.
MatGeoBuilderFromDDD::eMatLayer::eMatLayer(handles::const_iterator begin,
					handles::const_iterator end) :
  theVolumes(begin,end),
  mlayer(0) 
{
  //  bool debug=MatGeoBuilderFromDDD::debug;

  // Sort in R  
  precomputed_value_sort(theVolumes.begin(), theVolumes.end(), ExtractR());

//   if (debug) {
//     cout << " elements: " << theVolumes.size() << " unique volumes: ";
//     volumeHandle::printUniqueNames(theVolumes.begin(), theVolumes.end());
//   }
}

MatGeoBuilderFromDDD::eMatLayer::~eMatLayer(){}

// double MatGeoBuilderFromDDD::eMatLayer::minR() const {
//   // ASSUMPTION: a layer is only 1 volume thick (by construction). 
//   return theVolumes.front()->minR();
// }

// double MatGeoBuilderFromDDD::eMatLayer::maxR() const {
//   // ASSUMPTION: a layer is only 1 volume thick (by construction). 
//   return theVolumes.front()->maxR();
// }

MatELayer * MatGeoBuilderFromDDD::eMatLayer::buildMatELayer() const {
  if (mlayer==0) {
    //FIXME not guaranteed that all volumes in layer have the same zmin
    // and zmax!
    double zmin = 1e19;
    double zmax = -1e19;
    vector<MatVolume6Faces*> mVols;
    for (handles::const_iterator vol = theVolumes.begin();
	 vol!=theVolumes.end(); ++vol) {
      mVols.push_back((*vol)->matVolume);
      zmin = min(zmin, (*vol)->minZ());
      zmax = max(zmax, (*vol)->maxZ());
    }
    mlayer = new MatELayer(mVols, zmin, zmax);
  }
  return mlayer;
}

