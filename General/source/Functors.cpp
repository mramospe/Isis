///////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------- //
//                                                                               //
//  General package      			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR : Miguel Ramos Pernas		                                 //
//  e-mail: miguel.ramos.pernas@cern.ch		                                 //
//                                                                               //
//  Last Update: 22/10/2015			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:				                                 //
//						                                 //
//  Implements different functions. All functions inherit from < Functor >       //
//  class. If the class is created ( when no static method is used ) the         //
//  < Calculate > method has to perform the calculations to get the desired      //
//  variable. All the input and output variables are given to the class by       //
//  reference, while the static method takes them by value.                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "Functors.h"

#include <cmath>


//______________________________________________________________________________
// Functor to calculate the half-life multiplied by the speed of light
double General::CTAU::Eval( General::LorentzVector vec,
			    General::Vector3       OwnPV,
			    General::Vector3       EndV ) {

  double
    beta2( vec.E()*vec.E()/( vec.P()*vec.P() ) ),
    distance( ( OwnPV - EndV ).Mod() );

  return distance*std::sqrt( beta2 - 1 );
}

  //______________________________________________________________________________
  // Functor to calculate the determinant of three Vector3 variables
double General::Det::Eval( General::Vector3 vec1,
			   General::Vector3 vec2,
			   General::Vector3 vec3 ) {
   
  return 
    vec1.X()*( vec2.Y()*vec3.Z() - vec2.Z()*vec3.Y() ) - 
    vec1.Y()*( vec2.X()*vec3.Z() - vec2.Z()*vec3.X() ) +
    vec1.Z()*( vec2.X()*vec3.Y() - vec2.Y()*vec3.X() );
}

//______________________________________________________________________________
// Functor to calculate the impact parameter given the momentum, the primary
// and the secondary vertex positions
double General::IP::Eval( General::Vector3 vec,
			  General::Vector3 pv,
			  General::Vector3 sv ) {

  General::Vector3
    u( vec.Unitary() ),
    ipvec( pv - sv );

  return ( ipvec - u.Dot( ipvec )*u ).Mod();
}

//______________________________________________________________________________
// Functor to calculate the distance of closest approach between two particles
double General::DOCA::Eval( General::Vector3 vec1,
			    General::Vector3 pos1,
			    General::Vector3 vec2,
			    General::Vector3 pos2 ) {

  General::Vector3
    u1( vec1.Unitary() ),
    u2( vec2.Unitary() );

  double Vol( std::abs( Det::Eval( pos1 - pos2, u1, u2 ) ) );

  return Vol/( u1.Cross( u2 ) ).Mod();
}
