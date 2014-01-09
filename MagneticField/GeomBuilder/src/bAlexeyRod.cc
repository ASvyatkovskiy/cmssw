// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  $Date: 2007/03/09 14:38:23 $
 *  $Revision: 1.5 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/bAlexeyRod.h"
#include "Utilities/BinningTools/interface/ClusterizingHistogram.h"
#include "MagneticField/Layers/interface/AlexeyBRod.h"
#include "MagneticField/Layers/interface/MagVerbosity.h"
#include "Utilities/General/interface/precomputed_value_sort.h"

using namespace SurfaceOrientation;

AlexeyGeoBuilderFromDDD::bAlexeyRod::~bAlexeyRod(){}

//The ctor is in charge of finding slabs inside the rod.
AlexeyGeoBuilderFromDDD::bAlexeyRod::bAlexeyRod(handles::const_iterator begin,
					handles::const_iterator end) :
  volumes(begin,end),
  mrod(0)
{
  precomputed_value_sort(volumes.begin(), volumes.end(), ExtractZ());

  // Clusterize in Z
  const float resolution = 5.; // cm
  float zmin = volumes.front()->center().z()-resolution;
  float zmax = volumes.back()->center().z()+resolution;
  ClusterizingHistogram  hisZ( int((zmax-zmin)/resolution) + 1, zmin, zmax);

  if (AlexeyGeoBuilderFromDDD::debug) std::cout << "     Z slabs: " << zmin << " " << zmax << std::endl;

  handles::const_iterator first = volumes.begin();
  handles::const_iterator last = volumes.end();  

  for (handles::const_iterator i=first; i!=last; ++i){
    hisZ.fill((*i)->center().z());
  }
  std::vector<float> zClust = hisZ.clusterize(resolution);

  if (AlexeyGeoBuilderFromDDD::debug) std::cout << "     Found " << zClust.size() << " clusters in Z, "
		  << " slabs: " << std::endl;

  handles::const_iterator slabStart = first;
  handles::const_iterator separ = first;

  for (unsigned int i=0; i<zClust.size() - 1; ++i) {
    float zSepar = (zClust[i] + zClust[i+1])/2.f;
    while ((*separ)->center().z() < zSepar) ++separ;
    if (AlexeyGeoBuilderFromDDD::debug) {
      std::cout << "     Slab at: " << zClust[i]
	   << " elements: " << separ-slabStart << " unique volumes: ";
      alexeyVolumeHandle::printUniqueNames(slabStart, separ);
    }
    
    slabs.push_back(bAlexeySlab(slabStart, separ));
    slabStart = separ;
  }
  {
    if (AlexeyGeoBuilderFromDDD::debug) {
      std::cout << "     Slab at: " << zClust.back() <<" elements: " << last-separ
	   << " unique volumes: ";
      alexeyVolumeHandle::printUniqueNames(separ,last);
    }
    slabs.push_back(bAlexeySlab(separ, last));
  }

  // Check that all slabs have the same dphi.
  std::vector<bAlexeySlab>::const_iterator i = slabs.begin();
  Geom::Phi<float> phimax = (*i).maxPhi();
  Geom::Phi<float> phimin = (*i).minPhi();
  for (++i; i!= slabs.end(); ++i) { 
    if(fabs(phimax - (*i).maxPhi()) > 0.001 ||
       fabs(phimin - (*i).minPhi()) > 0.001){
      if (AlexeyGeoBuilderFromDDD::debug) std::cout << "*** WARNING: slabs in this rod have different dphi!" <<std::endl;
    }
  }
}



AlexeyBRod* AlexeyGeoBuilderFromDDD::bAlexeyRod::buildAlexeyBRod() const{
  if (mrod==0) {
    std::vector<AlexeyBSlab*> mSlabs;
    for (std::vector<bAlexeySlab>::const_iterator slab = slabs.begin();
	 slab!=slabs.end(); ++slab) {
      mSlabs.push_back((*slab).buildAlexeyBSlab());
    }
    mrod = new AlexeyBRod(mSlabs,slabs.front().minPhi()); //FIXME
  }
  return mrod;
}
