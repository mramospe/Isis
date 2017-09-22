////////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#include "Definitions.hpp"
#include "Messenger.hpp"
#include "OutputTable.hpp"
#include "Utils.hpp"

#include <iomanip>
#include <limits>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  OutputTable::OutputTable() : fNlines( 0 ), fStrLength( 2 ) {
    
    fBuffer.precision( 5 );
  }

  //_______________________________________________________________________________
  //
  OutputTable::OutputTable( const unsigned short int &prec,
			    const unsigned short int len )
    : fNlines( 0 ), fStrLength( len ) {
    
    fBuffer.precision( prec );
  }

  //_______________________________________________________________________________
  //
  OutputTable::~OutputTable() { }

  //_______________________________________________________________________________
  //
  std::string OutputTable::getLine() {
    
    std::string str, var;
    for ( auto it = fSizes.begin(); it != fSizes.end(); it++ ) {
      fBuffer >> var;
      str += centerString( var, *it );
    }
    
    return str;
  }

  //_______________________________________________________________________________
  //
  void OutputTable::display( const char &chdeco ) {
    
    std::string var, fSeparator = this->start();
    for ( unsigned short int iline = 0; iline < fNlines; iline++ ) {
      std::cout << "| ";
      auto its = fSizes.begin(), itp = fPrecisions.begin();
      while ( its != fSizes.end() ) {
	fBuffer >> var;
	if ( *itp )
	  std::cout << std::setprecision( *itp );
	std::cout << std::setw( *its ) << var << " |";
	its++;
	itp++;
      }
      std::cout << std::endl;
    }
    std::cout << fSeparator << std::endl;
  }

  //_______________________________________________________________________________
  //
  void OutputTable::setFormat( const char *format, const Strings &titles ) {
    
    fIvar = 0;
    if ( fSizes.size() )
      fSizes.clear();
    
    // Adding 1 because of the sign
    size_t
      short_size = calcIntLength( std::numeric_limits<short int>::max() + 1 ),
      int_size   = calcIntLength( std::numeric_limits<int>::max() + 1 ),
      size;
    
    const char* num;
    auto it = titles.begin();
    
    while ( *format != '\0' ) {
      fTitles.push_back( *it++ );
      fSizes.push_back( fTitles.back().size() );
      if ( *( format + 1 ) > '0' && *( format + 1 ) < '9' ) {
	size = 0;
	num  = format + 1;
	while ( *num > '0' && *num < '9' )
	  size = *num++ - '0' + 10*size;
	fPrecisions.push_back( size );
	if ( *format == 'b' ) {
	  size = 1;
	  fPrecisions.back() = 0;
	  IWarning <<
	    "Precision specification not allowed for boolean numbers"
		   << IEndMsg;
	}
	else if ( *format == 'i' )
	  fPrecisions.back() = 0;
	else if ( *format == 'I' )
	  fPrecisions.back() = 0;
	else if ( *format == 'f' )
	  // The 6 amounts from: sign (x2) + comma + exponential + argument (x2)
	  size += 6;
	else if ( *format == 'd' )
	  // The 7 amounts from: sign (x2) + comma + exponential + argument (x3)
	  size += 7;
	else if ( *format == 's' )
	  fPrecisions.back() = 0;
	else
	  IError << "Unknown variable type < " << *format << " > in table" << IEndMsg;
	format += 2;
      }
      else {
	fPrecisions.push_back( 0 );
	if ( *format == 'b' )
	  size = 1;
	else if ( *format == 'i' )
	  size = short_size;
	else if ( *format == 'I' )
	  size = int_size;
	else if ( *format == 'f' ) {
	  // The 6 amounts from: sign (x2) + comma + exponential + argument (x2)
	  size = fBuffer.precision() + 6;
	  fPrecisions.back() = fBuffer.precision();
	}
	else if ( *format == 'd' ) {
	  // The 7 amounts from: sign (x2) + comma + exponential + argument (x3)
	  size = fBuffer.precision() + 7;
	  fPrecisions.back() = fBuffer.precision();
	}
	else if ( *format == 's' )
	  size = fStrLength;
	else
	  IError << "Unknown variable type < " << *format << " > in table" << IEndMsg;
	format++;
      }
      if ( size > fSizes.back() )
	fSizes.back() = size;
    }
  }

  //_______________________________________________________________________________
  //
  void OutputTable::setFormat( const char *format ... ) {
    
    Strings titles;
    va_list args;
    
    va_start( args, format );
    const char *cptr = format;
    
    while ( *cptr != '\0' ) {
      titles.push_back( va_arg( args, const char* ) );
      cptr++;
    }
    
    this->setFormat( format, titles );
  }

  //_______________________________________________________________________________
  //
  std::string OutputTable::start( const char &chdeco ) {
    
    // Displays the title
    fDecoChar = chdeco;
    if ( fBuffer.tellg() )
      fBuffer.seekg( 0 );
    unsigned short int lsize = 2;
    for ( auto it = fSizes.begin(); it != fSizes.end(); it++ )
      lsize += *it + 2;
    fSeparator = std::string( lsize, chdeco );
    std::cout << fSeparator << std::endl;
    std::cout << std::right << "| ";
    
    // Displays the elements of the table
    auto itt = fTitles.begin();
    auto its = fSizes.begin();

    while( its != fSizes.end() )
      std::cout << centerString( *itt++, *its++ ) << " |";
    std::cout << '\n' << fSeparator << std::endl;
    return fSeparator;
  }

}
