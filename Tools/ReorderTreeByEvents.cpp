/////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------------- //
//                                                                                     //
//  Tool that allows to remake a given tree in a file, reordering                      //
//  the event identifiers.                                                             //
//                                                                                     //
// ----------------------------------------------------------------------------------- //
//                                                                                     //
//  AUTHOR : Miguel Ramos Pernas                                                       //
//  e-mail: miguel.ramos.pernas@cern.ch                                                //
//                                                                                     //
//  Last Update: 09/12/2015                                                            //
//                                                                                     //
// ----------------------------------------------------------------------------------- //
//                                                                                     //
// Usage:                                                                              //
//                                                                                     //
// >> ./ReorderTreeByEvents.out OutputFileName InputFileName InputTreePath EvtVarName  //
//                                                                                     //
// ----------------------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"


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
  TFile *inputFile = TFile::Open( argv[ 2 ] );
  TTree *inputTree = (TTree*) inputFile -> Get( argv[ 3 ] );
  inputTree -> SetBranchStatus( argv[ 4 ], false );

  // Creates the output file
  TFile *outputFile = TFile::Open( argv[ 1 ], "RECREATE" );
  std::cout << " Created output file with name: < " << argv[ 1 ] << " >" << std::endl;
  TTree *outputTree = inputTree -> CloneTree();
  outputTree -> Write();

  // Creates the event branch again from the old one
  Long64_t evtIndex = 0, evtOld = 0, evtNew;
  std::string evtTitle = argv[ 4 ];
  evtTitle += "/I";
  TBranch *evtBranch = outputTree -> Branch( argv[ 4 ], &evtIndex, evtTitle.c_str() );
  TLeaf   *evtLeaf;

  std::cout << " Rewriting < " << argv[ 4 ] << " > branch..." << std::endl;
  inputTree -> SetBranchStatus( argv[ 4 ], true );
  evtLeaf = inputTree -> GetLeaf( argv[ 4 ] );
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
