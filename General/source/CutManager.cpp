//////////////////////////////////////////////////////////
//                                                      //
//  General package                                     //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  AUTHOR: Miguel Ramos Pernas                         //
//  e-mail: miguel.ramos.pernas@cern.ch                 //
//                                                      //
//  Last update: 24/07/2015                             //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  Description:                                        //
//                                                      //
//  Class to storage cuts present in a cuts-file. This  //
//  files have to be written in the way:                //
//                                                      //
//  Define cut_name                                     //
//                                                      //
//  Cuts   some_cut and another_cut                     //
//                                                      //
//  or                                                  //
//                                                      //
//  Sets   cut_set_1 or cut_set_2                       //
//                                                      //
//  End                                                 //
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
  
  fCuts.insert( std::pair<std::string, std::string>( key, this -> GetCut( key ) ) );

  if ( key.size() > fSize )
    fSize = key.size();

  return fCuts[ key ].c_str();
}

//_______________________________________________________________________________       
// Functions that gets a cut from the cuts-file
const char* General::CutManager::GetCut( std::string key ) {

  fFile.seekg( 0 );

  std::string line, result;
  
  bool condition( false );

  std::string::size_type pos;

  while ( !fFile.eof() ) {

    fFile >> line;

    // Checks whether the line is empty
    if ( !line.empty() ) {

      if ( condition ) {

	// Option <Cuts>
	if ( !line.compare( "Cuts" ) ) {

	  std::getline( fFile, line );

	  result += ( "( " + line + " )" );
	}
	
	// Option <Sets>
	else if ( !line.compare( "Sets" ) ) {

	  std::getline( fFile, line );
	  
	  std::stringstream stream( line );

	  while ( stream >> line ) {

	    if ( fOptions.find( line ) != fOptions.end() )
	      result += ( " " + line + " " );
	    else if ( ! line.compare( "(" ) )
	      result += ( line + " " );
	    else if ( ! line.compare( ")" ) )
	      result += ( " " + line );
	    else {

	      unsigned int fpos( fFile.tellg() );

	      result += std::string( this -> GetCut( line ) );

	      fFile.seekg( fpos );
	    }
	  }
	}

	// Option <End>
	else if ( !line.compare( "End" ) ) {

	  condition = false;
	  break;
	}

	// Adds the line to the string if it is a conditional word
	else if ( fOptions.find( line ) != fOptions.end() )
	  result += ( " " + line + " " );
      }

      // Option <Define>
      else if ( !line.compare( "Define" ) ) {

	fFile >> line;

	if ( !line.compare( key ) )
	  condition = true; 
      }
    }

    fFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  // Changes the name of the conditionals in the cuts set
  for ( std::map<std::string, std::string>::iterator it = fOptions.begin();
	it != fOptions.end(); it++ )
    while ( ( pos = result.find( it -> first ) ) != std::string::npos )
      result.replace( pos, ( it -> first ).size(), it -> second );

  result.erase( remove( result.begin(), result.end(), '\t'), result.end());

  // If the cut does not exist, it sends a warning
  if ( result.empty() ) {

    std::cout << " WARNING: cut with name <" << key << "> does not exist." << std::endl;
    return 0;
  }

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

//_______________________________________________________________________________
