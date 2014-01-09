// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:33 $
 *  $Revision: 1.4 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/AlexeyBLayer.h"
#include "MagneticField/Layers/interface/AlexeyBSector.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"
#include "Utilities/BinningTools/interface/PeriodicBinFinderInPhi.h"
#include "DataFormats/GeometryVector/interface/Phi.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"

#include <iostream>

using namespace std;

AlexeyBLayer::AlexeyBLayer(vector<AlexeyBSector*>& sectors, double rMin) :
  theSectors(sectors),
  theSingleVolume(0),
  theRMin(rMin),
  theBinFinder(0)
{
  // TOFIX
//   if (verbose.debugOut) cout << "Building AlexeyBLayer with " << theSectors.size()
// 		  << " sectors, minR " << theRMin << endl;
  //FIXME: PeriodicBinFinderInPhi gets *center* of first bin
  theBinFinder = new PeriodicBinFinderInPhi<float>(theSectors.front()->minPhi()+Geom::pi()/12.,12);

}

/// Constructor for a trivial layer consisting of one single volume.
AlexeyBLayer::AlexeyBLayer(AlexeyVolume6Faces* aVolume, double rMin) :
  theSingleVolume(0),
  theRMin(rMin),
  theBinFinder(0) 
{
  // TOFIX
//   if (verbose.debugOut) cout << "Building AlexeyBLayer with " << 0
// 		  << " sectors, minR " << theRMin << endl;    
}



AlexeyBLayer::~AlexeyBLayer() {
  delete theBinFinder;
  
  delete theSingleVolume;

  for (vector<AlexeyBSector *>::const_iterator isec = theSectors.begin();
       isec != theSectors.end(); ++isec) {
    delete (*isec);
  }
}


AlexeyVolume6Faces* AlexeyBLayer::findVolume(const GlobalPoint & gp, double tolerance) const {
  AlexeyVolume6Faces * result = 0;

  //In case the layer is composed of a single volume...
  if (theSingleVolume) {
    // TOFIX
//     if (verbose.debugOut) cout << "   Trying the unique volume " << endl;
    if (theSingleVolume->inside(gp, tolerance)) {
      result = theSingleVolume;
    // TOFIX
//       if (verbose.debugOut) cout << "***In unique bsector"
// 		  << (result==0? " failed " : " OK ") <<endl;
    }
    return result;
  }

  // Normal cases - query the sectors.
  
  Geom::Phi<float> phi = gp.phi();

  // FIXME assume sectors are sorted in phi!
  int bin= theBinFinder->binIndex(phi);
    // TOFIX
  if (verbose::debugOut) cout << "   Trying sector at phi " << theSectors[bin]->minPhi()
			      << " " << phi << endl ;
  result = theSectors[bin]->findVolume(gp, tolerance);
    // TOFIX
  if (verbose::debugOut) cout << "***In guessed bsector"
			     << (result==0? " failed " : " OK ") <<endl;

  if (result==0) { // If fails, can be in previous bin.
    // TOFIX
    if (verbose::debugOut) cout << "   Trying sector at phi "
			       << theSectors[theBinFinder->binIndex(bin-1)]->minPhi()
			       << " " << phi << endl ;
    
    result = theSectors[theBinFinder->binIndex(bin-1)]->findVolume(gp, tolerance);
    // TOFIX
    if (verbose::debugOut) cout << "***In previous bsector"
			       << (result==0? " failed " : " OK ") <<endl;

  }
  return result;

}



