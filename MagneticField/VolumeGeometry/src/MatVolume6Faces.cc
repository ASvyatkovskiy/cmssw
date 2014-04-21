#include "MagneticField/VolumeGeometry/interface/MatVolume6Faces.h"
#include "MagneticField/VolumeGeometry/interface/MatVolume.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"

MatVolume6Faces::MatVolume6Faces( const PositionType& pos,
				  const RotationType& rot, 
				  DDSolidShape shape,
				  const std::vector<VolumeSide>& faces, DDMaterial material)
  : MatVolume(pos,rot,shape,material),  copyno(0), theFaces(faces)
{}

bool MatVolume6Faces::inside( const GlobalPoint& gp, double tolerance) const 
{

  // check if the point is on the correct side of all delimiting surfaces
  for (std::vector<VolumeSide>::const_iterator i=theFaces.begin(); i!=theFaces.end(); ++i) {
    Surface::Side side = i->surface().side( gp, tolerance);
    if ( side != i->surfaceSide() && side != SurfaceOrientation::onSurface) return false;
  }
  return true;
}
