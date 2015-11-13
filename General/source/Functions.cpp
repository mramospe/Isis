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
//  Last Update: 13/11/2015			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:				                                 //
//						                                 //
//  Implements different functions to perform calculations of physical and       //
//  algebraic variables.                                                         //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "Functions.h"

#include <cmath>


//______________________________________________________________________________
// CTAU function
double General::CTAU( General::LorentzVector vec,
		      General::Vector        OwnPV,
		      General::Vector        EndV ) {

  double
    beta2( vec.E()*vec.E()/( vec.P()*vec.P() ) ),
    distance( ( OwnPV - EndV ).Mod() );

  return distance*std::sqrt( beta2 - 1 );
}

//______________________________________________________________________________
// Calculates the determinant of three Vector
double General::Det( General::Vector vec1,
		     General::Vector vec2,
		     General::Vector vec3 ) {
   
  return 
    vec1.X()*( vec2.Y()*vec3.Z() - vec2.Z()*vec3.Y() ) - 
    vec1.Y()*( vec2.X()*vec3.Z() - vec2.Z()*vec3.X() ) +
    vec1.Z()*( vec2.X()*vec3.Y() - vec2.Y()*vec3.X() );
}

//______________________________________________________________________________
// Calculates the determinant of three LorentzVector 
double General::Det( General::LorentzVector vec1,
		     General::LorentzVector vec2,
		     General::LorentzVector vec3 ) {
   
  return 
    vec1.Px()*( vec2.Py()*vec3.Pz() - vec2.Pz()*vec3.Py() ) - 
    vec1.Py()*( vec2.Px()*vec3.Pz() - vec2.Pz()*vec3.Px() ) +
    vec1.Pz()*( vec2.Px()*vec3.Py() - vec2.Py()*vec3.Px() );
}

//______________________________________________________________________________
// Calculates the IP given the momentum and the primary and secondary vertex
double General::IP( General::Vector vec,
		    General::Vector pv,
		    General::Vector sv ) {

  General::Vector
    u( vec.Unitary() ),
    ipvec( pv - sv );

  return ( ipvec - u.Dot( ipvec )*u ).Mod();
}

//______________________________________________________________________________
// DOCA function (distance of closest approach)
double General::DOCA( General::LorentzVector vec1,
		      General::LorentzVector vec2,
		      General::Vector        EndV1,
		      General::Vector        EndV2 ) {

  General::Vector
    u1( vec1.Unitary() ),
    u2( vec2.Unitary() );

  double Vol( std::abs( Det( EndV1 - EndV2, u1, u2 ) ) );

  return Vol/( u1.Cross( u2 ) ).Mod();
}
