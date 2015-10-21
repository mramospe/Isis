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


#ifndef KIN_VARIABLES
#define KIN_VARIABLES

#include "LorentzVector.h"


//_______________________________________________________________________________

namespace General {

  double CTAU( LorentzVector TLV, Vector3 OwnPV, Vector3 EndV );
  double Det( Vector3 vec1, Vector3 vec2, Vector3 vec3 );
  double Det( LorentzVector vec1, LorentzVector vec2, LorentzVector vec3 );
  double IP( Vector3 vec, Vector3 pv, Vector3 sv );
  double DOCA( LorentzVector vec1, LorentzVector vec2, Vector3 EndV1, Vector3 EndV2 );

}

#endif
