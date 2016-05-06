///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 06/05/2016                                                      //
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


#include "Cluster.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::Cluster::Cluster() { }

//_______________________________________________________________________________
// Copy constructor
General::Cluster::Cluster( const Cluster &other ) {
  
  fCenterOfMass = other.fCenterOfMass;
  fPoints       = other.fPoints;
  fWeights      = other.fWeights;
}

//_______________________________________________________________________________
// Constructor given the array of weights
General::Cluster::Cluster( const std::vector<double> &weights ) : fWeights( weights ) { }

//_______________________________________________________________________________
// Destructor
General::Cluster::~Cluster() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Adds a new point to the cluster. The center of mass is automatically calculated.
void General::Cluster::AddPoint( const General::ClusterPoint &point ) {

  fCenterOfMass = ClusterPoint::CenterOfMass( fCenterOfMass, point, fWeights );
  fPoints.push_back( point );
}

//_______________________________________________________________________________
// Calculates the dispersion (squared standard deviation) of the points in the cluster
double General::Cluster::Dispersion() const {

  double s2 = 0;
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    s2 += this -> DistanceToCluster( *it );

  return s2/( fPoints.size() - 1 );
}

//_______________________________________________________________________________
// Returns the weighted distance between two points
double General::Cluster::Distance( const General::ClusterPoint &pointA,
				   const General::ClusterPoint &pointB ) const {

  double dist2 = 0, val;
  auto
    itc = pointA.GetValues().cbegin(),
    itv = pointB.GetValues().cbegin(),
    itw = fWeights.cbegin();
  
  while ( itc != pointA.GetValues().end() ) {
    val    = (*itw++)*( (*itv++) - (*itc++) );
    dist2 += val*val;
  }
  
  return dist2;
}

//_______________________________________________________________________________
// Merges two clusters into one, owning all the points
General::Cluster General::Cluster::MergeClusters( const Cluster &clusterA,
						  const Cluster &clusterB ) {

  Cluster cluster( clusterA );

  std::vector<double> weights( clusterA.fWeights );
  auto itwB = clusterB.fWeights.begin();
  auto itw  = weights.begin();
  
  while ( itw != weights.end() )
    *itw++ += *itwB++;

  cluster.fPoints.insert( cluster.fPoints.end(), clusterB.fPoints.cbegin(), cluster.fPoints.cend() );
  cluster.fCenterOfMass = General::ClusterPoint::CenterOfMass( clusterA.fCenterOfMass,
							       clusterB.fCenterOfMass,
							       weights );
  return cluster;
}

//_______________________________________________________________________________
// Normalizes the values in the points of the cluster
void General::Cluster::Normalize( const std::vector<double> &values ) {
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    it -> Normalize( values );
  fCenterOfMass.Normalize( values );
}
