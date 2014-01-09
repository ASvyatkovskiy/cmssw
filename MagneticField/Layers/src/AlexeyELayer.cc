// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.2 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/AlexeyELayer.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"

// #include "AlexeyneticField/AlexeyLayers/interface/AlexeyVerbosity.h"
#include <iostream>

using namespace std;


AlexeyELayer::AlexeyELayer(vector<AlexeyVolume6Faces*> volumes, double zMin, double zMax) :
  theVolumes(volumes),
  theZMin(zMin),
  theZMax(zMax)
{}

AlexeyELayer::~AlexeyELayer(){
  for (vector<AlexeyVolume6Faces *>::const_iterator ivol = theVolumes.begin();
       ivol != theVolumes.end(); ++ivol) {
    delete (*ivol);
  }
}


AlexeyVolume6Faces * 
AlexeyELayer::findVolume(const GlobalPoint & gp, double tolerance) const {
  for(vector<AlexeyVolume6Faces*>::const_iterator ivol = theVolumes.begin();
	ivol != theVolumes.end(); ++ivol) {
    // FIXME : use a binfinder
    // TOFIX
//     if (verbose.debugOut) cout << "        Trying volume "
// 		    << (static_cast<AlexeyVolume6Faces*> (*ivol))->name << endl;
    if ( (*ivol)->inside(gp,tolerance) ) return (*ivol);
  }

  return 0;
}

