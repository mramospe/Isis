///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 09/05/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  This class defines a set of cluster points. It has attached the weights for  //
//  each variable, the points and a center of mass, which is automatically       //
//  recalculated each time another point is attached.                            //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __CLUSTER__
#define __CLUSTER__

#include "ClusterPoint.h"
#include <iostream>


//_______________________________________________________________________________

namespace Analysis {

  class Cluster {

  public:
  
    // Constructor and destructor
    Cluster();
    Cluster( const Cluster &other );
    Cluster( const std::vector<double> &weights );
    virtual ~Cluster();

    // Methods
    void   AddPoint( const ClusterPoint &point );
    double Dispersion() const;
    double Distance( const ClusterPoint &pointA, const ClusterPoint &pointB ) const;
    void   Normalize( const std::vector<double> &values );
  
    // Inline methods
    inline void AddPoint( const std::vector<double> &values );
    inline void Clear();
    inline double DistanceToCluster( const ClusterPoint &point ) const;
    inline const ClusterPoint& GetCenterOfMass() const;
    inline const std::vector<ClusterPoint>& GetPoints() const;
    inline const double GetSumOfWeights() const;
    inline const double GetWeight() const;
    inline void NormalizeCenterOfMass( const std::vector<double> &values );
    inline void SetCenterOfMass( const ClusterPoint &com );
    inline void SetWeights( const std::vector<double> &weights );

    // Static method
    static Cluster MergeClusters( const Cluster &clusterA, const Cluster &clusterB );

    // Internal definition
    typedef std::vector<ClusterPoint> PointArray;

  protected:
  
    // Attributes
    ClusterPoint        fCenterOfMass;
    PointArray          fPoints;
    std::vector<double> fWeights;
    
  };

  // Adds a new point given a vector
  void Cluster::AddPoint( const std::vector<double> &values ) {
    this -> AddPoint( ClusterPoint( values ) );
  }
  // Clears the current cluster
  void Cluster::Clear() { fPoints.clear(); }
  // Returns the weighted distance from the given point to the center of mass of the cluster
  double Cluster::DistanceToCluster( const ClusterPoint &point ) const {
    return this -> Distance( point, fCenterOfMass );
  }
  // Returns the current center of mass
  const ClusterPoint& Cluster::GetCenterOfMass() const { return fCenterOfMass; }
  // Returns the vector of points in the cluster
  const std::vector<ClusterPoint>& Cluster::GetPoints() const { return fPoints; }
  // Returns the sum of weights of the cluster
  const double Cluster::GetSumOfWeights() const { return fCenterOfMass.GetWeight(); }
  // Normalizes the center of mass of this cluster
  void Cluster::NormalizeCenterOfMass( const std::vector<double> &values ) {
    fCenterOfMass.Normalize( values );
  }
  // Sets a new point as the center of mass
  void Cluster::SetCenterOfMass( const ClusterPoint &com ) { fCenterOfMass = com; }
  // Sets the weights for each variable in the points for this cluster
  void Cluster::SetWeights( const std::vector<double> &weights ) { fWeights = weights; }

}

#endif
