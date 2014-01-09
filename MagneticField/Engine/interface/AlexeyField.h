#ifndef MagneticField_AlexeyField_h
#define MagneticField_AlexeyField_h

/** \class MagneticField
 *
 *  Base class for the different implementation of magnetic field engines.
 *
 *  $Date: 2010/12/25 16:23:18 $
 *  $Revision: 1.9 $
 *  \author N. Amapane - CERN
 */

#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class AlexeyField
{
 public:
  AlexeyField();
  virtual ~AlexeyField();

  /// Derived classes can implement cloning without ownership of the 
  /// underlying engines.
  virtual AlexeyField* clone() const {
    return 0;
  }
  

  /// Field value ad specified global point, in Tesla
  //virtual GlobalVector inTesla (const GlobalPoint& gp) const = 0;

  /// Field value ad specified global point, in KGauss
  //GlobalVector inKGauss(const GlobalPoint& gp) const  {
  //  return inTesla(gp) * 10.F;
  //}

  /// Field value ad specified global point, in 1/Gev
  //GlobalVector inInverseGeV(const GlobalPoint& gp) const {
  //  return inTesla(gp) * 2.99792458e-3F;
  //}

  /// True if the point is within the region where the concrete field
  // engine is defined.
  virtual bool isDefined(const GlobalPoint& gp) const {
    return true;
  }
  
};

#endif
