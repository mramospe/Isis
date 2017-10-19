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


#include "AdaptiveBinning1D.hpp"
#include "Definitions.hpp"
#include "Exceptions.hpp"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>
#include <vector>


//______________________________________________________________________________

namespace isis {

  //______________________________________________________________________________
  //
  AdaptiveBinning1D::AdaptiveBinning1D( size_t  occ,
					double  vmin,
					double  vmax,
					const Doubles &values,
					const Doubles &weights ) :
    AdaptiveBinning(),
    fMax(vmax),
    fMin(vmin) {

    std::vector<std::pair<double, double> > data;

    auto itv = values.begin();
    if ( weights.size() ) {
      if ( values.size() != weights.size() )
	throw BaseException("The lengths of the vectors containing the values "\
			    "and the weights do no match");
      auto itw = weights.begin();
      while( itv != values.end() ) {
	if ( *itv >= vmin && *itv < vmax )
	  data.emplace_back(*itv, *itw);
	++itv;
	++itw;
      }
    }
    else {
      while( itv != values.end() ) {
	if ( *itv >= vmin && *itv < vmax )
	  data.emplace_back(*itv, 1);
	++itv;
      }
    }

    // Calculates the sum of weights
    double sw = 0;
    for ( auto it = data.begin(); it != data.end(); ++it )
      sw += it->second;

    // Calculates the number of bins
    size_t nbins = size_t( sw )/occ;
  
    // If the number of bins is zero an error is displayed
    if ( nbins == 0 )
      throw BaseException("Occupancy requested is too big: " + std::to_string(occ));
  
    // Creates the vector of bins
    fBinList = std::vector<Bin*>( nbins );
    std::generate(fBinList.begin(), fBinList.end(),
		  [this] () {
		    
		    return new Bin1D(fMax);
		    
		  });
    
    // Sorts the data and the weights
    std::sort( data.begin(), data.end(), [] ( std::pair<double, double> it1,
					      std::pair<double, double> it2 ) {
		 return it1.first < it2.first; } );

    // Depending if it is working with weights or with entries it fills the bins
    auto id = data.begin();

    double auxsw = sw, swpb = 0;
    size_t binsout = 0;
    for ( auto ib = fBinList.begin(); ib != fBinList.end(); ++ib ) {
      
      swpb = auxsw/(nbins - binsout++);
      
      while ( (*ib)->getSumOfWeights() < swpb && id != data.end() ) {
	
	static_cast<Bin1D*>(*ib)->fill(id->first, id->second);
	
	++id;
	
      }
      auxsw -= (*ib)->getSumOfWeights();
    }
    // If the end of the data has not been reached, it fills the last bin
    // with the rest of the events
    while ( id != data.end() )
      static_cast<Bin1D*>( fBinList.back() )->fill( id->first, id++->second );
  
    // Sets the different bin limits
    static_cast<Bin1D*>( fBinList.front() )->fMin = fMin;
  }

  //______________________________________________________________________________
  //
  TH1D* AdaptiveBinning1D::getStruct( const char *name, const char *title ) const {
    
    double *bins = new double[fBinList.size() + 1];
    
    for ( size_t i = 0; i < fBinList.size(); ++i )
      bins[i] = static_cast<Bin1D*>( fBinList[i])->fMin;
    
    bins[fBinList.size()] = fMax;
    
    TH1D *hist = new TH1D(name, title, fBinList.size(), bins);
    
    delete[] bins;
    
    return hist;
  }

}
