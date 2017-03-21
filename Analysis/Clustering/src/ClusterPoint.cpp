///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "ClusterPoint.h"
#include "Definitions.h"


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  ClusterPoint::ClusterPoint( const size_t &nvars, const double &wgt ) :
    fValues( nvars ), fWeight( wgt ) { }

  //_______________________________________________________________________________
  //
  ClusterPoint::ClusterPoint( const Doubles &values, const double &wgt ) :
    fValues( values ), fWeight( wgt ) { }

  //_______________________________________________________________________________
  //
  ClusterPoint::~ClusterPoint() { };

}
