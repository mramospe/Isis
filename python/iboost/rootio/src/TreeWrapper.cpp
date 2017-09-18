//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 18/09/2017
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "GlobalWrappers.h"
#include "Messenger.h"
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
#include "RootUtils.h"
#include "TreeBuffer.h"
#include "TreeManagement.h"
#include "Utils.h"

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


//_______________________________________________________________________________

namespace iboost {

  //_______________________________________________________________________________
  //
  py::dict treeToBoostDict( std::string fpath,
			    std::string tpath,
			    py::object &vars,
			    std::string cuts,
			    bool use_regex ) {
  
    TFile *ifile = TFile::Open(fpath.c_str());
    TTree *itree = static_cast<TTree*>(isis::getSafeObject(ifile, tpath));

    // Extracts the list of events to be used
    itree->Draw(">>evtlist", cuts.c_str());
    TEventList *evtlist = (TEventList*) gDirectory->Get("evtlist");

    size_t nentries = evtlist->GetN();
  
    itree->SetBranchStatus("*", 0);
  
    isis::TreeBuffer buffer(itree);
    const py::ssize_t nvars = py::len(vars);
    std::map<std::string, BuffVarWriter*> outmap;
    for ( py::ssize_t i = 0; i < nvars; ++i ) {

      auto var = extractFromIndex<std::string>(vars, i);

      isis::Strings brnames;
      
      if ( use_regex )
	// Get the variables from the given expressions
	isis::getBranchNames(brnames, itree, var);
      else
	if ( itree->GetBranch(var.c_str()) )
	    brnames.push_back(var);
      
      if ( !brnames.size() )
	IWarning << "No variables have been found following expression < "
		 << var << " >" << IEndMsg;

      // Do not swap these two lines, since the path must be set after the
      // variable is enabled
      for ( const auto br : brnames ) {
      
	itree->SetBranchStatus(br.c_str(), 1);
      
	isis::BufferVariable *bvar = buffer.loadVariable(br);
	outmap[br] = new BuffVarWriter(nentries, bvar);
      }
    }
  
    // Calling to < append > is faster than assigning by index
    size_t counter = 0;
    for ( int ievt = 0; ievt < evtlist->GetN(); ++ievt, ++counter ) {

      Long64_t tievt = evtlist->GetEntry( ievt );
      itree->GetEntry( tievt );
    
      for ( auto &el : outmap )
	el.second->appendToArray(counter);
    }

    // Build the output dictionary
    py::dict output;
    for ( auto &el : outmap ) {
      output[ el.first ] = el.second->getArray();
      delete el.second;
    }

    // The branches are enabled again
    itree->SetBranchStatus("*", 1);
    ifile->Close();
  
    return output;
  }

  //_______________________________________________________________________________
  //
  py::object boostDictToTree( py::tuple args, py::dict kwargs ) {

    checkArgs(args, 1);
    checkKwargs(kwargs, {"name", "tree", "variables"});
  
    py::dict vardict = extractFromIndex<py::dict>(args, 0);
    py::list varkeys = vardict.keys();

    // Get the tree name or the given tree
    TTree *tree = 0;
    if ( kwargs.has_key( py::object("tree") ) ) {

      py::object el = kwargs["tree"];

      TObject *treeproxy =
	(TObject*) TPython::ObjectProxy_AsVoidPtr( el.ptr() );
      
      tree = dynamic_cast<TTree*>( treeproxy );
    }
    std::string tname;
    if ( kwargs.has_key("name") )
      tname = py::extract<std::string>( kwargs["name"] );
    py::list variables;
    if ( kwargs.has_key("variables") ) {
      variables = py::extract<py::list>( kwargs["variables"] );
      if ( !py::len( variables ) )
	variables = varkeys;
    }
    else
      variables = varkeys;
  
    // Warning message if both the tree and tree name are provided
    if ( !tree ) {
      tree = new TTree(tname.c_str(), tname.c_str(), 0);
      tree->AutoSave();
    }
    else if ( !tname.empty() )
      IWarning << "The given tree name is not being used" << IEndMsg;
  
    // Prepare the variables to iterate with. The vector keeps
    // track of the types for each variable.
    isis::TreeBuffer buffer( tree );

    auto vars = boostListToStdCont<std::vector, std::string>(variables);
    auto vec_keys = boostListToStdCont<std::vector, std::string>(varkeys);

    std::map<std::string, BuffVarWriter*> varmap;
  
    for ( const auto &exp : vars ) {

      // Get the variables from the given expressions
      isis::Strings brnames;
      isis::stringVectorFilter(brnames, vec_keys, exp);

      for ( const auto &br : brnames )
	varmap[br] =
	  new BuffVarWriter( buffer, br,
			     py::extract<np::ndarray>(vardict[br]));
    }
    
    // Loop over all the events in the dictionary
    py::ssize_t nvals = boostDictListSize(vardict);
    for ( py::ssize_t ievt = 0; ievt < nvals; ++ievt ) {

      // Loop over all the variables in the dictionary
      for ( auto &el : varmap )
	el.second->appendToVar(ievt);
    
      tree->Fill();
    }
    tree->AutoSave();

    // Delete the allocated memory
    for ( auto &el : varmap )
      delete el.second;

    // Returns void (None)
    return py::object();
  }

  //_______________________________________________________________________________
  //
  np::ndarray treeToNumpyArray( std::string fpath,
				std::string tpath,
				std::string var,
				std::string cuts ) {
    py::list varlist;
    varlist.append(var);
    py::dict dict = treeToBoostDict(fpath, tpath, varlist, cuts);
    return py::extract<np::ndarray>(dict[var]);
  }

  //_______________________________________________________________________________
  //
  py::object numpyArrayToTree( py::tuple args, py::dict kwargs ) {

    checkArgs(args, 2);
    // The check on the kwargs is already made by BoostDictToTree
  
    std::string var = extractFromIndex<std::string>(args, 0);
    py::list values = extractFromIndex<py::list>(args, 1);

    py::dict dict;
    dict[var] = values;
  
    return boostDictToTree(py::make_tuple( dict ), kwargs);
  }

}
