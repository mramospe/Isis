///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 29/06/2017
//
// ------------------------------------------------------------------------
//
//  Description:
//
//  Class to define a lower and an upper bound. The convention is to accept
//  that a value is inside it if "value >= lower and value < upper".
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef __BOUNDS__
#define __BOUNDS__

#include <cstddef>


//_________________________________________________________________________

namespace isis {

  template<class type>
  class Bounds {

  public:

    // Constructor
    Bounds() { };

    // Constructor given the two bounds
    Bounds( const type &min, const type &max ) : fMin( min ), fMax( max ) { }

    // Copy constructor
    template<class vtype>
    Bounds( const Bounds<vtype> &other ) { *this = other; }

    // Destructor
    virtual ~Bounds() { }

    // Return the upper bound
    inline const type& getMax() const {

      return fMax;
    }
    
    // Return the lower bound
    inline const type& getMin() const {

      return fMin;
    }

    // Return whether the given value is inside the bounds or not. If
    // "endpoint" is set to true, then it is considered inside even if
    // it is equal to the maximum value.
    template<class vtype>
    inline bool isInside( const vtype &value,
			  const bool &endpoint = false ) const {

      return (value >= fMin && (endpoint ? value <= fMax : value < fMax));
    }

    // Return the value between this bounds corresponding to the position
    // "pos" of "size" iterations. If "endpoint" is set to true, then the
    // maximum value of the bound is reached (thus it is considered that
    // "size > 1").
    type iterStep( const size_t &size,
		   const size_t &pos,
		   const bool &endpoint = false ) const {

      return pos*(fMax - fMin)/(size - endpoint) + fMin;
    }

    // Set the upper bound
    template<class vtype>
    inline void setMax( const vtype &value ) {

      fMax = value;
    }

    // Set the lower bound
    template<class vtype>
    inline void setMin( const vtype &value ) {

      fMin = value;
    }

    // Set both upper and lower bounds
    template<class vtype>
    inline void setMinMax( const vtype &min, const vtype &max ) {

      fMax = max;
      fMin = min;
    }

    // Assignment operator
    template<class vtype>
    inline Bounds<type>& operator = ( const Bounds<vtype> &other ) {
      
      fMin = other.getMin();
      fMax = other.getMax();

      return *this;
    }

  protected:

    // Lower bound
    type fMin;

    // Upper bound
    type fMax;
    
  };

}

#endif
