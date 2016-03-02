///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 02/03/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class Vector, that allows to compute and perform operations   //
//  that concern three-dimensional vectors.                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "Vector.h"


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::Vector::Vector() { }

//_______________________________________________________________________________
// Constructor given the three components of the vector
General::Vector::Vector( const double &x, const double &y, const double &z ) :
  fX( x ), fY( y ), fZ( z ) { }

//_______________________________________________________________________________
// Copy constructor
General::Vector::Vector( const General::Vector &vec ) :
  fX( vec.fX ), fY( vec.fY ), fZ( vec.fZ ) { }

//_______________________________________________________________________________
// Destructor
General::Vector::~Vector() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Gets the longitudinal vector
General::Vector General::Vector::GetLong( const General::Vector &vec ) const {
  General::Vector u( this -> Unitary() );
  return ( u.Dot( vec ) )*u;
}

//_______________________________________________________________________________
// Gets the transversal vector
General::Vector General::Vector::GetTran( const General::Vector &vec ) const {
  General::Vector u( this -> Unitary() );
  return ( vec - ( u.Dot( vec ) )*u );
}

//_______________________________________________________________________________
// Gets the phi angle in the x, y, z coordinate system
double General::Vector::Phi() const {
  double phi = std::atan2( this -> SinPhi(), this -> CosPhi() );
  return phi > 0 ? phi : phi + 2*M_PI;
}

//_______________________________________________________________________________
// Method to rotate the vector in the x axis
void General::Vector::RotateX( const double &angle ) {
  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    y( fY );
  fY = c*y - s*fZ;
  fZ = s*y + c*fZ;
}

//_______________________________________________________________________________
// Method to rotate the vector in the y axis
void General::Vector::RotateY( const double &angle ) {
  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    z( fZ );
  fZ = c*z - s*fX;
  fX = s*z + c*fX;
}

//_______________________________________________________________________________
// Method to rotate the vector in the z axis
void General::Vector::RotateZ( const double &angle ) {
  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    x( fX );
  fX = c*x - s*fY;
  fY = s*x + c*fY;
}

//_______________________________________________________________________________
// Gets the theta angle in the x, y, z coordinate system
double General::Vector::Theta() const {
  double theta( std::atan2( this -> SinTheta(), this -> CosTheta() ) );
  return theta > 0 ? theta : theta + 2*M_PI;
}

//_______________________________________________________________________________
// Gets the unitary vector
General::Vector General::Vector::Unitary() const {
  double
    m2( this -> Mod2() ),
    m( m2 > 0 ? std::sqrt( m2 ) : 1 );
  return General::Vector( fX/m, fY/m, fZ/m );
}
