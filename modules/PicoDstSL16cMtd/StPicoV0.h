#ifndef StPicoV0_hh
#define StPicoV0_hh

#include "TObject.h"
#include "StThreeVectorF.hh"

class StPicoV0 : public TObject {
 public:
  StPicoV0() {}
  ~StPicoV0() {}
  void Clear(const Option_t* opt="") {}

  StThreeVectorF v0Pos() const { return mV0Pos; }
  Float_t dcaDaughters() const { return (Float_t)mDcaDaughters/1000.; }
  Float_t   dca2Vertex() const { return (Float_t)mDca2Vtx/1000.; }
  Float_t            m() const { return mM; }

    
 protected:
  Short_t   mIndex2Track[2];
  StThreeVectorF mMomentum[2];
  StThreeVectorF mV0Pos;
  UShort_t       mDcaDaughters;
  UShort_t       mDca2Vtx;
  Float_t        mM;

  friend class StPicoDst;

  ClassDef(StPicoV0, 1)
};

#endif
