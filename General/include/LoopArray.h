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


#ifndef LOOP_ARRAY
#define LOOP_ARRAY

#include <iostream>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class LoopArray {

  public:

    // Constructors
    LoopArray();
    LoopArray( const LoopArray &other );
    LoopArray( size_t size, size_t max );
    LoopArray( size_t size, size_t ( *func )( size_t ) );

    // Destructor
    ~LoopArray();

    // Methods
    inline void         AddIndex( size_t max );
    inline const size_t GetNindices() const;
    inline const size_t GetNloops() const;

    // Iterative methods
    inline std::vector<std::pair<size_t, size_t> >::const_iterator cBegin();
    inline std::vector<std::pair<size_t, size_t> >::const_iterator cEnd();

    // Operators
    LoopArray&  operator ++ ();
    LoopArray   operator ++ ( int );
    const size_t operator [] ( size_t index ) const;

  protected:

    // Attributes
    size_t                                  fNloops;
    std::vector<std::pair<size_t, size_t> > fVector;
  };

  //______________________________
  // INLINE METHODS

  // Adds a new index variable to the array
  inline void LoopArray::AddIndex( size_t max ) {
    fVector.push_back( std::make_pair( 0, max ) );
    fNloops *= max;
  }
  // Gets the number of indices stored in the array
  inline const size_t LoopArray::GetNindices() const {
    return fVector.size();
  }
  // Gets the number of loops that would be made using this array
  inline const size_t LoopArray::GetNloops() const {
  size_t res = 1;
  for ( auto it = fVector.begin(); it != fVector.end(); it++ )
    res *= it -> second;
  return res;
  }
  // Returns the position to the first element of the vector of indices
  inline std::vector<std::pair<size_t, size_t> >::const_iterator LoopArray::cBegin() {
    return fVector.cbegin();
  }
  // Returns the position to the next-to-last element of the vector of indices
  inline std::vector<std::pair<size_t, size_t> >::const_iterator LoopArray::cEnd() {
    return fVector.cend();
  }

}

#endif
