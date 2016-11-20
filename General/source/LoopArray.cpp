///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/11/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to generate an array of indices to perform loops. This
//  class is useful when one defines a general method to perform a number of
//  loops ( usually "for" statements ) that can vary depending on different
//  external conditions. This class behaves in the same way as a concatenation
//  of "for" statements. Using the < ++ > operator, it will add units to the
//  last index till the maximum is reached, when it is reset and a new unit is
//  added to the previous one, on a recursively process. The maximum number of
//  loops is obtained using the < GetNloops > method.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "LoopArray.h"
#include "Utils.h"


//_______________________________________________________________________________
// Nested struct class
General::LoopArray::LoopArrayIndex::LoopArrayIndex( const size_t &min,
						    const size_t &max ) :
  Index( min ), Min( min ), Max( max ) { }
General::LoopArray::LoopArrayIndex::~LoopArrayIndex() { }

//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor. The number of loops must be one since as new entries are
// added, this number is multiplied by their range.
General::LoopArray::LoopArray() : fNloops( 1 ), fPos( 0 ) { }

//_______________________________________________________________________________
// Copy constructor
General::LoopArray::LoopArray( const LoopArray &other ) :
  fNloops( other.fNloops ), fPos( 0 ), fVector( other.fVector ) { }

//_______________________________________________________________________________
// Constructor given the sice of the array and the maximum loop value
General::LoopArray::LoopArray( const size_t &size,
			       const size_t &min,
			       const size_t &max ) :
  fNloops( General::IPow( max - min, size ) ),
  fPos( 0 ),
  fVector( std::vector<LoopArrayIndex> ( size, LoopArrayIndex( min, max ) ) ) { }

//_______________________________________________________________________________
// Destructor
General::LoopArray::~LoopArray() { }

//_______________________________________________________________________________


// -- OPERATORS

//_______________________________________________________________________________
// Moves one element forward in the array. This implies to add one unit to the
// last index stored. If it reaches the associated maximum value it is reset,
// and a unit is added to the next index ( doing this recursively ).
General::LoopArray& General::LoopArray::operator ++ () {
  fVector.back().Index++;
  auto it = fVector.rbegin();
  while ( it -> Index == it -> Max ) {
    it -> Index = it -> Min;
    if ( it + 1 != fVector.rend() )
      ( ++it ) -> Index++;
  }
  ++fPos;
  return *this;
}

//_______________________________________________________________________________
// Creates a copy of this class and adds a new unit to the current one. The old
// class is returned.
General::LoopArray General::LoopArray::operator ++ ( int ) {
  General::LoopArray tmp( *this );
  this -> operator ++ ();
  return tmp;
}

//_______________________________________________________________________________
// Returns the value of the index at position < index >
const size_t General::LoopArray::operator [] ( size_t index ) const {
  return fVector[ index ].Index;
}
