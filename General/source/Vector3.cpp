///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //       
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 23/07/2015                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class Vector3, that allows to compute and perform operations  //
//  that concern three-dimensional vectors.                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "Vector3.h"


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::Vector3::Vector3() { }

//_______________________________________________________________________________
// Constructor given the three components of the vector
General::Vector3::Vector3( double x, double y, double z ) :
  fX( x ), fY( y ), fZ( z ) { }

//_______________________________________________________________________________
// Copy constructor
General::Vector3::Vector3( const General::Vector3 &vec ) :
  fX( vec.fX ), fY( vec.fY ), fZ( vec.fZ ) { }

//_______________________________________________________________________________
// Destructor
General::Vector3::~Vector3() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Gets the longitudinal vector
General::Vector3 General::Vector3::GetLong( General::Vector3 vec ) {

  General::Vector3 u( this -> Unitary() );

  return ( u.Dot( vec ) )*u;
}

//_______________________________________________________________________________
// Gets the transversal vector
General::Vector3 General::Vector3::GetTran( General::Vector3 vec ) {

  General::Vector3 u( this -> Unitary() );

  return ( vec - ( u.Dot( vec ) )*u );
}

//_______________________________________________________________________________
// Gets the phi angle in the x, y, z coordinate system
double General::Vector3::Phi() {

  double phi = std::atan2( this -> SinPhi(), this -> CosPhi() );

  return phi > 0 ? phi : phi + 2*M_PI;
}

//_______________________________________________________________________________
// Method to rotate the vector in the x axis
void General::Vector3::RotateX( double angle ) {

  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    y( fY );

  fY = c*y - s*fZ;
  fZ = s*y + c*fZ;
}

//_______________________________________________________________________________
// Method to rotate the vector in the y axis
void General::Vector3::RotateY( double angle ) {

  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    z( fZ );

  fZ = c*z - s*fX;
  fX = s*z + c*fX;
}

//_______________________________________________________________________________
// Method to rotate the vector in the z axis
void General::Vector3::RotateZ( double angle ) {

  double
    s( std::sin( angle ) ),
    c( std::cos( angle ) ),
    x( fX );

  fX = c*x - s*fY;
  fY = s*x + c*fY;
}

//_______________________________________________________________________________
// Gets the theta angle in the x, y, z coordinate system
double General::Vector3::Theta() {
  
  double theta( std::atan2( this -> SinTheta(), this -> CosTheta() ) );

  return theta > 0 ? theta : theta + 2*M_PI;
}

//_______________________________________________________________________________
// Gets the unitary vector
General::Vector3 General::Vector3::Unitary() {

  double
    m2( this -> Mod2() ),
    m( m2 > 0 ? std::sqrt( m2 ) : 1 );

  return General::Vector3( fX/m, fY/m, fZ/m );
}
