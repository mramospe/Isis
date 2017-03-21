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
///////////////////////////////////////////////////////////////////////////////////


#include "Cluster.h"
#include "Definitions.h"


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  Cluster::Cluster() { }

  //_______________________________________________________________________________
  //
  Cluster::Cluster( const Cluster &other ) {
  
    fCenterOfMass = other.fCenterOfMass;
    fPoints       = other.fPoints;
    fWeights      = other.fWeights;
  }

  //_______________________________________________________________________________
  //
  Cluster::Cluster( const Doubles &weights ) :
    fWeights( weights ) { }

  //_______________________________________________________________________________
  //
  Cluster::~Cluster() { }

  //_______________________________________________________________________________
  //
  double Cluster::Dispersion() const {
  
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
  double Cluster::DistanceBetweenPoints( const ClusterPoint &pointA,
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
  Cluster Cluster::MergeClusters( const Cluster &clusterA,
				  const Cluster &clusterB ) {

    Cluster cluster( clusterA );

    Doubles &weights = cluster.fWeights;
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
  void Cluster::Normalize( const Doubles &values ) {
    for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
      it->Normalize( values );
    fCenterOfMass.Normalize( values );
  }

}
