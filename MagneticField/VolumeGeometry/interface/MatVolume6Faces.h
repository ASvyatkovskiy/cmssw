#ifndef MatVolume6Faces_h
#define MatVolume6Faces_h

/** \class MatVolume6Faces
 *
 *  A MatVolume defined by a number of sides (surfaces)
 *  NOTE that despite the name the sides can be less (or more) than 6!!! <br>
 *
 *  inside() is implemented by checking that the given point is on the 
 *  correct side of each of the surfaces sides.
 *
 *  $Date: 2008/11/14 10:51:38 $
 *  $Revision: 1.4 $
 *  \author T. Todorov, N. Amapane
 */

#include "MagneticField/VolumeGeometry/interface/MatVolume.h"
#include "MagneticField/VolumeGeometry/interface/VolumeSide.h"


#include <vector>

//-- FIXME
#include <string>
//--

class DDMaterial;

class MatVolume6Faces : public MatVolume {
public:

  MatVolume6Faces( const PositionType& pos, const RotationType& rot, 
		   DDSolidShape shape, const std::vector<VolumeSide>& faces, DDMaterial material);

  using MatVolume::inside;
  virtual bool inside( const GlobalPoint& gp, double tolerance=0.) const;

  /// Access to volume faces
  virtual const std::vector<VolumeSide>& faces() const {return theFaces;}

  //--> These are used for debugging purposes only
  std::string name;
  char copyno;
  //<--

private:

  std::vector<VolumeSide> theFaces;

};

#endif
