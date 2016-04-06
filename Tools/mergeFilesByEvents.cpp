/////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------- //
//                                                                 //
//  Tool that allows to modify the event identifiers of a          //
//  given set of trees, merging all trees in the files into        //
//  one.                                                           //
//                                                                 //
// --------------------------------------------------------------- //
//                                                                 //
//  AUTHOR : Miguel Ramos Pernas                                   //
//  e-mail: miguel.ramos.pernas@cern.ch                            //
//                                                                 //
//  Last Update: 06/04/2016                                        //
//                                                                 //
// --------------------------------------------------------------- //
//                                                                 //
// Usage:                                                          //
//                                                                 //
// >> ./mergeFilesByEvents.out OutputFileName TreePath EvtVarName  //
//    InputFileName_1 InputFileName_2 ...                          //
//                                                                 //
// --------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////


#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"


int main( int argc, const char *argv[] ) {

  if ( argc < 6 ) {
    std::cout <<
      " Number of input parameters not valid.\n" <<
      " Input parameters have to be given as: " <<
      "< output file > < tree path > < event branch name > < input files >" <<
      std::endl;
    exit( 0 );
  }
  
  // Creates the output file
  TFile *outputFile = TFile::Open( argv[ 1 ], "RECREATE" );
  std::cout << " Created output file with name: < " << argv[ 1 ] << " >" << std::endl;

  // Number of input files
  int nfiles = argc - 4;
  
  // Lists to almacenate the input files and output trees
  TList
    *fileList = new TList(),
    *treeList = new TList();

  // Pointer to the iterator tree over the input files
  TTree *iterTree;

  // Gets all the trees from the input files
  std::cout << " Modifying input trees..." << std::endl;
  for ( int i = 0; i < nfiles; i++ ) {
    fileList -> Add( TFile::Open( argv[ i + 4 ] ) );
    iterTree = (TTree*) ( (TFile*) fileList -> At( i ) ) -> Get( argv[ 2 ] );
    if ( iterTree ) {
      treeList -> Add( iterTree );
      std::cout << " Added < " << argv[ i + 4 ] << " > to the list of input files" << std::endl;
    }
    else
      std::cout <<
	" Tree with path < " << argv[ 2 ] <<
	" > does not exist in file < " << fileList -> At( i ) -> GetName() << " >" << 
	std::endl;
  }
  
  // Merges the output trees
  std::cout << " Merging " << treeList -> GetSize() << " trees..." << std::endl;
  outputFile -> cd();
  TTree *outputTree = TTree::MergeTrees( treeList );
  std::cout << " Cloned " <<  outputTree -> GetEntries() << " entries" << std::endl;
  outputTree -> Write();

  // Creates the event branch again from the old ones
  Long64_t evtIndex = 0, evt;
  std::string evtTitle = argv[ 3 ];
  evtTitle += "/I";
  TBranch *evtBranch = outputTree -> Branch( argv[ 3 ], &evt, evtTitle.c_str() );
  TLeaf   *evtLeaf;

  std::cout << " Rewriting < " << argv[ 3 ] << " > branch..." << std::endl;
  for ( int i = 0; i < treeList -> GetSize(); i++ ) {
    iterTree = (TTree*) treeList -> At( i );
    iterTree -> SetBranchStatus( argv[ 3 ], true );
    evtLeaf = iterTree -> GetLeaf( argv[ 3 ] );
    for ( int ievt = 0; ievt < iterTree -> GetEntries(); ievt++ ) {
      iterTree -> GetEntry( ievt );
      evt = evtIndex + evtLeaf -> GetValue();
      evtBranch -> Fill();
    }
    evtIndex = iterTree -> GetMaximum( argv[ 3 ] ) + 1;
  }
  outputTree -> AutoSave();

  // Closes the files and deletes the lists
  for ( int i = 0; i < nfiles; i++ )
    ( (TFile*) fileList -> At( i ) ) -> Close();
  delete treeList;
  delete fileList;

  // Finishes the process
  outputFile -> Close();
  std::cout << " Finished!" << std::endl;
}
