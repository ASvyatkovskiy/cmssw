/*
 *  See header file for a description of this class.
 *
 *  $Date: 2013/05/21 13:21:17 $
 *  $Revision: 1.33 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/matVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bMatSlab.h"
#include "MagneticField/GeomBuilder/src/bMatRod.h"
#include "MagneticField/GeomBuilder/src/bMatSector.h"
#include "MagneticField/GeomBuilder/src/bMatLayer.h"
#include "MagneticField/GeomBuilder/src/eMatSector.h"
#include "MagneticField/GeomBuilder/src/eMatLayer.h"
#include "MagneticField/GeomBuilder/src/FakeInterpolator.h"

#include "MagneticField/Layers/interface/MatBLayer.h"
#include "MagneticField/Layers/interface/MatESector.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDFilter.h"

#include "Utilities/BinningTools/interface/ClusterizingHistogram.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"

#include "MagneticField/Interpolation/interface/MFGridFactory.h"
#include "MagneticField/Interpolation/interface/MFGrid.h"

#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"
#include "MagneticField/VolumeGeometry/interface/MagExceptions.h"
#include "MagneticField/Layers/interface/MagVerbosity.h"

#include "DataFormats/GeometryVector/interface/Pi.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <iomanip>
#include <boost/algorithm/string/replace.hpp>
#include "Utilities/General/interface/precomputed_value_sort.h"


bool MatGeoBuilderFromDDD::debug;

using namespace std;


MatGeoBuilderFromDDD::MatGeoBuilderFromDDD(string tableSet_,int geometryVersion_, bool debug_) :
  tableSet (tableSet_),
  geometryVersion(geometryVersion_)
{  
  debug = debug_;
  if (debug) cout << "Constructing a MatGeoBuilderFromDDD" <<endl;
}

MatGeoBuilderFromDDD::~MatGeoBuilderFromDDD(){
  for (handles::const_iterator i=bVolumes.begin();
       i!=bVolumes.end(); ++i){
    delete (*i);
  }
  
  for (handles::const_iterator i=eVolumes.begin();
       i!=eVolumes.end(); ++i){
    delete (*i);
  }
}


void MatGeoBuilderFromDDD::summary(handles & volumes){  
  // The final countdown.
  int ivolumes  = volumes.size();  // number of volumes
  int isurfaces = ivolumes*6;       // number of individual surfaces
  int iassigned = 0;                // How many have been assigned
  int iunique   = 0;                // number of unique surfaces
  int iref_ass  = 0;
  int iref_nass = 0;

  set<const void *> ptrs;

  handles::const_iterator first = volumes.begin();
  handles::const_iterator last = volumes.end();

  for (handles::const_iterator i=first; i!=last; ++i){
    if (int((*i)->shape())>4) continue; // FIXME: implement test for missing shapes...
    for (int side = 0; side < 6; ++side) {
      int references = 	(*i)->references(side);
      if ((*i)->isPlaneMatched(side)) {
	++iassigned;
	bool firstOcc = (ptrs.insert(&((*i)->surface(side)))).second;
	if (firstOcc) iref_ass+=references;
	//if (references<2){  
	//  cout << "*** Only 1 ref, vol: " << (*i)->volumeno << " # "
	//       << (*i)->copyno << " side: " << side << endl;
	//}	
      } else {
	iref_nass+=references;
	if (references>1){
	  cout << "*** Ref_nass >1 " <<endl;
	}
      }
    }
  }
  iunique = ptrs.size();

  cout << "    volumes   " << ivolumes  << endl
       << "    surfaces  " << isurfaces << endl
       << "    assigned  " << iassigned << endl
       << "    unique    " << iunique << endl
       << "    iref_ass  " << iref_ass << endl
       << "    iref_nass " << iref_nass << endl;
}


void MatGeoBuilderFromDDD::build(const DDCompactView & cpva)
{
//    DDCompactView cpv;
  DDExpandedView fv(cpva);

  if (debug) cout << "**********************************************************" <<endl;

  // Counter of different volumes
  int bVolCount = 0;
  int eVolCount = 0;

  if (fv.logicalPart().name().name()!="MAGF") {
     std::string topNodeName(fv.logicalPart().name().name());

     //see if one of the children is MAGF
     bool doSubDets = fv.firstChild();
     
     bool go=true;
     while(go&& doSubDets) {
	if (fv.logicalPart().name().name()=="MAGF")
	   break;
	else
	   go = fv.nextSibling();
     }
     if (!go) {
	throw cms::Exception("NoMAGFinDDD")<<" Neither the top node, nor any child node of the DDCompactView is \"MAGF\" but the top node is instead \""<<topNodeName<<"\"";
     }
  }
  // Loop over MAGF volumes and create volumeHandles. 
  if (debug) { cout << endl << "*** MAGF: " << fv.geoHistory() << endl
		    << "translation: " << fv.translation() << endl
		    << " rotation: " << fv.rotation() << endl;
  }
  
  bool doSubDets = fv.firstChild();
  while (doSubDets){
    
    string name = fv.logicalPart().name().name();
    if (debug) cout << endl << "Name: " << name << endl
			       << "      " << fv.geoHistory() <<endl;
    
    // FIXME: single-volyme cylinders - this is feature has been removed 
    //        and should be revisited.
    //    bool mergeCylinders=false;

    // If true, In the barrel, cylinders sectors will be skipped to build full 
    // cylinders out of sector copyno #1.
    bool expand = false;

    matVolumeHandle* v = new matVolumeHandle(fv, expand);

    // Select volumes, build volume handles.
    float Z = v->center().z();
    float R = v->center().perp();

    // v 85l: Barrel is everything up to |Z| = 661.0, excluding 
    // volume #7, centered at 6477.5
    // v 1103l: same numbers work fine. #16 instead of #7, same coords;
    // see comment below for V6,7
    //ASSUMPTION: no misalignment is applied to mag volumes.
    //FIXME: implement barrel/endcap flags as DDD SpecPars.
    if ((fabs(Z)<647. || (R>350. && fabs(Z)<662.)) &&
	!(fabs(Z)>480 && R<172) // in 1103l we place V_6 and V_7 in the 
	                        // endcaps to preserve nice layer structure
	                        // in the barrel. This does not hurt in v85l
	                        // where there is a single V1 
	) { // Barrel
      if (debug) cout << " (Barrel)" <<endl;
      bVolumes.push_back(v);


      // Build the interpolator of the "master" volume (the one which is
      // not replicated in phi)
      // ASSUMPTION: copyno == sector.
      if (v->copyno==v->masterSector) {
	++bVolCount;
      }
    } else {               // Endcaps
      if (debug) cout << " (Endcaps)" <<endl;
      eVolumes.push_back(v);
      if (v->copyno==v->masterSector) { 
	++eVolCount;
      }
    }

    doSubDets = fv.nextSibling(); // end of loop over MAGF
  }
    
  if (debug) {
    cout << "Number of volumes (barrel): " << bVolumes.size() <<endl
		  << "Number of volumes (endcap): " << eVolumes.size() <<endl;
    cout << "**********************************************************" <<endl;
  }

  // Now all volumeHandles are there, and parameters for each of the planes
  // are calculated.

  //----------------------------------------------------------------------
  // Print summary information

  //if (debug) {
    cout << "-----------------------" << endl;
    cout << "Mat SUMMARY: Barrel " << endl;
    summary(bVolumes);
    
    cout << endl << "Mat SUMMARY: Endcaps " << endl;
    summary(eVolumes);
    cout << "-----------------------" << endl;
  //}


  //----------------------------------------------------------------------
  // Find barrel layers.

  vector<bMatLayer> layers; // the barrel layers
  precomputed_value_sort(bVolumes.begin(), bVolumes.end(), ExtractRN());

  // Find the layers (in R)
  const float resolution = 1.; // cm
  float rmin = bVolumes.front()->RN()-resolution;
  float rmax = bVolumes.back()->RN()+resolution;
  ClusterizingHistogram  hisR( int((rmax-rmin)/resolution) + 1, rmin, rmax);

  if (debug) cout << " R layers: " << rmin << " " << rmax << endl;

  handles::const_iterator first = bVolumes.begin();
  handles::const_iterator last = bVolumes.end();  

  for (handles::const_iterator i=first; i!=last; ++i){
    hisR.fill((*i)->RN());
  }
  vector<float> rClust = hisR.clusterize(resolution);

  handles::const_iterator ringStart = first;
  handles::const_iterator separ = first;

  for (unsigned int i=0; i<rClust.size() - 1; ++i) {
    if (debug) cout << " Layer at RN = " << rClust[i];
    float rSepar = (rClust[i] + rClust[i+1])/2.f;
    while ((*separ)->RN() < rSepar) ++separ;

    bMatLayer thislayer(ringStart, separ);
    layers.push_back(thislayer);
    ringStart = separ;
  }
  {
    if (debug) cout << " Layer at RN = " << rClust.back();
    bMatLayer thislayer(separ, last);
    layers.push_back(thislayer);
  }

  if (debug) cout << "Barrel: Found " << rClust.size() << " clusters in R, "
		  << layers.size() << " layers " << endl << endl;


  //----------------------------------------------------------------------
  // Find endcap sectors
  vector<eMatSector> sectors; // the endcap sectors

  // Find the number of sectors (should be 12 or 24 depending on the geometry model)
  float phireso = 0.05; // rad
  ClusterizingHistogram hisPhi( int((Geom::ftwoPi())/phireso) + 1,
				-Geom::fpi(), Geom::fpi());
  
  for (handles::const_iterator i=eVolumes.begin(); i!=eVolumes.end(); ++i){
    hisPhi.fill((*i)->minPhi());
  }
  vector<float> phiClust = hisPhi.clusterize(phireso);
  int nESectors = phiClust.size();
  if (debug && (nESectors%12)!=0) cout << "ERROR: unexpected # of sectors: " << nESectors << endl;

  //Sort in phi
  precomputed_value_sort(eVolumes.begin(), eVolumes.end(), ExtractPhi());

  //Group volumes in sectors
  for (int i = 0; i<nESectors; ++i) {
    int offset = eVolumes.size()/nESectors;
    if (debug) cout << " Sector at phi = "
		    << (*(eVolumes.begin()+((i)*offset)))->center().phi()
		    << endl;
    sectors.push_back(eMatSector(eVolumes.begin()+((i)*offset),
			      eVolumes.begin()+((i+1)*offset)));
  }
   
  if (debug) cout << "Endcap: Found " 
		  << sectors.size() << " sectors " << endl;


  //----------------------------------------------------------------------  
  // Match surfaces.

//  cout << "------------------" << endl << "Now associating planes..." << endl;

//   // Loop on layers
//   for (vector<bMatLayer>::const_iterator ilay = layers.begin();
//        ilay!= layers.end(); ++ilay) {
//     cout << "On Layer: " << ilay-layers.begin() << " RN: " << (*ilay).RN()
// 	 <<endl;     

//     // Loop on wheels
//     for (vector<bWheel>::const_iterator iwheel = (*ilay).wheels.begin();
// 	 iwheel != (*ilay).wheels.end(); ++iwheel) {
//       cout << "  On Wheel: " << iwheel- (*ilay).wheels.begin()<< " Z: "
// 	   << (*iwheel).minZ() << " " << (*iwheel).maxZ() << " " 
// 	   << ((*iwheel).minZ()+(*iwheel).maxZ())/2. <<endl;

//       // Loop on sectors.
//       for (int isector = 0; isector<12; ++isector) {
// 	// FIXME: create new constructor...
// 	bMatSectorNavigator navy(layers,
// 			      ilay-layers.begin(),
// 			      iwheel-(*ilay).wheels.begin(),isector);
	
// 	const bMatSector & isect = (*iwheel).sector(isector);
	
// 	isect.matchPlanes(navy); //FIXME refcount
//       }
//     }
//   }


  //----------------------------------------------------------------------
  // Build MatVolumes and the MatGeometry hierarchy.

  //--- Barrel

  // Build MatVolumes and associate interpolators to them
  buildMatVolumes(bVolumes);

  // Build MatBLayers
  for (vector<bMatLayer>::const_iterator ilay = layers.begin();
       ilay!= layers.end(); ++ilay) {
    mBLayers.push_back((*ilay).buildMatBLayer());
  }

  if (debug) {  
    cout << "*** BARREL ********************************************" << endl
	 << "Number of different volumes   = " << bVolCount << endl
	 //<< "Number of interpolators built = " << bInterpolators.size() << endl
    	 << "Number of MatBLayers built    = " << mBLayers.size() << endl;

    testInside(bVolumes); // FIXME: all volumes should be checked in one go.
  }
  
  //--- Endcap
  // Build MatVolumes  and associate interpolators to them
  buildMatVolumes(eVolumes);

  // Build the MatESectors
  for (vector<eMatSector>::const_iterator isec = sectors.begin();
       isec!= sectors.end(); ++isec) {
    mESectors.push_back((*isec).buildMatESector());
  }

  if (debug) {
    cout << "*** ENDCAP ********************************************" << endl
	 << "Number of different volumes   = " << eVolCount << endl
	 //<< "Number of interpolators built = " << eInterpolators.size() << endl
    	 << "Number of MatESector built    = " << mESectors.size() << endl;

    testInside(eVolumes); // FIXME: all volumes should be checked in one go.
  }
}


void MatGeoBuilderFromDDD::buildMatVolumes(const handles & volumes) { //, map<string, MagProviderInterpol*> & interpolators) {

  // Build all MagVolumes setting the MagProviderInterpol
  for (handles::const_iterator vol=volumes.begin(); vol!=volumes.end();
       ++vol){

    const GloballyPositioned<float> * gpos = (*vol)->placement();
    (*vol)->matVolume = new MatVolume6Faces(gpos->position(),
                                            gpos->rotation(),
                                            (*vol)->shape(),
                                            (*vol)->sides(),(*vol)->getMaterialType());

    //FIXME SAV
    (*vol)->matVolume->setMaterialType((*vol)->getMaterialType().name().name());

    // The name and sector of the volume are saved for debug purposes only. They may be removed at some point...
    (*vol)->matVolume->name = (*vol)->name;
    (*vol)->matVolume->copyno = (*vol)->copyno;
  }
}


void MatGeoBuilderFromDDD::testInside(handles & volumes) {
  // test inside() for all volumes.
  cout << "--------------------------------------------------" << endl;
  cout << " inside(center) test" << endl;
  for (handles::const_iterator vol=volumes.begin(); vol!=volumes.end();
       ++vol){
    for (handles::const_iterator i=volumes.begin(); i!=volumes.end();
	 ++i){
      if ((*i)==(*vol)) continue;
      //if ((*i)->matVolume == 0) continue;
      //if ((*i)->matVolume->inside((*vol)->center())) {
	//cout << "*** ERROR: center of V " << (*vol)->volumeno << " is inside V " 
	//     << (*i)->volumeno <<endl;
      //}
    }
    
    //if ((*vol)->matVolume->inside((*vol)->center())) {
    //  cout << "V " << (*vol)->volumeno << " OK " << endl;
    //} else {
    //  cout << "*** ERROR: center of volume is not inside it, "
//	   << (*vol)->volumeno << endl;
 //   }
  }
  cout << "--------------------------------------------------" << endl;
}


vector<MatBLayer*> MatGeoBuilderFromDDD::barrelLayers() const{
  return mBLayers;
}

vector<MatESector*> MatGeoBuilderFromDDD::endcapSectors() const{
  return mESectors;
}

vector<MatVolume6Faces*> MatGeoBuilderFromDDD::barrelVolumes() const{
  vector<MatVolume6Faces*> v;
  v.reserve(bVolumes.size());
  for (handles::const_iterator i=bVolumes.begin();
       i!=bVolumes.end(); ++i){
    v.push_back((*i)->matVolume);
  }
  return v;
}

vector<MatVolume6Faces*> MatGeoBuilderFromDDD::endcapVolumes() const{
  vector<MatVolume6Faces*> v;
  v.reserve(eVolumes.size());
  for (handles::const_iterator i=eVolumes.begin();
       i!=eVolumes.end(); ++i){
    v.push_back((*i)->matVolume);
  }
  return v;
}


float MatGeoBuilderFromDDD::maxR() const{
  //FIXME: should get it from the actual geometry
  return 900.;
}

float MatGeoBuilderFromDDD::maxZ() const{
  //FIXME: should get it from the actual geometry
  if (geometryVersion>=120812) return 2000.;
  else return 1600.;
}
