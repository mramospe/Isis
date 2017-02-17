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
///////////////////////////////////////////////////////////////////////////////////


#include "Cluster.h"


//_______________________________________________________________________________
//
Analysis::Cluster::Cluster() { }

//_______________________________________________________________________________
//
Analysis::Cluster::Cluster( const Cluster &other ) {
  
  fCenterOfMass = other.fCenterOfMass;
  fPoints       = other.fPoints;
  fWeights      = other.fWeights;
}

//_______________________________________________________________________________
//
Analysis::Cluster::Cluster( const std::vector<double> &weights ) :
  fWeights( weights ) { }

//_______________________________________________________________________________
//
Analysis::Cluster::~Cluster() { }

//_______________________________________________________________________________
//
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
//
double Analysis::Cluster::DistanceBetweenPoints( const ClusterPoint &pointA,
						 const ClusterPoint &pointB ) const {
  auto
    itc = pointA.GetValues().cbegin(),
    itv = pointB.GetValues().cbegin(),
    itw = fWeights.cbegin();
  
  double dist2 = 0;
  while ( itc != pointA.GetValues().end() ) {
    double
      pnt = (*itv++),
      ctr = (*itc++),
      wgt = (*itw++),
      val = ( pnt - ctr )/wgt;
    
    dist2 += val*val;
  }
  
  return dist2;
}

//_______________________________________________________________________________
//
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
//
void Analysis::Cluster::Normalize( const std::vector<double> &values ) {
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    it -> Normalize( values );
  fCenterOfMass.Normalize( values );
}
