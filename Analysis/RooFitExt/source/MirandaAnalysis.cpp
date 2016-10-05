/////////////////////////////////////////////////////////
//
//  Analysis package
//
// ------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 13/06/2016
//
// ------------------------------------------------------
//
//  Description:
//
//  Implements the class to calculate the asymmetries
//  bin to bin using two-dimensional adaptive binning
//  histograms. Be careful using sWeighted data
//  because for large amount of bins they could
//  appear variation in the number of entries in each
//  bin.
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#include "MirandaAnalysis.h"

#include <cmath>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::MirandaAnalysis::MirandaAnalysis( unsigned int occ_min,
					    double       xmin,
					    double       xmax,
					    double       ymin,
					    double       ymax ) :
  fMinOcc( occ_min ),
  fTopTree( 0 ),
  fTopLeaves( 0 ),
  fBotTree( 0 ),
  fBotLeaves( 0 ),
  fXmax( xmax ),
  fXmin( xmin ),
  fYmax( ymax ),
  fYmin( ymin ) { }

//______________________________________________________________________________
// Destructor
Analysis::MirandaAnalysis::~MirandaAnalysis() {

  delete[] fTopLeaves;
  delete[] fBotLeaves;
}

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Gets the adjusted asymmetries histogram
TH2Poly* Analysis::MirandaAnalysis::GetAdjAsymHist( const char *name,
						    const char *title ) {

  TH2Poly
    *hist_1( this -> GetTopHist( "h1", "h1" ) ),
    *hist_2( this -> GetBotHist( "h2", "h2" ) ),
    *result( this -> GetAdjStruct( name, title ) );

  double h1, h2, s2h1, s2h2, sqrtSw2;

  for ( unsigned int ibin = 1; ibin <= fStructure.GetNbins(); ibin++ ) {

    h1      = hist_1 -> GetBinContent( ibin );
    h2      = hist_2 -> GetBinContent( ibin );
    s2h1    = hist_1 -> GetBinError( ibin )*hist_1 -> GetBinError( ibin );
    s2h2    = hist_2 -> GetBinError( ibin )*hist_2 -> GetBinError( ibin );
    sqrtSw2 = std::sqrt( s2h1 + s2h2 );

    result -> SetBinContent( ibin, ( h1 - h2 )/sqrtSw2 );
    result -> SetBinError  ( ibin, 1. );
  }

  delete hist_1;
  delete hist_2;

  return result;
}

//______________________________________________________________________________
// Gets the adjusted bottom histogram
TH2Poly* Analysis::MirandaAnalysis::GetAdjBotHist( const char *name,
						   const char *title ) {

  TH2Poly *hist = this -> GetAdjStruct( name, title );

  if ( fBotLeaves[ 2 ] )
    for ( unsigned int ievt = 0; ievt < fBotTree -> GetEntries(); ievt++ ) {

      fBotTree -> GetEntry( ievt );

      hist -> Fill( fBotLeaves[ 0 ] -> GetValue(),
		    fBotLeaves[ 1 ] -> GetValue(),
		    fBotLeaves[ 2 ] -> GetValue() );
    }
  else
    for ( unsigned int ievt = 0; ievt < fBotTree -> GetEntries(); ievt++ ) {

      fBotTree -> GetEntry( ievt );

      hist -> Fill( fBotLeaves[ 0 ] -> GetValue(),
		    fBotLeaves[ 1 ] -> GetValue() );
    }

  return hist;
}

//______________________________________________________________________________
// Gets an adjusted structured histogram
TH2Poly* Analysis::MirandaAnalysis::GetAdjStruct( const char *name,
						  const char *title ) {

  return fStructure.GetAdjStruct( name, title );
}

//______________________________________________________________________________
// Gets the adjusted bottom histogram
TH2Poly* Analysis::MirandaAnalysis::GetAdjTopHist( const char *name,
						   const char *title ) {

  TH2Poly *hist = this -> GetAdjStruct( name, title );

  if ( fTopLeaves[ 2 ] )
    for ( unsigned int ievt = 0; ievt < fTopTree -> GetEntries(); ievt++ ) {

      fTopTree -> GetEntry( ievt );

      hist -> Fill( fTopLeaves[ 0 ] -> GetValue(),
		    fTopLeaves[ 1 ] -> GetValue(),
		    fTopLeaves[ 2 ] -> GetValue() );
    }
  else
    for ( unsigned int ievt = 0; ievt < fTopTree -> GetEntries(); ievt++ ) {

      fTopTree -> GetEntry( ievt );

      hist -> Fill( fTopLeaves[ 0 ] -> GetValue(),
		    fTopLeaves[ 1 ] -> GetValue() );
    }

  return hist;
}

//______________________________________________________________________________
// Gets the asymmetries histogram
TH2Poly* Analysis::MirandaAnalysis::GetAsymHist( const char *name,
						 const char *title ) {

  TH2Poly
    *hist_1( this -> GetTopHist( "h1", "h1" ) ),
    *hist_2( this -> GetBotHist( "h2", "h2" ) ),
    *result( this -> GetStruct( name, title ) );

  double h1, h2, s2h1, s2h2, sqrtSw2;

  for ( unsigned int ibin = 1; ibin <= fStructure.GetNbins(); ibin++ ) {

    h1      = hist_1 -> GetBinContent( ibin );
    h2      = hist_2 -> GetBinContent( ibin );
    s2h1    = hist_1 -> GetBinError( ibin )*hist_1 -> GetBinError( ibin );
    s2h2    = hist_2 -> GetBinError( ibin )*hist_2 -> GetBinError( ibin );
    sqrtSw2 = std::sqrt( s2h1 + s2h2 );

    result -> SetBinContent( ibin, ( h1 - h2 )/sqrtSw2 );
    result -> SetBinError  ( ibin, 1. );
  }

  delete hist_1;
  delete hist_2;

  return result;
}

