//////////////////////////////////////////////////////////
//                                                      //
//  Analysis package                                    //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  AUTHOR: Miguel Ramos Pernas                         //
//  e-mail: miguel.ramos.pernas@cern.ch                 //
//                                                      //
//  Last update: 30/09/2015                             //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  Description:                                        //
//                                                      //
//  Here are defined different tools to manage TTree    //
//  objects.                                            //
//                                                      //
// ---------------------------------------------------- //
//////////////////////////////////////////////////////////


#include "TBranch.h"
#include "TLeaf.h"
#include "TObjArray.h"

#include "TreeManagement.h"

#include <iostream>


//_______________________________________________________________________________
// Gets all the variables whose name contains a given keyword
std::vector<std::string> Analysis::GetAllVarsWith( TTree *inputTree, std::string kw ) {

  std::string  iname;
  TObjArray   *brList = inputTree -> GetListOfBranches();

  std::vector<std::string> ovector;
  
  for ( int ibr = 0; ibr < brList -> GetSize(); ibr++ ) {
    iname = brList -> At( ibr ) -> GetName();
    if ( iname.find( kw ) != std::string::npos )
      ovector.push_back( iname );
  }

  return ovector;
}

//_______________________________________________________________________________
// Returns a vector that contains all the names of the branches in a tree
std::vector<std::string> Analysis::GetBranchNames( TTree *inputTree ) {

  TObjArray *brList = inputTree -> GetListOfBranches();

  std::vector<std::string> brVector( brList -> GetSize() );

  for ( int ibr = 0; ibr < brList -> GetSize(); ibr++ )
    brVector[ ibr ] = brList -> At( ibr ) -> GetName();

  return brVector;
}

//_______________________________________________________________________________
// Returns a vector that contains all the titles of the branches in a tree
std::vector<std::string> Analysis::GetBranchTitles( TTree *inputTree ) {

  TObjArray *brList = inputTree -> GetListOfBranches();

  std::vector<std::string> brVector( brList -> GetSize() );

  for ( int ibr = 0; ibr < brList -> GetSize(); ibr++ )
    brVector[ ibr ] = brList -> At( ibr ) -> GetTitle();

  return brVector;
}

//_______________________________________________________________________________
// Gets the type of a variable in a tree
std::string Analysis::GetVarType( TTree *inputTree, std::string var ) {

  TObjArray *brList = inputTree -> GetListOfBranches();

  std::string type = brList -> FindObject( var.c_str() ) -> GetTitle();
  type.replace( 0, var.length(), "" );

  return type;
}

//_______________________________________________________________________________
// Gets the number of variables in a tree whose name contains a given keyword
size_t Analysis::GetNvarsWithString( TTree *inputTree, std::string kw ) {

  TObjArray *brList = inputTree -> GetListOfBranches();
  std::string title;
  size_t      counter( 0 );
  for ( int ibr = 0; ibr < brList -> GetSize(); ibr++ ) {
    title = brList -> At( ibr ) -> GetTitle();
    if ( title.find( kw ) != std::string::npos )
      counter++;
  }

  return counter;
}

//_______________________________________________________________________________
// Returns the number of variables of a given type
size_t Analysis::GetNvarsWithType( TTree *inputTree, std::string type ) {

  if ( type.find( '/' ) == std::string::npos )
    type = '/' + type;

  TObjArray *brList = inputTree -> GetListOfBranches();
  std::string title;
  size_t      counter( 0 );
  for ( int ibr = 0; ibr < brList -> GetSize(); ibr++ ) {
    title = brList -> At( ibr ) -> GetTitle();
    if ( title.find( type ) != std::string::npos )
      counter++;
  }

  return counter;
}

//_______________________________________________________________________________
// Returns the number of variables of a certain type in a given vector and tree
size_t Analysis::GetNvarsWithTypeIn( TTree *inputTree,
				     std::string type,
				     std::vector<std::string> &vector ) {

  if ( type.find( '/' ) == std::string::npos )
    type = '/' + type;

  TObjArray *brList = inputTree -> GetListOfBranches();
  std::string title;
  size_t      counter( 0 );

  for ( std::vector<std::string>::iterator it = vector.begin();
	it != vector.end();
	it++ ) {
    title = brList -> FindObject( it -> c_str() ) -> GetTitle();
    if ( title.find( type ) != std::string::npos )
      counter++;
  }

  return counter;
}

