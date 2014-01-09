// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/02/03 16:19:08 $
 *  $Revision: 1.4 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/eAlexeySector.h"
#include "Utilities/BinningTools/interface/ClusterizingHistogram.h"
#include "MagneticField/Layers/interface/AlexeyESector.h"
#include "MagneticField/Layers/interface/MagVerbosity.h"

#include <algorithm>
#include "Utilities/General/interface/precomputed_value_sort.h"

using namespace SurfaceOrientation;
using namespace std;

// The ctor is in charge of finding layers inside the sector.
AlexeyGeoBuilderFromDDD::eAlexeySector::eAlexeySector(handles::const_iterator begin,
				       handles::const_iterator end) :
  theVolumes(begin,end),
  msector(0)
{
  //FIXME!!!
  //precomputed_value_sort(theVolumes.begin(), theVolumes.end(), ExtractAbsZ());
  precomputed_value_sort(theVolumes.begin(), theVolumes.end(), ExtractZ());
  

  // Clusterize in Z
  const float resolution = 1.; // cm //FIXME ??
  float zmin = theVolumes.front()->center().z()-resolution;
  float zmax = theVolumes.back()->center().z()+resolution;
  ClusterizingHistogram  hisZ( int((zmax-zmin)/resolution) + 1, zmin, zmax);

  if (AlexeyGeoBuilderFromDDD::debug) cout << "     Z layers: " << zmin << " " << zmax << endl;

  handles::const_iterator first = theVolumes.begin();
  handles::const_iterator last = theVolumes.end();  

  for (handles::const_iterator i=first; i!=last; ++i){
    hisZ.fill((*i)->center().z());
  }
  vector<float> zClust = hisZ.clusterize(resolution);

  if (AlexeyGeoBuilderFromDDD::debug) cout << "     Found " << zClust.size() << " clusters in Z, "
		  << " layers: " << endl;

  handles::const_iterator layStart = first;
  handles::const_iterator separ = first;

  for (unsigned int i=0; i<zClust.size() - 1; ++i) {
    float zSepar = (zClust[i] + zClust[i+1])/2.f;
    while ((*separ)->center().z() < zSepar) ++separ;
    if (AlexeyGeoBuilderFromDDD::debug) {
      cout << "     Layer at: " << zClust[i]
	   << " elements: " << separ-layStart << " unique volumes: ";
      alexeyVolumeHandle::printUniqueNames(layStart, separ);
    }
    
    layers.push_back(eAlexeyLayer(layStart, separ));
    layStart = separ;
  }
  {
    if (AlexeyGeoBuilderFromDDD::debug) {
      cout << "     Layer at: " << zClust.back() <<" elements: " << last-separ
	   << " unique volumes: ";
      alexeyVolumeHandle::printUniqueNames(separ,last);
    }
    layers.push_back(eAlexeyLayer(separ, last));
  }

  // FIXME: Check that all layers have the same dz?. 
  
}


AlexeyGeoBuilderFromDDD::eAlexeySector::~eAlexeySector(){}


AlexeyESector* AlexeyGeoBuilderFromDDD::eAlexeySector::buildAlexeyESector() const{
  if (msector==0) {
    vector<AlexeyELayer*> mLayers;
    for (vector<eAlexeyLayer>::const_iterator lay = layers.begin();
	 lay!=layers.end(); ++lay) {
      mLayers.push_back((*lay).buildAlexeyELayer());
    }
    msector = new AlexeyESector(mLayers, theVolumes.front()->minPhi()); //FIXME
  }
  return msector;
}
