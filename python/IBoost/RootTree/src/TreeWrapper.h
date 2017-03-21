//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Functions to convert python dictionaries and lists onn branches in a Root
//  tree. The output is saved on the current directory.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef TREE_WRAPPER
#define TREE_WRAPPER

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>

#include "BufferVariable.h"

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"

#include <vector>


//_______________________________________________________________________________

namespace IBoost {

  // CLASSES
  struct BuffVarWriter {

    BuffVarWriter( Isis::BufferVariable *var = 0 );
    BuffVarWriter( Isis::BufferVariable *var, boost::python::list lst );
    ~BuffVarWriter();

    void AutoAppend();

    Isis::BufferVariable *Var;
    boost::python::list      List;
  };

  
  // FUNCTIONS
  boost::python::dict BoostDictFromTree( const char *fpath,
					 const char *tpath,
					 boost::python::object &vars,
					 const char *cuts = 0 );
  boost::python::object BoostDictToTree( boost::python::tuple args,
					 boost::python::dict kwargs );
  boost::python::list BoostListFromTree( const char *fpath,
					 const char *tpath,
					 const char *var,
					 const char *cuts = 0 );
  boost::python::object BoostListToTree( boost::python::tuple args,
					 boost::python::dict kwargs );
  char PyTypeFromObject( boost::python::object object );

}
  
#endif
