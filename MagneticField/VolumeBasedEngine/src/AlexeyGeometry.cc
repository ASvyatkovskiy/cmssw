/*
 *  See header file for a description of this class.
 *
 *  $Date: 2010/04/20 09:56:20 $
 *  $Revision: 1.18 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/VolumeBasedEngine/interface/AlexeyGeometry.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"
#include "MagneticField/Layers/interface/AlexeyBLayer.h"
#include "MagneticField/Layers/interface/AlexeyESector.h"

#include "Utilities/BinningTools/interface/PeriodicBinFinderInPhi.h"

#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include "MagneticField/Layers/interface/MagVerbosity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;
using namespace edm;

AlexeyGeometry::AlexeyGeometry(const edm::ParameterSet& config, std::vector<AlexeyBLayer *> tbl,
			 std::vector<AlexeyESector *> tes,
			 std::vector<AlexeyVolume6Faces*> tbv,
			 std::vector<AlexeyVolume6Faces*> tev) : 
  lastVolume(0), theBLayers(tbl), theESectors(tes), theBVolumes(tbv), theEVolumes(tev)
{
  
  cacheLastVolume = config.getUntrackedParameter<bool>("cacheLastVolume", true);
  v_85l = (config.getParameter<std::string>("version")=="grid_85l_030919");

  vector<double> rBorders;

  for (vector<AlexeyBLayer *>::const_iterator ilay = theBLayers.begin();
       ilay != theBLayers.end(); ++ilay) {
    if (verbose::debugOut) cout << "  Barrel layer at " << (*ilay)->minR() <<endl;
    //FIXME assume layers are already sorted in minR
    rBorders.push_back((*ilay)->minR());
  }

  theBarrelBinFinder = new AlexeyBinFinders::GeneralBinFinderInR<double>(rBorders);

  if (verbose::debugOut) {
    for (vector<AlexeyESector *>::const_iterator isec = theESectors.begin();
	 isec != theESectors.end(); ++isec) {
      cout << "  Endcap sector at " << (*isec)->minPhi() << endl;
    }
  }

  //FIXME assume sectors are already sorted in phi
  //FIXME: PeriodicBinFinderInPhi gets *center* of first bin
  theEndcapBinFinder = new PeriodicBinFinderInPhi<float>(theESectors.front()->minPhi()+Geom::pi()/12., 12);

}

AlexeyGeometry::~AlexeyGeometry(){
  delete theBarrelBinFinder;
  delete theEndcapBinFinder;

  for (vector<AlexeyBLayer *>::const_iterator ilay = theBLayers.begin();
       ilay != theBLayers.end(); ++ilay) {
    delete (*ilay);
  }

  for (vector<AlexeyESector *>::const_iterator ilay = theESectors.begin();
       ilay != theESectors.end(); ++ilay) {
    delete (*ilay);
  }
}

/*
// Return field vector at the specified global point
GlobalVector AlexeyGeometry::fieldInTesla(const GlobalPoint & gp) const {
  AlexeyVolume * v = 0;

  // Map version 85l is Z-symmetric; -> implement Z reflection
  if (v_85l && gp.z()>0) { 
    GlobalPoint gpSym(gp.x(), gp.y(), -gp.z());
    // Check volume cache
    v = findVolume(gpSym);

    if (v!=0) {
      GlobalVector bresult = v->fieldInTesla(gpSym);
      return GlobalVector(-bresult.x(), -bresult.y(), bresult.z());
    }
    
  } else { // No reflection
    v = findVolume(gp);
    if (v!=0) {
      return v->fieldInTesla(gp);
    }
  }
  
  // Fall-back case: no volume found
  
  if (isnan(gp.mag())) {
    LogWarning("InvalidInput") << "Input value invalid (not a number): " << gp << endl;
      
  } else {
    LogWarning("AlexeyneticField") << "AlexeyGeometry::fieldInTesla: failed to find volume for " << gp << endl;
  }
  return GlobalVector();
}
*/

// Linear search implementation (just for testing)
AlexeyVolume* 
AlexeyGeometry::findVolume1(const GlobalPoint & gp, double tolerance) const {  

  AlexeyVolume6Faces * found = 0;

  if (inBarrel(gp)) { // Barrel
    for (vector<AlexeyVolume6Faces*>::const_iterator v = theBVolumes.begin();
	 v!=theBVolumes.end(); ++v){
      if ((*v)==0) { //FIXME: remove this check
	cout << endl << "***ERROR: AlexeyGeometry::findVolume: AlexeyVolume not set" << endl;
	continue;
      }
      if ((*v)->inside(gp,tolerance)) {
	found = (*v);
	break;
      }
    }

  } else { // Endcaps
    for (vector<AlexeyVolume6Faces*>::const_iterator v = theEVolumes.begin();
	 v!=theEVolumes.end(); ++v){
      if ((*v)==0) {  //FIXME: remove this check
	cout << endl << "***ERROR: AlexeyGeometry::findVolume: AlexeyVolume not set" << endl;
	continue;
      }
      if ((*v)->inside(gp,tolerance)) {
	found = (*v);
	break;
      }
    }
  }  
  
  return found;
}

// Use hierarchical structure for fast lookup.
AlexeyVolume* 
AlexeyGeometry::findVolume(const GlobalPoint & gp, double tolerance) const{
  // Check volume cache
  if (lastVolume!=0 && lastVolume->inside(gp)){
    return lastVolume;
  }

  AlexeyVolume * result=0;
  if (inBarrel(gp)) { // Barrel
    double R = gp.perp();
    int bin = theBarrelBinFinder->binIndex(R);
    
    for (int bin1 = bin; bin1 >= max(0,bin-2); --bin1) {
      if (verbose::debugOut) cout << "Trying layer at R " << theBLayers[bin1]->minR()
		      << " " << R << endl ;
      result = theBLayers[bin1]->findVolume(gp, tolerance);
      if (verbose::debugOut) cout << "***In blayer " << bin1-bin << " " 
		      << (result==0? " failed " : " OK ") <<endl;
      if (result != 0) break;
    }

  } else { // Endcaps
    Geom::Phi<float> phi = gp.phi();
    int bin = theEndcapBinFinder->binIndex(phi);
    if (verbose::debugOut) cout << "Trying endcap sector at phi "
		    << theESectors[bin]->minPhi() << " " << phi << endl ;
    result = theESectors[bin]->findVolume(gp, tolerance);
    if (verbose::debugOut) cout << "***In guessed esector "
		    << (result==0? " failed " : " OK ") <<endl;
  }


  if (result==0 && tolerance < 0.0001) {
    // If search fails, retry with a 300 micron tolerance.
    // This is a hack for thin gaps on air-iron boundaries,
    // which will not be present anymore once surfaces are matched.
    if (verbose::debugOut) cout << "Increasing the tolerance to 0.03" <<endl;
    result = findVolume(gp, 0.03);
  }

  if (cacheLastVolume) lastVolume = result;

  return result;
}




bool AlexeyGeometry::inBarrel(const GlobalPoint& gp) const {
  float Z = fabs(gp.z());
  float R = gp.perp();

  // FIXME: Get these dimensions from the builder. 
  // For this we can wait the next generation of tables, when the picture 
  // may be more complicated
  if (v_85l){
    return (Z<634.49 || (R>308.755 && Z<661.01));
  } else {
    return (Z<350. ||
	    (R>172.4 && Z<633.29) || 
	    (R>308.755 && Z<661.01));
  }
}


bool AlexeyGeometry::isZSymmetric() const {
  return v_85l;
}

