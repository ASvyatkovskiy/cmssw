#ifndef ErrorFrameTransformer_H
#define ErrorFrameTransformer_H

#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/LocalError.h"

struct   ErrorFrameTransformer {
  
  typedef Surface::Scalar         Scalar;

  //folds matrix in 6D
  void foldMatrix64(AlgebraicSymMatrix44 input, AlgebraicSymMatrix66 & output) {
    output[0][0] = input[0][0];
    output[1][0] = input[1][0];
    output[2][0] = input[2][0];
    output[3][0] = input[3][0];
    output[1][1] = input[1][1];
    output[2][1] = input[2][1];
    output[3][1] = input[3][1];
    output[2][2] = input[2][2];
    output[3][2] = input[3][2];
    output[3][3] = input[3][3];
  } 
 
  static GlobalErrorExtended transformExtended(const LocalErrorExtended& le, const Surface& surf) {

    //to be able to use the Similarity method for sym matrices, fold 4x4 to 6x6 matrix
    AlgebraicSymMatrix44 LE_pre = le.matrix();
    AlgebraicSymMatrix66 LE;
    ErrorFrameTransformer().foldMatrix64(LE_pre,LE);

    //Standard rotation matrix in 3D, from SO(3) group
    Surface::RotationType r=surf.rotation();
    //Construct full matrix. Does not have to be symmatric 
    AlgebraicMatrix66 ROT;
    ROT[0][0] = r.xx();
    ROT[1][1] = r.yy();
    ROT[0][1] = r.xy();
    ROT[1][0] = r.yx();
    ROT[2][2] = r.zz();
    ROT[0][2] = r.xz();
    ROT[2][0] = r.zx();
    ROT[1][2] = r.yz();
    ROT[2][1] = r.zy();
    ROT[3][3] = 1.;
    ROT[4][4] = 1.;
    ROT[5][5] = 1.;

    AlgebraicSymMatrix66 GE;
    GE = ROOT::Math::Similarity(ROT,LE); 
    
    return GlobalErrorExtended(GE);
  }

  //Uses old error matrix formats
  static GlobalError transform(const LocalError& le, const Surface& surf) { 
    Scalar cxx = le.xx();
    Scalar cxy = le.xy();
    Scalar cyy = le.yy();

    Surface::RotationType r=surf.rotation();

    return GlobalError( r.xx()*(r.xx()*cxx+r.yx()*cxy) + r.yx()*(r.xx()*cxy+r.yx()*cyy) ,
                        r.xx()*(r.xy()*cxx+r.yy()*cxy) + r.yx()*(r.xy()*cxy+r.yy()*cyy) ,
                        r.xy()*(r.xy()*cxx+r.yy()*cxy) + r.yy()*(r.xy()*cxy+r.yy()*cyy) ,
                        r.xx()*(r.xz()*cxx+r.yz()*cxy) + r.yx()*(r.xz()*cxy+r.yz()*cyy) ,
                        r.xy()*(r.xz()*cxx+r.yz()*cxy) + r.yy()*(r.xz()*cxy+r.yz()*cyy) ,
                        r.xz()*(r.xz()*cxx+r.yz()*cxy) + r.yz()*(r.xz()*cxy+r.yz()*cyy) );
  }
 
  static LocalErrorExtended transformExtended(const GlobalErrorExtended& ge, const Surface& surf) {
    AlgebraicSymMatrix66 GE = ge.matrix();
 
    Surface::RotationType r=surf.rotation();
    AlgebraicMatrix66 ROT; //transposed
    ROT[0][0] = r.xx();
    ROT[1][1] = r.yy();
    ROT[0][1] = r.yx();
    ROT[1][0] = r.xy();
    ROT[2][2] = r.zz();
    ROT[0][2] = r.zx();
    ROT[2][0] = r.xz();
    ROT[1][2] = r.zy();
    ROT[2][1] = r.yz();
    ROT[3][3] = 1.;
    ROT[4][4] = 1.;
    ROT[5][5] = 1.;

    AlgebraicSymMatrix66 LE_pre;
    LE_pre = ROOT::Math::Similarity(ROT,GE);

    //Get the submatrix corresponding to x,y,phix,phiy
    AlgebraicSymMatrix44 LE;
    LE[0][0] = LE_pre[0][0];
    LE[1][0] = LE_pre[1][0];
    LE[2][0] = LE_pre[3][0];
    LE[3][0] = LE_pre[4][0];
    LE[1][1] = LE_pre[1][1];
    LE[2][1] = LE_pre[3][1];
    LE[3][1] = LE_pre[4][1];
    LE[2][2] = LE_pre[3][3];
    LE[3][2] = LE_pre[4][3];
    LE[3][3] = LE_pre[4][4];

    return LocalErrorExtended(LE);
  }

  static LocalError transform(const GlobalErrorExtended& ge, const Surface& surf) {

    AlgebraicSymMatrix66 GE = ge.matrix(); //6x6  
    Surface::RotationType r=surf.rotation();
    AlgebraicMatrix66 ROT; //transposed
    ROT[0][0] = r.xx();
    ROT[1][1] = r.yy();
    ROT[0][1] = r.yx();
    ROT[1][0] = r.xy();
    ROT[2][2] = r.zz();
    ROT[0][2] = r.zx();
    ROT[2][0] = r.xz();
    ROT[1][2] = r.zy();
    ROT[2][1] = r.yz();
    ROT[3][3] = 1.;
    ROT[4][4] = 1.;
    ROT[5][5] = 1.;

    AlgebraicSymMatrix66 LE_pre;
    LE_pre = ROOT::Math::Similarity(ROT,GE);

    //Get the submatrix corresponding to x,y
    Scalar l11 = LE_pre[0][0];
    Scalar l12 = LE_pre[1][0];
    Scalar l22 = LE_pre[1][1];

    return LocalError(l11,l12,l22);
  }
};

#endif
