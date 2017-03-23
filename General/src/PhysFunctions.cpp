///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR : Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last Update: 23/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "PhysFunctions.h"

#include <cmath>


//______________________________________________________________________________

namespace Isis {

  //______________________________________________________________________________
  //
  double ctau( LorentzVector vec,
	       Vector        OwnPV,
	       Vector        EndV ) {

    double
      beta2( vec.E()*vec.E()/( vec.P()*vec.P() ) ),
      distance( ( OwnPV - EndV ).mod() );

    return distance*std::sqrt( beta2 - 1 );
  }

  //______________________________________________________________________________
  //
  double det( Vector vec1,
	      Vector vec2,
	      Vector vec3 ) {
   
    return 
      vec1.X()*( vec2.Y()*vec3.Z() - vec2.Z()*vec3.Y() ) - 
      vec1.Y()*( vec2.X()*vec3.Z() - vec2.Z()*vec3.X() ) +
      vec1.Z()*( vec2.X()*vec3.Y() - vec2.Y()*vec3.X() );
  }

  //______________________________________________________________________________
  //
  double det( LorentzVector vec1,
	      LorentzVector vec2,
	      LorentzVector vec3 ) {
   
    return 
      vec1.Px()*( vec2.Py()*vec3.Pz() - vec2.Pz()*vec3.Py() ) - 
      vec1.Py()*( vec2.Px()*vec3.Pz() - vec2.Pz()*vec3.Px() ) +
      vec1.Pz()*( vec2.Px()*vec3.Py() - vec2.Py()*vec3.Px() );
  }

  //______________________________________________________________________________
  //
  double ip( Vector vec,
	     Vector pv,
	     Vector sv ) {

    Vector
      u( vec.unitary() ),
      ipvec( pv - sv );

    return ( ipvec - u.dot( ipvec )*u ).mod();
  }

  //______________________________________________________________________________
  //
  double doca( LorentzVector vec1,
	       Vector        EndV1,
	       LorentzVector vec2,
	       Vector        EndV2 ) {

    Vector
      u1( vec1.unitary() ),
      u2( vec2.unitary() );

    double vol = std::abs( det( EndV1 - EndV2, u1, u2 ) );

    return vol/( u1.cross( u2 ) ).mod();
  }

}
