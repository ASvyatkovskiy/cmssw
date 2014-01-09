#ifndef AlexeyGeoBuilderFromDDD_H
#define AlexeyGeoBuilderFromDDD_H

/** \class AlexeyGeoBuilderFromDDD
 *  Parse the XML magnetic geometry, build individual volumes and match their
 *  shared surfaces. Build MagVolume6Faces and organise them in a hierarchical
 *  structure. Build AlexeyGeometry out of it.
 *
 *  $Date: 2010/10/13 15:26:09 $
 *  $Revision: 1.13 $
 *  \author N. Amapane - INFN Torino
 */
#include "DataFormats/GeometrySurface/interface/ReferenceCounted.h" 
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include <string>
#include <vector>
#include <map>

class Surface;
class AlexeyBLayer;
class AlexeyESector;
class AlexeyVolume6Faces;
class SteppingHelixPropagator;
class VolumeBasedAlexeyESProducer;

class AlexeyGeoBuilderFromDDD  {
public:
  /// Constructor. 
  /// overrideMasterSector is a hack to allow switching between 
  /// phi-symmetric maps and maps with sector-specific tables. 
  /// It won't be necessary anymore once the geometry is decoupled from 
  /// the specification of tables, ie when tables will come from the DB.
  AlexeyGeoBuilderFromDDD(std::string tableSet_, bool debug=false, bool overrideMasterSector=false);

  /// Destructor
  virtual ~AlexeyGeoBuilderFromDDD();

  /// Get barrel layers
  std::vector<AlexeyBLayer*> barrelLayers() const;

  /// Get endcap layers
  std::vector<AlexeyESector*> endcapSectors() const;

  float maxR() const;

  float maxZ() const;  

private:
  typedef ConstReferenceCountingPointer<Surface> RCPS;

  // Build the geometry. 
  //virtual void build();
  virtual void build(const DDCompactView & cpv);


  // FIXME: only for temporary tests and debug, to be removed
  friend class AlexeyGeometry;
  friend class VolumeBasedAlexeyESProducer;
  friend class SteppingHelixPropagator; 
  friend class SteppingHelixPropagatorESProducer;

  std::vector<AlexeyVolume6Faces*> barrelVolumes() const;  
  std::vector<AlexeyVolume6Faces*> endcapVolumes() const;

  // Temporary container to manipulate volumes and their surfaces.
  class alexeyVolumeHandle;
  typedef std::vector<alexeyVolumeHandle*> handles;

  // Build all MagVolumes setting the MagProviderInterpol
  void buildAlexeyVolumes(const handles & volumes);

  // Print checksums for surfaces.
  void summary(handles & volumes);

  // Perform simple sanity checks
  void testInside(handles & volumes);

  // A layer of barrel volumes.
  class bAlexeyLayer;
  // A sector of volumes in a layer.
  class bAlexeySector;
  // A rod of volumes in a sector.
  class bAlexeyRod;
  // A slab of volumes in a rod.
  class bAlexeySlab;
  // A sector of endcap volumes.
  class eAlexeySector;  
  // A layer of volumes in an endcap sector.
  class eAlexeyLayer;
 
  
  // Extractors for precomputed_value_sort (to sort containers of volumeHandles). 
  typedef std::unary_function<const alexeyVolumeHandle*, double> uFcn;
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

  std::vector<AlexeyBLayer*> mBLayers; // Finally built barrel geometry
  std::vector<AlexeyESector*> mESectors; // Finally built barrel geometry

  std::string tableSet; // Version of the data files to be used
  int geometryVersion;  // Version of MF geometry 

  bool overrideMasterSector; // see comment on ctor

  static bool debug;//

};
#endif
