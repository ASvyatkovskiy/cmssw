// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.3 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/MatBSlab.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"
#include <iostream>

using namespace std;

MatBSlab::MatBSlab(vector<MatVolume6Faces*> volumes, double zMin) :
  theVolumes(volumes),
  theZMin(zMin)
{}

MatBSlab::~MatBSlab(){  
  for (vector<MatVolume6Faces *>::const_iterator ivol = theVolumes.begin();
       ivol != theVolumes.end(); ++ivol) {
    delete (*ivol);
  }
}


MatVolume6Faces* MatBSlab::findVolume(const GlobalPoint & gp, double tolerance) const {
  for(vector<MatVolume6Faces*>::const_iterator ivol = theVolumes.begin();
	ivol != theVolumes.end(); ++ivol) {
    // FIXME : use a binfinder
    // TOFIX
    //if (verbose::debugOut) cout << "        Trying volume "
    //			       << (static_cast<MatVolume6Faces*>(*ivol))->name << endl;
    if ( (*ivol)->inside(gp,tolerance) ) return (*ivol);
  }

  return 0;
}
