//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#include "Messenger.h"
#include "TreeManagement.h"
#include "Utils.h"

#include "TBranch.h"
#include "TDirectory.h"
#include "TLeaf.h"
#include "TObjArray.h"

#include <iostream>
#include <map>
#include <utility>


//_______________________________________________________________________________
//
size_t Analysis::GetBranchNames( std::vector<std::string> &vector,
				 TTree *inputTree,
				 const std::string &expr ) {
  
  TObjArray *brList = inputTree -> GetListOfBranches();
  std::vector<std::string> brVector( brList -> GetEntries() );
  for ( int ibr = 0; ibr < brList -> GetEntries(); ++ibr )
    brVector[ ibr ] = brList -> At( ibr ) -> GetName();
  
  size_t n = vector.size();
  if ( expr.size() )
    General::StringVectorFilter( vector, brVector, expr );
  else
    vector.insert( vector.end(), brVector.begin(), brVector.end() );
  
  return vector.size() - n;
}

//_______________________________________________________________________________
//
size_t Analysis::GetBranchTitles( std::vector<std::string> &vector,
				  TTree *inputTree,
				  const std::string &expr ) {
  
  TObjArray *brList = inputTree -> GetListOfBranches();
  std::vector<std::string> brVector( brList -> GetEntries() );
  for ( int ibr = 0; ibr < brList -> GetEntries(); ++ibr )
    brVector[ ibr ] = brList -> At( ibr ) -> GetTitle();

  size_t n = vector.size();
  if ( expr.size() )
    General::StringVectorFilter( vector, brVector, expr );
  else
    vector.insert( vector.end(), brVector.begin(), brVector.end() );

  return vector.size() - n;
}

//_______________________________________________________________________________
//
size_t Analysis::GetNvarsWithExpr( TTree *inputTree, const std::string &expr ) {
  
  std::vector<std::string> vector;
  Analysis::GetBranchNames( vector, inputTree, expr );
  return vector.size();
}

//_______________________________________________________________________________
//
size_t Analysis::GetNvarsWithType( TTree *inputTree, const char &type ) {
  
  TObjArray *brList = inputTree -> GetListOfBranches();
  std::string title;
  size_t      counter( 0 );
  for ( int ibr = 0; ibr < brList -> GetEntries(); ++ibr ) {
    title = brList -> At( ibr ) -> GetTitle();
    if ( title.back() == type )
      ++counter;
  }
  return counter;
}

//_______________________________________________________________________________
//
size_t Analysis::GetNvarsWithTypeIn( TTree *inputTree,
				     const char &type,
				     const std::vector<std::string> &vector ) {
  
  TObjArray *brList = inputTree -> GetListOfBranches();
  std::string title;
  size_t      counter( 0 );
  for ( auto it = vector.begin(); it != vector.end(); ++it ) {
    title = brList -> FindObject( it -> c_str() ) -> GetTitle();
    if ( title.back() == type )
      ++counter;
  }
  return counter;
}

//_______________________________________________________________________________
//
char Analysis::GetVarType( TTree *inputTree, const std::string &var ) {
  
  TObjArray *brList = inputTree -> GetListOfBranches();
  TObject *obj = brList -> FindObject( var.c_str() );
  if ( !obj )
    IError << "Unable to get branch with name < " << var << " >" << IEndMsg;
  std::string type = obj -> GetTitle();
  type.replace( 0, var.length(), "" );
  return type[ 1 ];
}

