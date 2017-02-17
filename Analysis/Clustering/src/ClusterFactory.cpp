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


#include "ClusterFactory.h"
#include "Messenger.h"
#include "Utils.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>


//_______________________________________________________________________________
//
Analysis::ClusterFactory::ClusterFactory( const std::string &opts ) :
  Cluster(),
  fClusteringMethod( &ClusterFactory::ConvergenceMethod ),
  fComDefMethod( &ClusterFactory::DistanceCentersOfMass ),
  fManageClusters( false ),
  fMaxComVar( 1 ),
  fMinNpoints( 100 ),
  fNcomStdDev( 1 ),
  fNclusters( 2 ),
  fNiter( 10 ),
  fVerbose( true ) {
  
  this->Configure( opts );
}

//_______________________________________________________________________________
//
Analysis::ClusterFactory::~ClusterFactory() { }

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::BuildCentersOfMass() {
  
  // Removes the points stored in the clusters
  std::cout << "Removing points in clusters" << std::endl;
  this->Reset();

  std::cout << "Building centers of mass" << std::endl;
  
  // Loops over the clusters to set the initial points
  for ( auto itc = fClusters.begin(); itc != fClusters.end(); ++itc ) {
    
    std::vector< std::pair<PointArray::iterator, double> >
      distances( fPoints.size() - fPointsToAvoid.size() );

    auto itd = distances.begin();
    auto itp = fPoints.begin();
    
    // The distance is taken according to the variance of the main cluster
    while ( itd != distances.end() ) {
      
      if ( std::find( fPointsToAvoid.begin(), fPointsToAvoid.end(), itp ) ==
	   fPointsToAvoid.end() ) {
	itd->first  = itp;
	itd->second = this->DistanceBetweenPoints( itc->GetCenterOfMass(), *itp );
	++itd;
      }
      
      ++itp;
    }
    
    // The points are sorted according to the distance to the cluster, but using the main standard
    // deviation (from the main cluster)
    std::sort( distances.begin(),
	       distances.end(),
	       [] ( const std::pair<PointArray::iterator, double> &itl,
		    const std::pair<PointArray::iterator, double> &itr ) {
		 return itl.second < itr.second; } );

    // Points are added till no null variances are found
    itd = distances.begin();
    do {
      itc->AddPoint( *(itd->first) );
      fPointsToAvoid.push_back( itd->first );
      ++itd;
    } while ( itc->GetCenterOfMass().AnyNullSigma() );

    size_t icluster = itc - fClusters.begin();
    std::cout <<
      "Cluster < " << icluster << " > built with < " << itc->GetPoints().size() << " > points"
		   << std::endl;
  }

  std::cout << "Defined initial centers of mass" << std::endl;
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::CalculateClusters() {

  // Displays the initial message and the configuration values
  std::cout << "*************************************" << std::endl;
  std::cout << "*** Starting to generate clusters ***" << std::endl;
  std::cout << "*************************************" << std::endl;
  std::cout << "Length of the input data: " << fPoints.size() << std::endl;
  std::cout << "Variables used (" << fVarOrder.size() << "):" << std::endl;
  for ( auto it = fVarOrder.begin(); it != fVarOrder.end(); ++it )
    std::cout << " - " << *it << std::endl;
  std::cout << "Initial number of clusters:      " << fNclusters << std::endl;
  std::cout << "Maximum variation of centers:    " << fMaxComVar*fVarNorm.size() << std::endl;
  std::cout << "Minimum distance among clusters: " << fNcomStdDev << std::endl;
  std::cout << "Maximum number of iterations:    " << fNiter   << std::endl;

  // Sorts the input points randomly to avoid ordering effects
  std::random_shuffle( fPoints.begin(), fPoints.end() );
  std::cout << "Input points randomly sorted" << std::endl;
  
  // Applies the normalization using the standard deviation of each sample to do it
  std::cout << "Normalizing values in points" << std::endl;
  size_t npoints = fPoints.size();
  for ( size_t inr = 0; inr < fVarNorm.size(); ++inr ) {
    
    double
      mean  = 0,
      mean2 = 0;
    
    for ( auto itp = fPoints.cbegin(); itp != fPoints.cend(); ++itp ) {
      double val = itp->GetValue( inr );
      mean  += val;
      mean2 += val*val;
    }
    fVarNorm[ inr ] = std::sqrt( mean2/npoints - mean/( npoints*npoints ) );
  }
  for ( auto it = fPoints.begin(); it != fPoints.end(); ++it )
    it->Normalize( fVarNorm );
  fCenterOfMass.Normalize( fVarNorm );
  
  // Defines the initial centers of mass. If this function manages the number of clusters, all of
  // them will have the same set of weights, equal to those of the factory.
  std::cout << "Defining cluster weights" << std::endl;
  if ( fManageClusters ) {
    std::cout <<
      "The factory manages the number of clusters; all weights set to those in the factory"
	      << std::endl;
    fClusters.insert( fClusters.end(), fNclusters, Analysis::Cluster( fWeights ) );
  }
  else {
    if ( fClusterWeights.size() == 0 ) {
      std::cout << "No weights provided; setting to those in the factory" << std::endl;
      fClusters.insert( fClusters.end(), fNclusters, Analysis::Cluster( fWeights ) );
    }
    else {
      for ( size_t i = 0; i < fNclusters; ++i )
	if ( fClusterWeights.find( i ) == fClusterWeights.end() ) {
	  std::cout <<
	    "Weights for cluster < " << i << " > not provided; set to those in the factory"
				     << std::endl;
	  fClusters.push_back( Analysis::Cluster( fWeights ) );
	}
	else
	  fClusters.push_back( Analysis::Cluster( fClusterWeights.at( i ) ) );
    }
  }
  (this->*fComDefMethod)();
  if ( fVerbose )
    this->Display( &ClusterFactory::PrintCentersOfMass, "Initial centers of mass" );
  
  
  // Calls the center of mass definition algorithm as well as the clustering algorithm. If the
  // capability to modify the number of clusters has been given, looks for the minimum number
  // of clusters that satisfy the requirements.
  if ( fManageClusters ) {
    
    std::cout << "*** Looking for the best cluster configuration ***" << std::endl;

    // Calls the clustering method for the first time
    (this->*fClusteringMethod)();
    
    // Defines the clusters for the first time to determine the satus
    bool
      dec       = true,
      oldstatus = this->ManageClusters();
    
    // The convergence process adding and substracting clusters starts here
    size_t counter = 0;
    while ( dec ) {
      
      std::cout << "--- Started new clustering process: " << counter++ << " ---" << std::endl;
      
      // Here a cluster is added or substracted
      if ( oldstatus )
	fClusters.push_back( Analysis::Cluster( fWeights ) );
      else
	fClusters.pop_back();

      // Calls again the two clustering methods to get the new status
      (this->*fComDefMethod)();
      (this->*fClusteringMethod)();
      bool newstatus = this->ManageClusters();
      
      // If the status changes it exits the loop
      if ( newstatus != oldstatus ) {
	dec = false;
	if ( oldstatus ) {
	  fClusters.pop_back();
	  (this->*fComDefMethod)();
	  (this->*fClusteringMethod)();
	}
      }
    }
  }
  else {
    // Here the class just calls the clustering method and checks if it has converged
    if ( (this->*fClusteringMethod)() )
      std::cout << "Clustering process successful" << std::endl;
    else
      IWarning << "The method has not converged yet" << IEndMsg;
  }
  std::cout << "Number of clusters: " << fClusters.size() << std::endl;
    
  // Displays the final results
  if ( fVerbose )
    this->Display( &ClusterFactory::PrintCentersOfMass, "Final centers of mass" );

  
  // Renormalizes the values in the different clusters, so the points now will have the true
  // value stored
  std::vector<double> invnorm( fVarNorm );
  for ( auto it = invnorm.begin(); it != invnorm.end(); ++it )
    *it = 1/(*it);
  for ( auto it = fClusters.begin(); it != fClusters.end(); ++it ) {
    it->Normalize( invnorm );
  }
  this->Normalize( invnorm );

  std::cout << "***************************************" << std::endl;
  std::cout << "*** Generation of clusters finished ***" << std::endl;
  std::cout << "***************************************" << std::endl;
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::Configure( const std::string &opts ) {

  // Checks that the given string is correctly written
  if ( opts.size() )
    General::CheckParseOpts( opts,
			     {   "ComDefMethod",
				 "ClusteringMethod",
				 "ManageClusters",
				 "MaxComVar",
				 "MinNpoints",
				 "nComStdDev",
				 "nClusters",
				 "nIter",
				 "Verbose"         } );

  // Parses the options referring to the center of mass definition
  if ( opts.find( "ComDefMethod" ) != std::string::npos ) {
    std::string method;
    General::ParseOpt( opts, "ComDefMethod", method );
    if ( method == "Random" )
      fComDefMethod = &ClusterFactory::RandomCentersOfMass;
    else if ( method == "Distance" )
      fComDefMethod = &ClusterFactory::DistanceCentersOfMass;
    else {
      IWarning << "Input method < " << method << " > not known; set to default" << IEndMsg;
      fComDefMethod = &ClusterFactory::RandomCentersOfMass;
    }
  }

  // Parses the option refering to the clustering method
  if ( opts.find( "ClusteringMethod" ) != std::string::npos ) {
    std::string method;
    General::ParseOpt( opts, "ClusteringMethod", method );
    if ( method == "Iterative" )
      fClusteringMethod = &ClusterFactory::IterativeMethod;
    else if ( method == "Convergence" )
      fClusteringMethod = &ClusterFactory::ConvergenceMethod;
    else {
      IWarning << "Input method < " << method << " > not known; set to default" << IEndMsg;
      fClusteringMethod = &ClusterFactory::IterativeMethod;
    }
  }

  // Allows the class to add or remove clusters if necessary
  if ( opts.find( "ManageClusters" ) != std::string::npos )
    General::ParseOpt( opts, "ManageClusters", fManageClusters );

  // Maximum distance allowed in the convergence method
  if ( opts.find( "MaxComVar" ) != std::string::npos )
    General::ParseOpt( opts, "MaxComVar", fMaxComVar );

  // Minimum number of points allowed per cluster
  if ( opts.find( "MinNpoints" ) != std::string::npos )
    General::ParseOpt( opts, "MinNpoints", fMinNpoints );

  // Number of standard deviations between clusters
  if ( opts.find( "nComStdDev" ) != std::string::npos )
    General::ParseOpt( opts, "nComStdDev", fNcomStdDev );

  // Number of initial clusters to look for
  if ( opts.find( "nClusters" ) != std::string::npos )
    General::ParseOpt( opts, "nClusters", fNclusters );

  // Maximum number of iterations in both the iterative and the convergence methods
  if ( opts.find( "nIter" ) != std::string::npos )
    General::ParseOpt( opts, "nIter"  , fNiter   );

  // Verbose level
  if ( opts.find( "Verbose" ) != std::string::npos )
    General::ParseOpt( opts, "Verbose", fVerbose );
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::PrintCentersOfMass( std::string title ) {
  
  // If the number of clusters or the maximum size of the names of the variables is smaller than
  // the default value, it is set to it
  size_t
    maxvarsize = std::max_element( fVarOrder.begin(), fVarOrder.end(),
				   [] ( const std::string &str1,
					const std::string &str2 ) {
				     return str1.size() < str2.size(); } )->size() + 2,
    nclsize = std::to_string( fClusters.size() ).size() + 2;
  if ( nclsize < 8 )
    nclsize = 8;
  if ( maxvarsize < 12 )
    maxvarsize = 12;

  // If a title is provided it displays it with the decorator. If not, only the decorator is shown.
  std::string decorator;
  size_t decosize;
  if ( title.size() ) {
    title    = "--- " + title + " ---";
    decosize = title.size();
    decorator.insert( decorator.end(), decosize, '-' );
    std::cout << decorator << "\n" << title << std::endl;
  }
  
  // The decorator is different if the title is given or not
  decosize = ( maxvarsize + 2 )*fVarNorm.size() + nclsize + 3;
  if ( decosize < title.size() ) {
    std::cout << decorator << std::endl;
    decorator = std::string( decosize, '-' );
  }
  else {
    decorator = std::string( decosize, '-' );
    std::cout << decorator << std::endl;
  }
  
  // Displays the titles of the magnitudes used
  std::cout << '|' << General::CenterString( "Cluster", nclsize + 1 );
  for ( auto itv = fVarOrder.begin(); itv != fVarOrder.end(); ++itv )
    std::cout << '|' << General::CenterString( *itv, maxvarsize ) << ' ';
  std::cout << "|\n" << decorator << std::endl;
  std::cout << std::right;
  std::cout.precision( 4 );

  // Prints the information for each cluster
  size_t icluster = 0;
  for ( auto it = fClusters.begin(); it != fClusters.end(); ++it, ++icluster ) {
    std::cout << '|' << std::setw( nclsize ) << icluster << ' ';
    for ( auto itv = it->GetCenterOfMass().GetValues().begin();
	  itv != it->GetCenterOfMass().GetValues().end(); ++itv ) {
      std::cout << '|' << std::setw( maxvarsize ) << *itv << ' ';
    }
    std::cout << '|' << std::endl;
  }
  std::cout << decorator << std::endl;

  // Prints the information of the main cluster
  std::cout << '|' << std::setw( nclsize + 1 ) << "Main ";
  for ( auto itv = fCenterOfMass.GetValues().begin();
	itv != fCenterOfMass.GetValues().end(); ++itv ) {
    std::cout << '|' << std::setw( maxvarsize ) << *itv << ' ';
  }
  std::cout << "|\n" << decorator << std::endl;
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::PrintDistances( std::string title ) {
  
  // If the size given the number of clusters is small, the size for the numbers is set to
  // the limit
  size_t
    clsize  = std::to_string( fClusters.size() ).size() + 7,
    numsize = clsize;
  if ( numsize < 12 )
    numsize = 12;
  std::string decorator( clsize + 6 + numsize*fClusters.size(), '-' );
  
  // Displays the title and the first row (with the numbers of the clusters)
  std::cout.precision( 4 );
  if ( title.size() ) {
    title = "--- " + title + " ---";
    std::string titledeco( title.size(), '-' );
    std::cout << titledeco << std::endl;
    std::cout << title << std::endl;
    if ( titledeco.size() > decorator.size() )
      std::cout << titledeco << std::endl;
    else
      std::cout << decorator << std::endl;
  }
  else
    std::cout << decorator << std::endl;
  std::cout << std::setfill( ' ' ) << std::right << "| " << std::setw( clsize ) << "Cluster" << " |";
  for ( size_t icl = 0; icl < fClusters.size(); ++icl )
    std::cout << General::CenterString( std::to_string( icl ), numsize );
  std::cout << " |" << std::endl;
  std::cout << decorator << std::endl;
  
  // Loops over the clusters to get the distances among them
  size_t icl = 0;
  for ( auto itcr = fClusters.begin(); itcr != fClusters.end(); ++itcr, ++icl ) {
    std::cout << "| " << std::setw( clsize ) << icl << " |";
    for ( auto itcc = fClusters.begin(); itcc != fClusters.end(); ++itcc ) {
      if ( itcr == itcc )
	std::cout << std::setw( numsize ) << 0;
      else
	std::cout << std::setw( numsize ) <<  itcr->DistanceToCluster( itcc->GetCenterOfMass() );
    }
    std::cout << " |" << std::endl;
  }
  std::cout << decorator << std::endl;
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::SetClusterWeights( const int &index,
						  const std::vector<double> &wgts ) {
  if ( wgts.size() != fVarNorm.size() ) {
    IError << "The length of the weights must match the number of variables" << IEndMsg;
    return;
  }
  if ( index > 0 )
    fClusterWeights[ index ] = wgts;
  else {
    this->SetWeights( wgts );
    for ( size_t i = 0; i < fClusters.size(); ++i )
      fClusterWeights[ i ] = wgts;
  }
}

//_______________________________________________________________________________
//
inline void Analysis::ClusterFactory::Reset() {

  for ( auto itc = fClusters.begin(); itc != fClusters.end(); ++itc ) {
    itc->ResetCenterOfMassWeight();
    itc->RemovePoints();}

  fPointsToAvoid.clear();
}

//_______________________________________________________________________________
//
inline void Analysis::ClusterFactory::Display( void (ClusterFactory::*funcptr)( std::string ),
					       const std::string &title ) {
  
  bool dec = funcptr == &ClusterFactory::PrintCentersOfMass;

  if ( dec ) {
    // Removes the normalization from the clusters
    std::vector<double> invnorm( fVarNorm );
    for ( auto it = invnorm.begin(); it != invnorm.end(); ++it )
      *it = 1./(*it);
    for ( auto it = fClusters.begin(); it != fClusters.end(); ++it )
      it->NormalizeCenterOfMass( invnorm );
    fCenterOfMass.Normalize( invnorm );
  }

  // Calls the function
  (this->*funcptr)( title );

  if ( dec ) {
    // Renormalizes the clusters back to the previous status
    for ( auto it = fClusters.begin(); it != fClusters.end(); ++it )
      it->NormalizeCenterOfMass( fVarNorm );
    fCenterOfMass.Normalize( fVarNorm );
  }
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::DistanceCentersOfMass() {
  
  // First calculates the distances from the points to the center of mass of the main cluster
  std::cout << "Calculating distance from points to the main center of mass" << std::endl;
  std::vector< std::pair<PointArray::iterator, double> > distances( fPoints.size() );
  auto itd = distances.begin();
  for ( auto itp = fPoints.begin(); itp != fPoints.end(); ++itp, ++itd ) {
    itd->first  = itp;
    itd->second = this->DistanceToCluster( *itp );
  }
  
  // Sorts the points to select the maximums
  std::sort( distances.begin(),
	     distances.end(),
	     [] ( const std::pair<PointArray::iterator, double> &itl,
		  const std::pair<PointArray::iterator, double> &itr ) {
	       return itl.second > itr.second; } );
  std::cout << "Cluster points sorted to look for those most separated" << std::endl;
  
  // The points are taken in such a way, that they are separated at least by the same amount
  // as the most distant point to the center of mass of the main cluster
  double mindist = distances.front().second;
  itd = distances.begin();
  for ( auto it = fClusters.begin(); it != fClusters.end(); ++it ) {
    
    it->InitCenterOfMass( *(itd++->first) );
    
    for ( auto itc = fClusters.begin(); itc != it; ++itc ) {
      
      const Analysis::ClusterCenterOfMass &currctr = itc->GetCenterOfMass();
      
      while ( this->DistanceBetweenPoints( it->GetCenterOfMass(), currctr ) < mindist &&
	      itd != distances.end() )
	it->InitCenterOfMass( *(itd++->first) );
    }
    
    if ( itd == distances.end() ) {
      IWarning <<
	"The number of clusters could be too high; using a smaller limit distance"
	       << IEndMsg;
      mindist /= 2;
      
      it  = fClusters.begin();
      itd = distances.begin();
    }
  }

  // Adds as many points as needed in such a way that the dispersion is different from zero
  this->BuildCentersOfMass();
}

//_______________________________________________________________________________
//
void Analysis::ClusterFactory::RandomCentersOfMass() {
  
  // Generates a random vector with the position associated with each center of mass
  std::vector<size_t> positions( fClusters.size() );
  size_t rndm;
  for ( auto it = positions.begin(); it != positions.end(); ++it ) {
    do {
      rndm = std::rand() % fPoints.size();
    } while ( std::find( positions.begin(), positions.end(), rndm ) == positions.end() );
    *it = rndm;
  }

  // Attaches one center of mass to each cluster
  std::vector<Cluster>::iterator it;
  std::vector<size_t>::iterator itp;
  for ( it = fClusters.begin(), itp = positions.begin(); it != fClusters.end(); ++it, ++itp )
    it->InitCenterOfMass( fPoints[ *itp ] );

  // Adds as many points as needed in such a way that the dispersion is different from zero
  this->BuildCentersOfMass();
}

//_______________________________________________________________________________
//
bool Analysis::ClusterFactory::ConvergenceMethod() {
    
  // Loops till the maximum variation on the center of mass is smaller than that required
  // or till the maximum number of iterations is satisfied.
  size_t iiter = 0;
  double maxdst = fMaxComVar*fVarNorm.size();
  std::vector<double> comdists( fClusters.size() );
  PointArray centersOfMass( fClusters.size() );
  
  do {
    
    // Gets the position of the centers of mass to calculate the variation of their position
    auto itc = fClusters.begin();
    for ( auto nitc = centersOfMass.begin(); nitc != centersOfMass.end(); ++nitc, ++itc )
      *nitc = itc->GetCenterOfMass();

    // Call to the main method to define the clusters
    this->DistanceMerging();
    
    auto itd = comdists.begin();
    auto itp = centersOfMass.begin();
    itc = fClusters.begin();
    
    while ( itc != fClusters.end() )
      *itd++ = itc++->DistanceToCluster( *itp++ );
        
  } while ( *std::max_element( comdists.begin(), comdists.end() ) > maxdst && ++iiter < fNiter );

  // Also returns < true > if the limit of iterations has not been reached
  if ( iiter != fNiter )
    return true;

  return false;
}

//_______________________________________________________________________________
//
inline void Analysis::ClusterFactory::DistanceMerging() {

  // Removes the points stored in the clusters
  std::cout << "Removing points in clusters" << std::endl;
  this->Reset();
  
  // Generates the clusters taking into account the distances from the points to them
  std::cout << "Merging process started" << std::endl;
  std::vector<double> distances( fClusters.size() );
  for ( auto itp = fPoints.begin(); itp != fPoints.end(); ++itp ) {
    
    if ( std::find( fPointsToAvoid.begin(), fPointsToAvoid.end(), itp ) == fPointsToAvoid.end() ) {

      auto itd = distances.begin();
      auto itc = fClusters.begin();
      
      while ( itc != fClusters.end() )
	*itd++ = itc++->DistanceToCluster( *itp );
    
      itd = std::min_element( distances.begin(), distances.end() );
      fClusters[ itd - distances.begin() ].AddPoint( *itp );
    }
  }
  std::cout << "Generated new set of clusters" << std::endl;
  
  if ( fVerbose )
    this->Display( &ClusterFactory::PrintDistances, "Normalized distances" );
}

//_______________________________________________________________________________
//
bool Analysis::ClusterFactory::IterativeMethod() {
  
  // Process flag
  bool converged = false;
  
  // Vector to store the distances between centers of mass and points, as well as among the clusters
  double maxdst = fMaxComVar*fVarNorm.size();
  std::vector<double> comdists( fClusters.size(), 2*maxdst );
  
  // Saves the current centers of mass for the clusters
  PointArray centersOfMass( fClusters.size() );
  auto itc = fClusters.begin();
  for ( auto nitc = centersOfMass.begin(); nitc != centersOfMass.end(); ++nitc, ++itc )
    *nitc = itc->GetCenterOfMass();
  
  // Loops < fNiter > times to generate the clusters
  for ( size_t iiter = 0; iiter < fNiter; ++iiter ) {

    // Displays the iteration number
    std::cout << "-- Iteration number " << iiter + 1 << " -- " << std::endl;
    
    // Call to the main method to define the clusters
    this->DistanceMerging();

    auto itd = comdists.begin();
    auto itp = centersOfMass.begin();
    auto itc = fClusters.begin();
    
    while ( itc != fClusters.end() )
      *itd++ = itc++->DistanceToCluster( *itp++ );

    if ( *std::max_element( comdists.begin(), comdists.end() ) > maxdst ) {
      std::cout << "The method has not converged yet" << std::endl;
      converged = false;
    }
    else
      converged = true;
    
    itc = fClusters.begin();
    for ( auto nitc = centersOfMass.begin(); nitc != centersOfMass.end(); ++nitc, ++itc )
      *nitc = itc->GetCenterOfMass();
  }
  
  return converged;
}

//_______________________________________________________________________________
//
bool Analysis::ClusterFactory::ManageClusters() {
  
  // Calculates the dispersions of the clusters to get the selection decision
  std::cout << "Calculating dispersions of clusters" << std::endl;
  std::vector<double> dispersions( fClusters.size() );
  auto itdr = dispersions.begin();
  for ( auto it = fClusters.begin(); it != fClusters.end(); ++it, ++itdr )
    *itdr = it->Dispersion();
  
  // Iterates over the clusters to apply the decision
  double nstddev2 = fNcomStdDev*fNcomStdDev;
  itdr = dispersions.begin();
  for ( auto itcr = fClusters.begin(); itcr != fClusters.end(); ++itcr, ++itdr ) {
    
    auto itcc = itcr + 1;
    auto itdc = itdr + 1;
    
    while ( itcc != fClusters.end() ) {
      double dist = itcr->DistanceToCluster( itcc->GetCenterOfMass() );
      if ( dist < nstddev2*( (*itdr) + (*itdc) ) ) {
	std::cout << "Removing cluster: two clusters are too close" << std::endl;
	return false;
      }
      else if ( itcr->GetPoints().size() < fMinNpoints ) {
	std::cout << "Removing cluster: number of points in cluster too small" << std::endl;
	return false;
      }
      else {
	++itcc;
	++itdc;
      }
    }
  }
  
  return true;
}
