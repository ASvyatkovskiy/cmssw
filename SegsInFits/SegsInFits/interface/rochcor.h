#ifndef SegsInFits_SegsInFits_rochester_H
#define SegsInFits_SegsInFits_rochester_H

#include <iostream>
#include <TChain.h>
#include <TClonesArray.h>
#include <TString.h>
#include <map>

#include <TSystem.h>
#include <TROOT.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TRandom3.h>


class rochcor {
 public:
  rochcor();
  ~rochcor();

  void momcor_mc(TLorentzVector& , TLorentzVector& , TLorentzVector& , TLorentzVector& , int);
  void momcor_data(TLorentzVector& , TLorentzVector& , int);

  float zptcor(float);
  int etabin(float);
  int phibin(float);

 private:
  
  TRandom3 eran1;
  TRandom3 eran2;
 
  static const double roch_pi() { return 3.14159265358979323846;}
  static const float netabin[9];
  
  //iteration2 after FSR : after Z Pt correction
  static const float Tfix() {return -0.0473928;}
  static const float Tfixer() {return 0.00366575;}
  
  static const float delta() {return 3.33594e-05;}
  static const float deltaer() {return 7.86248e-07;}
  
  static const float sf() {return 1.25545;}
  static const float sfer() {return 0.333992;}
  
  //---------------------------------------------------------------------------------------------
  
  static const float dcor_bf[8][8];  
  static const float dcor_ma[8][8];
  static const float mcor_bf[8][8];
  static const float mcor_ma[8][8];

  //=======================================================================================================
  
  static const float dmavg[8][8];  
  static const float dpavg[8][8];  
  static const float mmavg[8][8];  
  static const float mpavg[8][8];
  
  //===============================================================================================
  //parameters for Z pt correction
  static const int nptbins=84;
  static const float ptlow[85];    
  
  static const float zptscl[84];
  static const float zptscler[84];

};
#endif  
