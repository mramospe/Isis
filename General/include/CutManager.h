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


#ifndef CUT_MANAGER
#define CUT_MANAGER

#include <iostream>
#include <fstream>
#include <string>
#include <map>


//_______________________________________________________________________________

namespace General {

  class CutManager {

  public:

    // Contructor
    CutManager( std::string file_path );

    // Destructor
    ~CutManager();

    // Methods
    const char*        BookCut( std::string key );
    inline void        Clear();
    inline void        Close();
    inline const char* Get( std::string key );
    const char*        GetCut( std::string key );
    void               Open( std::string file_path );
    void               Print();

    inline std::map<std::string, std::string>::iterator Begin();
    inline std::map<std::string, std::string>::iterator End();

    // Operator
    inline const char* operator [] ( std::string key );

  protected:

    // Attributes
    std::map<std::string, std::string> fCuts;
    std::ifstream                      fFile;
    std::map<std::string, std::string> fOptions;
    std::string::size_type             fSize;

  };

  //______________________________
  // INLINE METHODS DEFINITION

  // Clears the data storaged in the class
  inline void General::CutManager::Clear() { fCuts.clear(); }
  // Closes the file related to this class
  inline void General::CutManager::Close() { fFile.close(); }
  // Returns the cut booked with name key
  inline const char* CutManager::Get( std::string key ) {
    return fCuts[ key ].c_str();
  }
  // Opens the file with the given path
  inline void General::CutManager::Open( std::string file_path ) {
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
  inline const char* CutManager::operator [] ( std::string key ) {
    return fCuts[ key ].c_str();
  }

}

#endif
