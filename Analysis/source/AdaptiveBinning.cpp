/////////////////////////////////////////////////////////
// --------------------------------------------------- //
//						       //
//  Analysis package	  			       //
//        					       //
// --------------------------------------------------- //
//						       //
//  AUTHOR: Miguel Ramos Pernas		               //
//  e-mail: miguel.ramos.pernas@cern.ch		       //
//						       //
//  Last update: 11/01/2016			       //
//   						       //
// --------------------------------------------------- //
//						       //
//  Description:				       //
//						       //
//  Implements the class to make adaptive binned       //
//  histograms given a TTree and the name of the two   //
//  leaves to work with. The relation between the      //
//  number of entries and the occupancy of each bin    //
//  is a power of two.				       //
//						       //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#include <cmath>
#include <algorithm>
#include <utility>
#include <numeric>

#include "TLeaf.h"

#include "AdaptiveBinning.h"


//______________________________________________________________________________
////////////////////////////////////
//     ADAPTIVE BINNING CLASS     //
////////////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning::AdaptiveBinning() { }

//______________________________________________________________________________
// Constructor given all the parameters
Analysis::AdaptiveBinning::AdaptiveBinning( size_t      occ_min,
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
  fXmax( xmax / xnorm ),
  fXmin( xmin / xnorm ),
  fYmax( ymax / ynorm ),
  fYmin( ymin / ynorm ) {

  std::cout << "--- Making an adaptive binning division ---" << std::endl;

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
  std::cout << "- Entries used:      " << fWdata.size() <<
    " ( out ot " << nentries << " )" << std::endl;

  // Gets the minimum distance between points
  double
    delta_x = std::abs( fXdata[ 0 ] - fXdata[ 1 ] ),
    delta_y = std::abs( fYdata[ 0 ] - fYdata[ 1 ] ),
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
  fDelta = std::min( delta_x, delta_y )/2;

  // Modifies the minimum and maximum values of the axis to properly construct the histograms
  fXmin -= fDelta;
  fXmax += fDelta;
  fYmin -= fDelta;
  fYmax += fDelta;

  // Sets the length and the bins' list
  fLength  = fWdata.size();
  fBinList = std::vector<Analysis::AdaptiveBinning::Bin>( 1, Bin( fXmin, fXmax, fYmin, fYmax ) );

  // Gets the sum of the weights (to get the number of true entries)
  double sum_of_evts = 0;
  for ( auto it = fWdata.begin(); it != fWdata.end(); it++ )
    sum_of_evts += *it;
  std::cout << "- Sum of weights:    " << sum_of_evts << std::endl;

  // Makes the adaptive bins
  size_t
    max_iter = std::floor( std::log( sum_of_evts/occ_min )/std::log( 2 ) ),
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
      for ( size_t ievt = 0; ievt < fLength; ievt++ )
	fBinList[ ibin ].Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );
      fBinList.push_back( fBinList[ ibin ].Divide( xrange, yrange ) );
    }
    // Sets the new number of bins
    nbins *= 2;
  }
  std::cout << "- Number of bins:    " << nbins << std::endl;
  std::cout << "- Occupancy per bin: " << sum_of_evts/nbins << std::endl;

  // Fills the data for the last time to get the limits of the bins
  for ( auto itbin = fBinList.begin(); itbin != fBinList.end(); itbin++ )
    for ( size_t ievt = 0; ievt < fLength; ievt++ )
      itbin -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );

  // Makes the list of adjusted bins
  fAdjBinList = std::vector<Analysis::AdaptiveBinning::Bin>( fBinList );
  for ( auto itbin = fAdjBinList.begin(); itbin != fAdjBinList.end();	itbin++ )
    itbin -> AdjustBin( fXmin, fXmax, fYmin, fYmax, fDelta );
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning::~AdaptiveBinning() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Makes a filled adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning::GetAdjHist( const char *name, const char *title ) {

  TH2Poly *hist = this -> GetAdjStruct( name, title );

  for ( size_t ievt = 0; ievt < fLength; ievt++ )
    hist -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );

  return hist;
}

//______________________________________________________________________________
// Makes an adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning::GetAdjStruct( const char *name, const char *title ) {

  TH2Poly *hist = new TH2Poly( name, title,
			       fXmin, fXmax,
			       fYmin, fYmax );

  for ( std::vector<Analysis::AdaptiveBinning::Bin>::iterator it = fAdjBinList.begin();
	it != fAdjBinList.end();
	it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );

  return hist;
}

//______________________________________________________________________________
// Makes a filled adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning::GetHist( const char *name, const char *title ) {

  TH2Poly *hist = this -> GetStruct( name, title );

  for ( size_t ievt = 0; ievt < fLength; ievt++ )
    hist -> Fill( fXdata[ ievt ], fYdata[ ievt ], fWdata[ ievt ] );

  return hist;
}

//______________________________________________________________________________
// Makes an adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning::GetStruct( const char *name, const char *title ) {

  TH2Poly *hist = new TH2Poly( name, title,
			       fXmin, fXmax,
			       fYmin, fYmax );

  for ( auto it = fBinList.begin(); it != fBinList.end(); it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );

  return hist;
}

