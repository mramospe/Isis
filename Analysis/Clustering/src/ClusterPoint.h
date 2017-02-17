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

#include <iostream>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class ClusterPoint {

  public:

    // Main constructor
    ClusterPoint( const size_t &nvars = 0, const double &wgt = 1 );

    // Constructor given a vector of values
    ClusterPoint( const std::vector<double> &values, const double &wgt = 1 );

    // Destructor
    virtual ~ClusterPoint();
    
    // Returns the value for the variable at position < index >
    inline const double GetValue( const size_t &index ) const;

    // Returns the vector with the values and the weights
    inline const std::vector<double>& GetValues() const;

    // Returns the global weight of the point
    inline const double GetWeight() const;

    // Normalizes the point given the normalization factors for each variable
    inline void Normalize( const std::vector<double> &norm );

    // Sets the values for this point
    inline void SetValues( const std::vector<double> &values,
			   const double              &wgt = 1 );

  protected:
  
    // Vector with the values for each dimension
    std::vector<double> fValues;

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
  const std::vector<double>& ClusterPoint::GetValues() const { return fValues; }

  //_______________________________________________________________________________
  //
  const double ClusterPoint::GetWeight() const { return fWeight; }

  //_______________________________________________________________________________
  //
  void ClusterPoint::Normalize( const std::vector<double> &norm ) {
    auto itv = fValues.begin();
    auto itm = norm.begin();
    while ( itv != fValues.end() )
      *itv++ /= *itm++;
  }

  //_______________________________________________________________________________
  //
  void ClusterPoint::SetValues( const std::vector<double> &values, const double &wgt ) {
    fValues = values;
    fWeight = wgt;
  }

}

#endif
