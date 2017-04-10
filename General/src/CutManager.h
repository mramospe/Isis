///////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
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


#ifndef CUT_MANAGER
#define CUT_MANAGER

#include "Definitions.h"
#include "Messenger.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <map>
#include <vector>


//_______________________________________________________________________________

namespace isis {

  class CutManager {

  public:

    // Main constructor
    CutManager( const std::string &file_path = std::string() );

    // Copy constructor
    CutManager( const CutManager &other );

    // Destructor
    ~CutManager();

    // Almacenates a new cut from the cuts-file
    std::string bookCut( const std::string &key, const bool &print = true );

    // Clears the data storaged in the class
    inline void clear();

    // Closes the file related to this class
    inline void close();

    // Functions that gets a cut from the cuts-file
    std::string getCut( const std::string &key );

    // Appends all the cuts in the same string using the given statement (< && > by
    // default)
    std::string makeMergedCut( std::string joinop = "&&" );

    // Opens the file in the given path. The old file will be closed first
    void open( const std::string &file_path );

    // Prints the cuts attached to this class
    void display() const;

    // Removes the cut booked as < key >. If "*" is provided all will be removed.
    inline void remove( const std::string &key );

    // Gets the location of the begin of the map of cuts
    inline StrMap::iterator begin();

    // Gets the location of the end of the map of cuts
    inline StrMap::iterator end();

    // Operator to get the cut with name < key >
    inline std::string operator [] ( const std::string &key ) const;

  protected:

    // Map of cuts stored by key
    StrMap fCuts;

    // Input file to read from
    std::shared_ptr<std::ifstream> fFile;

    // Map to store conversions from 'and' => '&&' and 'or' => '||'
    StrMap fOptions;

  };

  //_______________________________________________________________________________
  //
  inline void CutManager::clear() { fCuts.clear(); }

  //_______________________________________________________________________________
  //
  inline void CutManager::close() {
    if ( fFile.use_count() == 1 )
      (*fFile).close();
    else
      IWarning<<
	"Attempt to close a file when more than one manager is accessing it"
		<< IEndMsg;
  }

  //_______________________________________________________________________________
  //
  inline StrMap::iterator CutManager::begin() {
    return fCuts.begin();
  }

  //_______________________________________________________________________________
  //
  inline StrMap::iterator CutManager::end() {
    return fCuts.end();
  }

  //_______________________________________________________________________________
  //
  inline void CutManager::remove( const std::string &key ) {
    if ( key == "*" )
      fCuts.erase( key );
    else
      fCuts.clear();
  }

  //_______________________________________________________________________________
  //
  inline std::string CutManager::operator [] ( const std::string &key ) const {
    return fCuts.at( key );
  }

}

#endif
