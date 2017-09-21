//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/09/2017
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


#ifndef BUFF_VAR_WRITER_H
#define BUFF_VAR_WRITER_H

#include "GlobalWrappers.h"

#include "BufferVariable.h"
#include "Definitions.h"
#include "TreeBuffer.h"

#include <iostream>


//_______________________________________________________________________________

namespace iboost {

  class BuffVarWriter {

  public:
    
    // Constructor
    BuffVarWriter( isis::BufferVariable *var, const np::ndarray &array );

    // Constructor given the variable and the list to write/read to/from
    BuffVarWriter( isis::TreeBuffer &buffer,
		   const std::string &name,
		   const np::ndarray &array );

    // Destructor
    ~BuffVarWriter();

    // Appends the current value stored in the BufferVariable object to the
    // numpy ndarray, where "n" is the size of the row
    void appendToArray( const size_t &idx, const size_t &n );

    // Append the value in the position "idx" to the BufferVariable object,
    // where "n" is the size of the row
    void appendToVar( const size_t &idx, const size_t &n );

  protected:
    
    // Variable to be used for input/output
    isis::BufferVariable *fVar;

    // Input/output array
    np::ndarray fArray;
  };

}

#endif
