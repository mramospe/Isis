///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 29/06/2017
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#include "LorentzVector.h"

#include <cmath>


//_________________________________________________________________________

namespace isis {

  //_______________________________________________________________________
  //
  double LorentzVector::armAlpha( const LorentzVector &vec1,
				  const LorentzVector &vec2 ) const {
    
    double
      pl1( this->armPl( vec1 ) ),
      pl2( this->armPl( vec2 ) );
    
    return pl1 + pl2 != 0 ? (pl1 - pl2)/(pl1 + pl2) : 1;
  }

  //_______________________________________________________________________
  //
  double LorentzVector::armPt( const LorentzVector &vec ) const {
    
    return std::sqrt(vec.fP.mod2() - this->armPl(vec)*this->armPl(vec));
  }

  //_______________________________________________________________________
  //
  double LorentzVector::armPl( const LorentzVector &vec ) const {
    
    return vec.fP.dot(fP.unitary());
  }

  //_______________________________________________________________________
  //
  double LorentzVector::cosHelAngle( const LorentzVector &vec,
				     const Vector &dir ) const {
    
    LorentzVector vec_transf(this->lorentzTransf(vec));
    return vec_transf.momentum().cosAngle(dir);
  }

  //_______________________________________________________________________
  //
  double LorentzVector::pseudoRapidity() const {
    
    double p(fP.mod());
    return 
      p - fP.Z() != 0 ? 0.5*std::log((p + fP.Z())/(p - fP.Z())) : 
      std::numeric_limits<double>::max();
  }

  //_______________________________________________________________________
  //
  LorentzVector LorentzVector::lorentzTransf( const LorentzVector &vec ) const {

    Vector 
      vec_long(fP.getLong(vec.fP)),
      vec_tran(fP.getTran(vec.fP)),
      u(vec_long.unitary());

    double
      mod(vec_long.mod()),
      gamma(this->gamma()),
      btgm(gamma*this->beta());

    return LorentzVector(vec_tran + (gamma*mod - btgm*vec.fE)*u,
			 gamma*vec.fE - btgm*mod);
  }

}
