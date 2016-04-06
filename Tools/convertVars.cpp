//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//                                                                          //
//  Tool that allows to make a tree with the type of variables changed      //
//  between float <-> double                                                //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
//  AUTHOR : Miguel Ramos Pernas                                            //
//  e-mail: miguel.ramos.pernas@cern.ch                                     //
//                                                                          //
//  Last Update: 06/04/2016                                                 //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// Usage:                                                                   //
//                                                                          //
// >> ./convertVars.out OutputFileName InputFileName InputTreePath VarType  //
//                                                                          //
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////


#include "TreeManagement.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>


int main( const int argc, const char *argv[] ) {

  // If the number of entries does not match the required, a message is sent and the
  // program is finished
  if ( argc != 4 && argc != 5 ) {
    std::cout <<
      "The input parameters are < output file > < input file > < tree name > < var type >"
	      << std::endl;
      return 0;
  }

  TFile
    *inputFile  = TFile::Open( argv[ 2 ], "READ" ),
    *outputFile = TFile::Open( argv[ 1 ], "RECREATE" );

  TTree *inputTree = ( TTree* ) inputFile -> Get( argv[ 3 ] );
  std::cout << "Preparing to convert variables in tree < " << 
    inputTree -> GetName() << " > from file < " << inputFile -> GetName() << " >" << std::endl;
  std::cout << "Output file set to < " << outputFile -> GetName() << " >" << std::endl;

  // By default it is assumed that one wants to change the input variables from float to double
  if ( argc == 4 )
    Analysis::MakeTreeConvertingVars( inputTree, 'F' );
  else
    if ( !strcmp( argv[ 4 ], "F" ) )
      Analysis::MakeTreeConvertingVars( inputTree, 'F' );
    else
      Analysis::MakeTreeConvertingVars( inputTree, 'D' );

  inputFile  -> Close();
  outputFile -> Close();
  std::cout << "Finished!" << std::endl;
  return 0;
}
