///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 02/01/2016
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


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::LoopArray::LoopArray() : fNloops( 1 ) { }

//_______________________________________________________________________________
// Copy constructor
General::LoopArray::LoopArray( const LoopArray &other ) {
  fNloops = other.fNloops;
  fVector = other.fVector;
}

//_______________________________________________________________________________
// Constructor given the sice of the array and the maximum loop value
General::LoopArray::LoopArray( size_t size, size_t max ) :
  fNloops( size*max ),
  fVector( std::vector<std::pair<size_t, size_t> >( size, std::make_pair( 0, max ) ) ) { }

//_______________________________________________________________________________
// Constructor given the size of the array and a function that defines, for each
// index, the maximum value of the loop
General::LoopArray::LoopArray( size_t size, size_t ( *func )( size_t ) ) :
  fNloops( 1 ),
  fVector( std::vector<std::pair<size_t, size_t> >( size, std::make_pair( 0, 0 ) ) ) {
  for ( size_t i = 0; i < size; i++ ) {
    fVector[ i ].second = func( i );
    fNloops *= fVector[ i ].second;
  }
}

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
  fVector.back().first++;
  auto it = fVector.rbegin();
  while ( it -> first == it -> second ) {
    it -> first = 0;
    if ( it + 1 != fVector.rend() )
      ( ++it ) -> first++;
  }
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
  return fVector[ index ].first;
}
