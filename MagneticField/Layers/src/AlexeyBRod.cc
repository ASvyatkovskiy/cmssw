// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.3 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/AlexeyBRod.h"
#include "MagneticField/Layers/interface/AlexeyBSlab.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"

#include <iostream>

using namespace std;

AlexeyBRod::AlexeyBRod(vector<AlexeyBSlab*>& slabs, Geom::Phi<float> phiMin) :
  theSlabs(slabs),
  thePhiMin(phiMin),
  theBinFinder(0)
{
  // TOFIX
//   if (verbose.debugOut) cout << "Building AlexeyBRod with " << theSlabs.size()
// 		  << " slabs, minPhi " << thePhiMin << endl;
  
  if (theSlabs.size()>1) { // Set the binfinder
    vector<double> zBorders;
    for (vector<AlexeyBSlab *>::const_iterator islab = theSlabs.begin();
	 islab != theSlabs.end(); ++islab) {
  // TOFIX
      if (verbose::debugOut) cout << (*islab)->minZ() <<endl;
      //FIXME assume layers are already sorted in Z
      zBorders.push_back((*islab)->minZ());
    }
    theBinFinder = new AlexeyBinFinders::GeneralBinFinderInZ<double>(zBorders);
  }
}

AlexeyBRod::~AlexeyBRod() {
  delete theBinFinder;
  
  for (vector<AlexeyBSlab *>::const_iterator islab = theSlabs.begin();
       islab != theSlabs.end(); ++islab) {
    delete (*islab);
  }
}

AlexeyVolume6Faces * AlexeyBRod::findVolume(const GlobalPoint & gp, double tolerance) const {
  AlexeyVolume6Faces * result = 0;
  float Z = gp.z();

  int bin = 0;
  if (theBinFinder!=0) { // true if there is > 1 bin
    bin = theBinFinder->binIndex(Z);
  }
  
  // TOFIX
  if (verbose::debugOut) cout << "       Trying slab at Z " << theSlabs[bin]->minZ()
			      << " " << Z << endl ;
  result = theSlabs[bin]->findVolume(gp, tolerance);
  // TOFIX
  if (verbose::debugOut) cout << "***In guessed bslab"
			      << (result==0? " failed " : " OK ") <<endl;  

  return result;
}



