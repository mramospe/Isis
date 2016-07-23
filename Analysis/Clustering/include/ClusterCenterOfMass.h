///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 23/07/2016                                                      //
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


#ifndef CLUSTER_CENTER_OF_MASS
#define CLUSTER_CENTER_OF_MASS

#include "ClusterPoint.h"


//_______________________________________________________________________________

namespace Analysis {

  class ClusterCenterOfMass : public ClusterPoint {

  public:

    // Constructor and destructor
    ClusterCenterOfMass();
    ~ClusterCenterOfMass();

    // Methods
    void AttachPoint( const ClusterPoint &point );
    void Normalize( const std::vector<double> &norm );
    void SetValues( const std::vector<double> &values );

    // Inline method
    inline void                       AddDimension();
    inline const std::vector<double>& GetMeanOfSquares() const;
    inline void                       InitPosition( const ClusterPoint &com );

    // Operators
    inline void operator = ( const ClusterCenterOfMass &point );
    inline void operator = ( const ClusterPoint &point );

  protected:

    // Attribute
    std::vector<double> fMeanOfSquares;

  };
  
  //__________________
  // -- INLINE METHODS

  // Adds one dimension to the center of mass
  inline void ClusterCenterOfMass::AddDimension() {
    fValues.push_back( 0. );
    fMeanOfSquares.push_back( 0. );
  }
  // Returns the vector to the mean of squares
  inline const std::vector<double>& ClusterCenterOfMass::GetMeanOfSquares() const {
    return fMeanOfSquares;
  }
  // Initializes the class setting the internal weight to zero and defining the position
  inline void ClusterCenterOfMass::InitPosition( const ClusterPoint &point ) {
    fValues        = point.GetValues();
    fMeanOfSquares = fValues;
    fWeight        = 0;
    for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
      *it *= (*it); 
  }

  //_____________
  // -- OPERATORS

  // Asign operator
  inline void ClusterCenterOfMass::operator = ( const ClusterCenterOfMass &point ) {
    fValues        = point.fValues;
    fMeanOfSquares = point.fMeanOfSquares;
    fWeight        = point.fWeight;
  }
  // If the class to be asigned to is a ClusterPoint, the mean of squares is set to its values
  inline void ClusterCenterOfMass::operator = ( const ClusterPoint &point ) {
    this -> InitPosition( point );
    fWeight = point.GetWeight();
  }

}

#endif
