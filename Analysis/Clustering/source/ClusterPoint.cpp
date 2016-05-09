///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 09/05/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Class that defines a point in a cluster. It owns a vector with the values    //
//  for each variable to work with, as well as a weight.                         //
//                                                                               //
// ----------------------------------------------------------------------------- //
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

//_______________________________________________________________________________


// -- PUBLIC METHOD

//_______________________________________________________________________________
// Returns a point corresponding to the center of mass calculated using two points
Analysis::ClusterPoint Analysis::ClusterPoint::CenterOfMass( const ClusterPoint &pointA,
							     const ClusterPoint &pointB,
							     const std::vector<double> &weights ) {

  ClusterPoint point( pointA.fValues.size() );
  
  double
    wa, wb,
    wgtA = pointA.fWeight,
    wgtB = pointB.fWeight;

  point.fWeight = wgtA + wgtB;
  
  auto
    itA = pointA.fValues.begin(),
    itB = pointB.fValues.begin(),
    itW = weights.begin();
  auto itV = point.fValues.begin();
  
  while ( itA != pointA.fValues.end() ) {
    wa     = (*itW)*wgtA;
    wb     = (*itW)*wgtB;
    *itV++ = ( wa*(*itA++) + wb*(*itB++) )/( wa + wb );
    ++itW;
  }
  
  return point;
}
