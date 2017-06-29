///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#include "Vector.h"


//_________________________________________________________________________

namespace isis {

  //_______________________________________________________________________
  //
  void Vector::rotateX( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      y( fY );
    
    fY = c*y - s*fZ;
    fZ = s*y + c*fZ;
  }

  //_______________________________________________________________________
  //
  void Vector::rotateY( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      z( fZ );
    
    fZ = c*z - s*fX;
    fX = s*z + c*fX;
  }

  //_______________________________________________________________________
  //
  void Vector::rotateZ( const double &angle ) {
    
    double
      s( std::sin( angle ) ),
      c( std::cos( angle ) ),
      x( fX );
    
    fX = c*x - s*fY;
    fY = s*x + c*fY;
  }

  //_______________________________________________________________________
  //
  double Vector::theta() const {
    
    double theta( std::atan2( this->sinTheta(), this->cosTheta() ) );
    
    return theta > 0 ? theta : theta + 2*M_PI;
  }

  //_______________________________________________________________________
  //
  Vector Vector::unitary() const {
    
    double
      m2( this->mod2() ),
      m( m2 > 0 ? std::sqrt( m2 ) : 1 );
    
    return Vector( fX/m, fY/m, fZ/m );
  }

}
