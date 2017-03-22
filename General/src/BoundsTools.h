////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -----------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 22/03/2017
//
// -----------------------------------------------------------------------------
//
//  Description:
//
//  Define functions and tools to work with vectors of Bounds, and double-type
//  vectors referring to bounds.
//
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////


#ifndef BOUNDS_TOOLS
#define BOUNDS_TOOLS

#include "Bounds.h"

#include <algorithm>
#include <vector>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  // Get the position in the vector of bounds where "value" would be inside. The
  // positions "0" and "1" refer to underflow and overflow values.
  template<class vtype, class btype>
  inline size_t BoundedPos( const vtype &value, const std::vector<btype> &bounds ) {

    size_t pos = 0;
    for ( auto b : bounds )
      if ( value >= b )
	++pos;
      else
	break;
    
    return pos;
  }

  //_______________________________________________________________________________
  // Return a vector with the "n + 1" bounds generated from "bounds". The last one
  // corresponds to the upper bound.
  template<class vtype>
  inline std::vector<vtype> BoundsVector( const Bounds<vtype> &bounds,
					  const size_t &n ) {

    Doubles values(n + 1, bounds.GetMin());
    
    double step = (bounds.GetMax() - bounds.GetMin())/n;

    // No need to set first, and last value is set afterwards
    size_t i = 0;
    std::generate(values.begin() + 1, values.end() - 1,
		  [&i, step] () { return (++i)*step; });
    values.back() = bounds.GetMax();
    
    return values;
  }

  //_______________________________________________________________________________
  // Return a vector of "n" Bounds generated from "bounds"
  template<class vtype>
  inline std::vector<Bounds<vtype> > VectorOfBounds( const Bounds<vtype> &bounds,
						     const size_t &n ) {

    Doubles vbounds = BoundsVector(bounds, n);

    std::vector<Bounds<vtype> > out(n, Bounds<vtype>());
    
    auto itbd = vbounds.cbegin();
    auto itbr = out.begin();

    while ( itbr != out.end() ) {

      itbr->SetMinMax(*itbd, *(itbd + 1));
      
      ++itbr;
      ++itbd;
    }
    
    return out;
  }

  //_______________________________________________________________________________
  // Fill a vector with the occurrences inside each bound when filling it with values
  // from "first" to "last"
  template<class InputIterator, class btype>
  inline Sizes BoundedFillVec( InputIterator first, InputIterator last,
					     const std::vector<btype> &bounds ) {

    Sizes out(bounds.size() + 1, 0);
    while ( first != last )
      out.at(BoundedPos(*first++, bounds)) += 1;

    return out;
  }

  //_______________________________________________________________________________
  // Same as "BoundedFillVec" but using weights. The user must check that "wgtit" is
  // able to move forward as many times as "last - first".
  template<class InputIterator, class btype, class WgtIterator>
  inline Doubles BoundedWgtFillVec( InputIterator first, InputIterator last,
				    const std::vector<btype> &bounds,
				    WgtIterator wgtit ) {

    Doubles out(bounds.size() + 1, 0);
    while ( first != last )
      out.at(BoundedPos(*first++, bounds)) += *wgtit++;

    return out;
  }
  
}

#endif
