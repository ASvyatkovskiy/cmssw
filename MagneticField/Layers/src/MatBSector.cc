// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:15:59 $
 *  $Revision: 1.4 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/Layers/interface/MatBSector.h"
#include "MagneticField/Layers/interface/MatBRod.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"

#include <iostream>

using namespace std;

MatBSector::MatBSector(vector<MatBRod*>& rods, Geom::Phi<float> phiMin) : 
  theRods(rods),
  thePhiMin(phiMin)
{}

MatBSector::~MatBSector(){
  for (vector<MatBRod *>::const_iterator irod = theRods.begin();
       irod != theRods.end(); ++irod) {
    delete (*irod);
  }
}

MatVolume6Faces * MatBSector::findVolume(const GlobalPoint & gp, double tolerance) const {
  MatVolume6Faces * result = 0;
  Geom::Phi<float> phi = gp.phi();

  // FIXME : use a binfinder
  for(vector<MatBRod*>::const_iterator irod = theRods.begin();
	irod != theRods.end(); ++irod) {
    // TOFIX
    //if (verbose::debugOut) 
    cout << "     Trying rod at phi " << (*irod)->minPhi()
				<< " " << phi << endl ;
    result = (*irod)->findVolume(gp, tolerance);
    if (result!=0) cout << "Result is zero" << endl;

    if (result!=0) return result;
  }

  return 0;
}
