#ifndef matVolumeHandle_H
#define matVolumeHandle_H

/** \class MagGeoBuilderFromDDD::matVolumeHandle
 * A temporary container to cache info on a six-surface volume during
 * the processing. Used to sort, organise, and build shared planes.
 * One instance is created for each DDVolume. The parameters of the 
 * boundary surfaces are calculated during construction.
 *
 *  $Date: 2009/05/13 15:55:54 $
 *  $Revision: 1.9 $
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/src/MatGeoBuilderFromDDD.h"
#include "DetectorDescription/Core/interface/DDSolid.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "MagneticField/VolumeGeometry/interface/VolumeSide.h"

class DDExpandedView;
class MatVolume6Faces;

class MatGeoBuilderFromDDD::matVolumeHandle {
public:
  typedef Surface::GlobalPoint     GlobalPoint;
  typedef Surface::LocalPoint      LocalPoint;
  typedef Surface::LocalVector     LocalVector;
  typedef SurfaceOrientation::GlobalFace Sides;
  matVolumeHandle(const DDExpandedView & fv, bool expand2Pi=false);
  ~matVolumeHandle();

  /// Return the center of the volume
  const GlobalPoint & center() const;
  /// Distance of (x,y) plane from origin 
  const double RN() const {return theRN;}
  /// Get the current surface on specified side.
  const Surface & surface(int which_side) const;
  const Surface & surface(Sides which_side) const;
  /// Find out if two surfaces are the same physical surface
  bool sameSurface(const Surface & s1, Sides which_side, float tolerance = 0.01);
  /// Assign a shared surface perorming sanity checks.
  bool setSurface(const Surface & s1, Sides which_side);
  /// if the specified surface has been matched.
  bool isPlaneMatched(int which_side) const {
    return isAssigned[which_side];
  }

  int references(int which_side) const { // FIXME!
/*     return surfaces[which_side]->references(); */
    return 0;
  }

  /// Name of the volume 
  std::string name;
  /// Name of magnetic field table file
  std::string matFile;
  /// copy number
  int copyno;

  /// Just for debugging...
  static void printUniqueNames(handles::const_iterator begin,
			       handles::const_iterator end);


  // Phi ranges: Used by: LessDPhiMax; bSector; bSlab::[min|max]Phi();
  // MagBSector, MagBRod

  /// Minimum value of phi covered by the volume
  Geom::Phi<float> minPhi() const {return thePhiMin;}
  /// Maximum value of phi covered by the volume
  Geom::Phi<float> maxPhi() const {return surface(SurfaceOrientation::phiplus).position().phi();}

  /// Z limits. 
  // ASSUMPTION: Computed on median Z plane, but this is not a problem since
  // all Z planes are orthogonal to the beam line in the current geometry.
  double minZ() const {return surface(SurfaceOrientation::zminus).position().z();}
  double maxZ() const {return surface(SurfaceOrientation::zplus).position().z();}

  /// Minimum R for any point within the volume
  double minR() const {return theRMin;}

  /// FIXME: currently returns max RN (to be fixed?). Used by: bLayer::maxR()
  //  double maxR() const {return theRMax;}  

  /// Position and rotation
  const GloballyPositioned<float> * placement() const {return refPlane;}

  /// Shape of the solid
  DDSolidShape shape() const {return solid.shape();}

  /// The surfaces and they orientation, as required to build a MagVolume.
  std::vector<VolumeSide> sides() const;

  /// Pointer to the final MagVolume (must be set from outside)
  MatVolume6Faces* matVolume;

  bool toExpand() const {return expand;}

  //FIXME SAV new
  DDMaterial getMaterialType() const {return materialType;}

  /// The sector for which an interpolator for this class of volumes should be built
  int masterSector;