//______________________________________________________________________________
// Creates a RooDataSet with the asymmetries data
RooDataSet* Analysis::MirandaAnalysis::GetAsymData( const char *name,
						    const char *title,
						    RooRealVar &var ) {

  RooDataSet *data = new RooDataSet( name, title, RooArgSet( var ) );

  TH2Poly *as_hist( this -> GetAsymHist( "hist", "hist" ) );

  double val;

  for ( unsigned int ibin = 1; ibin <= fStructure.GetNbins(); ibin++ ) {

    val = as_hist -> GetBinContent( ibin );

    if ( val > var.getMin() && val < var.getMax() ) {

      var.setVal( val );

      data -> add( RooArgSet( var ) );
    }
  }

  return data;
}

//______________________________________________________________________________
// Gets the top histogram
TH2Poly* Analysis::MirandaAnalysis::GetTopHist( const char *name,
						const char *title ) {

  TH2Poly *hist = fStructure.GetStruct( name, title );

  if ( fTopLeaves[ 2 ] )
    for ( unsigned int ievt = 0; ievt < fTopTree -> GetEntries(); ievt++ ) {

      fTopTree -> GetEntry( ievt );

      hist -> Fill( fTopLeaves[ 0 ] -> GetValue(),
		    fTopLeaves[ 1 ] -> GetValue(),
		    fTopLeaves[ 2 ] -> GetValue() );
    }
  else
    for ( unsigned int ievt = 0; ievt < fTopTree -> GetEntries(); ievt++ ) {

      fTopTree -> GetEntry( ievt );

      hist -> Fill( fTopLeaves[ 0 ] -> GetValue(),
		    fTopLeaves[ 1 ] -> GetValue() );
    }

  return hist;
}

//______________________________________________________________________________
// Gets the bottom histogram
TH2Poly* Analysis::MirandaAnalysis::GetBotHist( const char *name,
						const char *title ) {

  TH2Poly *hist = fStructure.GetStruct( name, title );

  if ( fBotLeaves[ 2 ] )
    for ( unsigned int ievt = 0; ievt < fBotTree -> GetEntries(); ievt++ ) {

      fBotTree -> GetEntry( ievt );

      hist -> Fill( fBotLeaves[ 0 ] -> GetValue(),
		    fBotLeaves[ 1 ] -> GetValue(),
		    fBotLeaves[ 2 ] -> GetValue() );
    }
  else
    for ( unsigned int ievt = 0; ievt < fBotTree -> GetEntries(); ievt++ ) {

      fBotTree -> GetEntry( ievt );

      hist -> Fill( fBotLeaves[ 0 ] -> GetValue(),
		    fBotLeaves[ 1 ] -> GetValue() );
    }

  return hist;
}

//______________________________________________________________________________
// Gets a structured histrogram
TH2Poly* Analysis::MirandaAnalysis::GetStruct( const char *name,
					       const char *title ) {

  return fStructure.GetStruct( name, title );
}

//______________________________________________________________________________
// Sets the bottom tree
void Analysis::MirandaAnalysis::SetBotTree( TTree      *tree,
					    const char *xleaf_name,
					    const char *yleaf_name,
					    const char *wleaf_name ) {

  fBotTree   = tree;
  fBotLeaves = new TLeaf*[ 3 ];

  fBotLeaves[ 0 ] = fBotTree -> GetLeaf( xleaf_name );
  fBotLeaves[ 1 ] = fBotTree -> GetLeaf( yleaf_name );

  if ( wleaf_name )
    fBotLeaves[ 2 ] = fBotTree -> GetLeaf( wleaf_name );
  else
    fBotLeaves[ 2 ] = 0;
}

//______________________________________________________________________________
// Sets the tree to make the structure of the adaptive binned histrograms
void Analysis::MirandaAnalysis::SetStructTree( TTree     *tree,
					       const char *xleaf_name,
					       const char *yleaf_name,
					       const char *wleaf_name ) {

  fStructure = Analysis::AdaptiveBinning2D( fMinOcc,
					    fXmin, fXmax,
					    fYmin, fYmax,
					    tree,
					    xleaf_name, yleaf_name, wleaf_name );
}

//______________________________________________________________________________
// Sets the top tree
void Analysis::MirandaAnalysis::SetTopTree( TTree      *tree,
					    const char *xleaf_name,
					    const char *yleaf_name,
					    const char *wleaf_name ) {

  fTopTree   = tree;
  fTopLeaves = new TLeaf*[ 3 ];

  fTopLeaves[ 0 ] = fTopTree -> GetLeaf( xleaf_name );
  fTopLeaves[ 1 ] = fTopTree -> GetLeaf( yleaf_name );

  if ( wleaf_name )
    fTopLeaves[ 2 ] = fTopTree -> GetLeaf( wleaf_name );
  else
    fTopLeaves[ 2 ] = 0;
}

//______________________________________________________________________________
