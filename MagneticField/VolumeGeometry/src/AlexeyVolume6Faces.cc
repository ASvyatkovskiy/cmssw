#include "MagneticField/VolumeGeometry/interface/AlexeyVolume6Faces.h"
#include "MagneticField/VolumeGeometry/interface/AlexeyVolume.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"

AlexeyVolume6Faces::AlexeyVolume6Faces( const PositionType& pos,
				  const RotationType& rot, 
				  DDSolidShape shape,
				  const std::vector<VolumeSide>& faces, DDMaterial material)
  : AlexeyVolume(pos,rot,shape,material),  copyno(0), theFaces(faces)
{}

bool AlexeyVolume6Faces::inside( const GlobalPoint& gp, double tolerance) const 
{

  // check if the point is on the correct side of all delimiting surfaces
  for (std::vector<VolumeSide>::const_iterator i=theFaces.begin(); i!=theFaces.end(); ++i) {
    Surface::Side side = i->surface().side( gp, tolerance);
    if ( side != i->surfaceSide() && side != SurfaceOrientation::onSurface) return false;
  }
  return true;
}
