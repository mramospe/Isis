////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -----------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// -----------------------------------------------------------------------------
//
//  Description:
//
//  Definition of the buffer to store different types of data in the same
//  vector. The variables are accessed as in a common map, using the
//  operator < [] > or the method < Get >. This class uses pointers to the
//  variables since the class BufferVariable does not support a copy
//  constructor. In order to modify them, one must use the operator. To add a
//  new variable, one must use the method < AddVariable >, specifying the
//  name and the type.
//
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////


#ifndef BUFFER_ARRAY
#define BUFFER_ARRAY

#include <map>
#include <string>
#include <vector>

#include "BufferVariable.h"


//_______________________________________________________________________________

namespace Isis {

  class BufferArray {
        
  public:
    
    // Definition of internal variables
    typedef BufferVariable BuffVar;
    typedef bool (*SortFunc)( const std::string &stra, const std::string &strb );
    typedef std::map<std::string, BuffVar*, SortFunc> BuffVarMap;
    
    // Copy constructor. Memory will be allocated to generate new variables. The
    // allocated memory is independent for each array.
    BufferArray( const BufferArray &other );
    
    // Main constructor, given the ordering function to follow. By default, the
    // ordering is alphabetic.
    BufferArray( SortFunc func = []
		 ( const std::string &strA,
		   const std::string &strB ) { return strA < strB; } );
    
    // Destructor
    virtual ~BufferArray();

    // Adds a new variable to the buffer, given the name and the type
    BufferVariable* AddVariable( const std::string &name, const char &type );
    
    // Appends to the new vector the names of the current array
    void ExtractNames( std::vector<std::string> &vector );

    // Converts the values stored in this array to a string
    std::string ToString() const;
    
    // Removes the variables booked in this class
    inline void Clear();

    // Checks whether the given variable is already booked  
    inline bool Contains( const std::string &name ) const;

    // Returns the variable related to the name given
    inline const BuffVar& Get( const std::string &name ) const;

    // Returns the map containing all the variables
    inline const BuffVarMap& GetMap() const;

    // Returns the size of the map
    inline size_t GetSize() const;

    // Reconstructs the map following the given sort function    
    inline void Sort( SortFunc func );
    
    // Operator to get a variable by index
    inline BuffVar& operator [] ( const std::string &name );
    
  protected:
    
    // Map containing the variables
    BuffVarMap fVarMap;
    
  };
  
  //_______________________________________________________________________________
  //
  inline void BufferArray::Clear() {
    for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
      delete it->second;
    fVarMap.clear();
  }

  //_______________________________________________________________________________
  //
  inline bool BufferArray::Contains( const std::string &name ) const {
    return fVarMap.count( name );
  }

  //_______________________________________________________________________________
  //
  inline const BufferArray::BuffVar& BufferArray::Get( const std::string &name ) const {
    return *(fVarMap.at( name ));
  }

  //_______________________________________________________________________________
  //
  inline const BufferArray::BuffVarMap& BufferArray::GetMap() const {
    return fVarMap;
  }

  //_______________________________________________________________________________
  //
  inline size_t BufferArray::GetSize() const { return fVarMap.size(); }

  //_______________________________________________________________________________
  //
  inline void BufferArray::Sort( SortFunc func ) {
    fVarMap = BufferArray::BuffVarMap( fVarMap.begin(), fVarMap.end(), func );
  }

  //_______________________________________________________________________________
  //
  inline BufferVariable& BufferArray::operator [] ( const std::string &name ) {
    return *(fVarMap.at( name ));
  }

}

#endif
