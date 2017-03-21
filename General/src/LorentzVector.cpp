///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "LorentzVector.h"

#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  LorentzVector::LorentzVector() { }

  //_______________________________________________________________________________
  //
  LorentzVector::LorentzVector( const double &px,
				const double &py,
				const double &pz,
				const double &pe ) :
    fP( px, py, pz ), fE( pe ) { }

  //_______________________________________________________________________________
  //
  LorentzVector::LorentzVector( const Vector &vec, const double &pe ) :
    fP( vec ), fE( pe ) { }

  //_______________________________________________________________________________
  //
  LorentzVector::LorentzVector( const LorentzVector &vec ) :
    fP( vec.fP ), fE( vec.fE ) { }

  //_______________________________________________________________________________
  //
  LorentzVector::~LorentzVector() { }

  //_______________________________________________________________________________
  //
  double LorentzVector::ArmAlpha( const LorentzVector &vec1,
				  const LorentzVector &vec2 ) const {
    double
      pl1( this->ArmPl( vec1 ) ),
      pl2( this->ArmPl( vec2 ) );
    return pl1 + pl2 != 0 ? ( pl1 - pl2 )/( pl1 + pl2 ) : 1;
  }

  //_______________________________________________________________________________
  //
  double LorentzVector::ArmPt( const LorentzVector &vec ) const {
    return std::sqrt( vec.fP.Mod2() - this->ArmPl( vec )*this->ArmPl( vec ) );
  }

  //_______________________________________________________________________________
  //
  double LorentzVector::ArmPl( const LorentzVector &vec ) const {
    return vec.fP.Dot( fP.Unitary() );
  }

  //_______________________________________________________________________________
  //
  double LorentzVector::CosHelAngle( const LorentzVector &vec, const Vector &dir ) const {
    LorentzVector vec_transf( this->LorentzTransf( vec ) );
    return vec_transf.CosAngle( dir );
  }

  //_______________________________________________________________________________
  //
  double LorentzVector::PseudoRapidity() const {
    double p( fP.Mod() );
    return 
      p - fP.Z() != 0 ? 0.5*std::log( ( p + fP.Z() ) / ( p - fP.Z() ) ) : 
      std::numeric_limits<double>::max();
  }

  //_______________________________________________________________________________
  //
  LorentzVector LorentzVector::LorentzTransf( const LorentzVector &vec ) const {

    Vector 
      vec_long( this->GetLong( vec ) ),
      vec_tran( this->GetTran( vec ) ),
      u( vec_long.Unitary() );

    double
      mod( vec_long.Mod() ),
      gamma( this->Gamma() ),
      btgm( gamma*this->Beta() );

    return LorentzVector( vec_tran + ( gamma*mod - btgm*vec.fE )*u,
			  gamma*vec.fE - btgm*mod );
  }

}
