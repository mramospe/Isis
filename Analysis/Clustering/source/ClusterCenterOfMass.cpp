///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 27/07/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Class defining a center of mass of a cluster, storing its position and the   //
//  mean of squares.                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "ClusterCenterOfMass.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::ClusterCenterOfMass::ClusterCenterOfMass() : ClusterPoint() { fWeight = 0; }

//_______________________________________________________________________________
// Constructor given a position
Analysis::ClusterCenterOfMass::ClusterCenterOfMass( const ClusterPoint &point ) :
  ClusterPoint( point ), fMeanOfSquares( fValues ) {
  for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
    *it *= (*it);
}

//_______________________________________________________________________________
// Destructor
Analysis::ClusterCenterOfMass::~ClusterCenterOfMass() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Looks if does exist any variance which could lead to infinites
bool Analysis::ClusterCenterOfMass::AnyNullSigma() const {

  auto
    itm = fValues.cbegin(),
    its = fMeanOfSquares.cbegin();

  while ( itm != fValues.cend() ) {
    
    if ( (*its) - (*itm)*(*itm) == 0. )
      return true;

    ++itm;
    ++its;
  }
  
  return false;
}

//_______________________________________________________________________________
// Recalculates the center of mass when adding a new point
void Analysis::ClusterCenterOfMass::AttachPoint( const ClusterPoint &point ) {
  
  double
    wgtCoM = fWeight,
    wgtPnt = point.GetWeight();
  
  fWeight = wgtCoM + wgtPnt;
  
  auto
    itCoM = fValues.begin(),
    itSqM = fMeanOfSquares.begin();
  
  auto itPnt = point.GetValues().begin();
  
  while ( itCoM != fValues.end() ) {
    
    double
      com  = wgtCoM*(*itCoM),
      pnt  = wgtPnt*(*itPnt),
      sqm  = wgtCoM*(*itSqM),
      pt2  = wgtPnt*(*itPnt)*(*itPnt);
    
    *itCoM = ( com + pnt )/fWeight;
    *itSqM = ( sqm + pt2 )/fWeight;
    
    ++itCoM;
    ++itPnt;
    ++itSqM;
  }
}

//_______________________________________________________________________________
// Normalizes the position of this center of mass
void Analysis::ClusterCenterOfMass::Normalize( const std::vector<double> &norm ) {

  ClusterPoint::Normalize( norm );

  auto itv = fMeanOfSquares.begin();
  auto itn = norm.begin();

  while ( itv != fMeanOfSquares.end() ) {
    (*itv) /= (*itn)*(*itn);

    ++itv;
    ++itn;
  }
}

//_______________________________________________________________________________
// Sets the position of the center of mass
void Analysis::ClusterCenterOfMass::SetValues( const std::vector<double> &values ) {
    
  ClusterPoint::SetValues( values );
  fMeanOfSquares = values;
  for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
    *it *= (*it);
}
