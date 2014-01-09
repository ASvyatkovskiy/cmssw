/*
 *  See header file for a description of this class.
 *
 *  $Date: 2011/10/13 16:31:37 $
 *  $Revision: 1.31 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/AlexeyGeoBuilderFromDDD.h"
#include "MagneticField/GeomBuilder/src/alexeyVolumeHandle.h"
#include "MagneticField/GeomBuilder/src/bAlexeySlab.h"
#include "MagneticField/GeomBuilder/src/bAlexeyRod.h"
#include "MagneticField/GeomBuilder/src/bAlexeySector.h"
#include "MagneticField/GeomBuilder/src/bAlexeyLayer.h"
#include "MagneticField/GeomBuilder/src/eAlexeySector.h"
#include "MagneticField/GeomBuilder/src/eAlexeyLayer.h"
#include "MagneticField/GeomBuilder/src/FakeInterpolator.h"

#include "MagneticField/Layers/interface/AlexeyBLayer.h"
#include "MagneticField/Layers/interface/AlexeyESector.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDFilter.h"

#include "Utilities/BinningTools/interface/ClusterizingHistogram.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"

#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"
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
#include "Utilities/General/interface/precomputed_value_sort.h"


bool AlexeyGeoBuilderFromDDD::debug;

using namespace std;

AlexeyGeoBuilderFromDDD::AlexeyGeoBuilderFromDDD(string tableSet_, bool debug_, bool overrideMasterSector_) :
  tableSet (tableSet_),
  geometryVersion(0),
  overrideMasterSector(overrideMasterSector_)
{  
  debug = debug_;
  if (debug) cout << "Constructing a AlexeyGeoBuilderFromDDD" <<endl;
}

AlexeyGeoBuilderFromDDD::~AlexeyGeoBuilderFromDDD(){
  for (handles::const_iterator i=bVolumes.begin();
       i!=bVolumes.end(); ++i){
    delete (*i);
  }
  
  for (handles::const_iterator i=eVolumes.begin();
       i!=eVolumes.end(); ++i){
    delete (*i);
  }
}


void AlexeyGeoBuilderFromDDD::summary(handles & volumes){  
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
	if (references<2){  
	  cout << "*** Only 1 ref, vol: " << (*i)->name << " # "
	       << (*i)->copyno << " side: " << side << endl;
	}	
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


void AlexeyGeoBuilderFromDDD::build(const DDCompactView & cpva)
{
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
  // Loop over MAGF volumes and create alexeyVolumeHandles. 
  if (debug) { cout << endl << "*** MAGF: " << fv.geoHistory() << endl
		    << "translation: " << fv.translation() << endl
		    << " rotation: " << fv.rotation() << endl;
  }
  
  bool doSubDets = fv.firstChild();
  while (doSubDets){
    
    string name = fv.logicalPart().name().name();
    if (debug) cout << endl << "Name: " << name << endl
			       << "      " << fv.geoHistory() <<endl;
    
    // If true, In the barrel, cylinders sectors will be skipped to build full 
    // cylinders out of sector copyno #1.
    bool expand = false;

    alexeyVolumeHandle* v = new alexeyVolumeHandle(fv, expand);
    if (overrideMasterSector && v->masterSector!=1) {
      v->masterSector=1;
      std::string::size_type ibeg, iend;
      ibeg = (v->alexeyFile).rfind('/');
      iend = (v->alexeyFile).size();
      v->alexeyFile = (v->alexeyFile).substr(ibeg+1,iend-ibeg-1);
    }

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
      //             Note this is not the case for obsolete grid_85l_030919 - 
      // In case in  future models this should no more be the case, can 
      // implement secotor numbers as SpecPars in the XML      
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

  // Now all alexeyVolumeHandles are there, and parameters for each of the planes
  // are calculated.

  //----------------------------------------------------------------------
  // Print summary information

  if (debug) {
    cout << "-----------------------" << endl;
    cout << "SUMMARY: Barrel " << endl;
    summary(bVolumes);
    
    cout << endl << "SUMMARY: Endcaps " << endl;
    summary(eVolumes);
    cout << "-----------------------" << endl;
  }


  //----------------------------------------------------------------------
  // Find barrel layers.

  vector<bAlexeyLayer> layers; // the barrel layers
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

    bAlexeyLayer thislayer(ringStart, separ);
    layers.push_back(thislayer);
    ringStart = separ;
  }
  {
    if (debug) cout << " Layer at RN = " << rClust.back();
    bAlexeyLayer thislayer(separ, last);
    layers.push_back(thislayer);
  }

  if (debug) cout << "Barrel: Found " << rClust.size() << " clusters in R, "
		  << layers.size() << " layers " << endl << endl;


  //----------------------------------------------------------------------
  // Find endcap sectors
  vector<eAlexeySector> sectors; // the endcap sectors

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
    sectors.push_back(eAlexeySector(eVolumes.begin()+((i)*offset),
			      eVolumes.begin()+((i+1)*offset)));
  }
   
  if (debug) cout << "Endcap: Found " 
		  << sectors.size() << " sectors " << endl;

  //----------------------------------------------------------------------
  // Build AlexeyVolumes and the AlexeyGeometry hierarchy.

  //--- Barrel

  // Build MagVolumes and associate interpolators to them
  buildAlexeyVolumes(bVolumes);

  // Build MagBLayers
  for (vector<bAlexeyLayer>::const_iterator ilay = layers.begin();
       ilay!= layers.end(); ++ilay) {
    mBLayers.push_back((*ilay).buildAlexeyBLayer());
  }

  if (debug) {  
    cout << "*** BARREL ********************************************" << endl
	 << "Number of different volumes   = " << bVolCount << endl
	 //<< "Number of interpolators built = " << bInterpolators.size() << endl
    	 << "Number of MagBLayers built    = " << mBLayers.size() << endl;

    testInside(bVolumes); // FIXME: all volumes should be checked in one go.
  }
  
  //--- Endcap
  // Build MagVolumes  and associate interpolators to them
  buildAlexeyVolumes(eVolumes);

  // Build the MagESectors
  for (vector<eAlexeySector>::const_iterator isec = sectors.begin();
       isec!= sectors.end(); ++isec) {
    mESectors.push_back((*isec).buildAlexeyESector());
  }

  if (debug) {
    cout << "*** ENDCAP ********************************************" << endl
	 << "Number of different volumes   = " << eVolCount << endl
	 //<< "Number of interpolators built = " << eInterpolators.size() << endl
    	 << "Number of MagESector built    = " << mESectors.size() << endl;

    testInside(eVolumes); // FIXME: all volumes should be checked in one go.
  }
}


void AlexeyGeoBuilderFromDDD::buildAlexeyVolumes(const handles & volumes) { //, map<string, MagProviderInterpol*> & interpolators) {

  // Build all MagVolumes setting the MagProviderInterpol
  for (handles::const_iterator vol=volumes.begin(); vol!=volumes.end();
       ++vol){

    const GloballyPositioned<float> * gpos = (*vol)->placement();
    (*vol)->alexeyVolume = new AlexeyVolume6Faces(gpos->position(),
					    gpos->rotation(),
					    (*vol)->shape(),
					    (*vol)->sides(),(*vol)->getMaterialType());

    //FIXME SAV
    (*vol)->alexeyVolume->setMaterialType((*vol)->getMaterialType().name().name());

    // The name and sector of the volume are saved for debug purposes only. They may be removed at some point...
    (*vol)->alexeyVolume->name = (*vol)->name;
    (*vol)->alexeyVolume->copyno = (*vol)->copyno;
  }
}

void AlexeyGeoBuilderFromDDD::testInside(handles & volumes) {
  // test inside() for all volumes.
  cout << "--------------------------------------------------" << endl;
  cout << " inside(center) test" << endl;
  for (handles::const_iterator vol=volumes.begin(); vol!=volumes.end();
       ++vol){
    for (handles::const_iterator i=volumes.begin(); i!=volumes.end();
	 ++i){
      if ((*i)==(*vol)) continue;
      //if ((*i)->alexeyVolume == 0) continue;
      if ((*i)->alexeyVolume->inside((*vol)->center())) {
	cout << "*** ERROR: center of " << (*vol)->name << " is inside " 
	     << (*i)->name <<endl;
      }
    }
    
    if ((*vol)->alexeyVolume->inside((*vol)->center())) {
      cout << (*vol)->name << " OK " << endl;
    } else {
      cout << "*** ERROR: center of volume is not inside it, "
	   << (*vol)->name << endl;
    }
  }
  cout << "--------------------------------------------------" << endl;
}


vector<AlexeyBLayer*> AlexeyGeoBuilderFromDDD::barrelLayers() const{
  return mBLayers;
}

vector<AlexeyESector*> AlexeyGeoBuilderFromDDD::endcapSectors() const{
  return mESectors;
}

vector<AlexeyVolume6Faces*> AlexeyGeoBuilderFromDDD::barrelVolumes() const{
  vector<AlexeyVolume6Faces*> v;
  v.reserve(bVolumes.size());
  for (handles::const_iterator i=bVolumes.begin();
       i!=bVolumes.end(); ++i){
    v.push_back((*i)->alexeyVolume);
  }
  return v;
}

vector<AlexeyVolume6Faces*> AlexeyGeoBuilderFromDDD::endcapVolumes() const{
  vector<AlexeyVolume6Faces*> v;
  v.reserve(eVolumes.size());
  for (handles::const_iterator i=eVolumes.begin();
       i!=eVolumes.end(); ++i){
    v.push_back((*i)->alexeyVolume);
  }
  return v;
}


float AlexeyGeoBuilderFromDDD::maxR() const{
  //FIXME: should get it from the actual geometry - MAGF is an option, 
  //       but that is not changed together the geometry itself 
  //       (it lives in cmsMagneticField.xml in CMSCommonData)
  return 900.;
}

float AlexeyGeoBuilderFromDDD::maxZ() const{
  //FIXME: should get it from the actual geometry - see above
  return 1600.;
}
