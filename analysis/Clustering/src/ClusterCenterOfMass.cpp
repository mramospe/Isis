///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "ClusterCenterOfMass.hpp"
#include "Definitions.hpp"


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  ClusterCenterOfMass::ClusterCenterOfMass() : ClusterPoint() { fWeight = 0; }

  //_______________________________________________________________________________
  //
  ClusterCenterOfMass::ClusterCenterOfMass( const ClusterPoint &point ) :
    ClusterPoint( point ), fMeanOfSquares( fValues ) {
    
    for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
      *it *= (*it);
  }

  //_______________________________________________________________________________
  //
  ClusterCenterOfMass::~ClusterCenterOfMass() { }

  //_______________________________________________________________________________
  //
  bool ClusterCenterOfMass::anyNullSigma() const {

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
  //
  void ClusterCenterOfMass::attachPoint( const ClusterPoint &point ) {
  
    double
      wgtCoM = fWeight,
      wgtPnt = point.getWeight();
  
    fWeight = wgtCoM + wgtPnt;
  
    auto
      itCoM = fValues.begin(),
      itSqM = fMeanOfSquares.begin();
  
    auto itPnt = point.getValues().begin();
  
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
  //
  void ClusterCenterOfMass::normalize( const Doubles &norm ) {

    ClusterPoint::normalize( norm );

    auto itv = fMeanOfSquares.begin();
    auto itn = norm.begin();

    while ( itv != fMeanOfSquares.end() ) {
      (*itv) /= (*itn)*(*itn);

      ++itv;
      ++itn;
    }
  }

  //_______________________________________________________________________________
  //
  void ClusterCenterOfMass::setValues( const Doubles &values ) {
    
    ClusterPoint::setValues( values );
    fMeanOfSquares = values;
    for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
      *it *= (*it);
  }

}