private:
  // Disallow Default/copy ctor & assignment op.
  // (we want to handle only pointers!!!)
  matVolumeHandle(const matVolumeHandle& v);
  matVolumeHandle operator=(const matVolumeHandle &v);

  // The volume's six surfaces.
  RCPS surfaces[6];
  // If the corresponding surface has been assigned to a shared surface.
  bool isAssigned[6];

  // initialise the refPlane
  void referencePlane(const DDExpandedView &fv);
  // Build the surfaces for a box
  void buildMatBox(const DDExpandedView & fv);
  // Build the surfaces for a trapezoid
  void buildMatTrap(const DDExpandedView & fv);
  // Build the surfaces for a ddtubs shape
  void buildMatTubs(const DDExpandedView & fv);  
  // Build the surfaces for a ddcons shape
  void buildMatCons(const DDExpandedView & fv);  
  // Build the surfaces for a ddpseudotrap shape
  void buildMatPseudoTrap(const DDExpandedView & fv);
  // Build the surfaces for a ddtrunctubs shape
  void buildMatTruncTubs(const DDExpandedView & fv);

  // Build phi, z surfaces (common for ddtubs and ddcons)
  void buildMatPhiZSurf(double startPhi, double deltaPhi, double zhalf,
		     double rCentr);

  // Distance from the origin along the normal to the volume's zphi plane.
  double theRN;
  
  // Max and min radius for _any_ point within the volume
  double theRMin;
  double theRMax;
  Geom::Phi<float> thePhiMin;

  // The refPlane is the "main plane" for the solid. It corresponds to the 
  // x,y plane in the DDD local frame, and defines a frame where the local
  // coordinates are the same as in DDD.
  GloballyPositioned<float> * refPlane;

  // the DDSolid.
  DDSolid solid;  

  // the center of the volume
  GlobalPoint center_;

  // Flag this as a master volume out of wich a 2pi volume should be built 
  // (e.g. central cylinder); this is taken into account by sides().
  bool expand;

  // Temporary hack to keep information on material. Will eventually be replaced!  
  //bool isIronFlag;
  DDMaterial materialType;
};


// Extractors for precomputed_value_sort() (safe sorting)

// To sort volumes in Z
struct MatGeoBuilderFromDDD::ExtractZ : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    return v->center().z();
  }
};

// To sort volumes in abs(Z)
struct MatGeoBuilderFromDDD::ExtractAbsZ : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    return fabs(v->center().z());
  }
};


// To sort volumes in phi (from -pi to pi).
struct MatGeoBuilderFromDDD::ExtractPhi : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    // note that Geom::Phi is implicitly converted to double.
    // Periodicity is guaranteed.
    return v->center().phi();
  }
};

// To sort volumes based on max phi(from -pi to pi).
struct MatGeoBuilderFromDDD::ExtractPhiMax : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    // note that Geom::Phi is implicitly converted to double.
    // Periodicity is guaranteed.
    return v->maxPhi();
  }  
};

// To sort volumes in R
struct MatGeoBuilderFromDDD::ExtractR : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    return v->center().perp();
  }
};

// To sort volumes in RN (distance of (x,y) plane from origin)
  struct MatGeoBuilderFromDDD::ExtractRN : public uFcn {
  double operator()(const matVolumeHandle* v) const {
    return v->RN();
  }
};


// To sort angles within any range SMALLER THAN PI "counter-clockwise",
// even if the angles cross the pi boundary.
// CAVEAT: // The result is undefined if the input values cover a 
// range larger than pi!!!
struct MatGeoBuilderFromDDD::LessDPhi{
  bool operator()(double phi1, double phi2) const {
    // handle periodicity
    return ((Geom::Phi<float>(phi2)-Geom::Phi<float>(phi1))>0.);
  }  
};

// Compare the Z of volumes.
// Should be used ONLY for std::max_element and std::min_element 
// and NEVER for sorting (use precomputed_value_sort with ExtractZ instead)
struct MatGeoBuilderFromDDD::LessZ{
  bool operator()(const matVolumeHandle * v1, const matVolumeHandle * v2) const
  {
    if (v1->center().z() < v2->center().z()) return true;
    return false;
  }
};

#endif

