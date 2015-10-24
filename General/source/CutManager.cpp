///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 24/10/2015                              //
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

#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <limits>


//_______________________________________________________________________________       


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________       
// Main constructor
General::CutManager::CutManager( std::string file_path ) : fSize( 0 ) {

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
// Almacenates a new cut from the cuts-file
const char* General::CutManager::BookCut( std::string key ) {

  if ( fCuts.find( key ) != fCuts.end() ) {
    std::cout << "Cut with name < " << key << " > already booked" << std::endl;
    return 0;
  }

  const char *cut = this -> GetCut( key );

  if ( cut ) {
  
    fCuts.insert( std::pair<std::string, std::string>( key, cut ) );

    if ( key.size() > fSize )
      fSize = key.size();

    std::cout << "Booked new cut < " << key << " >: " << fCuts[ key ] << std::endl;
  }

  return cut;
}

//_______________________________________________________________________________       
// Functions that gets a cut from the cuts-file
const char* General::CutManager::GetCut( std::string key ) {

  // Each time a cut is obtained the pointer to the file is set to its start
  if ( fFile.eof() )
    fFile.clear();
  fFile.seekg( 0 );

  std::string cuts, newcut, sstr;
  std::string::size_type pos, ifirst, ilast;
  size_t fpos;
  bool   cond( true );

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
      std::cout << "WARNING: cut with name < " << key << " > does not exist." << std::endl;
      return 0;
    }
  }
  // Changes the name of the conditionals in the cuts set
  for ( std::map<std::string, std::string>::iterator it = fOptions.begin();
	it != fOptions.end(); it++ )
    while ( ( pos = cuts.find( it -> first ) ) != std::string::npos )
      cuts.replace( pos, ( it -> first ).size(), it -> second );

  return cuts.c_str();
}

//_______________________________________________________________________________       
// Prints the cuts attached to this class
void General::CutManager::Print() {

  for ( std::map<std::string, std::string>::iterator it = fCuts.begin();
	it != fCuts.end(); it++ )
    std::cout << std::left << std::setw( fSize ) <<
      it -> first  <<
      "\t=>\t"     <<
      it -> second << std::endl;
}
