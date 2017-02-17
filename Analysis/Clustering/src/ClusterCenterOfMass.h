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
//  Class defining a center of mass of a cluster, storing its position and the
//  mean of squares.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLUSTER_CENTER_OF_MASS
#define CLUSTER_CENTER_OF_MASS

#include "ClusterPoint.h"


//_______________________________________________________________________________

namespace Analysis {

  class ClusterCenterOfMass : public ClusterPoint {

  public:

    // Main constructor
    ClusterCenterOfMass();

    // Constructor given a position
    ClusterCenterOfMass( const ClusterPoint &point );

    // Destructor
    ~ClusterCenterOfMass();

    // Looks if does exist any variance which could lead to infinites
    bool AnyNullSigma() const;

    // Recalculates the center of mass when adding a new point
    void AttachPoint( const ClusterPoint &point );

    // Normalizes the position of this center of mass
    void Normalize( const std::vector<double> &norm );

    // Sets the position of the center of mass
    void SetValues( const std::vector<double> &values );

    // Adds one dimension to the center of mass
    inline void AddDimension();

    // Returns the vector to the mean of squares
    inline const std::vector<double>& GetMeanOfSquares() const;

    // Initializes the class setting the internal weight to zero and defining
    // the position
    inline void InitPosition( const ClusterPoint &com );

    // Reset the weight for this center of mass
    inline void ResetWeight();

    // Set this class from another center of pass
    inline void operator = ( const ClusterCenterOfMass &point );

    // Set this class from a cluster point
    inline void operator = ( const ClusterPoint &point );

  protected:

    // Vector storing the mean of squares for each point
    std::vector<double> fMeanOfSquares;
    
  };
  
  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::AddDimension() {
    fValues.push_back( 0. );
    fMeanOfSquares.push_back( 0. );
  }

  //_______________________________________________________________________________
  //
  inline const std::vector<double>& ClusterCenterOfMass::GetMeanOfSquares() const {
    return fMeanOfSquares;
  }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::InitPosition( const ClusterPoint &point ) {
    fValues        = point.GetValues();
    fMeanOfSquares = fValues;
    fWeight        = 0;
    for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
      *it *= (*it); 
  }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::ResetWeight() { fWeight = 0; }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::operator = ( const ClusterCenterOfMass &point ) {
    fValues        = point.fValues;
    fMeanOfSquares = point.fMeanOfSquares;
    fWeight        = point.fWeight;
  }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::operator = ( const ClusterPoint &point ) {
    this -> InitPosition( point );
    fWeight = point.GetWeight();
  }

}

#endif
