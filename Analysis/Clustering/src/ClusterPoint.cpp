///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "ClusterPoint.h"


//_______________________________________________________________________________
//
Analysis::ClusterPoint::ClusterPoint( const size_t &nvars, const double &wgt ) :
  fValues( nvars ), fWeight( wgt ) { }

//_______________________________________________________________________________
//
Analysis::ClusterPoint::ClusterPoint( const std::vector<double> &values,
				      const double &wgt ) :
  fValues( values ), fWeight( wgt ) { }

//_______________________________________________________________________________
//
Analysis::ClusterPoint::~ClusterPoint() { };
