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
//  Implements the class to make one-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning1D.h"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>
#include <vector>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning1D::AdaptiveBinning1D() : AdaptiveBinning() { }

//______________________________________________________________________________
// Constructor given vectors of values
Analysis::AdaptiveBinning1D::AdaptiveBinning1D( size_t  occ,
						double  vmin,
						double  vmax,
						const std::vector<double> &values,
						const std::vector<double> &weights ) :
  AdaptiveBinning(),
  fMax( vmax ),
  fMin( vmin ) {

  std::vector< std::pair<double, double> > data;

  auto itv = values.begin();
  if ( weights.size() ) {
    if ( values.size() != weights.size() )
      std::cerr <<
	"ERROR: The lengths of the vectors containing the values and the weights do no match"
		<< std::endl;
    auto itw = weights.begin();
    while( itv != values.end() ) {
      if ( *itv >= vmin && *itv < vmax )
	data.push_back( std::make_pair( *itv, *itw ) );
      ++itv;
      ++itw;
    }
  }
  else {
    while( itv != values.end() ) {
      if ( *itv >= vmin && *itv < vmax )
	data.push_back( std::make_pair( *itv, 1 ) );
      ++itv;
    }
  }

  // Calculates the sum of weights
  double sw = 0;
  for ( auto it = data.begin(); it != data.end(); ++it )
    sw += it -> second;

  // Calculates the number of bins
  size_t nbins = size_t( sw )/occ;
  
  // If the number of bins is zero an error is displayed
  if ( nbins == 0 )
    std::cerr << "ERROR: Occupancy requested is too big: " << occ << std::endl;
  
  // Creates the vector of bins
  fBinList = std::vector<Bin*>( nbins );
  for ( auto it = fBinList.begin(); it != fBinList.end(); ++it )
    (*it) = new Bin1D( fMax );
  
  // Sorts the data and the weights
  std::sort( data.begin(), data.end(), [] ( std::pair<double, double> it1,
						    std::pair<double, double> it2 ) {
	       return it1.first < it2.first; } );

  // Depending if it is working with weights or with entries it fills the bins
  auto id = data.begin();

  double auxsw = sw, swpb = 0;
  size_t binsout = 0;
  for ( auto ib = fBinList.begin(); ib != fBinList.end(); ++ib ) {
    swpb = auxsw/( nbins - binsout++ );
    while ( (*ib) -> GetSumOfWeights() < swpb && id != data.end() ) {
      static_cast<Analysis::Bin1D*>( *ib ) -> Fill( id -> first, id -> second );
      ++id;
    }
    auxsw -= (*ib) -> GetSumOfWeights();
  }
  // If the end of the data has not been reached, it fills the last bin
  // with the rest of the events
  while ( id != data.end() )
    static_cast<Analysis::Bin1D*>( fBinList.back() )
      -> Fill( id -> first, id++ -> second );
  
  // Sets the different bin limits
  static_cast<Analysis::Bin1D*>( fBinList.front() ) -> fMin = fMin;
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning1D::~AdaptiveBinning1D() { }

//______________________________________________________________________________


// -- PUBLIC METHOD

//______________________________________________________________________________
// Returns an empty histogram with the adaptive binning structure
TH1D* Analysis::AdaptiveBinning1D::GetStruct( const char *name, const char *title ) const {
 double *bins = new double[ fBinList.size() + 1 ];
  for ( size_t i = 0; i < fBinList.size(); ++i )
    bins[ i ] = static_cast<Analysis::Bin1D*>( fBinList[ i ] ) -> fMin;
  bins[ fBinList.size() ] = fMax;
  TH1D *hist = new TH1D( name, title, fBinList.size(), bins );
  delete[] bins;
  return hist;
}
