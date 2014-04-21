/* \file
 *  See header file for a description of this class.
 *
 *  $Date: 2007/03/09 14:38:23 $
 *  $Revision: 1.5 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/bMatSlab.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"
#include "MagneticField/Layers/interface/MatBSlab.h"
#include "MagneticField/Layers/interface/MagVerbosity.h"

#include "Utilities/General/interface/precomputed_value_sort.h"

using namespace SurfaceOrientation;
using namespace std;

MatGeoBuilderFromDDD::bMatSlab::~bMatSlab(){}

MatGeoBuilderFromDDD::bMatSlab::bMatSlab(handles::const_iterator begin, handles::const_iterator end) :
  volumes(begin, end),
  mslab(0)
{
  if (volumes.size() > 1) {
    // Sort volumes by dphi i.e. phi(j)-phi(i) > 0 if j>1.
    precomputed_value_sort(volumes.begin(), volumes.end(),
			   ExtractPhiMax(), LessDPhi());

  if (MatGeoBuilderFromDDD::debug) cout << "        Slab has " << volumes.size()
		  << " volumes" << endl;

    // Check that all volumes have the same dZ
    handles::const_iterator i = volumes.begin();
    float Zmax = (*i)->surface(zplus).position().z();
    float Zmin= (*i)->surface(zminus).position().z();
    for (++i; i != volumes.end(); ++i){
      const float epsilon = 0.001;      
      if (fabs(Zmax - (*i)->surface(zplus).position().z()) > epsilon ||
	  fabs(Zmin - (*i)->surface(zminus).position().z()) > epsilon) {
	if (MatGeoBuilderFromDDD::debug) cout << "*** WARNING: slabs Z coords not matching: D_Zmax = "
			<< fabs(Zmax - (*i)->surface(zplus).position().z())
			<< " D_Zmin = " 
			<< fabs(Zmin - (*i)->surface(zminus).position().z())
			<< endl;
      }
    }
  }
}

Geom::Phi<float> MatGeoBuilderFromDDD::bMatSlab::minPhi() const {
  return volumes.front()->minPhi();
}

Geom::Phi<float>  MatGeoBuilderFromDDD::bMatSlab::maxPhi() const {
  return volumes.back()->maxPhi();
}


MatBSlab * MatGeoBuilderFromDDD::bMatSlab::buildMatBSlab() const {
  if (mslab==0) {
    vector<MatVolume6Faces*> mVols;
    for (handles::const_iterator vol = volumes.begin();
	 vol!=volumes.end(); ++vol) {
      mVols.push_back((*vol)->matVolume);
    }
    mslab = new MatBSlab(mVols, volumes.front()->surface(zminus).position().z()); //FIXME
  }
  return mslab;
}
