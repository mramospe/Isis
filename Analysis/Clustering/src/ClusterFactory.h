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
//  Main class to perform cluster analysis. It can work by two different ways:
//  the first of them implies that the number of clusters has to be defined
//  previously, while in the second is the proper class which defines the
//  number of clusters that fit the given data. The different options are set
//  using the constructor or the method < Configure >. All methods work better
//  with gaussian distributions. One must specify after the construction, the
//  variables to work with, in order to avoid dimensional errors.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLUSTER_FACTORY
#define CLUSTER_FACTORY

#include "Cluster.h"

#include <map>
#include <vector>
#include <string>


//_______________________________________________________________________________

namespace Isis {

  class ClusterFactory : public Cluster {

  public:

    // Main constructor. The options for the factory must be given in a string,
    // following the convention of the < Parse > functions defined in Utils.h. The
    // configuration options can be seen in the method < Configure >.
    ClusterFactory( const std::string &opts = "" );

    // Destructor
    ~ClusterFactory();

    // Function to be called after the position of the center of mass is set for
    // each cluster. Since for the calculation of the variance more than one point
    // is needed, it collects the minimal number of closest points that are necessary
    // to avoid this quantity from being zero.
    void BuildCentersOfMass();

    // Main function that calculates the clusters
    void CalculateClusters();

    // Configure the current factory with the options given. The different options
    // are the following:
    //  - ComDefMethod     => Method to define the initial position of the centers of
    //                        mass of the clusters (Distance/Random).
    //  - ClusteringMethod => Method to perform the clustering process (Iterative/
    //                        Convergence).
    //  - ManageClusters   => Enables or disables the capability of this class to add
    //                        or substract clusters.
    //  - MaxComVar        => Maximum variation of the centers of mass allowed for
    //                        the < Convergence > method. In the < Iterative > method
    //                        only a warning will be displayed.
    //  - nComStdDev       => Number of standard deviations between clusters. If two
    //                        clusters are separated by an amount smaller than the
    //                        sum of both standard deviations multiplied by this
    //                        quantity, one of them will be removed.
    //  - MinNpoints       => Minimum number of points needed to consider a cluster.
    //  - nClusters        => Initial number of clusters.
    //  - nIter            => Number of iterations. For the < Iterative > method, it
    //                        will be considered as the maximum number of iterations
    //                        to be performed. In the < Convergence > method, it is
    //                        set to the maximum number of iterations that can be
    //                        performed to look for convergence.
    //  - Verbose          => Controls the display of the messages of the class.
    //
    void Configure( const std::string &opts );

    // Displays the current centers of mass of the clusters
    void PrintCentersOfMass( std::string title = std::string() );

    // Displays the distances among the different clusters
    void PrintDistances( std::string title = std::string() );
  
    // Add a new variable, attaching also its weight
    inline void AddVariable( const std::string &name, const double &wgt = 1 );

    // Gets the cluster at position < icl >
    inline Cluster* GetCluster( const size_t &icl );

    // Returns the vector of clusters
    inline std::vector<Cluster>& GetClusters();

    // Returns the current number of clusters
    inline size_t GetNClusters() const;

    // Sets the weights for a given cluster. This weights are only taken into account
    // if the class does not manage the number of clusters. If < index > is negative,
    // the weights will be applied to all clusters (included the factory).
    inline void SetClusterWeights( const int &index,
				   const std::vector<double> &wgts );

  protected:

    // ----------
    // Attributes

    // Vector of clusters
    std::vector<Cluster> fClusters;

    // Weights associated with each cluster
    std::map< size_t, std::vector<double> > fClusterWeights;

    // Internal variable to store the points which have already been processed
    std::vector<PointArray::iterator> fPointsToAvoid;

    // Vector with the normalization for each of the variables
    std::vector<double> fVarNorm;

    // Vector storing the order of the variables
    std::vector<std::string> fVarOrder;

    // -------------
    // Configuration
  
    // Pointer to the clustering method
    bool (ClusterFactory::*fClusteringMethod) (void);

    // Pointer to the method to define the center of mass
    void (ClusterFactory::*fComDefMethod) (void);

    // Whether this class is allowed to manage the clusters
    bool fManageClusters;

    // Maximum variation of the center of masses allowed
    double fMaxComVar;

    // Minimum number of points needed to make a cluster
    size_t fMinNpoints;

    // Number of standard deviations between centers of mass to perform the merging
    double fNcomStdDev;

    // Initial number of clusters to be considered
    size_t fNclusters;

    // Maximum number of iterations allowed
    size_t fNiter;

    // Verbose level
    bool fVerbose;

  private:
    
    // This method resets the all the clusters stored in the factory. The infomation
    // about the center of mass and the points to avoid is also lost.
    void Reset();

    // Funcion used to display the information inside the different methods. Here the
    // clusters are normalized if required.
    void Display( void (ClusterFactory::*funcptr)( std::string title ),
		  const std::string &title = std::string() );
  
    // Method to calculate the initial centers of mass of the clusters taking into
    // account the minimum position allowed between clusters.
    void DistanceCentersOfMass();

    // Method to calculate the initial centers of mass in a random way
    void RandomCentersOfMass();

    // Clustering method that constantly reconstructs the clusters till the variation
    // of the centers of mass is negligible, or the maximum number of iterations is
    // reached
    bool ConvergenceMethod();

    // Clustering method that reconstructs the clusters till the maximum number of
    // iterations is reached
    bool IterativeMethod();

    // Main clustering method that calculates the distances between the different
    // points and the clusters and attaches them using this quantity
    void DistanceMerging();

    // Function used when the class is able to add or substract clusters. It returns
    // the decision of whether a cluster must be removed or not.
    bool ManageClusters();
  };
  
  //_______________________________________________________________________________
  //
  void ClusterFactory::AddVariable( const std::string &name, const double &wgt ) {
    fVarNorm.push_back( 0 );
    fVarOrder.push_back( name );
    fWeights.push_back( wgt );
    fCenterOfMass.AddDimension();
  }

  //_______________________________________________________________________________
  //
  Cluster* ClusterFactory::GetCluster( const size_t &icl ) { return &fClusters[ icl ]; }

  //_______________________________________________________________________________
  //
  std::vector<Cluster>& ClusterFactory::GetClusters() { return fClusters; }

  //_______________________________________________________________________________
  //
  size_t ClusterFactory::GetNClusters() const { return fClusters.size(); }

}

#endif
