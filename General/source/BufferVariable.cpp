//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 26/08/2016
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Main class to store any kind of primitive data types. Different objects of
//  this type can be stored in the same container, while refering to different
//  data types. It results very useful when dealing with buffers were each
//  variable has a different type.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "BufferVariable.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Constructor given the type as a character
General::BufferVariable::BufferVariable( const char &type ) : fType( type ) {
  this -> Construct();
}

//_______________________________________________________________________________
// Destructor
General::BufferVariable::~BufferVariable() { this -> Delete(); }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Converts this class to a string
std::string General::BufferVariable::ToString() const {
  switch( fType ) {
  case 'B': return std::string( 1, *static_cast<char*>                   ( fPath ) );
  case 'b': return std::string( 1, *static_cast<unsigned char*>          ( fPath ) );
  case 'S': return std::to_string( *static_cast<short int*>              ( fPath ) );
  case 's': return std::to_string( *static_cast<unsigned short int*>     ( fPath ) );
  case 'I': return std::to_string( *static_cast<int*>                    ( fPath ) );
  case 'i': return std::to_string( *static_cast<unsigned int*>           ( fPath ) );
  case 'F': return std::to_string( *static_cast<float*>                  ( fPath ) );
  case 'D': return std::to_string( *static_cast<double*>                 ( fPath ) );
  case 'L': return std::to_string( *static_cast<long long int*>          ( fPath ) );
  case 'l': return std::to_string( *static_cast<unsigned long long int*> ( fPath ) );
  case 'O': return std::to_string( *static_cast<bool*>                   ( fPath ) );
  default :
    std::cerr << "ERROR: The type of the buffer variable has not been specified yet" << std::endl;
    return std::string();
  }
}

//_______________________________________________________________________________


// -- PRIVATE METHODS

//_______________________________________________________________________________
// Method to construct this class, given the type
void General::BufferVariable::Construct() {
  switch( fType ) {
  case 'B' : fPath = new char;                   break;
  case 'b' : fPath = new unsigned char;          break;
  case 'S' : fPath = new short int;              break;
  case 's' : fPath = new unsigned short int;     break;
  case 'I' : fPath = new int;                    break;
  case 'i' : fPath = new unsigned int;           break;
  case 'F' : fPath = new float;                  break;
  case 'D' : fPath = new double;                 break;
  case 'L' : fPath = new long long int;          break;
  case 'l' : fPath = new unsigned long long int; break;
  case 'O' : fPath = new bool;                   break;
  case '\0': fPath = 0;                          break;
  default:
    std::cerr << "ERROR: Unknown type for buffer variable < " << fType << " >" << std::endl;
    break;
  }
}

//_______________________________________________________________________________
// Method to delete the value stored
void General::BufferVariable::Delete() {
  if ( fPath )
    switch( fType ) {
    case 'B': delete static_cast<char*>                   ( fPath ); break;
    case 'b': delete static_cast<unsigned char*>          ( fPath ); break;
    case 'S': delete static_cast<short int*>              ( fPath ); break;
    case 's': delete static_cast<unsigned short int*>     ( fPath ); break;
    case 'I': delete static_cast<int*>                    ( fPath ); break;
    case 'i': delete static_cast<unsigned int*>           ( fPath ); break;
    case 'F': delete static_cast<float*>                  ( fPath ); break;
    case 'D': delete static_cast<double*>                 ( fPath ); break;
    case 'L': delete static_cast<long long int*>          ( fPath ); break;
    case 'l': delete static_cast<unsigned long long int*> ( fPath ); break;
    case 'O': delete static_cast<bool*>                   ( fPath ); break;
    }
}


//_______________________________________________________________________________


// -- OPERATOR

//_______________________________________________________________________________
// Definition of the stream operator
std::ostream& General::operator << ( std::ostream &os, const General::BufferVariable &var ) {
  os << var.ToString();
  return os;
}
