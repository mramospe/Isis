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


#ifndef LOOP_ARRAY
#define LOOP_ARRAY

#include <iostream>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class LoopArray {

  public:

    // Nested struct class
    struct LoopArrayIndex {
      LoopArrayIndex( const size_t &min, const size_t &max );
      ~LoopArrayIndex();

      size_t Index;
      size_t Min;
      size_t Max;
    };

    // Constructors
    LoopArray();
    LoopArray( const LoopArray &other );
    LoopArray( const size_t &size, const size_t &min, const size_t &max );

    // Destructor
    ~LoopArray();

    // Methods
    inline void         AddIndex( const size_t &min, const size_t &max );
    inline const size_t GetNindices() const;
    inline const size_t GetNloops() const;
    inline const size_t GetPos() const;
    inline void         Next();
    inline void         Start();
    inline bool         Status() const;

    // Iterative methods
    inline std::vector<LoopArrayIndex>::const_iterator cBegin();
    inline std::vector<LoopArrayIndex>::const_iterator cEnd();

    // Operators
    LoopArray&  operator ++ ();
    LoopArray   operator ++ ( int );
    const size_t operator [] ( size_t index ) const;

  protected:

    // Attributes
    size_t                      fNloops;
    size_t                      fPos;
    std::vector<LoopArrayIndex> fVector;
  };

  //______________________________
  // INLINE METHODS

  // Adds a new index variable to the array
  inline void LoopArray::AddIndex( const size_t &min, const size_t &max ) {
    fVector.push_back( LoopArrayIndex( min, max ) );
    fNloops *= ( max - min );
  }
  // Gets the number of indices stored in the array
  inline const size_t LoopArray::GetNindices() const {
    return fVector.size();
  }
  // Returns the current position of the array
  inline const size_t LoopArray::GetPos() const {
    return fPos;
  }
  // This method is set to enable the pythonization of the class
  inline void LoopArray::Next() {
    this -> operator ++ ();
  }
  // Gets the number of loops that would be made using this array
  inline const size_t LoopArray::GetNloops() const {
    return fNloops;
  }
  // Resets the values of the array to their minimums to start a new iteration
  inline void LoopArray::Start() {
    fPos = 0;
    for ( auto it = fVector.begin(); it != fVector.end(); ++it )
      it -> Index = it -> Min;    
  }
  // Return the status of the iteration
  inline bool LoopArray::Status() const {
    return ( fPos < fNloops );
  }
  // Returns the position to the first element of the vector of indices
  inline std::vector<LoopArray::LoopArrayIndex>::const_iterator LoopArray::cBegin() {
    return fVector.cbegin();
  }
  // Returns the position to the next-to-last element of the vector of indices
  inline std::vector<LoopArray::LoopArrayIndex>::const_iterator LoopArray::cEnd() {
    return fVector.cend();
  }

}

#endif
