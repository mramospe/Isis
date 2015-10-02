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
//  Implements two classes perform analysis with the   //
//  TMVA package. BDTClassifier is the class that      //
//  allows to storage and manage all possible methods  //
//  provided by TMVA and to get the weights related    //
//  to one of them from a signal and a background      //
//  files. BDTApplier allows to apply some or all of   //
//  this methods in various data files.                //
//                                                     //
// --------------------------------------------------- //
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
    void           AddBackgroundFile( const char*,
				      const char *tree_path = "DecayTree",
				      Double_t weight = 1 );
    void           AddBackgroundFile( TFile*,
				      const char *tree_path = "DecayTree",
				      Double_t weight = 1 );
    void           AddSignalFile( const char*,
				  const char *tree_path = "DecayTree",
				  Double_t weight = 1 );
    void           AddSignalFile( TFile*,
				  const char *tree_path = "DecayTree",
				  Double_t weight = 1 );
    void           AddSpectator( const char*, const char*, const char *unit = "" );
    void           AddVariable( const char*, const char*, char );
    void           AddVariable( const char*, const char*, const char*, char );
    void           Classificate( const char *opts = 0 );
    TFile*         GetBackgroundFile();
    TMVA::Factory* GetFactory();
    TFile*         GetSignalFile();
    TFile*         GetStoreFile();
    void           BookMethod( const char*, const char *opts = 0 );
    void           SetBackgroundCuts( const char* );
    void           SetSignalCuts( const char* );
    void           SetFactory( const char*, const char *opts = 0 );
    void           SetStoreFile( const char*, const char *opt = "READ" );

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


  class BDTApplier : public FileManager {

  public:
    
    // Constructors
    BDTApplier();
    BDTApplier( const char*, const char*, const char *opts = 0);

    // Destructor
    ~BDTApplier();

    // Methods
    void          AddSpectator( const char* );
    void          AddVariable( const char* );
    void          Apply();
    TMVA::Reader *GetReader();
    void          SetInputTreePath( Int_t, const char* );
    void          SetOutputTreeName( const char* );
    void          SetPathToTree( Int_t, const char* );
    void          SetReader( const char *opts = "!Color:!Silent");
    void          SetFactoryName( const char* );
    void          UseMethod( const char* );

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

}

#endif
