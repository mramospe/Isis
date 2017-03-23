///////////////////////////////////////////////////////////////////
// Example of cluster generation (related script ClusterPlot.py) //
///////////////////////////////////////////////////////////////////

#include "ClusterFactory.h"
#include "Definitions.h"

#include "TDirectory.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TRandom3.h"

#include <algorithm>
#include <iostream>
#include <vector>

// Function to write the objects for a given cluster (histograms and scatter plot) in the output folder
void WriteForCluster( Isis::ClusterFactory &factory, const size_t &icl ) {
  
  std::cout << "Accesing cluster " << icl << std::endl;
  Isis::Cluster *cl = factory.getCluster( icl );

  // Fills the vectors with the information of the cluster points
  Isis::Doubles vec, vx, vy, vg, vh;
  for ( auto it = cl -> getPoints().begin(); it != cl -> getPoints().end(); ++it ) {
    vec = it -> getValues();
    vx.push_back( vec[ 0 ] );
    vy.push_back( vec[ 1 ] );
    vg.push_back( vec[ 3 ] );
    vh.push_back( vec[ 2 ] );
  }

  // Creates the folder to contain the objects
  TDirectory *folder = gDirectory -> mkdir( ( "Cluster_" + std::to_string( icl ) ).c_str() );
  folder -> cd();

  // Defines the histograms and fills them
  std::cout << "Creating histograms" << std::endl;
  TH1D *hx = new TH1D( "hx", "hx", 100, *std::min_element( vx.begin(), vx.end() ), *std::max_element( vx.begin(), vx.end() ) );
  TH1D *hy = new TH1D( "hy", "hy", 100, *std::min_element( vy.begin(), vy.end() ), *std::max_element( vy.begin(), vy.end() ) );
  TH1D *hg = new TH1D( "hg", "hg", 100, *std::min_element( vg.begin(), vg.end() ), *std::max_element( vg.begin(), vg.end() ) );
  TH1D *hh = new TH1D( "hh", "hh", 100, *std::min_element( vh.begin(), vh.end() ), *std::max_element( vh.begin(), vh.end() ) );
  TGraph *gr = new TGraph();

  std::cout << "Filling histograms" << std::endl;
  auto itx = vx.begin(), ity = vy.begin(), itg = vg.begin(), ith = vh.begin();
  size_t i = 0;
  while ( itx != vx.end() ) {
    hx -> Fill( *itx );
    hy -> Fill( *ity );
    hg -> Fill( *itg++ );
    hh -> Fill( *ith++ );
    gr -> SetPoint( i++, *itx++, *ity++ );
  }

  // Writes the object in the output folder
  std::cout << "Writing histograms" << std::endl;
  hx -> Write();
  hy -> Write();
  hg -> Write();
  hh -> Write();
  gr -> SetNameTitle( "gr", "gr" );
  gr -> Write();
}


int main() {
  
  // Creates the output file
  TFile *of = TFile::Open( "Clustering.root", "RECREATE" );
  of -> cd();

  // Creates the cluster factory parsing the corresponding options
  Isis::ClusterFactory factory( "nClusters=7:nIter=10:ClusteringMethod=Convergence:ComDefMethod=Distance:Verbose:ManageClusters:MaxComVar=0.1:nComStdDev=1" );
  factory.addVariable( "x" );
  factory.addVariable( "y" );
  factory.addVariable( "g" );
  factory.addVariable( "h" );

  // Generates the data. Four different variables are defined
  TRandom3 rndm;
  Isis::Doubles VX, VY, VG, VH, value( 4 );
  bool xdec, ydec;
  double
    sigmaXY = 1.5,
    sigmaGH = 0.1;
  for ( size_t i = 0; i < 100000; ++i ) {
    
    xdec = rndm.Rndm() > 0.5;
    if ( xdec )
      VX.push_back( rndm.Gaus( +4, sigmaXY ) );
    else
      VX.push_back( rndm.Gaus( -4, sigmaXY ) );
    
    ydec = rndm.Rndm() > 0.5;
    if ( ydec )
      VY.push_back( rndm.Gaus( +4, sigmaXY ) );
    else
      VY.push_back( rndm.Gaus( -4, sigmaXY ) );
      
    if ( xdec && ydec ) {
      VG.push_back( rndm.Gaus( 0.5, sigmaGH ) );
      VH.push_back( rndm.Gaus(  -1, sigmaGH ) );
    }
    else if ( xdec && !ydec ) {
      VG.push_back( rndm.Gaus(    1, sigmaGH ) );
      VH.push_back( rndm.Gaus( -0.5, sigmaGH ) );
    }
    else if ( !xdec && ydec ) {
      VG.push_back( rndm.Gaus(   -1, sigmaGH ) );
      VH.push_back( rndm.Gaus( +0.5, sigmaGH ) );
    }
    else {
      VG.push_back( rndm.Gaus( -0.5, sigmaGH ) );
      VH.push_back( rndm.Gaus(   +1, sigmaGH ) );
    }
    
    // Sets the values to construct the point
    value[ 0 ] = VX.back();
    value[ 1 ] = VY.back();
    value[ 3 ] = VG.back();
    value[ 2 ] = VH.back();
    factory.addPoint( value );
  }

  // Calls the main method
  factory.calculateClusters();

  // Generates the scatter plots and histograms for each cluster and writes them
  for ( size_t i = 0; i < factory.getNClusters(); ++i ) {
    of -> cd();
    WriteForCluster( factory, i );
  }

  std::cout << "Closing output file" << std::endl;
  of -> Close();
  
  std::cout << "Finished!" << std::endl;
  return 0;
}
