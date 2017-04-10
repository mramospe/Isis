///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "Bin2D.h"
#include "Definitions.h"

#include <algorithm>


//______________________________________________________________________________

namespace isis {

  //______________________________________________________________________________
  //
  Bin2D::Bin2D( const double &xmin,
		const double &xmax,
		const double &ymin,
		const double &ymax ) :
    Bin(), fXmax( xmax ), fXmin( xmin ), fYmax( ymax ), fYmin( ymin ) { }

  //______________________________________________________________________________
  //
  Bin2D::~Bin2D() { }

  //______________________________________________________________________________
  //
  void Bin2D::adjustBin( const double &xmin,
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
  //
  void Bin2D::fill( const double &x, const double &y, const double &w ) {

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
  //
  Bin2D* Bin2D::divide( const double &xrange, const double &yrange ) {
    
    double tmp;
    this->calcMedians();
    
    if ( std::min( fXmedian - fXmin, fXmax - fXmedian )/xrange > 
	 std::min( fYmedian - fYmin, fYmax - fYmedian )/yrange ) {
      tmp   = fXmax;
      fXmax = fXmedian;
      this->clear();
      return new Bin2D( fXmedian, tmp, fYmin, fYmax );
    }
    else {
      tmp   = fYmax;
      fYmax = fYmedian;
      this->clear();
      return new Bin2D( fXmin, fXmax, fYmedian, tmp );
    }
  }

  //______________________________________________________________________________
  //
  void Bin2D::calcMedians() {
  
    // Sorts the data ( with the weights )
    Doubles
      xw_sorted( this->sort( fXpoints, fWpoints ) ),
      yw_sorted( this->sort( fYpoints, fWpoints ) );

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
  //
  void Bin2D::clear() {
    
    fXpoints.clear();
    fYpoints.clear();
    fWpoints.clear();
    fSumOfWeights = 0;
  }

  //______________________________________________________________________________
  //
  Doubles Bin2D::sort( Doubles &dvector, Doubles &wvector ) {
    
    std::vector< std::pair<double, double> > order( dvector.size() );
    for ( size_t i = 0; i < order.size(); i++ )
      order[ i ] = std::make_pair( dvector[ i ], wvector[ i ] );
    std::sort( order.begin(), order.end(),
	       [] ( const std::pair<double, double> &a,
		    const std::pair<double, double> &b ) { return a.first < b.first; } );
    Doubles swvector( order.size() );
    for ( size_t i = 0; i < order.size(); i++ ) {
      dvector [ i ] = order[ i ].first;
      swvector[ i ] = order[ i ].second;
    }
    
    return swvector;
  }

}
