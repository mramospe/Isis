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
//  This class defines a set of cluster points. It has attached the weights for
//  each variable, the points and a center of mass, which is automatically
//  recalculated each time another point is attached.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef __CLUSTER__
#define __CLUSTER__

#include "ClusterCenterOfMass.hpp"
#include "Definitions.hpp"

#include <iostream>


//_______________________________________________________________________________

namespace isis {

  class Cluster {

  public:

    // Internal definition of a vector of cluster points
    typedef std::vector<ClusterPoint> PointArray;
    
    // Main constructor
    Cluster();

    // Copy constructor
    Cluster( const Cluster &other );

    // Constructor given the array of weights
    Cluster( const Doubles &weights );

    // Destructor
    virtual ~Cluster();

    // Calculates the dispersion (squared standard deviation) of the points in the
    // cluster. The same could be achieved summing all the distances of the points
    // to the cluster, dividing by the number of points.
    double dispersion() const;

    // Returns the weighted distance between two points. The weight is dividing since
    // as its value grows, the distance must turn smaller.
    double distanceBetweenPoints( const ClusterPoint &pointA,
				  const ClusterPoint &pointB ) const;

    // Normalizes the values in the points of the cluster
    void normalize( const Doubles &values );
  
    // Adds a new point given a cluster point
    inline void addPoint( const ClusterPoint &point );

    // Returns the distance to the center of mass
    inline double distanceToCluster( const ClusterPoint &point ) const;

    // Returns the current center of mass
    inline const ClusterCenterOfMass& getCenterOfMass() const;

    // Returns the vector of points in the cluster
    inline const PointArray& getPoints() const;

    // Returns the sum of weights of the cluster
    inline const double getSumOfWeights() const;

    // Initializes the center of mass for the cluster
    inline void initCenterOfMass( const ClusterPoint &com );

    // Normalizes the center of mass of this cluster
    inline void normalizeCenterOfMass( const Doubles &values );

    // Removes the points of the current cluster
    inline void removePoints();

    // Reset the center of mass weight
    inline void resetCenterOfMassWeight();

    // Sets a new point as the center of mass
    inline void setCenterOfMass( const ClusterPoint &com );

    // Sets the weights for each variable in the points for this cluster
    inline void setWeights( const Doubles &weights );

    // Merges two clusters into one, owning all the points
    static Cluster mergeClusters( const Cluster &clusterA, const Cluster &clusterB );
    
  protected:
  
    // Center of mass of the cluster
    ClusterCenterOfMass fCenterOfMass;

    // Vector of points attached to the cluster
    PointArray fPoints;

    // Vector of weights for each point
    Doubles fWeights;
    
  };

  //_______________________________________________________________________________
  //
  inline void Cluster::addPoint( const ClusterPoint &point ) {
    
    fCenterOfMass.attachPoint( point );
    fPoints.push_back( point );
  }

  //_______________________________________________________________________________
  //
  double Cluster::distanceToCluster( const ClusterPoint &point ) const {
    
    return this->distanceBetweenPoints( fCenterOfMass, point );
  }

  //_______________________________________________________________________________
  //
  const ClusterCenterOfMass& Cluster::getCenterOfMass() const { return fCenterOfMass; }

  //_______________________________________________________________________________
  //
  const std::vector<ClusterPoint>& Cluster::getPoints() const { return fPoints; }

  //_______________________________________________________________________________
  //
  const double Cluster::getSumOfWeights() const { return fCenterOfMass.getWeight(); }

  //_______________________________________________________________________________
  //
  void Cluster::initCenterOfMass( const ClusterPoint &com ) {
    
    fCenterOfMass.initPosition( com );
  }

  //_______________________________________________________________________________
  //
  void Cluster::normalizeCenterOfMass( const Doubles &values ) {
    
    fCenterOfMass.normalize( values );
  }

  //_______________________________________________________________________________
  //
  void Cluster::removePoints() { fPoints.clear(); }

  //_______________________________________________________________________________
  //
  void Cluster::resetCenterOfMassWeight() { fCenterOfMass.resetWeight(); }

  //_______________________________________________________________________________
  //
  void Cluster::setCenterOfMass( const ClusterPoint &com ) { fCenterOfMass = com; }

  //_______________________________________________________________________________
  //
  void Cluster::setWeights( const Doubles &weights ) { fWeights = weights; }

}

#endif