//_______________________________________________________________________________
//
void Analysis::MakeTreeChangingNames( TTree *inputTree,
				      const std::vector<std::string> &ivars,
				      const std::vector<std::string> &ovars ) {

  // Sends a warning message if the number of input variables is different to
  // the number of output variables
  if ( ivars.size() != ovars.size() ) {
    IBegMsg << " -- Number of input and output variables doesn't match --" << IEndMsg;
    exit( 0 );
  }

  IBegMsg << " Changing variables in tree: < "   << inputTree  -> GetName()
	  << " >" << IEndMsg;
  IBegMsg << " Output tree will be saved in: < " << gDirectory -> GetName()
	  << " >" << IEndMsg;

  // Deactivates the branches of the input variables
  for ( auto it = ivars.begin(); it != ivars.end(); ++it )
    inputTree -> SetBranchStatus( it -> c_str(), false );

  // Clones the input tree
  TTree *outputTree = inputTree -> CloneTree();

  // Writes the output tree to avoid link errors
  outputTree -> Write();

  // Reactivates the branches of the input variables
  for ( auto it = ivars.begin(); it != ivars.end(); ++it )
    inputTree -> SetBranchStatus( it -> c_str(), true );

  // Generates a set of vectors to almacenate the data
  std::vector< std::pair<float , TLeaf*> >
    fvector( Analysis::GetNvarsWithTypeIn( inputTree, 'F', ivars ) );
  std::vector< std::pair<double, TLeaf*> >
    dvector( Analysis::GetNvarsWithTypeIn( inputTree, 'D', ivars ) );
  std::vector< std::pair<int   , TLeaf*> >
    ivector( Analysis::GetNvarsWithTypeIn( inputTree, 'I', ivars ) );
  std::vector< std::pair<bool  , TLeaf*> >
    bvector( Analysis::GetNvarsWithTypeIn( inputTree, 'O', ivars ) );

  IBegMsg << " Variables to change:" << IEndMsg;
  IBegMsg << " * Float:  \t" << fvector.size() << IEndMsg;
  IBegMsg << " * Double: \t" << dvector.size() << IEndMsg;
  IBegMsg << " * Integer:\t" << ivector.size() << IEndMsg;
  IBegMsg << " * Boolean:\t" << bvector.size() << IEndMsg;

  // Loops over all the variables generating the corresponding branches
  std::vector<TBranch*> brList( ovars.size() );
  std::string           title;
  TLeaf                *leaf;
  size_t                pos;
  for ( size_t i = 0; i != ivars.size(); ++i ) {
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
  for ( Long64_t ievt = 0; ievt < inputTree -> GetEntries(); ++ievt ) {

    inputTree -> GetEntry( ievt );

    for ( fit = fvector.begin(); fit != fvector.end(); ++fit )
      fit -> first = fit -> second -> GetValue();
    for ( dit = dvector.begin(); dit != dvector.end(); ++dit )
      dit -> first = dit -> second -> GetValue();
    for ( iit = ivector.begin(); iit != ivector.end(); ++iit )
      iit -> first = iit -> second -> GetValue();
    for ( bit = bvector.begin(); bit != bvector.end(); ++bit )
      bit -> first = bit -> second -> GetValue();

    for ( brit = brList.begin(); brit != brList.end(); ++brit )
      (*brit) -> Fill();

    if ( ievt % 100000 == 0 )
      outputTree -> AutoSave();
  }

  // Writes the output tree in the current directory
  outputTree -> AutoSave();
  IBegMsg << " Output tree < " << outputTree -> GetName() << " > saved" << IEndMsg;
}

//_______________________________________________________________________________
//
TTree* Analysis::MakeTreeConvertingVars( TTree *inputTree, const char &itype ) {

  TObjArray *brList = inputTree -> GetListOfBranches();

  std::map<std::string, std::pair<float, double> > vars;
  std::string brname, brtitle, vtype, tail;
  size_t nvars = 0;

  // Checks the type of conversion that is going to be made
  if ( itype == 'F' ) {
    IBegMsg << "Converting variables from float to double in tree < " <<
      inputTree -> GetName() << " >" << IEndMsg;
    tail = "/F";
    vtype = "float";
  }
  else {
    IBegMsg << "Converting variables from double to float in tree < " <<
      inputTree -> GetName() << " >" << IEndMsg;
    tail = "/D";
    vtype = "double";
  }

  // Searches for the variables of the input type
  for ( size_t ibr = 0; ibr < (size_t) brList -> GetEntries(); ++ibr ) {
    brname  = brList -> At( ibr ) -> GetName();
    brtitle = brList -> At( ibr ) -> GetTitle();
    if ( brtitle.find( tail ) != std::string::npos ) {
      inputTree -> SetBranchStatus( brname.c_str(), false );
      vars[ brname ] = std::make_pair( 0, 0 );
      ++nvars;
    }
  }
  IBegMsg << "Found " << nvars << " " << vtype << " variables" << IEndMsg;

  IBegMsg << "Cloning input tree" << IEndMsg;
  TTree *outputTree = inputTree -> CloneTree( 0 );
  
  if ( !nvars )
    IWarning << "Number of " << vtype <<
      " variables is zero. It will be saved a clone of the input tree." << IEndMsg;

  inputTree -> SetBranchStatus( "*", true );
  
  // Depending on the input type the address from the TTree is changed
  if ( itype == 'F' )
    for ( auto it = vars.begin(); it != vars.end(); ++it ) {
      brname  = it -> first;
      brtitle = brname + "/D";
      inputTree  ->
	SetBranchAddress( brname.c_str(), &( it -> second ).first );
      outputTree ->
	Branch( brname.c_str(), &( it -> second ).second, brtitle.c_str() );
    }
  else
    for ( auto it = vars.begin(); it != vars.end(); ++it ) {
      brname  = it -> first;
      brtitle = brname + "/F";
      inputTree  ->
	SetBranchAddress( brname.c_str(), &( it -> second ).second );
      outputTree ->
	Branch( brname.c_str(), &( it -> second ).first, brtitle.c_str() );
    }

  IBegMsg << "Constructing the double variables" << IEndMsg;
  std::pair<float, double> *pair;

  // Performs the loop over the input tree to fill the output variables depending
  // on the input variable type
  if ( itype == 'F' )
    for ( size_t ievt = 0; ievt < ( size_t ) inputTree -> GetEntries(); ++ievt ) {
      inputTree -> GetEntry( ievt );
      for ( auto it = vars.begin(); it != vars.end(); ++it ) {
	pair = &( it -> second );
	pair -> second = pair -> first;
      }
      outputTree -> Fill();
      if ( ievt % 100000 == 0 )
	outputTree -> AutoSave();
    }
  else
    for ( size_t ievt = 0; ievt < ( size_t ) inputTree -> GetEntries(); ++ievt ) {
      inputTree -> GetEntry( ievt );
      for ( auto it = vars.begin(); it != vars.end(); ++it ) {
	pair = &( it -> second );
	pair -> first = pair -> second;
      }
      outputTree -> Fill();
      if ( ievt % 100000 == 0 )
	outputTree -> AutoSave();
    }

  outputTree -> AutoSave();
  IBegMsg << "Saved output tree < " << outputTree -> GetName() << " > in directory < "
	  << gDirectory -> GetName() << " >" << IEndMsg;

  return outputTree;
}
