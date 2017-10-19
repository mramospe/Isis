///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning2D.hpp"
#include "Definitions.hpp"
#include "Exceptions.hpp"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>


//______________________________________________________________________________

namespace isis {

  //______________________________________________________________________________
  //
  AdaptiveBinning2D::AdaptiveBinning2D( size_t min_occ,
					double xmin,
					double xmax,
					double ymin,
					double ymax,
					const Doubles &xvalues,
					const Doubles &yvalues,
					const Doubles &weights ) :
    AdaptiveBinning(),
    fXmax(xmax),
    fXmin(xmin),
    fYmax(ymax),
    fYmin(ymin) {
  
    const Doubles *wData( &weights );
    if ( !wData->size() )
      wData = new Doubles( xvalues.size(), 1 );

    // Gets the minimum distance between points
    double
      delta_x = std::abs( xvalues.front() - xvalues.back() ),
      delta_y = std::abs( yvalues.front() - yvalues.back() );

    Doubles xvals( xvalues.begin(), xvalues.end() );
    std::sort( xvals.begin(), xvals.end() );
    for ( auto it = xvals.begin() + 1; it != xvals.end(); ++it ) {
      double new_delta = std::abs( *it - *(it - 1) );
      if ( new_delta != 0 && new_delta < delta_x )
	delta_x = new_delta;
    }
  
    Doubles yvals( yvalues.begin(), yvalues.end() );
    std::sort( yvals.begin(), yvals.end() );
    for ( auto it = yvals.begin() + 1; it != yvals.end(); ++it ) {
      double new_delta = std::abs( *it - *(it - 1) );
      if ( new_delta != 0 && new_delta < delta_y )
	delta_y = new_delta;
    }

    double delta = std::min( delta_x, delta_y )/2;

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
    for ( auto it = wData->begin(); it != wData->end(); ++it )
      sum_of_evts += *it;

    // Makes the adaptive bins
    size_t
      max_iter = std::floor( std::log( sum_of_evts/min_occ )/std::log( 2 ) ),
      nbins    = 1;
  
    if ( max_iter == 0 )
      throw BaseException("Minimum occupancy is so big, decrease it.");

    double
      xrange( *std::max_element( xvalues.begin(), xvalues.end() ) -
	      *std::min_element( xvalues.begin(), xvalues.end() ) ),
      yrange( *std::max_element( yvalues.begin(), yvalues.end() ) -
	      *std::min_element( yvalues.begin(), yvalues.end() ) );
  
    for ( size_t i = 0; i < max_iter; ++i ) {
      for ( size_t ibin = 0; ibin < nbins; ++ibin ) {
      
	Bin2D *bin = static_cast<Bin2D*>(fBinList[0]);
      
	for ( size_t ievt = 1; ievt < wData->size(); ++ievt ) {
	
	  bin = static_cast<Bin2D*>( fBinList[ ibin ] );
	  bin->fill( xvalues.at( ievt ), yvalues.at( ievt ), wData->at( ievt ) );
	}
      
	fBinList.emplace_back( bin->divide( xrange, yrange ) );
      }
      // Sets the new number of bins
      nbins *= 2;
    }

    // Fills the data for the last time to get the limits of the bins
    for ( auto itbin = fBinList.begin(); itbin != fBinList.end(); ++itbin )
      for ( size_t ievt = 0; ievt < wData->size(); ++ievt )
	static_cast<Bin2D*>( *itbin )->fill( xvalues.at( ievt ),
					     yvalues.at( ievt ),
					     wData->at( ievt ) );

    // Makes the list of adjusted bins
    fAdjBinList = std::vector<Bin2D*>( fBinList.size() );
    {
      auto itadj = fAdjBinList.begin();
      auto itbin = fBinList.cbegin();
      while ( itadj != fAdjBinList.end() )
	( *itadj++ ) =
	  new Bin2D( *static_cast<Bin2D*>( *itbin++ ) );
    }

    for ( auto itbin = fAdjBinList.begin(); itbin != fAdjBinList.end(); ++itbin )
      (*itbin)->adjustBin( fXmin, fXmax, fYmin, fYmax, delta );
  
    // If the vector of weights has been allocated, it is destroyed
    if ( !weights.size() )
      delete wData;
  }

  //______________________________________________________________________________
  //
  TH2Poly* AdaptiveBinning2D::getAdjStruct( const char *name,
					    const char *title ) const {
  
    TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
    for ( auto it = fAdjBinList.begin(); it != fAdjBinList.end(); ++it ) {
    
      Bin2D *bin = static_cast<Bin2D*>( *it );
    
      hist->AddBin( bin->fXmin, bin->fYmin, bin->fXmax, bin->fYmax );
    }
    return hist;
  }

  //______________________________________________________________________________
  //
  TH2Poly* AdaptiveBinning2D::getStruct( const char *name,
					 const char *title ) const {
  
    TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
    for ( auto it = fBinList.begin(); it != fBinList.end(); ++it ) {
    
      Bin2D *bin = static_cast<Bin2D*>( *it );
    
      hist->AddBin( bin->fXmin, bin->fYmin, bin->fXmax, bin->fYmax );
    }
    return hist;
  }

}
