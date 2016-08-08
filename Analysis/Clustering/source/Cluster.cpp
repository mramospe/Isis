///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 08/08/2016                                                      //
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
// Calculates the dispersion (squared standard deviation) of the points in the
// cluster. The same could be achieved summing all the distances of the points
// to the cluster, dividing by the number of points.
double Analysis::Cluster::Dispersion() const {
  
  auto
    &mean  = fCenterOfMass.GetValues(),
    &mean2 = fCenterOfMass.GetMeanOfSquares();
  auto
    itm = mean.cbegin(),
    it2 = mean2.cbegin();

  double s2 = 0;
  while ( itm != mean.cend() ) {
    
    s2 += (*it2) - (*itm)*(*itm);
    
    ++itm;
    ++it2;
  }
  
  return s2;
}

//_______________________________________________________________________________
// Returns the weighted distance between two points. The weight is dividing since
// as its value grows, the distance must turn smaller.
double Analysis::Cluster::DistanceBetweenPoints( const ClusterPoint &pointA,
						 const ClusterPoint &pointB ) const {
  auto
    itc = pointA.GetValues().cbegin(),
    itv = pointB.GetValues().cbegin(),
    itw = fWeights.cbegin(),
    its = fCenterOfMass.GetMeanOfSquares().cbegin();
  
  double dist2 = 0;
  while ( itc != pointA.GetValues().end() ) {
    double
      pnt = (*itv++),
      ctr = (*itc++),
      wgt = (*itw++),
      val = ( pnt - ctr )/wgt,
      sm2 = (*its++) - ctr*ctr;
    
    dist2 += val*val;///sm2;
  }
  
  return dist2;
}

//_______________________________________________________________________________
// Merges two clusters into one, owning all the points
Analysis::Cluster Analysis::Cluster::MergeClusters( const Cluster &clusterA,
						    const Cluster &clusterB ) {

  Cluster cluster( clusterA );

  std::vector<double> &weights = cluster.fWeights;
  auto itwB = clusterB.fWeights.begin();
  auto itw  = weights.begin();
  
  while ( itw != weights.end() ) {
    *itw = ( (*itw) + (*itwB) )/2.;

    ++itw;
    ++itwB;
  }

  Cluster::PointArray &array = cluster.fPoints;
  array.insert( array.end(), clusterB.fPoints.cbegin(), array.cend() );
  cluster.fCenterOfMass.AttachPoint( clusterB.fCenterOfMass );
  
  return cluster;
}

//_______________________________________________________________________________
// Normalizes the values in the points of the cluster
void Analysis::Cluster::Normalize( const std::vector<double> &values ) {
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    it -> Normalize( values );
  fCenterOfMass.Normalize( values );
}
