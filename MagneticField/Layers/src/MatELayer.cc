// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/MatELayer.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"

// #include "MatneticField/MatLayers/interface/MatVerbosity.h"
#include <iostream>

using namespace std;


MatELayer::MatELayer(vector<MatVolume6Faces*> volumes, double zMin, double zMax) :
  theVolumes(volumes),
  theZMin(zMin),
  theZMax(zMax)
{}

MatELayer::~MatELayer(){
  for (vector<MatVolume6Faces *>::const_iterator ivol = theVolumes.begin();
       ivol != theVolumes.end(); ++ivol) {
    delete (*ivol);
  }
}


MatVolume6Faces * 
MatELayer::findVolume(const GlobalPoint & gp, double tolerance) const {
  for(vector<MatVolume6Faces*>::const_iterator ivol = theVolumes.begin();
	ivol != theVolumes.end(); ++ivol) {
    // FIXME : use a binfinder
    // TOFIX
//     if (verbose.debugOut) cout << "        Trying volume "
// 		    << (static_cast<MatVolume6Faces*> (*ivol))->name << endl;
    if ( (*ivol)->inside(gp,tolerance) ) return (*ivol);
  }

  return 0;
}

