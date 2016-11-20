//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/11/2016
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Functions to convert python dictionaries and lists on branches in a Root
//  tree. The output is saved on the current directory.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "CppToPyTypes.h"
#include "GlobalWrappers.h"
#include "TreeWrapper.h"

#include <Python.h>
#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/object_operators.hpp>
#include <boost/python/tuple.hpp>

#include "BufferVariable.h"
#include "TreeBuffer.h"

#include "TBranch.h"
#include "TDirectory.h"
#include "TEventList.h"
#include "TFile.h"
#include "TObject.h"
#include "TPython.h"
#include "TTree.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace py = boost::python;


//_______________________________________________________________________________
// Simple struct class to read from a BufferVariable object and append the ouput
// to a list

// Constructor
IBoost::BuffVarWriter::BuffVarWriter( General::BufferVariable *var ) : Var( var ) { }
// Constructor given the variable and a list
IBoost::BuffVarWriter::BuffVarWriter( General::BufferVariable *var, py::list lst ) : Var( var ), List( lst ) { }
// Destructor
IBoost::BuffVarWriter::~BuffVarWriter() {  }
// Appends the current value stored in the BufferVariable object to the python list
inline void IBoost::BuffVarWriter::AutoAppend() {
  List.append( IBoost::BoostObjectFromBufferVariable( *Var ) );
}

//_______________________________________________________________________________
// Store in a dictionary the lists with the values for each of the given
// variables stored in a Root tree. A set of cuts can be specified.
py::dict IBoost::BoostDictFromTree( const char *fpath,
				    const char *tpath,
				    py::object &vars,
				    const char *cuts ) {
  
  TFile *ifile = TFile::Open( fpath );
  TTree *itree = IBoost::GetRootTree( ifile, tpath );

  // Extracts the list of events to be used
  itree->Draw(">>evtlist", cuts);
  TEventList *evtlist = (TEventList*) gDirectory->Get("evtlist");
  
  itree->SetBranchStatus("*", 0);
  
  Analysis::TreeBuffer buffer( itree );
  const py::ssize_t nvars = py::len( vars );
  std::map<const char*, IBoost::BuffVarWriter*> outmap;
  for ( py::ssize_t i = 0; i < nvars; ++i ) {

    auto var = IBoost::ExtractFromIndex<const char*>( vars, i );

    // Do not swap these two lines, since the path must be set after the
    // variable is enabled
    itree->SetBranchStatus(var, 1);
    General::BufferVariable *bvar = buffer.LoadVariable( var );

    outmap[ var ] = new IBoost::BuffVarWriter( bvar );
  }

  // Calling to < append > is faster than assigning by index
  for ( int ievt = 0; ievt < evtlist->GetN(); ++ievt ) {

    Long64_t tievt = evtlist->GetEntry( ievt );
    itree->GetEntry( tievt );
    
    for ( auto it = outmap.begin(); it != outmap.end(); ++it )
      it->second->AutoAppend();
  }

  // Build the output dictionary
  py::dict output;
  for ( auto it = outmap.begin(); it != outmap.end(); ++it ) {
    output[ it->first ] = it->second->List;
    delete it->second;
  }

  // The branches are enabled again
  itree->SetBranchStatus("*", 1);
  ifile->Close();
  
  return output;
}

