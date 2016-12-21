/////////////////////////////////////////////////////////
//
//  Analysis package
//
// ------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 13/11/2015
//
// ------------------------------------------------------
//
//  Description:
//
//  Implements two classes perform analysis with the
//  TMVA package. BDTClassifier is the class that
//  allows to storage and manage all possible methods
//  provided by TMVA and to get the weights related
//  to one of them from a signal and a background
//  files. BDTApplier allows to apply some or all of
//  this methods in various data files.
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#ifndef BDT_ANALYSIS
#define BDT_ANALYSIS

#include <map>

#include "TROOT.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include "FileManager.h"


//_______________________________________________________________________________

namespace Analysis {

  class BDTClassifier {

  public:

    // Constructors
    BDTClassifier();
    BDTClassifier( const char*, const char*, const char *opts = 0 );

    // Destructor
    ~BDTClassifier();

    // Methods
    void                  AddBackgroundFile( const char*,
					     const char *tree_path = "DecayTree",
					     Double_t weight = 1 );
    void                  AddBackgroundFile( TFile*,
					     const char *tree_path = "DecayTree",
					     Double_t weight = 1 );
    void                  AddSignalFile( const char*,
					 const char *tree_path = "DecayTree",
					 Double_t weight = 1 );
    void                  AddSignalFile( TFile*,
					 const char *tree_path = "DecayTree",
					 Double_t weight = 1 );
    inline void           AddSpectator( const char*, const char*, const char *unit = "" );
    inline void           AddVariable( const char*, const char*, char );
    inline void           AddVariable( const char*, const char*, const char*, char );
    void                  Classificate( const char *opts = 0 );
    inline TFile*         GetBackgroundFile();
    inline TMVA::Factory* GetFactory();
    inline TFile*         GetSignalFile();
    inline TFile*         GetStoreFile();
    void                  BookMethod( const char*, const char *opts = 0 );
    inline void           SetBackgroundCuts( const char* );
    inline void           SetSignalCuts( const char* );
    void                  SetFactory( const char*, const char *opts = 0 );
    void                  SetStoreFile( const char*, const char *opt = "READ" );

  protected:

    // Attributes
    const char                      *fBackgroundCuts;
    TFile                           *fBackgroundFile;
    Int_t                            fBackgroundWeight;
    TMVA::Factory                   *fFactory;
    const char                      *fSignalCuts;
    TFile                           *fSignalFile;
    Int_t                            fSignalWeight;
    TFile                           *fStoreFile;
    std::map < const char*, Bool_t > fUse;

  };

  // Adds a spectator variable to the reader
  inline void BDTClassifier::AddSpectator( const char *name,
					      const char *title,
					      const char *unit ) {
    fFactory -> AddSpectator( name, title, unit );
  }
  // Adds a variable to the reader
  inline void BDTClassifier::AddVariable( const char *name,
					     const char *title,
					     char dtype ) {
    this -> AddVariable( name, title, "", dtype );
  }
  // Adds a variable to the reader
  inline void BDTClassifier::AddVariable( const char *name,
					     const char *title,
					     const char *unit,
					     char dtype ) {
    fFactory -> AddVariable( name, title, unit, dtype );
  }
  // Returns the input background file
  inline TFile *BDTClassifier::GetBackgroundFile() { return fBackgroundFile; }
  // Returns the factory
  inline TMVA::Factory *BDTClassifier::GetFactory() { return fFactory; }
  // Returns the input signal file
  inline TFile *BDTClassifier::GetSignalFile() { return fSignalFile; }
  // Returns the input store file
  inline TFile *BDTClassifier::GetStoreFile() { return fStoreFile; }
  // Sets the background cuts
  inline void BDTClassifier::SetBackgroundCuts( const char *cuts ) { fBackgroundCuts = cuts; }
  // Sets the signal cuts
  inline void BDTClassifier::SetSignalCuts( const char *cuts ) { fSignalCuts = cuts; }


  class BDTApplier : public FileManager {

  public:
    
    // Constructors
    BDTApplier();
    BDTApplier( const char*, const char*, const char *opts = 0);

    // Destructor
    ~BDTApplier();

    // Methods
    inline void          AddSpectator( const char* );
    inline void          AddVariable( const char* );
    void                 Apply();
    inline TMVA::Reader *GetReader();
    void                 SetInputTreePath( Int_t, const char* );
    void                 SetOutputTreeName( const char* );
    inline void          SetPathToTree( Int_t, const char* );
    void                 SetReader( const char *opts = "!Color:!Silent");
    inline void          SetFactoryName( const char* );
    inline void          UseMethod( const char* );

  protected:

    // Attributes
    const char                 *fFactoryName;
    const char                 *fTreeName;
    TMVA::Reader               *fReader;
    std::vector < const char* > fSpectators;
    std::vector < const char* > fTrees;
    std::vector < const char* > fMethods;
    std::vector < const char* > fVariables;

  };

  // Adds a new spectator variable to the reader
  inline void BDTApplier::AddSpectator( const char *name ) { fSpectators.push_back( name ); }
  // Adds a new variable to the reader
  inline void BDTApplier::AddVariable( const char *name ) { fVariables.push_back( name ); }
  // Gets the reader
  inline TMVA::Reader *BDTApplier::GetReader() { return fReader; }
  // Sets the factory name
  inline void BDTApplier::SetFactoryName( const char *name ) { fFactoryName = name; }
  // Sets the name of the output tree
  inline void BDTApplier::SetOutputTreeName( const char *name ) { fTreeName = name; }
  // Adds a method to this class
  inline void BDTApplier::UseMethod( const char *method ) { fMethods.push_back( method ); }

}

#endif
