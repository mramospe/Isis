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
//  This class defines a set of cluster points. It has attached the weights for
//  each variable, the points and a center of mass, which is automatically
//  recalculated each time another point is attached.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef __CLUSTER__
#define __CLUSTER__

#include "ClusterCenterOfMass.h"
#include "Definitions.h"

#include <iostream>


//_______________________________________________________________________________

namespace Isis {

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
    double Dispersion() const;

    // Returns the weighted distance between two points. The weight is dividing since
    // as its value grows, the distance must turn smaller.
    double DistanceBetweenPoints( const ClusterPoint &pointA,
				  const ClusterPoint &pointB ) const;

    // Normalizes the values in the points of the cluster
    void Normalize( const Doubles &values );
  
    // Adds a new point given a cluster point
    inline void AddPoint( const ClusterPoint &point );

    // Returns the distance to the center of mass
    inline double DistanceToCluster( const ClusterPoint &point ) const;

    // Returns the current center of mass
    inline const ClusterCenterOfMass& GetCenterOfMass() const;

    // Returns the vector of points in the cluster
    inline const PointArray& GetPoints() const;

    // Returns the sum of weights of the cluster
    inline const double GetSumOfWeights() const;

    // Initializes the center of mass for the cluster
    inline void InitCenterOfMass( const ClusterPoint &com );

    // Normalizes the center of mass of this cluster
    inline void NormalizeCenterOfMass( const Doubles &values );

    // Removes the points of the current cluster
    inline void RemovePoints();

    // Reset the center of mass weight
    inline void ResetCenterOfMassWeight();

    // Sets a new point as the center of mass
    inline void SetCenterOfMass( const ClusterPoint &com );

    // Sets the weights for each variable in the points for this cluster
    inline void SetWeights( const Doubles &weights );

    // Merges two clusters into one, owning all the points
    static Cluster MergeClusters( const Cluster &clusterA, const Cluster &clusterB );
    
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
  inline void Cluster::AddPoint( const ClusterPoint &point ) {
    fCenterOfMass.AttachPoint( point );
    fPoints.push_back( point );
  }

  //_______________________________________________________________________________
  //
  double Cluster::DistanceToCluster( const ClusterPoint &point ) const {
    return this -> DistanceBetweenPoints( fCenterOfMass, point );
  }

  //_______________________________________________________________________________
  //
  const ClusterCenterOfMass& Cluster::GetCenterOfMass() const { return fCenterOfMass; }

  //_______________________________________________________________________________
  //
  const std::vector<ClusterPoint>& Cluster::GetPoints() const { return fPoints; }

  //_______________________________________________________________________________
  //
  const double Cluster::GetSumOfWeights() const { return fCenterOfMass.GetWeight(); }

  //_______________________________________________________________________________
  //
  void Cluster::InitCenterOfMass( const ClusterPoint &com ) {
    fCenterOfMass.InitPosition( com );
  }

  //_______________________________________________________________________________
  //
  void Cluster::NormalizeCenterOfMass( const Doubles &values ) {
    fCenterOfMass.Normalize( values );
  }

  //_______________________________________________________________________________
  //
  void Cluster::RemovePoints() { fPoints.clear(); }

  //_______________________________________________________________________________
  //
  void Cluster::ResetCenterOfMassWeight() { fCenterOfMass.ResetWeight(); }

  //_______________________________________________________________________________
  //
  void Cluster::SetCenterOfMass( const ClusterPoint &com ) { fCenterOfMass = com; }

  //_______________________________________________________________________________
  //
  void Cluster::SetWeights( const Doubles &weights ) { fWeights = weights; }

}

#endif
