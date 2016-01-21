////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//  General package                                                               //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  AUTHOR: Miguel Ramos Pernas                                                   //
//  e-mail: miguel.ramos.pernas@cern.ch                                           //
//                                                                                //
//  Last update: 21/01/2016                                                       //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  Description:                                                                  //
//                                                                                //
//  Implements the class to generate tables in c++ output. It allows both to      //
//  construct the table and then print it, or print the lines manually. Do not    //
//  forget to rewind the class before making another print. The precision of      //
//  the numbers displayed, as well as the length of the strings can be modified.  //
//  The characters decorating the table can be modified too when calling the      //
//  methods < Print > or < Start >. The inputs can be performed wih the < << >    //
//  operator or with the < AppendStream > method. Calling the operator < >> >     //
//  one can get the elements one by one from the buffer. In this case the user    //
//  has to control the number of times that has to call this operator.            //
//                                                                                //
// ------------------------------------------------------------------------------ //
////////////////////////////////////////////////////////////////////////////////////


#include "OutputTable.h"
#include "Utils.h"

#include <iomanip>
#include <limits>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::OutputTable::OutputTable() : fNlines( 0 ), fStrLength( 2 ) {
  fBuffer.precision( 5 );
}

//_______________________________________________________________________________
// Constructor given the number and string format information
General::OutputTable::OutputTable( const unsigned short int &prec,
				   const unsigned short int len )
  : fNlines( 0 ), fStrLength( len ) {
  fBuffer.precision( prec );
}

//_______________________________________________________________________________
// Destructor
General::OutputTable::~OutputTable() { }

//_______________________________________________________________________________
// This static method allows to center a string in a place with width equal to
// < size >. In case of working with odd sizes the right hand will be always
// greater.
std::string General::OutputTable::CenteredString( const std::string &str, const size_t &size ) {
  unsigned short int
    rst = size - str.size(),
    coc = rst / 2;
  std::string output( coc + rst % 2, ' ' );
  output += str;
  output += std::string( coc, ' ' );
  return output;
}

//_______________________________________________________________________________
// Returns a new line from the buffer
std::string General::OutputTable::GetLine() {
  std::string str, var;
  for ( auto it = fSizes.begin(); it != fSizes.end(); it++ ) {
    fBuffer >> var;
    str += this -> CenteredString( var, *it );
  }
  return str;
}

//_______________________________________________________________________________
// Displays the whole table, including the title and the elements. If this method
// is used no < Start > has to be called before.
void General::OutputTable::Print( const char &chdeco ) {
  std::string var, fSeparator = this -> Start();
  for ( unsigned short int iline = 0; iline < fNlines; iline++ ) {
    for ( auto it = fSizes.begin(); it != fSizes.end(); it++ ) {
      fBuffer >> var;
      std::cout << this -> CenteredString( var, *it );
    }
    std::cout << std::endl;
  }
  std::cout << fSeparator << std::endl;
}

//_______________________________________________________________________________
// Sets the format of the elements displayed in the table. There can be displayed
// values of any kind: boolean (b), short integers (i), integers (I), floats (f),
// doubles (d) and strings (s).
void General::OutputTable::SetFormat( const char *format ... ) {
  fIvar = 0;
  if ( fSizes.size() )
    fSizes.clear();
  // Adding 1 because of the sign
  size_t
    short_size = General::CalcIntLength( std::numeric_limits<short int>::max() + 1 ),
    int_size   = General::CalcIntLength( std::numeric_limits<int>::max() + 1 ),
    size;
  va_list args;
  va_start( args, format );
  while ( *format != '\0' ) {
    fTitles.push_back( va_arg( args, const char* ) );
    fSizes.push_back( fTitles.back().size() );
    if ( *format == 'b' )
      size = 1;
    else if ( *format == 'i' )
      size = short_size;
    else if ( *format == 'I' )
      size = int_size;
    else if ( *format == 'f' )
      // The 6 amounts from: sign (x2) + comma + exponential + argument (x2)
      size = fBuffer.precision() + 6;
    else if ( *format == 'd' )
      // The 7 amounts from: sign (x2) + comma + exponential + argument (x3)
      size = fBuffer.precision() + 7;
    else if ( *format == 's' )
      size = fStrLength;
    else
      std::cerr << "ERROR: Unknown variable type < " << *format << " > in table" << std::endl;
    if ( size > fSizes.back() )
      fSizes.back() = size;
    format++;
    // This is the number of whitespaces that will be distributed to both sides of the elements
    fSizes.back() += 4;
  }
}

//_______________________________________________________________________________
// Prints the title and initializes the table. If the pointer of the buffer is
// not located at the start it is moved to it. Returns the string used to
// separate the title from the elements.
std::string General::OutputTable::Start( const char &chdeco ) {
  // Displays the title
  fDecoChar = chdeco;
  if ( fBuffer.tellg() )
    fBuffer.seekg( 0 );
  unsigned short int lsize = 0;
  for ( auto it = fSizes.begin(); it != fSizes.end(); it++ )
    lsize += *it;
  fSeparator = std::string( lsize, chdeco );
  std::cout << fSeparator << std::endl;
  std::cout << std::left;
  // Displays the elements of the table
  auto itt = fTitles.begin();
  auto its = fSizes.begin();
  while( its != fSizes.end() )
    std::cout << this -> CenteredString( *itt++, *its++ );
  std::cout << '\n' << fSeparator << std::endl;
  return fSeparator;
}
