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


#ifndef CUT_MANAGER
#define CUT_MANAGER

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class CutManager {

  public:

    // Contructor
    CutManager( const std::string &file_path );

    // Destructor
    ~CutManager();

    // Methods
    void               AppendCuts( std::vector<std::string> &vector );
    void               AppendCuts( std::map<std::string, std::string> &map );
    std::string        BookCut( const std::string &key, const bool &print = true );
    inline void        Clear();
    inline void        Close();
    inline std::string Get( const std::string &key );
    std::string        GetCut( const std::string &key );
    std::string        MakeMergedCut();
    void               Open( const std::string &file_path );
    void               Print();

    // Inline methods
    inline std::map<std::string, std::string>::iterator Begin();
    inline std::map<std::string, std::string>::iterator End();

    // Operator
    inline std::string operator [] ( const std::string &key );

  protected:

    // Attributes
    std::map<std::string, std::string> fCuts;
    std::ifstream                      fFile;
    std::map<std::string, std::string> fOptions;

  };

  //______________________________
  // INLINE METHODS DEFINITION

  // Clears the data storaged in the class
  inline void General::CutManager::Clear() { fCuts.clear(); }
  // Closes the file related to this class
  inline void General::CutManager::Close() { fFile.close(); }
  // Returns the cut booked with name key
  inline std::string CutManager::Get( const std::string &key ) {
    return fCuts[ key ].c_str();
  }
  // Opens the file with the given path
  inline void General::CutManager::Open( const std::string &file_path ) {
    fFile.open( file_path.c_str() );
  }
  // Gets the location of the begin of the map of cuts
  inline std::map<std::string, std::string>::iterator CutManager::Begin() {
    return fCuts.begin();
  }
  // Gets the location of the end of the map of cuts 
  inline std::map<std::string, std::string>::iterator CutManager::End() {
    return fCuts.end();
  }

  //______________________________
  // OPERATOR
  inline std::string CutManager::operator [] ( const std::string &key ) {
    return fCuts[ key ];
  }

}

#endif
