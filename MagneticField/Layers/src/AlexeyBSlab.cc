// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.3 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/AlexeyBSlab.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"
#include <iostream>

using namespace std;

AlexeyBSlab::AlexeyBSlab(vector<AlexeyVolume6Faces*> volumes, double zMin) :
  theVolumes(volumes),
  theZMin(zMin)
{}

AlexeyBSlab::~AlexeyBSlab(){  
  for (vector<AlexeyVolume6Faces *>::const_iterator ivol = theVolumes.begin();
       ivol != theVolumes.end(); ++ivol) {
    delete (*ivol);
  }
}


AlexeyVolume6Faces* AlexeyBSlab::findVolume(const GlobalPoint & gp, double tolerance) const {
  for(vector<AlexeyVolume6Faces*>::const_iterator ivol = theVolumes.begin();
	ivol != theVolumes.end(); ++ivol) {
    // FIXME : use a binfinder
    // TOFIX
    //if (verbose::debugOut) cout << "        Trying volume "
    //			       << (static_cast<AlexeyVolume6Faces*>(*ivol))->name << endl;
    if ( (*ivol)->inside(gp,tolerance) ) return (*ivol);
  }

  return 0;
}
