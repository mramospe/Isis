///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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

namespace Isis {

  class LoopArray {

  public:

    // Nested struct class
    struct LoopArrayIndex {

      // Constructor
      LoopArrayIndex( const size_t &min, const size_t &max );

      // Destructor
      ~LoopArrayIndex();

      size_t Index;
      size_t Min;
      size_t Max;
    };

    // Main constructor. The number of loops must be one since as new entries are
    // added, this number is multiplied by their range.
    LoopArray();

    // Copy constructor
    LoopArray( const LoopArray &other );

    // Constructor given the sice of the array and the maximum loop value
    LoopArray( const size_t &size, const size_t &min, const size_t &max );

    // Destructor
    ~LoopArray();

    // Adds a new index variable to the array
    inline void         AddIndex( const size_t &min, const size_t &max );

    // Gets the number of indices stored in the array
    inline const size_t GetNindices() const;

    // Gets the number of loops that would be made using this array
    inline const size_t GetNloops() const;

    // Returns the current position of the array
    inline const size_t GetPos() const;

    // This method is set to enable the pythonization of the class
    inline void Next();

    // Resets the values of the array to their minimums to start a new iteration
    inline void Start();

    // Return the status of the iteration
    inline bool Status() const;

    // Returns the position to the first element of the vector of indices
    inline std::vector<LoopArrayIndex>::const_iterator cBegin();

    // Returns the position to the next-to-last element of the vector of indices
    inline std::vector<LoopArrayIndex>::const_iterator cEnd();

    //  Moves one element forward in the array. This implies to add one unit to the
    // last index stored. If it reaches the associated maximum value it is reset,
    // and a unit is added to the next index ( doing this recursively ).
    LoopArray&  operator ++ ();

    // Creates a copy of this class and adds a new unit to the current one. The old
    // class is returned.
    LoopArray   operator ++ ( int );

    // Returns the value of the index at position < index >
    const size_t operator [] ( size_t index ) const;

  protected:

    // Number of loops to be performed
    size_t fNloops;

    // Current position
    size_t fPos;

    // Vector storing each of the indices
    std::vector<LoopArrayIndex> fVector;
  };

  //_______________________________________________________________________________
  //
  inline void LoopArray::AddIndex( const size_t &min, const size_t &max ) {
    fVector.push_back( LoopArrayIndex( min, max ) );
    fNloops *= ( max - min );
  }

  //_______________________________________________________________________________
  //
  inline const size_t LoopArray::GetNindices() const {
    return fVector.size();
  }

  //_______________________________________________________________________________
  //
  inline const size_t LoopArray::GetPos() const {
    return fPos;
  }

  //_______________________________________________________________________________
  //
  inline void LoopArray::Next() {
    this->operator ++ ();
  }

  //_______________________________________________________________________________
  //
  inline const size_t LoopArray::GetNloops() const {
    return fNloops;
  }

  //_______________________________________________________________________________
  //
  inline void LoopArray::Start() {
    fPos = 0;
    for ( auto it = fVector.begin(); it != fVector.end(); ++it )
      it->Index = it->Min;    
  }

  //_______________________________________________________________________________
  //
  inline bool LoopArray::Status() const {
    return ( fPos < fNloops );
  }

  //_______________________________________________________________________________
  //
  inline std::vector<LoopArray::LoopArrayIndex>::const_iterator LoopArray::cBegin() {
    return fVector.cbegin();
  }

  //_______________________________________________________________________________
  //
  inline std::vector<LoopArray::LoopArrayIndex>::const_iterator LoopArray::cEnd() {
    return fVector.cend();
  }

}

#endif
