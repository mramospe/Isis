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
//
//  Description:
//
//  Class that defines a point in a cluster. It owns a vector with the values
//  for each variable to work with, as well as a weight.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLUSTER_POINT
#define CLUSTER_POINT

#include "Definitions.h"

#include <iostream>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Isis {

  class ClusterPoint {

  public:

    // Main constructor
    ClusterPoint( const size_t &nvars = 0, const double &wgt = 1 );

    // Constructor given a vector of values
    ClusterPoint( const Doubles &values, const double &wgt = 1 );

    // Destructor
    virtual ~ClusterPoint();
    
    // Returns the value for the variable at position < index >
    inline const double GetValue( const size_t &index ) const;

    // Returns the vector with the values and the weights
    inline const Doubles& GetValues() const;

    // Returns the global weight of the point
    inline const double GetWeight() const;

    // Normalizes the point given the normalization factors for each variable
    inline void Normalize( const Doubles &norm );

    // Sets the values for this point
    inline void SetValues( const Doubles &values, const double  &wgt = 1 );

  protected:
  
    // Vector with the values for each dimension
    Doubles fValues;

    // Weight of the point
    double fWeight;
    
  };

  //_______________________________________________________________________________
  //
  const double ClusterPoint::GetValue( const size_t &index ) const {
    return fValues[ index ];
  }

  //_______________________________________________________________________________
  //
  const Doubles& ClusterPoint::GetValues() const { return fValues; }

  //_______________________________________________________________________________
  //
  const double ClusterPoint::GetWeight() const { return fWeight; }

  //_______________________________________________________________________________
  //
  void ClusterPoint::Normalize( const Doubles &norm ) {
    auto itv = fValues.begin();
    auto itm = norm.begin();
    while ( itv != fValues.end() )
      *itv++ /= *itm++;
  }

  //_______________________________________________________________________________
  //
  void ClusterPoint::SetValues( const Doubles &values, const double &wgt ) {
    fValues = values;
    fWeight = wgt;
  }

}

#endif
