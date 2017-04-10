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
#include "Definitions.h"

#include <iostream>


//_______________________________________________________________________________

namespace iboost {
  
  //_______________________________________________________________________________
  // Construct a numpy ndarray object given a number of entries
  np::ndarray numpyArrayConstructor( const long int &lgth,
				     isis::BufferVariable *var);

  class BuffVarWriter {

  public:
    
    // Constructor
    BuffVarWriter( const size_t &nentries, isis::BufferVariable *var = 0 );

    // Constructor given the variable and the list to write/read to/from
    BuffVarWriter( isis::TreeBuffer &buffer,
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
    isis::BufferVariable *fVar;

    // Input/output array
    np::ndarray fArray;
  };

}

#endif
