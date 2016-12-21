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


#ifndef CLUSTER_POINT
#define CLUSTER_POINT

#include <iostream>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class ClusterPoint {

  public:

    // Constructor and destructor
    ClusterPoint( const size_t &nvars = 0, const double &wgt = 1 );
    ClusterPoint( const std::vector<double> &values, const double &wgt = 1 );
    virtual ~ClusterPoint();
    
    // Inline methods
    inline const double               GetValue( const size_t &index ) const;
    inline const std::vector<double>& GetValues() const;
    inline const double               GetWeight() const;
    inline void                       Normalize( const std::vector<double> &norm );
    inline void                       SetValues( const std::vector<double> &values,
						 const double              &wgt = 1 );

  protected:
  
    // Attributes
    std::vector<double> fValues;
    double              fWeight;
    
  };

  //__________________
  // -- INLINE METHODS

  // Returns the value for the variable at position < index >
  const double ClusterPoint::GetValue( const size_t &index ) const { return fValues[ index ]; }
  // Returns the vector with the values and the weights
  const std::vector<double>& ClusterPoint::GetValues() const { return fValues; }
  // Returns the global weight of the point
  const double ClusterPoint::GetWeight() const { return fWeight; }
  // Normalizes the point given the normalization factors for each variable
  void ClusterPoint::Normalize( const std::vector<double> &norm ) {
    auto itv = fValues.begin();
    auto itm = norm.begin();
    while ( itv != fValues.end() )
      *itv++ /= *itm++;
  }
  // Sets the values for this point
  void ClusterPoint::SetValues( const std::vector<double> &values, const double &wgt ) {
    fValues = values;
    fWeight = wgt;
  }

}

#endif
