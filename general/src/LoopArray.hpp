///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 30/06/2017
//
// ------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to generate an array of indices to perform
//  loops. This class is useful when one defines a general method to
//  perform a number of loops ( usually "for" statements ) that can vary
//  depending on different external conditions. This class behaves in the
//  same way as a concatenation of "for" statements. Using the < ++ >
//  operator, it will add units to the last index till the maximum is
//  reached, when it is reset and a new unit is added to the previous one,
//  on a recursively process. The maximum number of loops is obtained
//  using the < nloops > method.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef LOOPARRAY_H
#define LOOPARRAY_H

#include "Looper.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <iostream>
#include <vector>


//_________________________________________________________________________

namespace isis {

  class LoopArray : public Looper {

  public:

    // Nested struct class
    struct LoopArrayIndex {

      // Constructor
      LoopArrayIndex( const size_t &min, const size_t &max ) :
	Index(min),
	Min(min),
	Max(max) { }

      // Destructor
      ~LoopArrayIndex() { };

      // Parse a given index. The given value is modified, so the iterations
      // corresponding to this item are removed. Returns the index for this
      // item.
      size_t parse( size_t &idx ) const {

	size_t d = Max - Min;
	size_t v = idx % d;

	idx /= d;
	
	return v + Min;
      }

      size_t Index;
      size_t Min;
      size_t Max;
    };

    // Main constructor
    LoopArray() : Looper() { };

    // Copy constructor
    LoopArray( const LoopArray &other ) :
      Looper(),
      fIndices(other.fIndices),
      fN(other.fN),
      fPos(other.fPos) { }
    
    // Constructor given the sice of the array and the maximum loop value
    LoopArray( const size_t &size, const size_t &min, const size_t &max ) :
      Looper(),
      fIndices(size, LoopArrayIndex(min, max)),
      fN(ipow(max - min, size)) { }

    // Destructor
    virtual ~LoopArray() { };

    // Adds a new index variable to the array
    inline void addIndex( const size_t &min, const size_t &max ) {

      fIndices.push_back(LoopArrayIndex(min, max));

      size_t d = max - min;
      
      fN = fN != 0 ? fN*d : d;
    }

    // Return the vector storing the indices
    inline std::vector<LoopArrayIndex> getIndices() const {

      return fIndices;
    }

    // Return the number of indices in this class
    inline size_t nindices() const {

      return fIndices.size();
    }

    // Gets the number of loops that would be made using this array
    inline size_t nloops() const override {

      return fN;
    }

    // Returns the current position of the array
    inline const size_t getPos() const {

      return fPos;
    }

    // This method is set to enable the pythonization of the class
    inline void next() override {

      this->operator++();
    }

    // Resets the values of the array to their minimums to start a new
    // iteration
    inline void start() override {

      fPos = 0;
      for ( auto it = fIndices.begin(); it != fIndices.end(); ++it )
	it->Index = it->Min;
    }

    // Return the status of the iteration
    inline bool status() const override {

      return (fPos < fN);
    }

    // Returns the position to the first element of the vector of indices
    inline std::vector<LoopArrayIndex>::const_iterator cbegin() const {

      return fIndices.cbegin();
    }

    // Returns the position to the next-to-last element of the vector of
    // indices
    inline std::vector<LoopArrayIndex>::const_iterator cend() const {

      return fIndices.cend();
    }

    // Return the current values for the indices
    inline Sizes values() const {

      Sizes res(fIndices.size());

      auto it = fIndices.cbegin();
      
      std::generate(res.begin(), res.end(),
		    [&it] () {
		      
		      return (it++)->Index;
		      
		    });
      
      return res;
    }

    // Moves one element forward in the array. This implies to add one
    // unit to the last index stored. If it reaches the associated maximum
    // value it is reset, and a unit is added to the next index ( doing
    // this recursively ).
    inline LoopArray& operator++() {

      fIndices.back().Index++;
      
      auto it = fIndices.rbegin();
      
      while ( it->Index == it->Max ) {
	
	it->Index = it->Min;
	
	if ( it + 1 != fIndices.rend() )
	  ( ++it )->Index++;
      }
      
      ++fPos;
    
      return *this;
    }

    // Creates a copy of this class and adds a new unit to the current
    // one. The old class is returned.
    inline LoopArray operator++( int ) {

      LoopArray tmp( *this );
	  
      this->operator++();
    
      return tmp;
    }

    // Returns the value of the index at position < index >
    const Sizes operator[]( size_t index ) const {

      Sizes result(fIndices.size());

      auto it = fIndices.crbegin();
      
      std::generate(result.rbegin(), result.rend(),
		    [&it, &index] {

		      return (it++)->parse(index);
		      
		    });
	
      return result;
    }

  protected:

    // Vector storing each of the indices
    std::vector<LoopArrayIndex> fIndices;
    
    // Number of loops to be performed
    size_t fN = 0;

    // Current position
    size_t fPos = 0;
    
  };

}

#endif
