#ifndef MatGeoBuilderFromDDD_H
#define MatGeoBuilderFromDDD_H

/** \class MatGeoBuilderFromDDD
 *  Parse the XML magnetic geometry, build individual volumes and match their
 *  shared surfaces. Build MatVolume6Faces and organise them in a hierarchical
 *  structure. Build MatGeometry out of it.
 *
 *  $Date: 2013/04/15 16:02:44 $
 *  $Revision: 1.14 $
 *  \author N. Amapane - INFN Torino
 */
#include "DataFormats/GeometrySurface/interface/ReferenceCounted.h" 
#include "MagneticField/Interpolation/interface/MagProviderInterpol.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

class Surface;
class MatBLayer;
class MatESector;
class MatVolume6Faces;
class VolumeBasedMatneticFieldESProducer;
class SteppingHelixPropagatorESProducer;
//typedef std::map<unsigned, std::pair<std::string, int> > TableFileMap;


class MatGeoBuilderFromDDD  {
public:
  /// Constructor. 
  MatGeoBuilderFromDDD(std::string tableSet_, int geometryVersion, bool debug=false);

  /// Destructor
  virtual ~MatGeoBuilderFromDDD();

  ///  Set scaling factors for individual volumes. 
  /// "keys" is a vector of 100*volume number + sector (sector 0 = all sectors)
  /// "values" are the corresponding scaling factors 
  //void setScaling(const std::vector<int>& keys, const std::vector<double>& values);

  //void setGridFiles(const std::auto_ptr<magneticfield::TableFileMap> gridFiles);

  /// Get barrel layers
  std::vector<MatBLayer*> barrelLayers() const;

  /// Get endcap layers
  std::vector<MatESector*> endcapSectors() const;

  float maxR() const;

  float maxZ() const;  

private:
  typedef ConstReferenceCountingPointer<Surface> RCPS;

  // Build the geometry. 
  //virtual void build();
  virtual void build(const DDCompactView & cpv);


  // FIXME: only for temporary tests and debug, to be removed
  friend class TestMagVolume;
  friend class MatGeometry;
  friend class VolumeBasedMatneticFieldESProducer;
  friend class SteppingHelixPropagatorESProducer; 
  //friend class magneticfield::AutoMagneticFieldESProducer;


  std::vector<MatVolume6Faces*> barrelVolumes() const;  
  std::vector<MatVolume6Faces*> endcapVolumes() const;

  // Temporary container to manipulate volumes and their surfaces.
  class matVolumeHandle;
  typedef std::vector<matVolumeHandle*> handles;

  // Build interpolator for the volume with "correct" rotation
  //void buildInterpolator(const volumeHandle * vol, 
  //			 std::map<std::string, MagProviderInterpol*>& interpolators);

  // Build all MagVolumes setting the MagProviderInterpol
  void buildMatVolumes(const handles & volumes);

  // Print checksums for surfaces.
  void summary(handles & volumes);

  // Perform simple sanity checks
  void testInside(handles & volumes);

  // A layer of barrel volumes.
  class bMatLayer;
  // A sector of volumes in a layer.
  class bMatSector;
  // A rod of volumes in a sector.
  class bMatRod;
  // A slab of volumes in a rod.
  class bMatSlab;
  // A sector of endcap volumes.
  class eMatSector;  
  // A layer of volumes in an endcap sector.
  class eMatLayer;
 
  
  // Extractors for precomputed_value_sort (to sort containers of volumeHandles). 
  typedef std::unary_function<const matVolumeHandle*, double> uFcn;
  struct ExtractZ;
  struct ExtractAbsZ;
  struct ExtractPhi;
  struct ExtractPhiMax;
  struct ExtractR;
  struct ExtractRN;
  struct LessDPhi;
  // This one to be used only for max_element and min_element
  struct LessZ;

  handles bVolumes;  // the barrel volumes.
  handles eVolumes;  // the endcap volumes.

  std::vector<MatBLayer*> mBLayers; // Finally built barrel geometry
  std::vector<MatESector*> mESectors; // Finally built barrel geometry

  std::string tableSet; // Version of the data files to be used
  int geometryVersion;  // Version of MF geometry 

  //std::map<int, double> theScalingFactors;
  //std::auto_ptr<magneticfield::TableFileMap> theGridFiles;

  static bool debug;

};
#endif
