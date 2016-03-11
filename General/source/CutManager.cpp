///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 11/03/2016                              //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  Description:                                         //
//                                                       //
//  Class to storage cuts present in a cuts-file. This   //
//  files have to be written in the way:                 //
//                                                       //
//  cut1 = j > 0 and b > 1                               //
//                                                       //
//  cut2 = $cut1$ or c == 0                              //
//                                                       //
//  The symbol < $ > before and after a word means that  //
//  is a cut, so the class is going to get that and      //
//  place it instead. This class is going to search      //
//  for the given keys, so one can comment anything as   //
//  long as no key has the same string value as it,      //
//  being situated at the beginning of the line.         //
//                                                       //
// ----------------------------------------------------- //
///////////////////////////////////////////////////////////


#include "CutManager.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>


//_______________________________________________________________________________       


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________       
// Main constructor
General::CutManager::CutManager( const std::string &file_path ) {
  fFile.open( file_path.c_str() );
  fOptions[ "and" ] = "&&" ;
  fOptions[ "or"  ] = "||" ;
}

//_______________________________________________________________________________       
// Destructor
General::CutManager::~CutManager() { if ( fFile.is_open() ) fFile.close(); }

//_______________________________________________________________________________       


// -- PUBLIC METHODS

//_______________________________________________________________________________       
// Appends to the given vector the cuts of the current class
void General::CutManager::AppendCuts( std::vector<std::string> &vector ) {
  std::vector<std::string> appvec( fCuts.size() );
  auto itv = appvec.begin();
  auto itm = fCuts.begin();
  while ( itm != fCuts.end() )
    *itv++ = itm++ -> second;
  vector.insert( vector.end(), appvec.begin(), appvec.end() );
}

//_______________________________________________________________________________       
// Appends to the given map the cuts of the current class. If the cut already
// exists, a warning message is displayed and the cut is replaced.
void General::CutManager::AppendCuts( std::map<std::string, std::string> &map ) {
  for ( auto itm = fCuts.begin(); itm != fCuts.end(); ++itm ) {
    if ( map.find( itm -> first ) != map.end() )
      std::cout << "WARNING: Replacing existing cut < " << itm -> first << " >" << std::endl;
    map[ itm -> first ] = itm -> second;
  }
}

//_______________________________________________________________________________       
// Almacenates a new cut from the cuts-file
std::string General::CutManager::BookCut( const std::string &key ) {
  if ( fCuts.find( key ) != fCuts.end() ) {
    std::cout << "Cut with name < " << key << " > already booked" << std::endl;
    return "";
  }
  std::string cut = this -> GetCut( key );
  if ( cut.size() ) {
    fCuts.insert( std::make_pair( key, cut ) );
    std::cout << "Booked new cut < " << key << " >: " << fCuts[ key ] << std::endl;
  }
  return cut;
}

//_______________________________________________________________________________       
// Functions that gets a cut from the cuts-file
std::string General::CutManager::GetCut( const std::string &key ) {

  // Each time a cut is obtained the pointer to the file is set to its start
  if ( fFile.eof() )
    fFile.clear();
  fFile.seekg( 0 );

  std::string cuts, newcut, sstr;
  std::string::size_type pos, ifirst, ilast, fpos;
  bool cond( true );

  while ( cond && std::getline( fFile, cuts ) ) {

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
	  ilast = cuts.find( '$', ifirst + 1 );
	  sstr  = cuts.substr( ifirst + 1, key.size() );
	  fpos  = fFile.tellg();

	  // The new cut is introduced between parentheses
	  newcut = "()";
	  newcut.insert( 1, std::string( this -> GetCut( sstr ) ) );
	  
	  cuts.replace( ifirst, ilast + 1, newcut );
	  fFile.seekg( fpos );
	}

	cond = false;
      }
    }

    if ( fFile.eof() && cond ) {
      std::cerr << "ERROR: Cut with name < " << key << " > does not exist." << std::endl;
      return 0;
    }
  }
  // Changes the name of the conditionals in the cuts set
  for ( std::map<std::string, std::string>::iterator it = fOptions.begin();
	it != fOptions.end(); it++ )
    while ( ( pos = cuts.find( it -> first ) ) != std::string::npos )
      cuts.replace( pos, ( it -> first ).size(), it -> second );

  return cuts;
}

//_______________________________________________________________________________
// Appends all the cuts in the same string using < && > statements
std::string General::CutManager::MakeMergedCut() {
  std::string gcut( '(' + fCuts.begin() -> second + ')' );
  auto it = fCuts.begin();
  for ( ++it; it != fCuts.end(); ++it )
    gcut.append( " && (" + it -> second + ')' );
  return gcut;
}

//_______________________________________________________________________________       
// Prints the cuts attached to this class
void General::CutManager::Print() {
  size_t maxsize = std::max_element( fCuts.begin(),
				     fCuts.end(),
				     [] ( const std::pair<std::string, std::string> &p1,
					  const std::pair<std::string, std::string> &p2 ) {
				       return p1.first.size() < p2.first.size(); } ) -> first.size();
  for ( auto it = fCuts.begin(); it != fCuts.end(); it++ )
    std::cout << std::left << std::setw( maxsize ) <<
      it -> first  << " => " << it -> second << std::endl;
}
