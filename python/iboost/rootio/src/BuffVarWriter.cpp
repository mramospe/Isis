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
//////////////////////////////////////////////////////////////////////////////////


#include "BuffVarWriter.h"
#include "GlobalWrappers.h"
#include "InitModule.h"
#include "NumpyConverters.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "TreeBuffer.h"
#include "Definitions.h"
#include "ValueTypeDef.h"

#include <iostream>

//_______________________________________________________________________________

namespace iboost {

  //_______________________________________________________________________________
  // Develop one internal operation in a BufferVarWriter class
#define I_BUFFVARWRITER_SELFOP( action )		\
  const char &type = fVar->getType();			\
  auto ptr = fArray.get_data();				\
  I_SWITCH_BY_DATA_TYPE(type, ptr, action, NOOP);

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( isis::BufferVariable *var,
				const np::ndarray &array ) :
    fVar(var), fArray(array) { }

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( isis::TreeBuffer &buffer,
				const std::string &name,
				const np::ndarray &array ) :
    fArray(array) {

    const char type = DTYPE_TO_TYPE.parse(fArray.get_dtype());
    
    fVar = buffer.createVariable(name, type);
  }
  
  //_______________________________________________________________________________
  //
  BuffVarWriter::~BuffVarWriter() {  }

  //_______________________________________________________________________________
  //
  void BuffVarWriter::appendToArray( const size_t &idx, const size_t &n ) {

#define I_BUFFVARWRITER_SET_ARRAY_VALUE( type, ptr ) {			\
      type* type ## _ptr = reinterpret_cast<type*>(ptr) + n*idx;	\
      type val = 0;							\
      fVar->extractValue(val);						\
      *type ## _ptr = val;						\
    }
    
    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_SET_ARRAY_VALUE);

#undef I_BUFFVARWRITER_SET_ARRAY_VALUE
  }

  //_______________________________________________________________________________
  //
  void BuffVarWriter::appendToVar( const size_t &idx, const size_t &n ) {

#define I_BUFFVARWRITER_GET_ARRAY_VALUE( type, ptr ) {			\
      type* type ## _ptr = reinterpret_cast<type*>(ptr) + n*idx;	\
      fVar->setValue(*type ## _ptr);					\
    }
    
    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_GET_ARRAY_VALUE);

#undef I_BUFFVARWRITER_GET_ARRAY_VALUE
  }

}
