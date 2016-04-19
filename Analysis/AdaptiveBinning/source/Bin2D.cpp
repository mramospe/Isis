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
//  Last update: 19/04/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the two-dimensional bin class to work together with   //
//  AdaptiveBinning2D to create adaptive binned histograms.          //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#include "Bin2D.h"

#include <algorithm>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Constructor
Analysis::Bin2D::Bin2D( const double &xmin,
			const double &xmax,
			const double &ymin,
			const double &ymax ) :
  Bin(), fXmax( xmax ), fXmin( xmin ), fYmax( ymax ), fYmin( ymin ) { }

//______________________________________________________________________________
// Destructor
Analysis::Bin2D::~Bin2D() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// If the bin is in a border of the histogram, adjusts it to the data
void Analysis::Bin2D::AdjustBin( const double &xmin,
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
// Fills the bin if the point is inside it
void Analysis::Bin2D::Fill( const double &x, const double &y, const double &w ) {

  if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {
    fXpoints.push_back( x );
    fYpoints.push_back( y );
    fWpoints.push_back( w );
    if ( !fSumOfWeights ) {
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
    fSumOfWeights += w;
  }
  fEntries++;
}

//______________________________________________________________________________


// -- PROTECTED METHODS

//______________________________________________________________________________
// Divides the bin in two, given the range of the < x > and < y > axis
Analysis::Bin2D Analysis::Bin2D::Divide( const double &xrange, const double &yrange ) {
  double tmp;
  this -> CalcMedians();
  if ( std::min( fXmedian - fXmin, fXmax - fXmedian )/xrange > 
       std::min( fYmedian - fYmin, fYmax - fYmedian )/yrange ) {
    tmp   = fXmax;
    fXmax = fXmedian;
    this -> Clear();
    return Bin2D( fXmedian, tmp, fYmin, fYmax );
  }
  else {
    tmp   = fYmax;
    fYmax = fYmedian;
    this -> Clear();
    return Bin2D( fXmin, fXmax, fYmedian, tmp );
  }
}

//______________________________________________________________________________


// -- PRIVATE METHODS

//______________________________________________________________________________
// If the bin is in a border of the histogram, adjusts it to the data
void Analysis::Bin2D::CalcMedians() {
  
  // Sorts the data ( with the weights )
  std::vector<double>
    xw_sorted( this -> Sort( fXpoints, fWpoints ) ),
    yw_sorted( this -> Sort( fYpoints, fWpoints ) );

  // Calculates the median
  double sw_max, sw;
  int    it;

  if ( ! ( int( std::round( fSumOfWeights ) ) % 2 ) )
    sw_max = fSumOfWeights/2;
  else
    sw_max = ( fSumOfWeights - 1 )/2;

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
void Analysis::Bin2D::Clear() {
  fXpoints.clear();
  fYpoints.clear();
  fWpoints.clear();
  fSumOfWeights = 0;
}

//______________________________________________________________________________
// Sorts the data and the weights and returns the sorted vector of weights for
// that data sample
std::vector<double> Analysis::Bin2D::Sort( std::vector<double> &dvector,
					   std::vector<double> &wvector ) {
  std::vector< std::pair<double, double> > order( dvector.size() );
  for ( size_t i = 0; i < order.size(); i++ )
    order[ i ] = std::make_pair( dvector[ i ], wvector[ i ] );
  std::sort( order.begin(), order.end(),
	     [] ( const std::pair<double, double> &a,
		  const std::pair<double, double> &b ) { return a.first < b.first; } );
  std::vector<double> swvector( order.size() );
  for ( size_t i = 0; i < order.size(); i++ ) {
    dvector [ i ] = order[ i ].first;
    swvector[ i ] = order[ i ].second;
  }
  return swvector;
}
