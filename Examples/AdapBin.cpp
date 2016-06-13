///////////////////////////////////////////////////////
//  Example to make adaptive binning histograms and  //
//  perform analysis of asymmetries bin to bin       //
///////////////////////////////////////////////////////

#include "AdaptiveBinning2D.h"
#include "MirandaAnalysis.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "TRandom.h"

#include <iostream>

int main() {

  // Creates the input sample
  TFile *ifile = TFile::Open( "files/AdapBin.root", "RECREATE" );
  TTree
    *ttree = new TTree( "TopTree", "TopTree", 0 ),
    *btree = new TTree( "BotTree", "BotTree", 0 );

  double xVar, yVar;

  ttree -> Branch( "xVar", &xVar, "xVar/D" );
  ttree -> Branch( "yVar", &yVar, "yVar/D" );
  btree -> Branch( "xVar", &xVar, "xVar/D" );
  btree -> Branch( "yVar", &yVar, "yVar/D" );

  for ( size_t i = 0; i < 10000; i++ ) {

    xVar = gRandom -> Gaus( 1000, 100 );
    yVar = gRandom -> Exp( 100 );

    ttree -> Fill();

    xVar = gRandom -> Gaus( 1000, 100 ) + gRandom -> Exp( 100 );
    yVar = gRandom -> Exp( 100 );

    btree -> Fill();
  }
  ttree -> Write();
  btree -> Write();
  std::cout << "Generated input sample" << std::endl;

  // Creates the main tree ( to make the divisions )
  TList tlist;
  tlist.Add( ttree );
  tlist.Add( btree );
  TTree *mtree = TTree::MergeTrees( &tlist );
  mtree -> SetNameTitle( "DecayTree", "DecayTree" );
  std::cout << "Created main tree" << std::endl;

  // Makes the adaptive binning histograms
  Analysis::AdaptiveBinning2D AB( 40, 300, 1400, 300, 1400, mtree, "xVar", "yVar", 0, 1, 1 );

  TH2Poly *hStr = AB.GetStruct( "hStr", "hStr" );
  TH2Poly *hAdj  = AB.GetAdjStruct( "hAdj", "hAdj" );
  TH2Poly *hAdjStr = AB.GetAdjStruct( "hAdjStr", "hAdjStr" );

  double xv, yv;
  mtree -> SetBranchAddress( "xVar", &xv );
  mtree -> SetBranchAddress( "yVar", &yv );
  for ( Long64_t ievt = 0; ievt < mtree -> GetEntries(); ++ievt ) {
    mtree -> GetEntry( ievt );
    hAdj -> Fill( xv, yv );
  }

  std::cout << "Made adaptive binning histograms" << std::endl;

  // Generates the asymmetry histograms
  Analysis::MirandaAnalysis MA( 40, 300, 1400, 300, 1400 );

  MA.SetStructTree( mtree, "xVar", "yVar" );

  TH2Poly *adjStr = MA.GetAdjStruct( "adjstr", "adjstr" );

  MA.SetTopTree( ttree, "xVar", "yVar" );
  MA.SetBotTree( btree, "xVar", "yVar" );

  TH2Poly *asHist = MA.GetAdjAsymHist( "asy", "asy" );
  
  // Draws the histograms
  std::cout << "Making histograms..." << std::endl;
  TCanvas *canvas = new TCanvas();
  asHist  -> Draw( "ZCOL" );
  canvas  -> Write( "Asymmetry" );
  adjStr  -> Draw( "L" );
  canvas  -> Write( "Structure" );
  canvas  -> Clear();
  canvas  -> Divide( 3, 1 );
  canvas  -> cd( 1 );
  hAdj    -> Draw( "LZCOL" );
  canvas  -> cd( 2 );
  hStr    -> Draw( "L" );
  canvas  -> cd( 3 );
  hAdjStr -> Draw( "L" );
  canvas  -> Write( "Histograms" );

  // Fits the asymmetry distribution to a gaussian function
  std::cout << "Fitting the asymmetry data..." << std::endl;
  RooRealVar
    MirandaVar( "MV"   , "MV"   , -4., 4. ),
    mean      ( "Mean" , "Mean" , 0. , -4 , 4  ),
    sigma     ( "Sigma", "Sigma", 1. , 0.2, 2. );
  RooGaussian gauss( "Gauss", "Gauss", MirandaVar, mean, sigma );
  RooDataSet *data = MA.GetAsymData( "data", "data", MirandaVar );
  gauss.fitTo( *data );

  RooPlot *frame = MirandaVar.frame( RooFit::Title( "" ) );
  data -> plotOn( frame, RooFit::Binning( 10 ) );
  gauss.plotOn( frame, RooFit::LineColor( kBlue ) );

  canvas -> Clear();
  frame  -> Draw();
  canvas -> Write( "Fit" );

  // Deletes the pointers
  delete mtree;
  delete data;
  delete canvas;
  delete hAdj;
  delete hStr;
  delete hAdjStr;
  delete asHist;
  
  ifile -> Close();

  return 0;
}
