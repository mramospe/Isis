///////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------- //
//						                     //
//  Analysis package	  			                     //
//        					                     //
// ----------------------------------------------------------------- //
//						                     //
//  AUTHOR: Miguel Ramos Pernas		                             //
//  e-mail: miguel.ramos.pernas@cern.ch		                     //
//						                     //
//  Last update: 13/06/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the class to make two-dimensional adaptive binning    //
//  histograms. The construction can be made given a set of vectors  //
//  or a TTree object and the name of the leaves.                    //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning2D.h"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning2D::AdaptiveBinning2D() : AdaptiveBinning() { }

//______________________________________________________________________________
// Constructor given vectors of values
Analysis::AdaptiveBinning2D::AdaptiveBinning2D( size_t      min_occ,
						double      xmin,
						double      xmax,
						double      ymin,
						double      ymax,
						const std::vector<double> &xvalues,
						const std::vector<double> &yvalues,
						const std::vector<double> &weights ) :
  AdaptiveBinning(),
  fXmax( xmax ),
  fXmin( xmin ),
  fYmax( ymax ),
  fYmin( ymin ) {

  fWeighted = weights.size();

  if ( weights.size() )
    fWdata = weights;
  fXdata = xvalues;
  fYdata = yvalues;
  this -> Construct( min_occ );
}

//______________________________________________________________________________
// Constructor using a TTree object
Analysis::AdaptiveBinning2D::AdaptiveBinning2D( size_t      min_occ,
						double      xmin,
						double      xmax,
						double      ymin,
						double      ymax,
						TTree      *tree,
						const char *xleaf_name,
						const char *yleaf_name,
						const char *wleaf_name,
						double      xnorm,
						double      ynorm ) :
  AdaptiveBinning(),
  fXmax( xmax / xnorm ),
  fXmin( xmin / xnorm ),
  fYmax( ymax / ynorm ),
  fYmin( ymin / ynorm ) {

  fWeighted = wleaf_name;

  size_t nentries = tree -> GetEntries();
  TLeaf
    *xleaf = tree -> GetLeaf( xleaf_name ),
    *yleaf = tree -> GetLeaf( yleaf_name );
  double x, y, w;
  // Gets the data and fills the vectors that contain the points and the weights
  if ( wleaf_name ) {
    TLeaf *wleaf( tree -> GetLeaf( wleaf_name ) );
    for ( size_t ievt = 0; ievt < nentries; ievt++ ) {
      tree -> GetEntry( ievt );
      x = xleaf -> GetValue() / xnorm;
      y = yleaf -> GetValue() / ynorm;
      w = wleaf -> GetValue();
      if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {
	fXdata.push_back( x );
	fYdata.push_back( y );
	fWdata.push_back( w );
      }
    }
  }
  else {
    for ( size_t ievt = 0; ievt < nentries; ievt++ ) {
      tree -> GetEntry( ievt );
      x = xleaf -> GetValue() / xnorm;
      y = yleaf -> GetValue() / ynorm;
      if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {
	fXdata.push_back( x );
	fYdata.push_back( y );
      }
    }
    fWdata = std::vector<double> ( fXdata.size(), 1 );
  }
  this -> Construct( min_occ );
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning2D::~AdaptiveBinning2D() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Makes a filled adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetAdjHist( const char *name, const char *title ) {
  TH2Poly *hist = this -> GetAdjStruct( name, title );
  for ( size_t ievt = 0; ievt < fWdata.size(); ievt++ )
    hist -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );
  return hist;
}

//______________________________________________________________________________
// Makes an adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetAdjStruct( const char *name, const char *title ) {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fAdjBinList.begin(); it != fAdjBinList.end();	it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );
  return hist;
}

//______________________________________________________________________________
// Makes a filled adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetHist( const char *name, const char *title ) {
  TH2Poly *hist = this -> GetStruct( name, title );
  for ( size_t ievt = 0; ievt < fWdata.size(); ievt++ )
    hist -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );
  return hist;
}

//______________________________________________________________________________
// Makes an adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetStruct( const char *name, const char *title ) {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fBinList.begin(); it != fBinList.end(); it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );
  return hist;
}

//______________________________________________________________________________


// -- PROTECTED METHOD

//______________________________________________________________________________
// Constructs the vector of two-dimensional adaptive bins
void Analysis::AdaptiveBinning2D::Construct( const double &min_occ ) {

  std::cout << "--- Making an adaptive binning division ---" << std::endl;

  // Gets the minimum distance between points
  double
    delta_x = std::abs( fXdata[ 0 ] - fXdata[ 1 ] ),
    delta_y = std::abs( fYdata[ 0 ] - fYdata[ 1 ] ),
    delta,
    new_delta;

  for ( auto it1 = fXdata.begin(); it1 != fXdata.end(); it1++ )
    for ( auto it2 = it1 + 1; it2 != fXdata.end(); it2++ ) {	  
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_x )
	delta_x = new_delta;
    }

  for ( auto it1 = fYdata.begin(); it1 != fYdata.end(); it1++ )
    for ( auto it2 = it1 + 1; it2 != fYdata.end(); it2++ ) {
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_y )
	delta_y = new_delta;
    }
  delta = std::min( delta_x, delta_y )/2;

  // Modifies the minimum and maximum values of the axis to properly construct the histograms
  fXmin -= delta;
  fXmax += delta;
  fYmin -= delta;
  fYmax += delta;

  // Initializes the bins list
  fBinList = std::vector<Analysis::Bin2D>( 1, Bin2D( fXmin, fXmax, fYmin, fYmax ) );

  // Gets the sum of the weights (to get the number of true entries)
  double sum_of_evts = 0;
  for ( auto it = fWdata.begin(); it != fWdata.end(); it++ )
    sum_of_evts += *it;

  // Makes the adaptive bins
  size_t
    max_iter = std::floor( std::log( sum_of_evts/min_occ )/std::log( 2 ) ),
    nbins    = 1;

  if ( max_iter == 0 )
    std::cerr << " ERROR: minimum occupancy is so big, decrease it." << std::endl;

  double
    xrange( *std::max_element( fXdata.begin(), fXdata.end() ) -
	     *std::min_element( fXdata.begin(), fXdata.end() ) ),
    yrange( *std::max_element( fYdata.begin(), fYdata.end() ) -
	     *std::min_element( fYdata.begin(), fYdata.end() ) );

  for ( size_t i = 0; i < max_iter; i++ ) {
    for ( size_t ibin = 0; ibin < nbins; ibin++ ) {
      for ( size_t ievt = 0; ievt < fWdata.size(); ievt++ )
	fBinList[ ibin ].Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );
      fBinList.push_back( fBinList[ ibin ].Divide( xrange, yrange ) );
    }
    // Sets the new number of bins
    nbins *= 2;
  }

  // Fills the data for the last time to get the limits of the bins
  for ( auto itbin = fBinList.begin(); itbin != fBinList.end(); itbin++ )
    for ( size_t ievt = 0; ievt < fWdata.size(); ievt++ )
      itbin -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );

  // Makes the list of adjusted bins
  fAdjBinList = std::vector<Analysis::Bin2D>( fBinList );
  for ( auto itbin = fAdjBinList.begin(); itbin != fAdjBinList.end();	itbin++ )
    itbin -> AdjustBin( fXmin, fXmax, fYmin, fYmax, delta );
  
  // Displays the information of the process
  this -> DisplayInfo( fWdata.size(), sum_of_evts, nbins, sum_of_evts );
}
