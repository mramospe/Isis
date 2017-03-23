///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
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
  Vector Vector::getLong( const Vector &vec ) const {
    
    Vector u( this->unitary() );
    return ( u.dot( vec ) )*u;
  }

  //_______________________________________________________________________________
  //
  Vector Vector::getTran( const Vector &vec ) const {
    
    Vector u( this->unitary() );
    return ( vec - ( u.dot( vec ) )*u );
  }

  //_______________________________________________________________________________
  //
  double Vector::phi() const {
    
    double phi = std::atan2( this->sinPhi(), this->cosPhi() );
    return phi > 0 ? phi : phi + 2*M_PI;
  }

  //_______________________________________________________________________________
  //
  void Vector::rotateX( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      y( fY );
    
    fY = c*y - s*fZ;
    fZ = s*y + c*fZ;
  }

  //_______________________________________________________________________________
  //
  void Vector::rotateY( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      z( fZ );
    
    fZ = c*z - s*fX;
    fX = s*z + c*fX;
  }

  //_______________________________________________________________________________
  //
  void Vector::rotateZ( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      x( fX );
    
    fX = c*x - s*fY;
    fY = s*x + c*fY;
  }

  //_______________________________________________________________________________
  //
  double Vector::theta() const {
    
    double theta( std::atan2( this->sinTheta(), this->cosTheta() ) );
    
    return theta > 0 ? theta : theta + 2*M_PI;
  }

  //_______________________________________________________________________________
  //
  Vector Vector::unitary() const {
    
    double
      m2( this->mod2() ),
      m( m2 > 0 ? std::sqrt( m2 ) : 1 );
    
    return Vector( fX/m, fY/m, fZ/m );
  }

}
