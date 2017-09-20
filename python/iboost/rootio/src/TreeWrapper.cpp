///////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// ----------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/09/2017
//
// ----------------------------------------------------------------
///////////////////////////////////////////////////////////////////


#include "GlobalWrappers.h"
#include "NumpyUtils.h"
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
#include "Definitions.h"
#include "Messenger.h"
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


//_______________________________________________________________

namespace iboost {

  //_______________________________________________________________
  //
  np::ndarray treeToNumpyArray( std::string fpath,
				std::string tpath,
				py::object &vars,
				std::string cuts,
				bool use_regex ) {

    // Open the Root file and access the tree
    TFile *ifile = TFile::Open(fpath.c_str());
    TTree *itree = static_cast<TTree*>(isis::getSafeObject(ifile, tpath));

    // Extracts the list of events to be used
    itree->Draw(">>evtlist", cuts.c_str());
    TEventList *evtlist = (TEventList*) gDirectory->Get("evtlist");

    // Get the branch names to be used
    itree->SetBranchStatus("*", 0);

    isis::Strings brnames;
    const py::ssize_t nvars = py::len(vars);
    for ( py::ssize_t i = 0; i < nvars; ++i ) {

      auto var = extractFromIndex<std::string>(vars, i);

      bool s = true;
      if ( use_regex )
	s = isis::getBranchNames(brnames, itree, var);
      else {
	if ( itree->GetBranch(var.c_str()) )
	    brnames.push_back(var);
	else
	  s = false;
      }
      
      if ( !s )
	IWarning << "No variables have been found following expression < "
		 << var << " >" << IEndMsg;
    }
    
    // Do not swap these two lines, since the path must be set after the
    // variable is enabled
    isis::TreeBuffer buffer(itree);
    for ( const auto br : brnames ) {
      
      itree->SetBranchStatus(br.c_str(), 1);
      
      buffer.loadVariable(br);
    }
    
    // Make the output array
    auto output = void_ndarray(evtlist->GetN(), buffer);
    
    // Build the writers
    std::vector<BuffVarWriter*> outvec;
    size_t n = brnames.size();
    outvec.reserve(n);
    for ( const auto el: buffer.getMap() ) {

      np::ndarray a = py::extract<np::ndarray>(output[el.first]);
      
      outvec.push_back(new BuffVarWriter(el.second, a));
    }
    
    // Calling to < append > is faster than assigning by index
    for ( int i = 0; i < evtlist->GetN(); ++i ) {

      Long64_t ievt = evtlist->GetEntry(i);
      itree->GetEntry(ievt);
      
      for ( auto &el : outvec )
	el->appendToArray(i, n);
    }

    // The branches are enabled again
    itree->SetBranchStatus("*", 1);
    ifile->Close();

    // Delete allocated memory
    for ( auto &v : outvec )
      delete v;
    
    return output;
  }

  //_______________________________________________________________
  //
  py::object numpyArrayToTree( py::tuple args, py::dict kwargs ) {

    checkArgs(args, 1);
    checkKwargs(kwargs, {"name", "tree", "variables", "use_regex"});
    
    np::ndarray vartup = extractFromIndex<np::ndarray>(args, 0);
    py::list varkeys   = struct_array_keys(vartup);
    
    // Get the tree name or the given tree
    TTree *tree = 0;
    if ( kwargs.has_key(py::object("tree")) ) {

      py::object el = kwargs["tree"];

      TObject *treeproxy =
	(TObject*) TPython::ObjectProxy_AsVoidPtr(el.ptr());
      
      tree = dynamic_cast<TTree*>(treeproxy);
    }
    
    std::string tname;
    if ( kwargs.has_key("name") )
      tname = py::extract<std::string>(kwargs["name"]);

    // Get the list of variables to work with
    py::list variables;
    if ( kwargs.has_key("variables") ) {
      variables = py::extract<py::list>(kwargs["variables"]);
      if ( !py::len(variables) )
	variables = varkeys;
    }
    else
      variables = varkeys;

    // Determine whether regex will be used or not
    bool use_regex = false;
    if ( kwargs.has_key("use_regex") )
      use_regex = py::extract<bool>(kwargs["use_regex"]);
  
    // Warning message if both the tree and tree name are provided
    if ( !tree ) {
      tree = new TTree(tname.c_str(), tname.c_str(), 0);
      tree->AutoSave();
    }
    else if ( !tname.empty() )
      IWarning << "The given tree name is not being used" << IEndMsg;
  
    // Prepare the variables to iterate with. The vector keeps
    // track of the types for each variable.
    isis::TreeBuffer buffer(tree);

    auto vars = boostListToStdCont<std::vector, std::string>(variables);
    auto vec_keys = boostListToStdCont<std::vector, std::string>(varkeys);

    // Get the variables from the given expressions
    isis::Strings brnames;
    for ( const auto &exp : vars ) {

      if ( use_regex )
	isis::stringVectorFilter(brnames, vec_keys, exp);
      else
	brnames.push_back(exp);
    }

    std::vector<BuffVarWriter*> varvec;
    varvec.reserve(brnames.size());
    for ( const auto &br : brnames )
      varvec.push_back(new BuffVarWriter( buffer, br,
					  py::extract<np::ndarray>(vartup[br]))
		       );
    
    // Loop over all the events in the dictionary
    py::ssize_t nvals = py::len(vartup);
    for ( py::ssize_t ievt = 0; ievt < nvals; ++ievt ) {

      // Loop over all the variables in the dictionary
      const size_t n = varvec.size();
      for ( auto &v : varvec )
	v->appendToVar(ievt, n);
    
      tree->Fill();
    }
    tree->AutoSave();

    // Delete the allocated memory
    for ( auto &v : varvec )
      delete v;
    
    // Returns "None"
    return py::object();
  }
  
}
