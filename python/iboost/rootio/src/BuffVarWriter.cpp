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
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "BuffVarWriter.h"
#include "GlobalWrappers.h"

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
  inline np::ndarray numpyArrayConstructor( const long int &lgth,
					    isis::BufferVariable *var) {
    
    Py_intptr_t shape[1] = {lgth};

#define I_CREATE_ZEROS( type, shape )				\
    return np::zeros(1, shape, np::dtype::get_builtin<type>());
    
    I_SWITCH_BY_DATA_TYPE(var->getType(), shape, I_CREATE_ZEROS,
			  
			  return np::array(py::list());
			  );
#undef I_CREATE_ZEROS
  }

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( const size_t &nentries,
				isis::BufferVariable *var ) :
    fVar( var ), fArray(numpyArrayConstructor(nentries, var)) { }

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( isis::TreeBuffer &buffer,
				const std::string &name,
				np::ndarray array ) :
    fArray(array) {

    const char type = DTypeMap.at(fArray.get_dtype());
    
    fVar = buffer.createVariable(name, type);
  }
  
  //_______________________________________________________________________________
  //
  BuffVarWriter::~BuffVarWriter() {  }

  //_______________________________________________________________________________
  //
  void BuffVarWriter::appendToArray(const size_t &idx) {

#define I_BUFFVARWRITER_SET_ARRAY_VALUE( type, ptr ) {		\
      type* type ## _ptr = reinterpret_cast<type*>(ptr) + idx;	\
      type val = 0;						\
      fVar->extractValue(val);					\
      *type ## _ptr = val;					\
    }
    
    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_SET_ARRAY_VALUE);

#undef I_BUFFVARWRITER_SET_ARRAY_VALUE
  }

  //_______________________________________________________________________________
  //
  void BuffVarWriter::appendToVar(const size_t &idx) {

#define I_BUFFVARWRITER_GET_ARRAY_VALUE( type, ptr ) {		\
      type* type ## _ptr = reinterpret_cast<type*>(ptr) + idx;	\
      fVar->setValue(*type ## _ptr);				\
    }
    
    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_GET_ARRAY_VALUE);

#undef I_BUFFVARWRITER_GET_ARRAY_VALUE
  }

}
