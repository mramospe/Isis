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


#ifndef CUT_MANAGER
#define CUT_MANAGER

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <map>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class CutManager {

  public:

    // Contructor
    CutManager( const std::string &file_path = std::string() );
    CutManager( const CutManager &other );

    // Destructor
    ~CutManager();

    // Methods
    std::string BookCut( const std::string &key, const bool &print = true );
    inline void Clear();
    inline void Close();
    std::string GetCut( const std::string &key );
    std::string MakeMergedCut( std::string joinop = "&&" );
    void        Open( const std::string &file_path );
    void        Print() const;
    inline void Remove( const std::string &key );

    // Iteration methods
    inline std::map<std::string, std::string>::iterator begin();
    inline std::map<std::string, std::string>::iterator end();

    // Operator
    inline std::string operator [] ( const std::string &key ) const;

  protected:

    // Attributes
    std::map<std::string, std::string> fCuts;
    std::shared_ptr<std::ifstream>     fFile;
    std::map<std::string, std::string> fOptions;

  };

  //______________________________
  // INLINE METHODS DEFINITION

  // Clears the data storaged in the class
  inline void CutManager::Clear() { fCuts.clear(); }
  // Closes the file related to this class
  inline void CutManager::Close() {
    if ( fFile.use_count() == 1 )
      (*fFile).close();
    else
      std::cout <<
	"WARNING: Attempt to close a file when more than one manager is accessing it"
		<< std::endl;
  }
  // Gets the location of the begin of the map of cuts
  inline std::map<std::string, std::string>::iterator CutManager::begin() {
    return fCuts.begin();
  }
  // Gets the location of the end of the map of cuts 
  inline std::map<std::string, std::string>::iterator CutManager::end() {
    return fCuts.end();
  }
  // Removes the cut booked as < key >. If "*" is provided all will be removed.
  inline void CutManager::Remove( const std::string &key ) {
    if ( key == "*" )
      fCuts.erase( key );
    else
      fCuts.clear();
  }
  
  //______________________________
  // OPERATOR
  inline std::string CutManager::operator [] ( const std::string &key ) const {
    return fCuts.at( key );
  }

}

#endif
