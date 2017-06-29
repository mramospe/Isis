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
//  Implements the class to generate an array of indices to perform
//  loops. This class is useful when one defines a general method to
//  perform a number of loops ( usually "for" statements ) that can vary
//  depending on different external conditions. This class behaves in the
//  same way as a concatenation of "for" statements. Using the < ++ >
//  operator, it will add units to the last index till the maximum is
//  reached, when it is reset and a new unit is added to the previous one,
//  on a recursively process. The maximum number of loops is obtained
//  using the < GetNloops > method.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef LOOP_ARRAY
#define LOOP_ARRAY

#include "Utils.h"

#include <iostream>
#include <vector>


//_________________________________________________________________________

namespace isis {

  class LoopArray {

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

      size_t Index;
      size_t Min;
      size_t Max;
    };

    // Main constructor. The number of loops must be one since as new
    // entries are added, this number is multiplied by their range.
    LoopArray() { };

    // Copy constructor
    LoopArray( const LoopArray &other ) :
      fIndices(other.fIndices),
      fNloops(other.fNloops),
      fPos(other.fPos) { }
    
    // Constructor given the sice of the array and the maximum loop value
    LoopArray( const size_t &size, const size_t &min, const size_t &max ) :
      fIndices(size, LoopArrayIndex(min, max)),
      fNloops(ipow(max - min, size)) { }

    // Destructor
    virtual ~LoopArray() { };

    // Adds a new index variable to the array
    inline void addIndex( const size_t &min, const size_t &max ) {

      fIndices.push_back( LoopArrayIndex( min, max ) );
      fNloops *= ( max - min );
    }

    // Return the vector storing the indices
    inline std::vector<LoopArrayIndex> getIndices() const {

      return fIndices;
    }

    // Gets the number of indices stored in the array
    inline const size_t getNindices() const {

      return fIndices.size();
    }

    // Gets the number of loops that would be made using this array
    inline const size_t getNloops() const {

      return fNloops;
    }

    // Returns the current position of the array
    inline const size_t getPos() const {

      return fPos;
    }

    // This method is set to enable the pythonization of the class
    inline void next() {

      this->operator++();
    }

    // Resets the values of the array to their minimums to start a new
    // iteration
    inline void start() {

      fPos = 0;
      for ( auto it = fIndices.begin(); it != fIndices.end(); ++it )
	it->Index = it->Min;
    }

    // Return the status of the iteration
    inline bool status() const {

      return (fPos < fNloops);
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
    const size_t operator[]( size_t index ) const {

      return fIndices[index].Index;
    }

  protected:

    // Vector storing each of the indices
    std::vector<LoopArrayIndex> fIndices;
    
    // Number of loops to be performed
    size_t fNloops = 1;

    // Current position
    size_t fPos = 0;
    
  };

}

#endif
