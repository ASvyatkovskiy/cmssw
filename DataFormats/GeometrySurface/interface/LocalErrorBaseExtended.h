#ifndef LocalErrorType_H
#define LocalErrorType_H

#include "DataFormats/GeometryCommonDetAlgo/interface/DeepCopyPointer.h"
#include "DataFormats/Math/interface/AlgebraicROOTObjects.h"
//
// Exceptions
//
#include "FWCore/Utilities/interface/Exception.h"

  /**
   * Templated class representing a symmetric 3*3 matrix describing,  according
   * to the ErrorWeightType tag, a (cartesian) covariance matrix or the weight
   * matrix (the inverse of the covariance matrix).
   * \li To have a covariance matrix, the ErrorMatrixTag has to be used, and a 
   * typedef is available as LocalError
   * \li To have a weight matrix, the WeightMatrixTag has to be used, and a 
   * typedef is available as Localweight
   * 
   * The typedefs should be used in the code.
   */


template <class T, class ErrorWeightType> 
class LocalErrorBaseExtended
{

public:
  /// Tag to request a null error matrix
  class NullMatrix{};

  /**
   * Default constructor, creating a null 3*3 matrix (all values are 0)
   */
  LocalErrorBaseExtended() {}

  /** 
   * Obsolete  Constructor that allocates a null LocalErrorBaseExtended (it does not create the error matrix at all)
   */
  LocalErrorBaseExtended(const NullMatrix &) {}

  LocalErrorBaseExtended(InvalidError) {
    theCartesianError[0][0] = -9999.e10f;
    theCartesianError[0][1] = 0;
    theCartesianError[1][1] = -9999.e10f;
    theCartesianError[2][2] = -9999.e10f;
    theCartesianError[3][3] = -9999.e10f;
  }

  bool invalid() const { return theCartesianError[0][0] <-1.e10f;}
  bool valid() const { return !invalid();}


  /**
   * Constructor.
   * The symmetric matrix stored as a lower triangular matrix
   */
  LocalErrorBaseExtended(T c11, T c21, T c31, T c41,
                 T c22, T c32, T c42,
                 T c33, T c43,
                 T c44) {
    theCartesianError(0,0)=c11;
    theCartesianError(1,0)=c21;
    theCartesianError(2,0)=c31;
    theCartesianError(3,0)=c41;

    theCartesianError(1,1)=c33;
    theCartesianError(2,1)=c32;
    theCartesianError(3,1)=c42;

    theCartesianError(2,2)=c33;
    theCartesianError(3,2)=c43;

    theCartesianError(3,3)=c44;

  }
  
   /**
   * Constructor from SymMatrix. The original matrix has to be a 3*3 matrix.
   */
    LocalErrorBaseExtended(const AlgebraicSymMatrix44 & err) : 
      theCartesianError(err) { }
  
  ~LocalErrorBaseExtended() {}

  T cxx() const {
    return theCartesianError(0,0);
  }
  
  T cyx() const {
    return theCartesianError(1,0);
  }
  
  T cphixx() const {
    return theCartesianError(2,0);
  }

  T cphiyx() const {
    return theCartesianError(3,0);
  }
 
  T cyy() const {
    return theCartesianError(1,1);
  }

  T cphixy() const {
    return theCartesianError(2,1);
  }

  T cphiyy() const {
    return theCartesianError(3,1);
  }

  T cphixphix() const {
    return theCartesianError(2,2);
  }

  T cphiyphix() const {
    return theCartesianError(3,2);
  }

  T cphiyphiy() const {
    return theCartesianError(3,3);
  }
 
 /**
   * Access method to the matrix,
   * /return The SymMatrix
   */
  const AlgebraicSymMatrix44 & matrix() const {
    return theCartesianError;
  }

  LocalErrorBaseExtended operator+ (const LocalErrorBaseExtended& err) const {
    return LocalErrorBaseExtended(theCartesianError + err.theCartesianError);
  }
  LocalErrorBaseExtended operator- (const LocalErrorBaseExtended& err) const {
    return LocalErrorBaseExtended(theCartesianError - err.theCartesianError);
  }
 
private:

  AlgebraicSymMatrix44 theCartesianError;

};

#endif
