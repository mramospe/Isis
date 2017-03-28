//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 28/03/2017
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
#include <map>

//_______________________________________________________________________________

namespace IBoost {

  // Declaration follows "ValueTypeDef.h"
  extern const std::map<np::dtype, const char> DTypeMap = {
    {np::dtype::get_builtin< Isis::Char   >(), 'B'},
    {np::dtype::get_builtin< Isis::uChar  >(), 'b'},
    {np::dtype::get_builtin< Isis::sInt   >(), 'S'},
    {np::dtype::get_builtin< Isis::usInt  >(), 's'},
    {np::dtype::get_builtin< Isis::Int    >(), 'I'},
    {np::dtype::get_builtin< Isis::uInt   >(), 'i'},
    {np::dtype::get_builtin< Isis::Float  >(), 'F'},
    {np::dtype::get_builtin< Isis::Double >(), 'D'},
    {np::dtype::get_builtin< Isis::llInt  >(), 'L'},
    {np::dtype::get_builtin< Isis::ullInt >(), 'l'},
    {np::dtype::get_builtin< Isis::Bool   >(), 'O'}
  };

  //_______________________________________________________________________________
  //
  np::ndarray numpyArrayConstructor( const long int &lgth,
				     Isis::BufferVariable *var) {
    
    Py_intptr_t shape[1] = {lgth};

    I_SWITCH_BY_DATA_TYPE(var->getType(), shape, I_CREATE_ZEROS,
			  
			  return np::array(py::list());
			  );
    
  }

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( const size_t &nentries,
				Isis::BufferVariable *var ) :
    fVar( var ), fArray(numpyArrayConstructor(nentries, var)) { }

  //_______________________________________________________________________________
  //
  BuffVarWriter::BuffVarWriter( Isis::TreeBuffer &buffer,
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
    
    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_SET_ARRAY_VALUE);
  }

  //_______________________________________________________________________________
  //
  void BuffVarWriter::appendToVar(const size_t &idx) {

    I_BUFFVARWRITER_SELFOP(I_BUFFVARWRITER_GET_ARRAY_VALUE);
  }

}
