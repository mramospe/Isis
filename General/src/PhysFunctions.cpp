///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR : Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last Update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "PhysFunctions.h"

#include <cmath>


//______________________________________________________________________________

namespace Isis {

  //______________________________________________________________________________
  //
  double CTAU( LorentzVector vec,
	       Vector        OwnPV,
	       Vector        EndV ) {

    double
      beta2( vec.E()*vec.E()/( vec.P()*vec.P() ) ),
      distance( ( OwnPV - EndV ).Mod() );

    return distance*std::sqrt( beta2 - 1 );
  }

  //______________________________________________________________________________
  //
  double Det( Vector vec1,
	      Vector vec2,
	      Vector vec3 ) {
   
    return 
      vec1.X()*( vec2.Y()*vec3.Z() - vec2.Z()*vec3.Y() ) - 
      vec1.Y()*( vec2.X()*vec3.Z() - vec2.Z()*vec3.X() ) +
      vec1.Z()*( vec2.X()*vec3.Y() - vec2.Y()*vec3.X() );
  }

  //______________________________________________________________________________
  //
  double Det( LorentzVector vec1,
	      LorentzVector vec2,
	      LorentzVector vec3 ) {
   
    return 
      vec1.Px()*( vec2.Py()*vec3.Pz() - vec2.Pz()*vec3.Py() ) - 
      vec1.Py()*( vec2.Px()*vec3.Pz() - vec2.Pz()*vec3.Px() ) +
      vec1.Pz()*( vec2.Px()*vec3.Py() - vec2.Py()*vec3.Px() );
  }

  //______________________________________________________________________________
  //
  double IP( Vector vec,
	     Vector pv,
	     Vector sv ) {

    Vector
      u( vec.Unitary() ),
      ipvec( pv - sv );

    return ( ipvec - u.Dot( ipvec )*u ).Mod();
  }

  //______________________________________________________________________________
  //
  double DOCA( LorentzVector vec1,
	       Vector        EndV1,
	       LorentzVector vec2,
	       Vector        EndV2 ) {

    Vector
      u1( vec1.Unitary() ),
      u2( vec2.Unitary() );

    double Vol( std::abs( Det( EndV1 - EndV2, u1, u2 ) ) );

    return Vol/( u1.Cross( u2 ) ).Mod();
  }

}
