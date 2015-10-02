/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //       
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 13/07/2015                            //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  Description:                                       //
//                                                     //
//  Implements the main methods to be owned by all     //
//  the posible classes which need to manage various   //
//  files.                                             //
//                                                     //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////
                                       

#ifndef PHYSICAL_PROJECT
#define PHYSICAL_PROJECT

#include <iostream>

#include "TFile.h"
#include "TList.h"


//_______________________________________________________________________________

namespace Analysis {

  class FileManager {

  public:

    // Constructors
    FileManager();
    FileManager(const char*);
    
    // Destructor
    virtual ~FileManager();
    
    // Methods
    void AddFile(TFile*);
    void AddFile(const char*, const char *opt = "READ");
    void cd(Int_t);
    void CloseAllFiles();
    void CloseAllReadFiles();
    void CloseAllWriteFiles();
    void CloseReadFile(Int_t);
    void CloseWriteFile(Int_t);
    TList *GetAllReadFiles();
    TList *GetAllWriteFiles();
    const char *GetName();
    Int_t GetNAllFiles();
    Int_t GetNReadFiles();
    Int_t GetNWriteFiles();
    TFile *GetReadFile(Int_t);
    TFile *GetReadFile(const char*);
    TFile *GetWriteFile(Int_t);
    TFile *GetWriteFile(const char*);
    void PrintAllFileNames();
    void PrintReadFileName(Int_t);
    void PrintWriteFileName(Int_t);
    void SetName(const char*);

  protected:
    
    // Attributes
    const char *fName;
    TList *fReadFiles;
    TList *fWriteFiles;

  };


  // Other functions
  void SendError(const char*);
  void SendError(const char*, const char*);

}

#endif
