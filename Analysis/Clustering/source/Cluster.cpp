///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 22/07/2016                                                      //
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
Analysis::Cluster::Cluster() { }

//_______________________________________________________________________________
// Copy constructor
Analysis::Cluster::Cluster( const Cluster &other ) {
  
  fCenterOfMass = other.fCenterOfMass;
  fPoints       = other.fPoints;
  fWeights      = other.fWeights;
}

//_______________________________________________________________________________
// Constructor given the array of weights
Analysis::Cluster::Cluster( const std::vector<double> &weights ) : fWeights( weights ) { }

//_______________________________________________________________________________
// Destructor
Analysis::Cluster::~Cluster() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Adds a new point to the cluster. The center of mass is automatically calculated.
void Analysis::Cluster::AddPoint( const Analysis::ClusterPoint &point ) {

  fCenterOfMass   = ClusterPoint::CenterOfMass( fCenterOfMass, point, fWeights );
  fPoints.push_back( point );
}

//_______________________________________________________________________________
// Calculates the dispersion (squared standard deviation) of the points in the cluster
double Analysis::Cluster::Dispersion() const {

  double s2 = 0;
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    s2 += this -> DistanceToCluster( *it );

  return s2/( fPoints.size() - 1 );
}

//_______________________________________________________________________________
// Returns the weighted distance between two points. The weight is dividing since
// as its value grows, the distance must turn smaller.
double Analysis::Cluster::Distance( const Analysis::ClusterPoint &pointA,
				    const Analysis::ClusterPoint &pointB ) const {

  double dist2 = 0, val;
  auto
    itc = pointA.GetValues().cbegin(),
    itv = pointB.GetValues().cbegin(),
    itw = fWeights.cbegin();
  
  while ( itc != pointA.GetValues().end() ) {
    val    = ( (*itv++) - (*itc++) )/(*itw++);
    dist2 += val*val;
  }
  
  return dist2;
}

//_______________________________________________________________________________
// Merges two clusters into one, owning all the points
Analysis::Cluster Analysis::Cluster::MergeClusters( const Cluster &clusterA,
						    const Cluster &clusterB ) {

  Cluster cluster( clusterA );

  std::vector<double> weights( clusterA.fWeights );
  auto itwB = clusterB.fWeights.begin();
  auto itw  = weights.begin();
  
  while ( itw != weights.end() )
    *itw++ += *itwB++;

  cluster.fPoints.insert( cluster.fPoints.end(), clusterB.fPoints.cbegin(), cluster.fPoints.cend() );
  cluster.fCenterOfMass = Analysis::ClusterPoint::CenterOfMass( clusterA.fCenterOfMass,
								clusterB.fCenterOfMass,
								weights );
  return cluster;
}

//_______________________________________________________________________________
// Normalizes the values in the points of the cluster
void Analysis::Cluster::Normalize( const std::vector<double> &values ) {
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    it -> Normalize( values );
  fCenterOfMass.Normalize( values );
}
