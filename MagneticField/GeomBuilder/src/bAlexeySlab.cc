/* \file
 *  See header file for a description of this class.
 *
 *  $Date: 2007/03/09 14:38:23 $
 *  $Revision: 1.5 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/bAlexeySlab.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"
#include "MagneticField/Layers/interface/AlexeyBSlab.h"
#include "MagneticField/Layers/interface/MagVerbosity.h"

#include "Utilities/General/interface/precomputed_value_sort.h"

using namespace SurfaceOrientation;
using namespace std;

AlexeyGeoBuilderFromDDD::bAlexeySlab::~bAlexeySlab(){}

AlexeyGeoBuilderFromDDD::bAlexeySlab::bAlexeySlab(handles::const_iterator begin, handles::const_iterator end) :
  volumes(begin, end),
  mslab(0)
{
  if (volumes.size() > 1) {
    // Sort volumes by dphi i.e. phi(j)-phi(i) > 0 if j>1.
    precomputed_value_sort(volumes.begin(), volumes.end(),
			   ExtractPhiMax(), LessDPhi());

  if (AlexeyGeoBuilderFromDDD::debug) cout << "        Slab has " << volumes.size()
		  << " volumes" << endl;

    // Check that all volumes have the same dZ
    handles::const_iterator i = volumes.begin();
    float Zmax = (*i)->surface(zplus).position().z();
    float Zmin= (*i)->surface(zminus).position().z();
    for (++i; i != volumes.end(); ++i){
      const float epsilon = 0.001;      
      if (fabs(Zmax - (*i)->surface(zplus).position().z()) > epsilon ||
	  fabs(Zmin - (*i)->surface(zminus).position().z()) > epsilon) {
	if (AlexeyGeoBuilderFromDDD::debug) cout << "*** WARNING: slabs Z coords not matching: D_Zmax = "
			<< fabs(Zmax - (*i)->surface(zplus).position().z())
			<< " D_Zmin = " 
			<< fabs(Zmin - (*i)->surface(zminus).position().z())
			<< endl;
      }
    }
  }
}

Geom::Phi<float> AlexeyGeoBuilderFromDDD::bAlexeySlab::minPhi() const {
  return volumes.front()->minPhi();
}

Geom::Phi<float>  AlexeyGeoBuilderFromDDD::bAlexeySlab::maxPhi() const {
  return volumes.back()->maxPhi();
}


AlexeyBSlab * AlexeyGeoBuilderFromDDD::bAlexeySlab::buildAlexeyBSlab() const {
  if (mslab==0) {
    vector<AlexeyVolume6Faces*> mVols;
    for (handles::const_iterator vol = volumes.begin();
	 vol!=volumes.end(); ++vol) {
      mVols.push_back((*vol)->alexeyVolume);
    }
    mslab = new AlexeyBSlab(mVols, volumes.front()->surface(zminus).position().z()); //FIXME
  }
  return mslab;
}
