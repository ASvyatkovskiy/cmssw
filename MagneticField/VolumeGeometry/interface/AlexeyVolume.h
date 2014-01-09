#ifndef AlexeyVolume_H
#define AlexeyVolume_H

#include "DataFormats/GeometrySurface/interface/GloballyPositioned.h"
#include "DetectorDescription/Core/interface/DDSolidShapes.h"
#include "MagneticField/VolumeGeometry/interface/VolumeSide.h"

#include "DetectorDescription/Core/interface/DDName.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"


#include <vector>

class AlexeyVolume : public GloballyPositioned<float> {
public:

  typedef GloballyPositioned<float>    Base;
  typedef GloballyPositioned<float>::LocalPoint     LocalPoint;
  typedef GloballyPositioned<float>::LocalVector    LocalVector;
  typedef GloballyPositioned<float>::GlobalPoint    GlobalPoint;
  typedef GloballyPositioned<float>::GlobalVector   GlobalVector;

  AlexeyVolume( const PositionType& pos, const RotationType& rot, 
	     DDSolidShape shape, DDMaterial material) :
    Base(pos,rot), theShape(shape), materialType(material) {}

  virtual ~AlexeyVolume();

  DDSolidShape shapeType() const {return theShape;}

  //FIXME not sure if we need inside
  virtual bool inside( const GlobalPoint& gp, double tolerance=0.) const = 0;
  virtual bool inside( const LocalPoint& lp, double tolerance=0.) const {
    return inside( toGlobal(lp), tolerance);
  }

  /// Access to volume faces
  virtual const std::vector<VolumeSide>& faces() const = 0;

  DDMaterial getMaterialType() const {return materialType;}
  void setMaterialType(std::string type) {
       DDName matName(type);
       DDMaterial matTmp(matName);
       materialType = matTmp;
  }

private:
  DDSolidShape theShape;

protected: 
  DDMaterial materialType;

};

#endif
