////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  General package                                                           //
//                                                                            //
// -------------------------------------------------------------------------- //
//                                                                            //
//  AUTHOR: Miguel Ramos Pernas                                               //
//  e-mail: miguel.ramos.pernas@cern.ch                                       //
//                                                                            //
//  Last update: 20/06/2016                                                   //
//                                                                            //
// -------------------------------------------------------------------------- //
//                                                                            //
//  Description:                                                              //
//                                                                            //
//  Definition of the buffer to store different types of data in the same     //
//  vector. The variables are accessed as in a common map, using the          //
//  operator < [] > or the method < Get >. In order to modify them, one must  //
//  use the operator. To add a new variable, one must use the method          //
//  < AddVariable >, specifying the name and the type.                        //
//                                                                            //
// -------------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////


#ifndef BUFFER_ARRAY
#define BUFFER_ARRAY

#include <map>
#include <string>
#include <vector>

#include "BufferVariable.h"


//_______________________________________________________________________________

namespace General {

  class BufferArray {
    
    typedef General::BufferVariable BuffVar;
    typedef bool (*SortFunc)( const std::string &stra, const std::string &strb );
    typedef std::map<std::string, BuffVar*, SortFunc> BuffVarMap;
    
  public:
    
    // Constructor and destructor
    BufferArray( const BufferArray &other );
    BufferArray( SortFunc func = [] ( const std::string &strA,
				      const std::string &strB ) { return strA < strB; } );
    ~BufferArray();

    // Methods
    void        AddVariable( const std::string &name, const char &type );
    void        ExtractNames( std::vector<std::string> &vector );
    std::string ToString() const;
    
    // Inline methods
    inline bool              Contains( const std::string &name ) const;
    inline const BuffVar&    Get( const std::string &name ) const;
    inline const BuffVarMap& GetMap() const;
    inline size_t            GetSize() const;
    inline void              Sort( SortFunc func );
    
    // Operator
    inline BuffVar& operator [] ( const std::string &name );
    
  protected:
    
    // Attribute
    BuffVarMap fVarMap;
    
  };

  //__________________
  // -- INLINE METHODS
  inline bool BufferArray::Contains( const std::string &name ) const {
    return (fVarMap.find( name ) != fVarMap.end());
  }
  inline const BufferArray::BuffVar& BufferArray::Get( const std::string &name ) const {
    return *(fVarMap.at( name ));
  }
  inline const BufferArray::BuffVarMap& BufferArray::GetMap() const {
    return fVarMap;
  }
  inline size_t BufferArray::GetSize() const { return fVarMap.size(); }
  inline void BufferArray::Sort( SortFunc func ) {
    fVarMap = BufferArray::BuffVarMap( fVarMap.begin(), fVarMap.end(), func );
  }

  //____________
  // -- OPERATOR
  inline General::BufferVariable& BufferArray::operator [] ( const std::string &name ) {
    return *(fVarMap.at( name ));
  }

}

#endif
