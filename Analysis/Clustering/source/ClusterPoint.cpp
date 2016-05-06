///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 06/05/2016                                                      //
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
General::ClusterPoint::ClusterPoint( const size_t &nvars, const double &wgt ) :
  fValues( nvars ), fWeight( wgt ) { }

//_______________________________________________________________________________
// Constructor given a vector of values
General::ClusterPoint::ClusterPoint( const std::vector<double> &values, const double &wgt ) :
  fValues( values ), fWeight( wgt ) { }

//_______________________________________________________________________________
// Destructor
General::ClusterPoint::~ClusterPoint() { };

//_______________________________________________________________________________


// -- PUBLIC METHOD

//_______________________________________________________________________________
// Returns a point corresponding to the center of mass calculated using two points
General::ClusterPoint General::ClusterPoint::CenterOfMass( const ClusterPoint &pointA,
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
