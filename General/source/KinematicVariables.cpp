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
//  Last Update: 22/07/2015			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:				                                 //
//						                                 //
//  Implements different kinematic functions that concern the topology of the    //
//  b-hadron decays.                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "KinematicVariables.h"

#include <cmath>


//______________________________________________________________________________
// CTAU function
double General::CTAU( General::LorentzVector vec,
		      General::Vector3       OwnPV,
		      General::Vector3       EndV ) {

  double
    beta2( vec.E()*vec.E()/( vec.P()*vec.P() ) ),
    distance( ( OwnPV - EndV ).Mod() );

  return distance*std::sqrt( beta2 - 1 );
}

//______________________________________________________________________________
// Calculates the determinant of three Vector3
double General::Det( General::Vector3 vec1,
		     General::Vector3 vec2,
		     General::Vector3 vec3 ) {
   
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
double General::IP( General::Vector3 vec,
		    General::Vector3 pv,
		    General::Vector3 sv ) {

  General::Vector3
    u( vec.Unitary() ),
    ipvec( pv - sv );

  return ( ipvec - u.Dot( ipvec )*u ) .Mod();
}

//______________________________________________________________________________
// DOCA function (distance of closest approach)
double General::DOCA( General::LorentzVector vec1,
		      General::LorentzVector vec2,
		      General::Vector3       EndV1,
		      General::Vector3       EndV2 ) {

  General::Vector3
    u1( vec1.Unitary() ),
    u2( vec2.Unitary() );

  double Vol( std::abs( Det( EndV1 - EndV2, u1, u2 ) ) );

  return Vol/( u1.Cross( u2 ) ).Mod();
}
