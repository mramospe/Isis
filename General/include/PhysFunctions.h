///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR : Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last Update: 09/03/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements different functions to perform calculations of physical and
//  algebraic variables.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef PHYS_FUNCTIONS
#define PHYS_FUNCTIONS

#include "LorentzVector.h"


//_______________________________________________________________________________

namespace General {

  double CTAU( LorentzVector TLV, Vector OwnPV, Vector EndV );
  double Det( Vector vec1, Vector vec2, Vector vec3 );
  double Det( LorentzVector vec1, LorentzVector vec2, LorentzVector vec3 );
  double IP( Vector vec, Vector pv, Vector sv );
  double DOCA( LorentzVector vec1, Vector EndV1, LorentzVector vec2, Vector EndV2 );

}

#endif
