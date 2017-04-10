//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Define the class to read and write from/to BufferVariable objects and numpy
//  ndarray instances
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef BUFF_VAR_WRITER
#define BUFF_VAR_WRITER

#include "BufferVariable.h"
#include "TreeBuffer.h"

#include "GlobalWrappers.h"
#include "ValueTypeDef.h"
#include "Definitions.h"

#include <iostream>


//_______________________________________________________________________________
// Develop one internal operation in a BufferVarWriter class
#define I_BUFFVARWRITER_SELFOP( action )		\
  const char &type = fVar->getType();			\
  auto ptr = fArray.get_data();				\
  I_SWITCH_BY_DATA_TYPE(type, ptr, action, NOOP);

//_______________________________________________________________________________
// Parse a value in the numpy array to the BufferVariable
#define I_BUFFVARWRITER_GET_ARRAY_VALUE( type, ptr ) {		\
    type* type ## _ptr = reinterpret_cast<type*>(ptr) + idx;	\
    fVar->setValue(*type ## _ptr);				\
  }

//_______________________________________________________________________________
// Append value in the BufferVariable object to the numpy array
#define I_BUFFVARWRITER_SET_ARRAY_VALUE( type, ptr ) {		\
    type* type ## _ptr = reinterpret_cast<type*>(ptr) + idx;	\
    type val = 0;						\
    fVar->extractValue(val);					\
    *type ## _ptr = val;					\
  }

//_______________________________________________________________________________
// Create an array of zeros. "shape"
#define I_CREATE_ZEROS( type, shape )					\
  return np::zeros(1, shape, np::dtype::get_builtin<type>());

//_______________________________________________________________________________

namespace iboost {

  // Map containing the way to translate the numpy ndarray dtype objects
  // into characters
  extern const std::map<np::dtype, const char> DTypeMap;
  
  //_______________________________________________________________________________
  // Construct a numpy ndarray object given a number of entries
  np::ndarray numpyArrayConstructor( const long int &lgth,
				     Isis::BufferVariable *var);

  class BuffVarWriter {

  public:
    
    // Constructor
    BuffVarWriter( const size_t &nentries, Isis::BufferVariable *var = 0 );

    // Constructor given the variable and the list to write/read to/from
    BuffVarWriter( Isis::TreeBuffer &buffer,
		   const std::string &name,
		   np::ndarray array );

    // Destructor
    ~BuffVarWriter();

    // Appends the current value stored in the BufferVariable object to the numpy ndarray
    void appendToArray( const size_t &idx );

    // Append the value in the position "idx" to the BufferVariable object
    void appendToVar( const size_t &idx );

    // Transform the output into an ndarray object
    inline np::ndarray getArray() { return fArray; };

  protected:
    
    // Variable to be used for input/output
    Isis::BufferVariable *fVar;

    // Input/output array
    np::ndarray fArray;
  };

}

#endif