//_______________________________________________________________________________
// Makes and saves a new tree with a given set of names changed by anothers. The
// output tree is going to be saved in the current directory, so an output file
// must be opened first.
void Analysis::MakeTreeChangingNames( TTree *inputTree,
				      std::vector<std::string> ivars,
				      std::vector<std::string> ovars ) {

  // Sends a warning message if the number of input variables is different to
  // the number of output variables
  if ( ivars.size() != ovars.size() ) {
    std::cout << " -- Number of input and output variables doesn't match --" << std::endl;
    exit( 0 );
  }

  std::cout << " Changing variables in tree: < " << inputTree -> GetName() << " >" << std::endl;

  // Deactivates the branches of the input variables
  for ( std::vector<std::string>::iterator it = ivars.begin();
	it != ivars.end();
	it++ )
    inputTree -> SetBranchStatus( it -> c_str(), false );

  // Clones the input tree
  TTree *outputTree = inputTree -> CloneTree();

  // Writes the output tree to avoid link errors
  outputTree -> Write();

  // Reactivates the branches of the input variables
  for ( std::vector<std::string>::iterator it = ivars.begin();
	it != ivars.end();
	it++ )
    inputTree -> SetBranchStatus( it -> c_str(), true );

  // Generates a set of vectors to almacenate the data
  std::vector< std::pair<float , TLeaf*> >
    fvector( GetNvarsWithTypeIn( inputTree, "/F", ivars ) );
  std::vector< std::pair<double, TLeaf*> >
    dvector( GetNvarsWithTypeIn( inputTree, "/D", ivars ) );
  std::vector< std::pair<int   , TLeaf*> >
    ivector( GetNvarsWithTypeIn( inputTree, "/I", ivars ) );
  std::vector< std::pair<bool  , TLeaf*> >
    bvector( GetNvarsWithTypeIn( inputTree, "/B", ivars ) );

  std::cout << " Variables to change:" << std::endl;
  std::cout << " - Float:  \t" << fvector.size() << std::endl;
  std::cout << " - Double: \t" << dvector.size() << std::endl;
  std::cout << " - Integer:\t" << ivector.size() << std::endl;
  std::cout << " - Boolean:\t" << bvector.size() << std::endl;

  // Loops over all the variables generating the corresponding branches
  std::vector<TBranch*> brList( ovars.size() );
  std::string           title;
  TLeaf                *leaf;
  size_t                pos;
  for ( size_t i = 0; i != ivars.size(); i++ ) {
    leaf  = inputTree -> GetLeaf( ivars[ i ].c_str() );
    title = inputTree -> GetBranch( ivars[ i ].c_str() ) -> GetTitle();
    if ( ( pos = title.find( "/F" ) ) != std::string::npos ) {
      fvector[ i ].second = leaf;
      title.replace( 0, pos, ovars[ i ] );
      brList[ i ] = outputTree -> Branch( ovars[ i ].c_str(),
					  &fvector[ i ].first,
					  title.c_str() );
    }
    else if ( ( pos = title.find( "/D" ) ) != std::string::npos ) {
      dvector[ i ].second = leaf;
      title.replace( 0, pos, ovars[ i ] );
      brList[ i ] = outputTree -> Branch( ovars[ i ].c_str(),
					  &dvector[ i ].first,
					  title.c_str() );
    }
    else if ( ( pos = title.find( "/I" ) ) != std::string::npos ) {
      ivector[ i ].second = leaf;
      title.replace( 0, pos, ovars[ i ] );
      brList[ i ] = outputTree -> Branch( ovars[ i ].c_str(),
					  &ivector[ i ].first,
					  title.c_str() );
    }
    else if ( ( pos = title.find( "/B" ) ) != std::string::npos ) {
      bvector[ i ].second = leaf;
      title.replace( 0, pos, ovars[ i ] );
      brList[ i ] = outputTree -> Branch( ovars[ i ].c_str(),
					  &bvector[ i ].first,
					  title.c_str() );
    }
  }

  // Iterates through the input tree to fill the branches
  std::vector< std::pair<float , TLeaf*> >::iterator fit;
  std::vector< std::pair<double, TLeaf*> >::iterator dit;
  std::vector< std::pair<int   , TLeaf*> >::iterator iit;
  std::vector< std::pair<bool  , TLeaf*> >::iterator bit;
  std::vector< TBranch* >::iterator brit;
  for ( Long64_t ievt = 0; ievt < inputTree -> GetEntries(); ievt++ ) {

    inputTree -> GetEntry( ievt );

    for ( fit = fvector.begin(); fit != fvector.end(); fit++ )
      fit -> first = fit -> second -> GetValue();
    for ( dit = dvector.begin(); dit != dvector.end(); dit++ )
      dit -> first = dit -> second -> GetValue();
    for ( iit = ivector.begin(); iit != ivector.end(); iit++ )
      iit -> first = iit -> second -> GetValue();
    for ( bit = bvector.begin(); bit != bvector.end(); bit++ )
      bit -> first = bit -> second -> GetValue();

    for ( brit = brList.begin(); brit != brList.end(); brit++ )
      (*brit) -> Fill();

    if ( ievt % 100000 == 0 )
      outputTree -> AutoSave();
  }

  // Writes the output tree in the current directory
  outputTree -> AutoSave();
  std::cout << " Output tree < " << outputTree -> GetName() << " > saved" << std::endl;
}
