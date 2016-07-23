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
//  This class defines a set of cluster points. It has attached the weights for  //
//  each variable, the points and a center of mass, which is automatically       //
//  recalculated each time another point is attached.                            //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __CLUSTER__
#define __CLUSTER__

#include "ClusterCenterOfMass.h"

#include <iostream>


//_______________________________________________________________________________

namespace Analysis {

  class Cluster {

  public:

    // Internal definition
    typedef std::vector<ClusterPoint> PointArray;
    
    // Constructor and destructor
    Cluster();
    Cluster( const Cluster &other );
    Cluster( const std::vector<double> &weights );
    virtual ~Cluster();

    // Methods
    void   AddPoint( const ClusterPoint &point );
    double Dispersion() const;
    double DistanceToCluster( const ClusterPoint &point ) const;
    void   Normalize( const std::vector<double> &values );
  
    // Inline methods
    inline void                       AddPoint( const std::vector<double> &values );
    inline void                       Clear();
    inline const ClusterCenterOfMass& GetCenterOfMass() const;
    inline const PointArray&          GetPoints() const;
    inline const double               GetSumOfWeights() const;
    inline const double               GetWeight() const;
    inline void                       InitCenterOfMass( const ClusterPoint &com );
    inline void                       NormalizeCenterOfMass( const std::vector<double> &values );
    inline void                       SetCenterOfMass( const ClusterPoint &com );
    inline void                       SetWeights( const std::vector<double> &weights );

    // Static method
    static Cluster MergeClusters( const Cluster &clusterA, const Cluster &clusterB );
    
  protected:
  
    // Attributes
    ClusterCenterOfMass fCenterOfMass;
    PointArray          fPoints;
    std::vector<double> fWeights;
    
  };
  
  //_______________
  // INLINE METHODS

  // Adds a new point given a vector
  void Cluster::AddPoint( const std::vector<double> &values ) {
    this -> AddPoint( ClusterPoint( values ) );
  }
  // Clears the current cluster
  void Cluster::Clear() { fPoints.clear(); }
  // Returns the current center of mass
  const ClusterCenterOfMass& Cluster::GetCenterOfMass() const { return fCenterOfMass; }
  // Returns the vector of points in the cluster
  const std::vector<ClusterPoint>& Cluster::GetPoints() const { return fPoints; }
  // Returns the sum of weights of the cluster
  const double Cluster::GetSumOfWeights() const { return fCenterOfMass.GetWeight(); }
  // Initializes the center of mass for the cluster
  void Cluster::InitCenterOfMass( const ClusterPoint &com ) {
    fCenterOfMass.InitPosition( com );
  }
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
