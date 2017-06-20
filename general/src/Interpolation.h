////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/06/2017
//
// -------------------------------------------------------------------------
//
//  Description:
//
//  Define functions to do interpolations among points
//
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////


#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "Messenger.h"

#include <algorithm>
#include <cmath>
#include <vector>


//________________________________________________________________________

namespace isis {

  namespace interpolation {

    //_______________________________________________________________________
    // Both vectors must be sorted and have the same lengths. Number of
    // points must be greater than "1".
    template<class typeA, class typeB>
    typeB lagrange_polynomials( const typeA &xvalue,
				const std::vector<typeA> &vx,
				const std::vector<typeB> &vy,
				unsigned short int npoints = 2 ) {

      // Get position and size of the vectors
      auto el = std::upper_bound(vx.cbegin(), vx.cend(), xvalue);
      
      size_t pos = el - vx.cbegin();
      size_t len = vx.size();

      // 1 less because we are already taking one element
      unsigned short int ndw = npoints/2;
      unsigned short int nup = npoints - ndw - 1;

      // Check the points available
      if ( (pos + nup) > len ) {
	nup = len - pos;
	ndw = npoints - nup;
      }
      else if ( (pos - ndw) < 0 ) {
	ndw = pos;
	nup = npoints - ndw;
      }

      // Calculate the y-value

      auto beg = pos - ndw;
      auto end = pos + nup;

      typeB yvalue = 0;
      
      for ( size_t i = beg; i < end; ++i ) {

	typeA xi = vx.at(i);
	typeA yi = vy.at(i);
	
	typeB prod = 1.;
	for ( size_t j = beg; j < end; ++j ) {

	  typeA xj = vx.at(j);
	  
	  if ( i != j )
	    prod *= (xvalue - xj)/(xi - xj);
	}

	yvalue += yi*prod;
      }

      return yvalue;
    }

  }

  //_______________________________________________________________________
  // Interpolate the < y > value given two vectors and the associated < x >
  // value. when dealing with integers, this function will return the
  // integer part of the result, so no rounding will be made.
  template<class typeA, class typeB>
  double linear( const std::vector<typeA> &xvalues,
		 const std::vector<typeB> &yvalues,
		 const typeA &value ) {

    auto it = std::lower_bound(xvalues.cbegin(), xvalues.cend(), value);

    size_t pos = it - xvalues.cbegin();

    switch ( pos ) {
      
    case 0:
      return yvalues.front();
      
    default:
      return (yvalues.at(pos - 1) + yvalues.at(pos))/2.;
    }
  }
  
}

#endif
