//////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------- //
//                                                                      //
//  Tool that allows to remake a given tree in a file, reordering       //
//  the event identifiers.                                              //
//                                                                      //
// -------------------------------------------------------------------- //
//                                                                      //
//  AUTHOR : Miguel Ramos Pernas                                        //
//  e-mail: miguel.ramos.pernas@cern.ch                                 //
//                                                                      //
//  Last Update: 25/09/2015                                             //
//                                                                      //
// -------------------------------------------------------------------- //
//                                                                      //
// Usage:                                                               //
//                                                                      //
// >> ./ReorderTreeByEvents OutputFileName InputTreePath EvtVarName     //
//    InputFileName                                                     //
//                                                                      //
// -------------------------------------------------------------------- //
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"


// ===================================== MAIN FUNCTION

int main( int argc, const char *argv[] ) {

  if ( argc < 5 ) {
    std::cout <<
      " Number of input parameters not valid.\n" <<
      " Input parameters have to be given as: " <<
      "< output file > < tree path > < event branch name > < input file >" <<
      std::endl;
    exit( 0 );
  }
  
  // Gets the input tree
  TFile *inputFile = TFile::Open( argv[ 4 ] );
  TTree *inputTree = (TTree*) inputFile -> Get( argv[ 2 ] );
  inputTree -> SetBranchStatus( argv[ 3 ], false );

  // Creates the output file
  TFile *outputFile = TFile::Open( argv[ 1 ], "RECREATE" );
  std::cout << " Created output file with name: < " << argv[ 1 ] << " >" << std::endl;
  TTree *outputTree = inputTree -> CloneTree();
  outputTree -> Write();

  // Creates the event branch again from the old one
  Long64_t evtIndex = 0, evtOld = 0, evtNew;
  std::string evtTitle = argv[ 3 ];
  evtTitle += "/I";
  TBranch *evtBranch = outputTree -> Branch( argv[ 3 ], &evtIndex, evtTitle.c_str() );
  TLeaf   *evtLeaf;

  std::cout << " Rewriting < " << argv[ 3 ] << " > branch..." << std::endl;
  inputTree -> SetBranchStatus( argv[ 3 ], true );
  evtLeaf = inputTree -> GetLeaf( argv[ 3 ] );
  for ( Long64_t ievt = 0; ievt < inputTree -> GetEntries(); ievt++ ) {
    inputTree -> GetEntry( ievt );
    evtNew = evtLeaf -> GetValue();
    if ( evtNew != evtOld ) {
      evtOld = evtNew;
      evtIndex++;
    }
    evtBranch -> Fill();
  }
  outputTree -> Write();

  // Finishes the process
  inputFile  -> Close();
  outputFile -> Close();
  std::cout << " Finished!" << std::endl;
}
