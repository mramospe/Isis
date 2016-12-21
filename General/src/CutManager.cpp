///////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 05/04/2016
//
// --------------------------------------------------------
//
//  Description:
//
//  Class to storage cuts present in a cuts-file. This
//  files have to be written in the way:
//
//  cut1 = j > 0 and b > 1
//
//  cut2 = $cut1$ or c == 0
//
//  The symbol < $ > before and after a word means that
//  is a cut, so the class is going to get that and
//  place it instead. This class is going to search
//  for the given keys, so one can comment anything as
//  long as no key has the same string value as it,
//  being situated at the beginning of the line.
//
// --------------------------------------------------------
///////////////////////////////////////////////////////////


#include "CutManager.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::CutManager::CutManager( const std::string &file_path ) :
  fFile( new std::ifstream ), fOptions( { {"and", "&&"}, {"or", "||"} } ) {
  if ( file_path.size() )
    this -> Open( file_path );
}

//_______________________________________________________________________________
// Copy constructor
General::CutManager::CutManager( const General::CutManager &other ) {
  fCuts    = other.fCuts;
  fFile    = other.fFile;
  fOptions = other.fOptions;
}

//_______________________________________________________________________________
// Destructor
General::CutManager::~CutManager() {
  if ( fFile.use_count() == 1 && (*fFile).is_open() )
    (*fFile).close();
}

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Almacenates a new cut from the cuts-file
std::string General::CutManager::BookCut( const std::string &key, const bool &print ) {
  if ( fCuts.find( key ) != fCuts.end() ) {
    std::cout << "Cut with name < " << key << " > already booked" << std::endl;
    return "";
  }
  std::string cut = this -> GetCut( key );
  if ( cut.size() ) {
    fCuts.insert( std::make_pair( key, cut ) );
    if ( print )
      std::cout << "Booked new cut < " << key << " >: " << fCuts[ key ] << std::endl;
  }
  return cut;
}

//_______________________________________________________________________________       
// Functions that gets a cut from the cuts-file
std::string General::CutManager::GetCut( const std::string &key ) {

  // Each time a cut is obtained the pointer to the file is set to its start
  (*fFile).clear();
  (*fFile).seekg( 0 );

  std::string cuts, newcut, sstr;
  std::string::size_type pos, ifirst, ilast, fpos;
  bool cond( true ), mismatch;

  while ( cond && std::getline( *fFile, cuts ) ) {

    // Checks whether the line is empty
    if ( !cuts.empty() ) {

      // If there are whitespaces before the keyword, they are omitted
      while ( cuts[ 0 ] == ' ' ) cuts.erase( 0, 1 );

      if ( !cuts.compare( 0, key.size(), key ) ) {

	cuts = cuts.substr( cuts.find( '=' ) + 1 );

	while ( cuts[ 0 ] == ' ' ) cuts.erase( 0, 1 );

	// If another cut is found it is replaced by its content. The position at the file
	// is saved and retrieved, in order to properly scan the file.
	while ( ( ifirst = cuts.find( '$' ) ) != std::string::npos ) {

	  mismatch = false;
	  ilast    = cuts.find( '$', ifirst + 1 );

	  if ( ilast == std::string::npos ) {
	    mismatch = true;
	    sstr.clear();
	  }
	  else {
	    sstr = cuts.substr( ifirst + 1, ilast - ifirst - 1 );
	    if ( sstr.find( ' ' ) != std::string::npos )
	      mismatch = true;
	  }
	  
	  if ( mismatch ) {
	    std::cerr <<
	      "ERROR: Mismatched < $ > symbol when scanning cut < "
		      << key << " >" << std::endl;
	    return std::string();
	  }

	  // Replaces the cut name by its value
	  fpos  = (*fFile).tellg();
	  newcut = this -> GetCut( sstr );	  
	  cuts.replace( ifirst, ilast + 1 - ifirst, newcut );
	  (*fFile).seekg( fpos );
	}

	cond = false;
      }
    }

    if ( (*fFile).eof() && cond ) {
      std::cerr << "ERROR: Cut with name < " << key << " > does not exist." << std::endl;
      return 0;
    }
  }
  // Changes the name of the conditionals in the cuts set
  for ( std::map<std::string, std::string>::iterator it = fOptions.begin();
	it != fOptions.end(); ++it )
    while ( ( pos = cuts.find( it -> first ) ) != std::string::npos )
      cuts.replace( pos, ( it -> first ).size(), it -> second );

  return '(' + cuts + ')';
}

//_______________________________________________________________________________
// Appends all the cuts in the same string using the given statement (< && > by
// default)
std::string General::CutManager::MakeMergedCut( std::string joinop ) {
  joinop = ' ' + joinop + ' ';
  auto it = fCuts.begin();
  std::string gcut( it -> second );
  for ( ++it; it != fCuts.end(); ++it )
    gcut.append( joinop + it -> second );
  return gcut;
}

//_______________________________________________________________________________
// Opens the file in the given path. The old file will be closed first.
void General::CutManager::Open( const std::string &file_path ) {
  if ( fFile.use_count() == 1 && (*fFile).is_open() )
    (*fFile).close();
  (*fFile).open( file_path.c_str() );
  if ( !(*fFile) ) {
    std::cerr << "ERROR: File < " << file_path << " > does not exist" << std::endl;
    return;
  }
  std::string line, str;
  size_t nl = 0, pos, newpos;
  while ( std::getline( (*fFile), line ) ) {
    ++nl;
    if ( line.size() )
      if ( line.front() != '#' ) {
	if ( ( pos = line.find( '=' ) ) != std::string::npos ) {

	  // Checks whether does exist any other assignment operator in the same line
	  if ( line.find( '=', pos + 1 ) != std::string::npos ) {
	    std::cerr << "ERROR: Found two assignment operators in line < " <<
	      nl << " >" << std::endl;
	    return;
	  }

	  // Checks whether the cut name is free of whitespaces
	  str = line.substr( 0, pos );
	  while ( str.front() == ' ' )
	    str.erase( str.begin() );
	  while ( str.back() == ' ' )
	    str.erase( str.end() - 1 );
	  if ( str.find( ' ' ) != std::string::npos ) {
	    std::cerr << "ERROR: The cut defined in line < " << nl <<
	      " > has whitespaces on its name" << std::endl;
	    return;
	  }

	  // Checks the matching of the < $ > symbols in the expression
	  while ( ( pos = line.find( '$', ++pos ) ) != std::string::npos ) {
	    newpos = line.find( '$', ++pos );
	    str    = line.substr( pos, newpos - pos );
	    if ( newpos == std::string::npos || str.find( ' ' ) != std::string::npos ) {
	      std::cerr << "ERROR: Mismatched < $ > symbol in line < " << nl << " >" << std::endl;
	      return;
	    }
	    pos = newpos + 1;
	  }
	}
	else
	  std::cout << "WARNING: Line number < " << nl <<
	    " > not a cut line; must be commented (starting by #)" << std::endl;
      }
  }
  (*fFile).clear();
  (*fFile).seekg( 0 );
}

//_______________________________________________________________________________       
// Prints the cuts attached to this class
void General::CutManager::Print() const {
  size_t maxsize = std::max_element( fCuts.begin(),
				     fCuts.end(),
				     [] ( const std::pair<std::string, std::string> &p1,
					  const std::pair<std::string, std::string> &p2 ) {
				       return p1.first.size() < p2.first.size(); } ) -> first.size();
  for ( auto it = fCuts.begin(); it != fCuts.end(); ++it )
    std::cout << std::left << std::setw( maxsize ) <<
      it -> first  << " => " << it -> second << std::endl;
}
