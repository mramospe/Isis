////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -----------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
//
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "BufferArray.h"
#include "Messenger.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
//
General::BufferArray::BufferArray( const BufferArray &other ) :
  fVarMap( other.fVarMap ) {
  
  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    it -> second = new BuffVar( it -> second -> GetType() );
}

//_______________________________________________________________________________
//
General::BufferArray::BufferArray( bool (*func)( const std::string &stra,
						 const std::string &strb ) ) :
  fVarMap( func ) { }

//_______________________________________________________________________________
//
General::BufferArray::~BufferArray() {

  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    delete it -> second;
}

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
//
General::BufferVariable* General::BufferArray::AddVariable( const std::string &name, const char &type ) {

  if ( fVarMap.count( name ) )
    Error << "ERROR: Variable with name < " << name << " > already booked" << EndMsg;
  else {
    General::BufferVariable *var = new BuffVar( type );
    fVarMap[ name ] = var;
    return var;
  }
  return 0;
}

//_______________________________________________________________________________
//
void General::BufferArray::ExtractNames( std::vector<std::string> &names ) {
  
  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    names.push_back( it -> first );
}

//_______________________________________________________________________________
//
std::string General::BufferArray::ToString() const {

  std::string res;

  auto itlast = fVarMap.end();
  --itlast;
  
  for ( auto it = fVarMap.begin(); it != itlast; ++it ) {
    res += (it -> second) -> ToString() + '\t';
  }
  res += fVarMap.rbegin() -> second -> ToString();

  return res;
}
