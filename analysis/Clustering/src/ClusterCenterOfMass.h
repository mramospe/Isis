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
#include "Definitions.h"


//_______________________________________________________________________________

namespace isis {

  class ClusterCenterOfMass : public ClusterPoint {

  public:

    // Main constructor
    ClusterCenterOfMass();

    // Constructor given a position
    ClusterCenterOfMass( const ClusterPoint &point );

    // Destructor
    ~ClusterCenterOfMass();

    // Looks if does exist any variance which could lead to infinites
    bool anyNullSigma() const;

    // Recalculates the center of mass when adding a new point
    void attachPoint( const ClusterPoint &point );

    // Normalizes the position of this center of mass
    void normalize( const Doubles &norm );

    // Sets the position of the center of mass
    void setValues( const Doubles &values );

    // Adds one dimension to the center of mass
    inline void addDimension();

    // Returns the vector to the mean of squares
    inline const Doubles& getMeanOfSquares() const;

    // Initializes the class setting the internal weight to zero and defining
    // the position
    inline void initPosition( const ClusterPoint &com );

    // Reset the weight for this center of mass
    inline void resetWeight();

    // Set this class from another center of pass
    inline void operator = ( const ClusterCenterOfMass &point );

    // Set this class from a cluster point
    inline void operator = ( const ClusterPoint &point );

  protected:

    // Vector storing the mean of squares for each point
    Doubles fMeanOfSquares;
    
  };
  
  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::addDimension() {
    
    fValues.push_back( 0. );
    fMeanOfSquares.push_back( 0. );
  }

  //_______________________________________________________________________________
  //
  inline const Doubles& ClusterCenterOfMass::getMeanOfSquares() const {
    
    return fMeanOfSquares;
  }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::initPosition( const ClusterPoint &point ) {
    
    fValues        = point.getValues();
    fMeanOfSquares = fValues;
    fWeight        = 0;
    
    for ( auto it = fMeanOfSquares.begin(); it != fMeanOfSquares.end(); ++it )
      *it *= (*it); 
  }

  //_______________________________________________________________________________
  //
  inline void ClusterCenterOfMass::resetWeight() { fWeight = 0; }

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
    
    this->initPosition( point );
    fWeight = point.getWeight();
  }

}

#endif