//_______________________________________________________________________________
// Write a python dictionary to a Root tree. Since in python there are only four
// numeric types: bool, int, long and float; only the associated c++ types
// are used.
py::object IBoost::BoostDictToTree( py::tuple args, py::dict kwargs ) {

  IBoost::CheckArgs(args, 1);
  IBoost::CheckKwargs(kwargs, {"name", "tree"});
  
  py::dict vardict = IBoost::ExtractFromIndex<py::dict>(args, 0);

  // Get the tree name or the given tree
  TTree *tree = 0;
  if ( kwargs.has_key( py::object("tree") ) ) {
    py::object el = kwargs["tree"];
    TObject *treeproxy  = (TObject*) TPython::ObjectProxy_AsVoidPtr( el.ptr() );
    tree = dynamic_cast<TTree*>( treeproxy );
  }
  const char *tname = 0;
  if ( kwargs.has_key("name") )
    tname = py::extract<const char*>( kwargs["name"] );

  // Warning message if both the tree and tree name are provided
  if ( !tree ) {
    tree = new TTree(tname, tname, 0);
    tree->AutoSave();
  }
  else if ( tname )
    std::cout << "WARNING: The given tree name is not being used" << std::endl;
  
  // Prepare the variables to iterate with. The vector keeps
  // track of the types for each variable.
  Analysis::TreeBuffer buffer( tree );

  py::list vars = vardict.keys();
  py::ssize_t nvars = py::len( vars );

  std::map<const char*, IBoost::BuffVarWriter*> varmap;

  for ( py::ssize_t i = 0; i < nvars; ++i ) {
    const char *var   = IBoost::ExtractFromIndex<const char*>(vars, i);
    char type         = IBoost::PyTypeFromObject( vardict[ var ][ 0 ] );
    
    General::BufferVariable *buffvar = buffer.CreateVariable( var, type );
    varmap[ var ] =
      new IBoost::BuffVarWriter( buffvar, py::extract<py::list>( vardict[ var ] ) );
  }
  
  // Loop over all the events in the dictionary
  py::ssize_t nvals = IBoost::BoostDictListSize( vardict );
  for ( py::ssize_t ievt = 0; ievt < nvals; ++ievt ) {

    // Loop over all the variables in the dictionary
    for ( auto it = varmap.begin(); it != varmap.end(); ++it ) {

      py::object value = it->second->List[ ievt ];
      char type = it->second->Var->GetType();
    
      switch ( type ) {
      case 'D':
	it->second->Var->SetValue( py::extract<double>( value ) );
	break;
      case 'L':
	it->second->Var->SetValue( py::extract<long long int>( value ) );
	break;
      case 'I':
	it->second->Var->SetValue( py::extract<int>( value ) );
	break;
      case 'O':
	it->second->Var->SetValue( py::extract<bool>( value ) );
	break;
      }
    }
    
    tree->Fill();
  }
  tree->AutoSave();

  // Delete the allocated memory
  for ( auto it = varmap.begin(); it != varmap.end(); ++it )
    delete it->second;

  // Returns void (None)
  return py::object();
}

//_______________________________________________________________________________
// Store in a list the values for a variable present in a Root tree
py::list IBoost::BoostListFromTree( const char *fpath,
				    const char *tpath,
				    const char *var,
				    const char *cuts ) {
  py::list varlist;
  varlist.append( var );
  py::dict dict = IBoost::BoostDictFromTree(fpath, tpath, varlist, cuts);
  return py::extract<py::list>( dict[ var ] );
}

//_______________________________________________________________________________
// Create/update a Root tree adding a variable with the given list of values. To
// see the possible < kwargs > arguments see < BoostDictToTree >.
py::object IBoost::BoostListToTree( py::tuple args, py::dict kwargs ) {

  IBoost::CheckArgs(args, 2);
  // The check on the kwargs is already made by BoostDictToTree
  
  const char *var = IBoost::ExtractFromIndex<const char*>(args, 0);
  py::list values = IBoost::ExtractFromIndex<py::list>(args, 1);

  py::dict dict;
  dict[ var ] = values;
  
  return IBoost::BoostDictToTree( py::make_tuple( dict ), kwargs );
}

//_______________________________________________________________________________
// Function to get a Root tree and send an error if no tree is found with the
// given path
TTree* IBoost::GetRootTree( TFile *ifile, const char *tpath ) {

  TTree *itree = (TTree*) ifile->Get( tpath );

  if ( !itree )
    std::cerr << "ERROR: No tree named < " << tpath
	      << " > in file < " << ifile->GetTitle()
	      << " >" << std::endl;
  
  return itree;
}

//_______________________________________________________________________________
// Returns the character corresponding to the python type associated with the
// given object
char IBoost::PyTypeFromObject( py::object object ) {

  const PyObject *po = object.ptr();
  
  if ( PyFloat_Check( po ) )
    return 'D';
  else if ( PyLong_Check( po ) )
    return 'L';
  else if ( PyInt_Check( po ) )
    return 'I';
  else if ( PyBool_Check( po ) )
    return 'O';
  else {
    std::cerr << "ERROR: Invalid numeric data type" << std::endl;
    return '\0';
  }
}
