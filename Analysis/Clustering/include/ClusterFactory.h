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
//  Main class to perform cluster analysis. It can work by two different ways:   //
//  the first of them implies that the number of clusters has to be defined      //
//  previously, while in the second is the proper class which defines the        //
//  number of clusters that fit the given data. The different options are set    //
//  using the constructor or the method < Configure >. All methods work better   //
//  with gaussian distributions. One must specify after the construction, the    //
//  variables to work with, in order to avoid dimensional errors.                //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLUSTER_FACTORY
#define CLUSTER_FACTORY

#include "Cluster.h"

#include <map>
#include <vector>
#include <string>


//_______________________________________________________________________________

namespace General {

  class ClusterFactory : public Cluster {

  public:

    // Constructor and destructor
    ClusterFactory( const std::string &opts = "" );
    ~ClusterFactory();

    // Methods
    void   CalculateClusters();
    void   Configure( const std::string &opts );
    void   PrintCentersOfMass( std::string title = std::string() );
    void   PrintDistances( std::string title = std::string() );
  
    // Inline methods
    inline void                  AddVariable( const std::string &name, const double &wgt = 1 );
    inline Cluster*              GetCluster( const size_t &icl );
    inline std::vector<Cluster>& GetClusters();
    inline void                  SetClusterWeights( const int &index,
						    const std::vector<double> &wgts );

  protected:
  
    // Attributes
    std::vector<Cluster>                    fClusters;
    std::map< size_t, std::vector<double> > fClusterWeights;
    std::vector<double>                     fVarNorm;
    std::vector<std::string>                fVarOrder;
  
    // Configurable attributes
    bool   (ClusterFactory::*fClusteringMethod) (void);
    void   (ClusterFactory::*fComDefMethod) (void);
    bool   fManageClusters;
    double fMaxComVar;
    size_t fMinNpoints;
    double fNcomStdDev;
    size_t fNclusters;
    size_t fNiter;
    bool   fVerbose;

  private:

    // Method to print information inside the processes
    void Display( void (ClusterFactory::*funcptr)( std::string title ),
		  const std::string &title = std::string() );
  
    // Center of mass definition methods
    void DistanceCentersOfMass();
    void RandomCentersOfMass();

    // Clustering methods
    bool ConvergenceMethod();
    void DistanceMerging();
    bool IterativeMethod();
    bool ManageClusters();
  };

  // Adds a new variable, attaching also its weight
  void ClusterFactory::AddVariable( const std::string &name, const double &wgt ) {
    fVarNorm.push_back( 0 );
    fVarOrder.push_back( name );
    fWeights.push_back( wgt );
    fCenterOfMass = ClusterPoint( fVarNorm.size(), 0 );
  }
  // Gets the cluster at position < icl >
  Cluster* ClusterFactory::GetCluster( const size_t &icl ) { return &fClusters[ icl ]; }
  // Returns the vector of clusters
  std::vector<Cluster>& ClusterFactory::GetClusters() { return fClusters; }

}

#endif