//_______________________________________________________________________________



//_______________________________________________________________________________
///////////////////////
//     BIN CLASS     //
///////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning::Bin::Bin( const double &xmin,
				     const double &xmax,
				     const double &ymin,
				     const double &ymax ) :
  fNpoints( 0 ),
  fXmax( xmax ),
  fXmin( xmin ),
  fYmax( ymax ),
  fYmin( ymin ) { }

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning::Bin::~Bin() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// If the bin is in a border of the histogram, adjusts it to the data
void Analysis::AdaptiveBinning::Bin::AdjustBin( const double &xmin,
						const double &xmax,
						const double &ymin,
						const double &ymax,
						const double &delta ) {

  if ( fXmin == xmin )
    fXmin = fXminPoint - delta;
  if ( fXmax == xmax )
    fXmax = fXmaxPoint + delta;
  if ( fYmin == ymin )
    fYmin = fYminPoint - delta;
  if ( fYmax == ymax )
    fYmax = fYmaxPoint + delta;
}

//______________________________________________________________________________
// If the bin is in a border of the histogram, adjusts it to the data
void Analysis::AdaptiveBinning::Bin::CalcMedians() {
  
  // Sorts the data ( with the weights )
  std::vector<double>
    xw_sorted( this -> Sort( fXpoints, fWpoints ) ),
    yw_sorted( this -> Sort( fYpoints, fWpoints ) );

  // Calculates the median
  double sw_max, sw;
  int    it;

  if ( ! ( int( std::round( fNpoints ) ) % 2 ) )
    sw_max = fNpoints/2;
  else
    sw_max = ( fNpoints - 1 )/2;

  sw = 0;
  it = -1;
  while ( sw < sw_max )
    sw += xw_sorted[ ++it ];
  fXmedian = ( fXpoints[ it ] + fXpoints[ it + 1 ] )/2;

  sw = 0;
  it = -1;
  while ( sw < sw_max )
    sw += yw_sorted[ ++it ];
  fYmedian = ( fYpoints[ it ] + fYpoints[ it + 1 ] )/2;
}

//______________________________________________________________________________
// Clears the content of the bin but the limits
void Analysis::AdaptiveBinning::Bin::Clear() {
  fXpoints.clear();
  fYpoints.clear();
  fWpoints.clear();
  fNpoints = 0;
}

//______________________________________________________________________________
// Fills the bin if the point is inside it
void Analysis::AdaptiveBinning::Bin::Fill( const double &x, const double &y, const double &w ) {

  if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {

    fXpoints.push_back( x );
    fYpoints.push_back( y );
    fWpoints.push_back( w );

    if ( !fNpoints ) {
	
      fXminPoint = x;
      fXmaxPoint = x;
      fYminPoint = y;
      fYmaxPoint = y;
    }
    else {

      if ( x < fXminPoint )
	fXminPoint = x;
      else if ( x > fXmaxPoint )
	fXmaxPoint = x;
    
      if ( y < fYminPoint )
	fYminPoint = y;
      else if ( y > fYmaxPoint )
	fYmaxPoint = y;
    }
    fNpoints += w;
  }
}

//______________________________________________________________________________


// -- PROTECTED METHODS

//______________________________________________________________________________
//
Analysis::AdaptiveBinning::Bin
Analysis::AdaptiveBinning::Bin::Divide( double &xrange,	double &yrange ) {
  double tmp;
  this -> CalcMedians();
  if ( std::min( fXmedian - fXmin, fXmax - fXmedian )/xrange > 
       std::min( fYmedian - fYmin, fYmax - fYmedian )/yrange ) {
    tmp   = fXmax;
    fXmax = fXmedian;
    this -> Clear();
    return Bin( fXmedian, tmp, fYmin, fYmax );
  }
  else {
    tmp   = fYmax;
    fYmax = fYmedian;
    this -> Clear();
    return Bin( fXmin, fXmax, fYmedian, tmp );
  }
}

//______________________________________________________________________________


// -- PRIVATE METHODS

//______________________________________________________________________________
// Sorts the data and the weights and returns the sorted vector of weights for
// that data sample
std::vector<double>
Analysis::AdaptiveBinning::Bin::Sort( std::vector<double> &d_vector,
				      std::vector<double> &w_vector ) {

  std::vector< std::pair<double, double> > order( d_vector.size() );

  for ( size_t i = 0; i < order.size(); i++ )
    order[ i ] = std::make_pair( d_vector[ i ], w_vector[ i ] );

  std::sort( order.begin(), order.end(),
	     [] ( const std::pair<double, double> &a,
		  const std::pair<double, double> &b ) { return a.first < b.first; } );

  std::vector<double> sw_vector( order.size() );

  for ( size_t i = 0; i < order.size(); i++ ) {
    d_vector [ i ] = order[ i ].first;
    sw_vector[ i ] = order[ i ].second;
  }

  return sw_vector;
}

//______________________________________________________________________________
