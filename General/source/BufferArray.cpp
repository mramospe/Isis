////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  General package                                                           //
//                                                                            //
// -------------------------------------------------------------------------- //
//                                                                            //
//  AUTHOR: Miguel Ramos Pernas                                               //
//  e-mail: miguel.ramos.pernas@cern.ch                                       //
//                                                                            //
//  Last update: 28/06/2016                                                   //
//                                                                            //
// -------------------------------------------------------------------------- //
//                                                                            //
//  Description:                                                              //
//                                                                            //
//  Definition of the buffer to store different types of data in the same     //
//  vector. The variables are accessed as in a common map, using the          //
//  operator < [] > or the method < Get >. This class uses pointers to the    //
//  variables since the class BufferVariable does not support a copy          //
//  constructor. In order to modify them, one must use the operator. To add a //
//  new variable, one must use the method < AddVariable >, specifying the     //
//  name and the type.                                                        //
//                                                                            //
// -------------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "BufferArray.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Copy constructor. Memory will be allocated to generate new variables. This
// must be done to avoid creating two arrays with the same allocated memory. Also
// implies that the new array will not be linked to the old one.
General::BufferArray::BufferArray( const BufferArray &other ) :
  fVarMap( other.fVarMap ) {
  
  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    it -> second = new BuffVar( it -> second -> GetType() );
}

//_______________________________________________________________________________
// Main constructor, given the ordering function to follow. By default, the
// ordering is alphabetic.
General::BufferArray::BufferArray( bool (*func)( const std::string &stra,
						 const std::string &strb ) ) :
  fVarMap( func ) { }

//_______________________________________________________________________________
// Destructor
General::BufferArray::~BufferArray() {

  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    delete it -> second;
}

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Adds a new variable to the buffer, given the name and the type
void General::BufferArray::AddVariable( const std::string &name, const char &type ) {

  if ( fVarMap.count( name ) )
    std::cerr << "ERROR: Variable with name < " << name << " > already booked" << std::endl;
  else
    fVarMap[ name ] = new BuffVar( type );
}

//_______________________________________________________________________________
// Appends to the new vector the names of the current array
void General::BufferArray::ExtractNames( std::vector<std::string> &names ) {
  
  std::vector<std::string> vec;
  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    vec.push_back( it -> first );
}

//_______________________________________________________________________________
// Converts the values stored in this array to a string
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
