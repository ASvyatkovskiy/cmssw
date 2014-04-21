/*
 *  See header file for a description of this class.
 *
 *  $Date: 2010/04/20 09:56:20 $
 *  $Revision: 1.18 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/VolumeBasedEngine/interface/MatGeometry.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"
#include "MagneticField/Layers/interface/MatBLayer.h"
#include "MagneticField/Layers/interface/MatESector.h"

#include "Utilities/BinningTools/interface/PeriodicBinFinderInPhi.h"

#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include "MagneticField/Layers/interface/MagVerbosity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;
using namespace edm;

MatGeometry::MatGeometry(const edm::ParameterSet& config, std::vector<MatBLayer *> tbl,
			 std::vector<MatESector *> tes,
			 std::vector<MatVolume6Faces*> tbv,
			 std::vector<MatVolume6Faces*> tev) : 
  lastVolume(0), theBLayers(tbl), theESectors(tes), theBVolumes(tbv), theEVolumes(tev)
{

  cout << "Constructed MatGeometry" << endl;  

  cacheLastVolume = true; //config.getUntrackedParameter<bool>("cacheLastVolume", true);
  v_85l = "grid_1103l_090322_3_8t"; //(config.getParameter<std::string>("version")=="grid_85l_030919");

  vector<double> rBorders;

  for (vector<MatBLayer *>::const_iterator ilay = theBLayers.begin();
       ilay != theBLayers.end(); ++ilay) {
    if (verbose::debugOut) cout << "  Barrel layer at " << (*ilay)->minR() <<endl;
    //FIXME assume layers are already sorted in minR
    rBorders.push_back((*ilay)->minR());
  }

  theBarrelBinFinder = new MatBinFinders::GeneralBinFinderInR<double>(rBorders);

  if (verbose::debugOut) {
    for (vector<MatESector *>::const_iterator isec = theESectors.begin();
	 isec != theESectors.end(); ++isec) {
      cout << "  Endcap sector at " << (*isec)->minPhi() << endl;
    }
  }

  //FIXME assume sectors are already sorted in phi
  //FIXME: PeriodicBinFinderInPhi gets *center* of first bin
  theEndcapBinFinder = new PeriodicBinFinderInPhi<float>(theESectors.front()->minPhi()+Geom::pi()/12., 12);

}

MatGeometry::~MatGeometry(){
  delete theBarrelBinFinder;
  delete theEndcapBinFinder;

  for (vector<MatBLayer *>::const_iterator ilay = theBLayers.begin();
       ilay != theBLayers.end(); ++ilay) {
    delete (*ilay);
  }

  for (vector<MatESector *>::const_iterator ilay = theESectors.begin();
       ilay != theESectors.end(); ++ilay) {
    delete (*ilay);
  }
}

// Linear search implementation (just for testing)
MatVolume* 
MatGeometry::findVolume1(const GlobalPoint & gp, double tolerance) const {  

  MatVolume6Faces * found = 0;

  if (inBarrel(gp)) { // Barrel
    for (vector<MatVolume6Faces*>::const_iterator v = theBVolumes.begin();
	 v!=theBVolumes.end(); ++v){
      if ((*v)==0) { //FIXME: remove this check
	cout << endl << "***ERROR: MatGeometry::findVolume: MatVolume not set" << endl;
	continue;
      }
      if ((*v)->inside(gp,tolerance)) {
	found = (*v);
	break;
      }
    }

  } else { // Endcaps
    for (vector<MatVolume6Faces*>::const_iterator v = theEVolumes.begin();
	 v!=theEVolumes.end(); ++v){
      if ((*v)==0) {  //FIXME: remove this check
	cout << endl << "***ERROR: MatGeometry::findVolume: MatVolume not set" << endl;
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
MatVolume* 
MatGeometry::findVolume(const GlobalPoint & gp, double tolerance) const{

  cout << "Entering MatGeometry::findVolume" << endl;

  // Check volume cache
  if (lastVolume!=0 && lastVolume->inside(gp)){
    return lastVolume;
  }

  cout << "1" << endl;

  MatVolume * result=0;
  if (inBarrel(gp)) { // Barrel

  cout << "2" << endl;

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

  cout << "3" << endl;

  } else { // Endcaps

  cout << "2b" << endl;
    Geom::Phi<float> phi = gp.phi();
    int bin = theEndcapBinFinder->binIndex(phi);
    if (verbose::debugOut) cout << "Trying endcap sector at phi "
		    << theESectors[bin]->minPhi() << " " << phi << endl ;
    result = theESectors[bin]->findVolume(gp, tolerance);
    if (verbose::debugOut) cout << "***In guessed esector "
		    << (result==0? " failed " : " OK ") <<endl;

  cout << "3b" << endl;
  }


  if (result==0 && tolerance < 0.0001) {
    // If search fails, retry with a 300 micron tolerance.
    // This is a hack for thin gaps on air-iron boundaries,
    // which will not be present anymore once surfaces are matched.
    if (verbose::debugOut) cout << "Increasing the tolerance to 0.03" <<endl;
    result = findVolume(gp, 0.03);
  }

  cout << "4" << endl;

  if (cacheLastVolume) lastVolume = result;


  if (result == 0) cout << "bad5" << endl; 
  return result;
}




bool MatGeometry::inBarrel(const GlobalPoint& gp) const {
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


bool MatGeometry::isZSymmetric() const {
  return v_85l;
}

