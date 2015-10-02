/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //       
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 11/08/2015                            //
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


#include <cstring>

#include "FileManager.h"


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main Constructor
Analysis::FileManager::FileManager() : 
  fName(0),
  fReadFiles(0),
  fWriteFiles(0) { }

//_______________________________________________________________________________
// Constructor given a name
Analysis::FileManager::FileManager(const char *name) : 
  fName(0),
  fReadFiles(0),
  fWriteFiles(0) { fName = name; }

//_______________________________________________________________________________
// Destructor
Analysis::FileManager::~FileManager() {

  if (fReadFiles) {
    fReadFiles->SetOwner();
    delete fReadFiles;
  }
  if (fWriteFiles) {
    fWriteFiles->SetOwner();
    delete fWriteFiles;
  }
}

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Attaches a file to the project given its pointer.
void Analysis::FileManager::AddFile(TFile *file) {

  if (!strcmp(file->GetOption(), "READ")) {
    if (fReadFiles)
      fReadFiles->Add(file);
    else {
      fReadFiles = new TList();
      fReadFiles->Add(file);
    }
  }
  else {
    if (fWriteFiles)
      fWriteFiles->Add(file);
    else {
      fWriteFiles = new TList();
      fWriteFiles->Add(file);
    }
  }
}

//_______________________________________________________________________________
// Adds a file to the project given its path.
void Analysis::FileManager::AddFile(const char *file_name, const char *opt) {

  if (!strcmp(opt, "READ") || !strcmp(opt, "read")) {
    if (fReadFiles)
      fReadFiles->Add(new TFile(file_name));
    else {
      fReadFiles = new TList();
      fReadFiles->Add(new TFile(file_name));
    }
  }
  else {
    if (fWriteFiles)
      fWriteFiles->Add(new TFile(file_name, opt));
    else {
      fWriteFiles = new TList();
      fWriteFiles->Add(new TFile(file_name, opt));
    }
  }
}

//_______________________________________________________________________________
// Changes the directory path to the write-file specified.
void Analysis::FileManager::cd(Int_t ifile) {

  ((TFile*) fWriteFiles->At(ifile))->cd();
}

//_______________________________________________________________________________
// Closes all the files.
void Analysis::FileManager::CloseAllFiles() {

  this->CloseAllReadFiles();
  this->CloseAllWriteFiles();
}

//_______________________________________________________________________________
// Closes the reading files.
void Analysis::FileManager::CloseAllReadFiles() {

  for (Int_t ifile = 0; ifile < this->GetNReadFiles(); ifile++)
    this->CloseReadFile(ifile);
}

//_______________________________________________________________________________
// Closes the writting files.
void Analysis::FileManager::CloseAllWriteFiles() {

  for (Int_t ifile = 0; ifile < this->GetNWriteFiles(); ifile++)
    this->CloseWriteFile(ifile);
}

//_______________________________________________________________________________
// Closes the reading file at position <ifile>.
void Analysis::FileManager::CloseReadFile(Int_t ifile) {

  ((TFile*) fReadFiles->At(ifile))->Close();
}

//_______________________________________________________________________________
// Closes the reading file at position <ifile>.
void Analysis::FileManager::CloseWriteFile(Int_t ifile) {

  ((TFile*) fWriteFiles->At(ifile))->Close();
}

//_______________________________________________________________________________
// Gets the list of reading files in the project.
TList *Analysis::FileManager::GetAllReadFiles() { return fReadFiles; }

//_______________________________________________________________________________
// Gets the list of writting files in the project.
TList *Analysis::FileManager::GetAllWriteFiles() { return fWriteFiles; }

//_______________________________________________________________________________
// Gets the name of the project.
const char *Analysis::FileManager::GetName() { return fName; }

//_______________________________________________________________________________
// Gets the number of all the files.
Int_t Analysis::FileManager::GetNAllFiles() {
  
  Int_t nfiles = 0;
  if (fReadFiles)
    nfiles += this->GetNReadFiles();
  if (fWriteFiles)
    nfiles += this->GetNWriteFiles();

  return nfiles;
}

//_______________________________________________________________________________
// Gets the number of files for reading in the project.
Int_t Analysis::FileManager::GetNReadFiles() {
  
  if (fReadFiles)
    return fReadFiles->GetSize();
  else
    return 0;
}

//_______________________________________________________________________________
// Gets the number of files for writting in the project.
Int_t Analysis::FileManager::GetNWriteFiles() {
  
  if (fWriteFiles)
    return fWriteFiles->GetSize();
  else
    return 0;
}

//_______________________________________________________________________________
// Gets the reading file at position <ifile>.
TFile *Analysis::FileManager::GetReadFile(Int_t ifile) {

  return (TFile*) fReadFiles->At(ifile);
}

//_______________________________________________________________________________
// Gets the reading file with name <file_name>.
TFile *Analysis::FileManager::GetReadFile(const char *file_name) {

  for (Int_t ifile = 0; ifile < this->GetNReadFiles(); ifile++)
    if (!strcmp(fReadFiles->At(ifile)->GetName(), file_name))
      return (TFile*) fReadFiles->At(ifile);

  return 0;
}

//_______________________________________________________________________________
// Gets the writting file at position <ifile>.
TFile *Analysis::FileManager::GetWriteFile(Int_t ifile) {

  return (TFile*) fWriteFiles->At(ifile);
}

//_______________________________________________________________________________
// Gets the writting file with name <file_name>.
TFile *Analysis::FileManager::GetWriteFile(const char *file_name) {

  for (Int_t ifile = 0; ifile < this->GetNWriteFiles(); ifile++)
    if (!strcmp(fWriteFiles->At(ifile)->GetName(), file_name))
      return (TFile*) fWriteFiles->At(ifile);

  return 0;
}

//_______________________________________________________________________________
// Prints al the files' names.
void Analysis::FileManager::PrintAllFileNames() {

  Int_t ifile;
  std::cout << "\n--- Files for reading:" << std::endl;
  for (ifile = 0; ifile < this->GetNReadFiles(); ifile++)
    std::cout << ((TFile*) fReadFiles->At(ifile))->GetName() << std::endl;
  std::cout << "\n--- Files for writting:" << std::endl;
  for (ifile = 0; ifile < this->GetNWriteFiles(); ifile++)
    std::cout << ((TFile*) fWriteFiles->At(ifile))->GetName() << std::endl;
}

//_______________________________________________________________________________
// Prints the write-file names.
void Analysis::FileManager::PrintReadFileName(Int_t ifile) {

  std::cout << ((TFile*) fReadFiles->At(ifile))->GetName() << std::endl;
}

//_______________________________________________________________________________
// Prints the write-file names.
void Analysis::FileManager::PrintWriteFileName(Int_t ifile) {

  std::cout << ((TFile*) fWriteFiles->At(ifile))->GetName() << std::endl;
}

//_______________________________________________________________________________
// Sets the name of the project.
void Analysis::FileManager::SetName(const char *name) { fName = name; }

//_______________________________________________________________________________


// -- OTHER FUNCTIONS

//_______________________________________________________________________________
// Sends an error and exits the program.
void Analysis::SendError(const char *error) {

  std::cout << error << std::endl;
  exit(0);
}

// Sends an error with a path and a file name and exits the program.
void Analysis::SendError(const char *path,
			 const char *file) {

  std::cout << " Couldn't find " << path << " in file <" << file << ">" << std::endl;
  exit(0);
}
