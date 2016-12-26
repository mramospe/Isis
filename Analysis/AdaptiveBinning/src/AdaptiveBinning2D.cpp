///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/12/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to make two-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
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
  
  const std::vector<double> *wData( &weights );
  if ( !wData -> size() )
    wData = new std::vector<double>( xvalues.size(), 1 );

  // Gets the minimum distance between points
  double
    delta_x = std::abs( xvalues.front() - xvalues.back() ),
    delta_y = std::abs( yvalues.front() - yvalues.back() ),
    delta,
    new_delta;
  
  for ( auto it1 = xvalues.begin(); it1 != xvalues.end(); ++it1 )
    for ( auto it2 = it1 + 1; it2 != xvalues.end(); ++it2 ) {	  
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_x )
	delta_x = new_delta;
    }
  
  for ( auto it1 = yvalues.begin(); it1 != yvalues.end(); ++it1 )
    for ( auto it2 = it1 + 1; it2 != yvalues.end(); ++it2 ) {
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_y )
	delta_y = new_delta;
    }
  delta = std::min( delta_x, delta_y )/2;

  // Modifies the minimum and maximum values of the axis to properly construct
  // the histograms
  fXmin -= delta;
  fXmax += delta;
  fYmin -= delta;
  fYmax += delta;

  // Initializes the bins list
  fBinList = std::vector<Bin*>( 1, new Bin2D( fXmin, fXmax, fYmin, fYmax ) );

  // Gets the sum of the weights (to get the number of true entries)
  double sum_of_evts = 0;
  for ( auto it = wData -> begin(); it != wData -> end(); ++it )
    sum_of_evts += *it;

  // Makes the adaptive bins
  size_t
    max_iter = std::floor( std::log( sum_of_evts/min_occ )/std::log( 2 ) ),
    nbins    = 1;

  if ( max_iter == 0 )
    std::cerr << " ERROR: minimum occupancy is so big, decrease it." << std::endl;

  double
    xrange( *std::max_element( xvalues.begin(), xvalues.end() ) -
	     *std::min_element( xvalues.begin(), xvalues.end() ) ),
    yrange( *std::max_element( yvalues.begin(), yvalues.end() ) -
	     *std::min_element( yvalues.begin(), yvalues.end() ) );
  
  for ( size_t i = 0; i < max_iter; ++i ) {
    for ( size_t ibin = 0; ibin < nbins; ++ibin ) {
      Analysis::Bin2D *bin;
      for ( size_t ievt = 0; ievt < wData -> size(); ++ievt ) {
	bin = static_cast<Analysis::Bin2D*>( fBinList[ ibin ] );
	bin -> Fill( xvalues.at( ievt ), yvalues.at( ievt ), wData -> at( ievt ) );
      }
      fBinList.push_back( bin -> Divide( xrange, yrange ) );
    }
    // Sets the new number of bins
    nbins *= 2;
  }

  // Fills the data for the last time to get the limits of the bins
  for ( auto itbin = fBinList.begin(); itbin != fBinList.end(); ++itbin )
    for ( size_t ievt = 0; ievt < wData -> size(); ++ievt )
      static_cast<Analysis::Bin2D*>( *itbin ) -> Fill( xvalues.at( ievt ),
						       yvalues.at( ievt ),
						       wData -> at( ievt ) );

  // Makes the list of adjusted bins
  fAdjBinList = std::vector<Analysis::Bin2D*>( fBinList.size() );
  {
    auto itadj = fAdjBinList.begin();
    auto itbin = fBinList.cbegin();
    while ( itadj != fAdjBinList.end() )
      ( *itadj++ ) =
	new Analysis::Bin2D( *static_cast<Analysis::Bin2D*>( *itbin++ ) );
  }

  for ( auto itbin = fAdjBinList.begin(); itbin != fAdjBinList.end(); ++itbin )
    (*itbin) -> AdjustBin( fXmin, fXmax, fYmin, fYmax, delta );
  
  // If the vector of weights has been allocated, it is destroyed
  if ( !weights.size() )
    delete wData;
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning2D::~AdaptiveBinning2D() {

  for ( auto it = fAdjBinList.begin(); it != fAdjBinList.end(); ++it )
    delete *it;
}

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Makes an adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetAdjStruct( const char *name,
						    const char *title ) const {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fAdjBinList.begin(); it != fAdjBinList.end();	++it ) {
    Analysis::Bin2D *bin = static_cast<Analysis::Bin2D*>( *it );
    hist -> AddBin( bin -> fXmin, bin -> fYmin, bin -> fXmax, bin -> fYmax );
  }
  return hist;
}

//______________________________________________________________________________
// Makes an adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetStruct( const char *name,
						 const char *title ) const {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fBinList.begin(); it != fBinList.end(); ++it ) {
    Analysis::Bin2D *bin = static_cast<Analysis::Bin2D*>( *it );
    hist -> AddBin( bin -> fXmin, bin -> fYmin, bin -> fXmax, bin -> fYmax );
  }
  return hist;
}
