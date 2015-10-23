//////////////////////////////////////////////////////////
//                                                      //
//  General package                                     //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  AUTHOR: Miguel Ramos Pernas                         //
//  e-mail: miguel.ramos.pernas@cern.ch                 //
//                                                      //
//  Last update: 23/10/2015                             //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  Description:                                        //
//                                                      //
//  Class to storage cuts present in a cuts-file. This  //
//  files have to be written in the way:                //
//                                                      //
//  cut1 = j > 0 and b > 1                              //
//                                                      //
//  cut2 = $cut1 or c == 0                              //
//                                                      //
//  The symbol < $ > after a word means that this word  //
//  is a cut, so the class is going to read that and    //
//  place it instead. This class is going to search     //
//  for the given keys, so one can comment anything as  //
//  long as no key has the same string value as it.     //
//                                                      //
// ---------------------------------------------------- //
//////////////////////////////////////////////////////////


#include "CutManager.h"

#include <iomanip>
#include <sstream>
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

  if ( fFile.eof() )
    fFile.clear();

  fFile.seekg( 0 );

  bool cond( true );

  std::string line, result;

  std::string::size_type pos;

  while ( std::getline( fFile, line ) && cond ) {

    // Checks whether the line is empty
    if ( !line.empty() ) {

      if ( !line.compare( 0, key.size(), key ) ) {

	line = line.substr( line.find( "=" ) + 1, line.size() );

	// Checks for parentheses and includes a whitespace ( necessary to load the cut
	// successfully
	size_t pos = 0;
	while ( ( pos = line.find( "(", pos ) ) != std::string::npos ) {
	  line.replace( pos, 1, "( " );
	  pos += 2;
	}
	pos = 0;
	while ( ( pos = line.find( ")", pos ) ) != std::string::npos ) {
	  line.replace( pos, 1, " )" );
	  pos += 2;
	}

	std::stringstream stream( line );

	// Adds the different conditions to the output cut
	while ( stream >> line ) {
	  if ( fOptions.find( line ) != fOptions.end() )
	    result += " " + line + " ";
	  else if ( !line.compare( "(" ) or !line.compare( ")" ) )
	    result += line;
	  else if ( line[ 0 ] == '$' ) {
	    
	    size_t fpos( fFile.tellg() );
	    line = line.substr( 1, line.size() );

	    const char *new_cut = this -> GetCut( line );

	    if ( new_cut )
	      result += "(" + std::string( new_cut ) + ")";
	    else
	      return 0;

	    fFile.seekg( fpos );
	  }
	  else
	    result += " " + line + " ";
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
    while ( ( pos = result.find( it -> first ) ) != std::string::npos )
      result.replace( pos, ( it -> first ).size(), it -> second );

  // Formats the output string
  while ( ( pos = result.find( "  " ) ) != std::string::npos ) result.replace( pos, 2, " " );
  while ( ( pos = result.find( " )" ) ) != std::string::npos ) result.replace( pos, 2, ")" );
  while ( ( pos = result.find( "( " ) ) != std::string::npos ) result.replace( pos, 2, "(" );
  if ( *result.begin()     == ' ' ) result.erase( result.begin()  , result.begin() + 1 );
  if ( *(result.end() - 1) == ' ' ) result.erase( result.end() - 1, result.end()       );

  return result.c_str();
}

//_______________________________________________________________________________       
// Prints the cuts atached to this class
void General::CutManager::Print() {

  for ( std::map<std::string, std::string>::iterator it = fCuts.begin();
	it != fCuts.end(); it++ )
    std::cout << std::left << std::setw( fSize ) <<
      it -> first  <<
      "\t=>\t"     <<
      it -> second << std::endl;
}
