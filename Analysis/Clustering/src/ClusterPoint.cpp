///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/07/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Class that defines a point in a cluster. It owns a vector with the values
//  for each variable to work with, as well as a weight.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "ClusterPoint.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::ClusterPoint::ClusterPoint( const size_t &nvars, const double &wgt ) :
  fValues( nvars ), fWeight( wgt ) { }

//_______________________________________________________________________________
// Constructor given a vector of values
Analysis::ClusterPoint::ClusterPoint( const std::vector<double> &values, const double &wgt ) :
  fValues( values ), fWeight( wgt ) { }

//_______________________________________________________________________________
// Destructor
Analysis::ClusterPoint::~ClusterPoint() { };
