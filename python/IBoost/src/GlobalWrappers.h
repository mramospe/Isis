//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  General functions to help on the generation of python wrappers.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef GLOBAL_WRAPPERS
#define GLOBAL_WRAPPERS

#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/object_operators.hpp>

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <vector>


//_______________________________________________________________________________

namespace IBoost {

  // _________
  // FUNCTIONS
  boost::python::ssize_t boostDictListSize( boost::python::dict dict );
  void checkArgs( boost::python::tuple &args, const unsigned int &nargs );
  void checkKwargs( boost::python::dict &kwargs,
		    const std::vector<const char*> &lst );

  // __________________
  // TEMPLATE FUNCTIONS

  // Extract the converted value from a python container at the given index
  template<class type, class idxtype>
  inline type extractFromIndex( boost::python::object &obj,
			 idxtype index ) {
    boost::python::api::object_item elem = obj[ index ];
    type output = boost::python::extract<type>( elem );
    return output;
  }

  // Transform a python list into a standard vector
  template<class type>
  inline std::vector<type> boostListToStdVec( boost::python::list &lst ) {

    size_t lgth = boost::python::len( lst );
    std::vector<type> res( lgth );

    size_t i = 0;
    for (auto it = res.begin(); it != res.end(); ++it, ++i)
      (*it) = extractFromIndex<type>(lst, i);

    return res;
  }

  // Transform a standard vector into a python list
  template<class type>
  inline boost::python::list stdVecToBoostList( const std::vector<type> &vector ) {
    boost::python::list list;
    for ( auto it = vector.begin(); it != vector.end(); ++it )
      list.append( *it );
    return list;
  }
}

#endif
