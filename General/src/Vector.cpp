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


#include "Vector.h"


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  Vector::Vector() { }

  //_______________________________________________________________________________
  //
  Vector::Vector( const double &x, const double &y, const double &z ) :
    fX( x ), fY( y ), fZ( z ) { }

  //_______________________________________________________________________________
  //
  Vector::Vector( const Vector &vec ) :
    fX( vec.fX ), fY( vec.fY ), fZ( vec.fZ ) { }

  //_______________________________________________________________________________
  //
  Vector::~Vector() { }

  //_______________________________________________________________________________
  //
  Vector Vector::GetLong( const Vector &vec ) const {
    Vector u( this->Unitary() );
    return ( u.Dot( vec ) )*u;
  }

  //_______________________________________________________________________________
  //
  Vector Vector::GetTran( const Vector &vec ) const {
    Vector u( this->Unitary() );
    return ( vec - ( u.Dot( vec ) )*u );
  }

  //_______________________________________________________________________________
  //
  double Vector::Phi() const {
    double phi = std::atan2( this->SinPhi(), this->CosPhi() );
    return phi > 0 ? phi : phi + 2*M_PI;
  }

  //_______________________________________________________________________________
  //
  void Vector::RotateX( const double &angle ) {
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      y( fY );
    fY = c*y - s*fZ;
    fZ = s*y + c*fZ;
  }

  //_______________________________________________________________________________
  //
  void Vector::RotateY( const double &angle ) {
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      z( fZ );
    fZ = c*z - s*fX;
    fX = s*z + c*fX;
  }

  //_______________________________________________________________________________
  //
  void Vector::RotateZ( const double &angle ) {
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      x( fX );
    fX = c*x - s*fY;
    fY = s*x + c*fY;
  }

  //_______________________________________________________________________________
  //
  double Vector::Theta() const {
    double theta( std::atan2( this->SinTheta(), this->CosTheta() ) );
    return theta > 0 ? theta : theta + 2*M_PI;
  }

  //_______________________________________________________________________________
  //
  Vector Vector::Unitary() const {
    double
      m2( this->Mod2() ),
      m( m2 > 0 ? std::sqrt( m2 ) : 1 );
    return Vector( fX/m, fY/m, fZ/m );
  }

}
