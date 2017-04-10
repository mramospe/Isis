///////////////////////////////////////////////////
//  Example to make adaptive binning histograms  //
///////////////////////////////////////////////////

#include "AdaptiveBinning2D.h"
#include "Definitions.h"

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "TRandom.h"

#include <iostream>
#include <vector>

int main() {

  // Creates the input sample
  TFile *ifile = TFile::Open( "files/AdapBin.root", "RECREATE" );

  isis::Doubles xVar, yVar;
  for ( size_t i = 0; i < 10000; i++ ) {

    if ( gRandom -> Uniform() > 0.5 ) {
      xVar.push_back( gRandom -> Gaus( +100, 75 ) );
      yVar.push_back( gRandom -> Exp( 100 ) );
    }
    else {
      xVar.push_back( gRandom -> Gaus( -100, 75 ) + gRandom -> Exp( 100 ) );
      yVar.push_back( gRandom -> Exp( 100 ) );
    }
  }
  std::cout << "Generated input sample" << std::endl;

  // Makes the adaptive binning histograms
  isis::AdaptiveBinning2D AB( 40, -200, 200, 0, 1000, xVar, yVar );

  TH2Poly *hStr    = AB.getStruct( "hStr", "hStr" );
  TH2Poly *hAdj    = AB.getAdjStruct( "hAdj", "hAdj" );
  TH2Poly *hAdjStr = AB.getAdjStruct( "hAdjStr", "hAdjStr" );

  for ( size_t ievt = 0; ievt < xVar.size(); ++ievt )
    hAdj -> Fill( xVar[ ievt ], yVar[ ievt ] );

  std::cout << "Made adaptive binning histograms" << std::endl;
  
  // Draws the histograms
  std::cout << "Making histograms..." << std::endl;
  TCanvas *canvas = new TCanvas();
  canvas  -> Divide( 3, 1 );
  canvas  -> cd( 1 );
  hStr    -> Draw( "L" );
  canvas  -> cd( 2 );
  hAdjStr -> Draw( "L" );
  canvas  -> cd( 3 );
  hAdj    -> Draw( "LZCOL" );
  canvas  -> Write( "Histograms" );

  // Deletes the pointers
  delete canvas;
  delete hAdj;
  delete hStr;
  delete hAdjStr;
  
  ifile -> Close();

  return 0;
}
