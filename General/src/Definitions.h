//////////////////////////////////////////////////////////////////////////////////
//
//  General package
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
//  General definitions and indications for the pre-processor
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef __DEFINITIONS__
#define __DEFINITIONS__

#include <map>
#include <string>
#include <vector>


//_______________________________________________________________________________
// The Isis version, set by cmake at build time
#define ISIS_VERSION "${PROJECT_VERSION}"

//_______________________________________________________________________________
// No-operation statement
#define NOOP

namespace Isis {
  
  //_______________________________________________________________________________
  // Define a vector of doubles
  using Doubles = std::vector<double>;

  //_______________________________________________________________________________
  // Define a vector of strings
  using Strings = std::vector<std::string>;

  //_______________________________________________________________________________
  // Define maps with strings as keys and values
  using StrMap = std::map<std::string, std::string>;
  
}

#endif
