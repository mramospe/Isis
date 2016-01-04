///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 04/01/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class TreeCategory. It allows to manage subsets of data       //
//  located in TTree objects that satisfy a given set of cuts. Also provides     //
//  a method to obtain a subset of the TreeCategory object itself, returning     //
//  the index of the entries.                                                    //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "StringParser.h"
#include "TreeCategory.h"

#include "TDirectory.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TLeaf.h"
#include "TObjArray.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::TreeCategory::TreeCategory( std::string name, TTree *itree, std::string cuts ) :
  fCuts( cuts ), fName( name ), fTree( itree ) {

  // Initializes the tree indices as an enumeration
  size_t n = 0;
  fTreeEntries = std::vector<size_t>( fTree -> GetEntries() );
  std::generate( fTreeEntries.begin(), fTreeEntries.end(), [ & ] { return n++; } );

  // If a set of cuts is providen it calculates the associated indices
  if ( cuts.size() )
    fTreeEntries = MakeSlice( fCuts );

  // Enables again all the branches
  fTree -> SetBranchStatus( "*", true );

  // Displays the information of the tree category
  std::cout << "*** Created new tree category < " << name << " > ***" << std::endl;
  std::cout << " - Directory:\t" << fTree -> GetDirectory() -> GetName() << std::endl;
  std::cout << " - Input tree:\t" << fTree -> GetName() << std::endl;
  if ( fCuts.size() )
    std::cout << " - Cuts:\t" << fCuts << std::endl;
  std::cout << " - Entries:\t" << fTreeEntries.size() << std::endl;
}

//_______________________________________________________________________________
// Constructor given another tree category
Analysis::TreeCategory::TreeCategory( std::string             name,
				      Analysis::TreeCategory &other,
				      std::string             cuts ) :
  fName( name ), fTree( other.GetTree() ) {

  fCuts = other.fCuts + " && " + cuts;
  fTreeEntries = other.MakeSlice( cuts );
}

//_______________________________________________________________________________
// Destructor
Analysis::TreeCategory::~TreeCategory() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Makes a new histogram for the variable, number of bins and range given. A set
// of cuts can be specified, together with the option to check it or not.
TH1* Analysis::TreeCategory::MakeHistogram( std::string var,
					    size_t      nbins,
					    double      vmin,
					    double      vmax,
					    std::string cuts,
					    bool        check ) {
  TH1 *hist;
  std::string
    hname   = ( var + "_" + fName ).c_str(),
    brtitle = fTree -> GetBranch( var.c_str() ) -> GetTitle();
  std::vector<size_t> ventries;
  char vtype;
  if ( cuts.size() )
    ventries = this -> MakeSlice( cuts, check );
  else
    ventries = fTreeEntries;

  // To boost the method, all the other branches are deactivated, and the pointer
  // to the value is taken
  fTree -> SetBranchStatus( "*", false );
  fTree -> SetBranchStatus( var.c_str(), true );
  void *value = fTree -> GetLeaf( var.c_str() ) -> GetValuePointer();

  // Checks for the variable type of the branch
  if ( brtitle.find( "/D" ) != std::string::npos ) {
    hist  = new TH1D( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
    vtype = 'D';
  }
  else if ( brtitle.find( "/F" ) != std::string::npos ) {
    hist  = new TH1F( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
    vtype = 'F';
  }
  else if ( brtitle.find( "/I" ) != std::string::npos ) {
    hist  = new TH1I( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
    vtype = 'I';
  }
  else {
    std::cerr << "ERROR: Type for leaf < " << var << " > not found" << std::endl;
    return 0;
  }

  // Depending on the variable type, a different static cast is made
  if ( vtype == 'D' )
    for ( auto it = ventries.begin(); it != ventries.end(); it++ ) {
      fTree -> GetEntry( *it );
      hist  -> Fill( *static_cast<double*>( value ) );
    }
  else if ( vtype == 'F' )
    for ( auto it = ventries.begin(); it != ventries.end(); it++ ) {
      fTree -> GetEntry( *it );
      hist  -> Fill( *static_cast<float*>( value ) );
    }
  else if ( vtype == 'I' )
    for ( auto it = ventries.begin(); it != ventries.end(); it++ ) {
      fTree -> GetEntry( *it );
      hist  -> Fill( *static_cast<int*>( value ) );
    }

  // Finally reactivates all the branches and returns the histogram
  fTree -> SetBranchStatus( "*", true );
  return hist;
}

//_______________________________________________________________________________
// Generates a vector with all the indices of this category that satisfy the
// given cuts
std::vector<size_t> Analysis::TreeCategory::MakeSlice( std::string cuts, bool check ) {

  std::vector<size_t> treeEntries;
  TObjArray *brlist = fTree -> GetListOfBranches();

  // Gets all the posible branches that could appear in the cuts. A vector of pairs
  // is constructed since it is easier to sort it by values than a map.
  std::string brname;
  size_t pos;
  std::vector<std::pair<std::string, size_t> > brvect;
  for ( long int ibr = 0; ibr < brlist -> GetSize(); ibr++ ) {
    brname = brlist -> At( ibr ) -> GetName();
    if ( ( pos = cuts.find( brname ) ) != std::string::npos )
      brvect.push_back( std::make_pair( brname, pos ) );
  }

  // If no variables are found in the cuts given, an error message is sent
  if ( !brvect.size() )
    std::cerr << "ERROR: No variables found in the current set of cuts" << std::endl;

  // Sorts the variables, so the largest appear first
  std::sort( brvect.begin(), brvect.end(),
	     [] ( std::pair<std::string, size_t> it1,
		  std::pair<std::string, size_t> it2 ) {
	       return it1.first.size() > it2.first.size(); } );

  // Gets only the true variables that appear in the cuts string
  std::string ccuts = cuts;
  for ( auto it = brvect.begin(); it != brvect.end(); it++ )
    if ( ( pos = ccuts.find( it -> first ) ) != std::string::npos )
      ccuts.replace( pos, ( it -> first ).size(), "%%%" );
    else {
      brvect.erase( it );
      it--;
    }

  // Sorts the branches given the position in the cuts
  std::sort( brvect.begin(), brvect.end(), [] ( std::pair<std::string, size_t> it1,
						std::pair<std::string, size_t> it2 ) {
	       return it1.second < it2.second; } );

  // Gets the pointers to the different variables, saving their type. Only the branches
  // of the variables to be used are enabled.
  std::map<void*, char> varsMap;
  std::string brtitle;
  fTree -> SetBranchStatus( "*", false );
  for ( auto it = brvect.begin(); it != brvect.end(); it++ ) {
    brname  = it -> first;
    fTree -> SetBranchStatus( brname.c_str(), true );
    brtitle = fTree -> GetBranch( brname.c_str() ) -> GetTitle();
    if ( brtitle.find( "/O" ) != std::string::npos )
      varsMap[ fTree -> GetLeaf( brname.c_str() ) -> GetValuePointer() ] = 'O';
    else if ( brtitle.find( "/D" ) != std::string::npos )
      varsMap[ fTree -> GetLeaf( brname.c_str() ) -> GetValuePointer() ] = 'D';
    else if ( brtitle.find( "/F" ) != std::string::npos )
      varsMap[ fTree -> GetLeaf( brname.c_str() ) -> GetValuePointer() ] = 'F';
    else if ( brtitle.find( "/I" ) != std::string::npos )
      varsMap[ fTree -> GetLeaf( brname.c_str() ) -> GetValuePointer() ] = 'I';
    else
      std::cerr << "ERROR: Type for leaf < " << brname << " > not found" << std::endl;
  }

  // If it is told to, checks the current expression
  if ( check )
    check = General::StringParser::CheckEvalExpression( ccuts, "%%%" );
  else
    check = true;

  // Either if the expression is correct or if it is not told to check it, performs the
  // calculation
  if ( check ) {

    // Loops over the tree to get the entries corresponding to this category
    size_t backpos;
    std::stringstream sout;

    for ( auto it = fTreeEntries.begin(); it != fTreeEntries.end(); it++ ) {
      fTree -> GetEntry( *it );
      sout.str( "" );
      backpos = 0;
      for ( auto vit = varsMap.begin(); vit != varsMap.end(); vit++ ) {
	pos = ccuts.find( "%%%", backpos );
	sout << ccuts.substr( backpos, pos - backpos );
	if ( vit -> second == 'O' )
	  sout << *static_cast<bool*>( vit -> first );
	else if ( vit -> second == 'D' )
	  sout << *static_cast<double*>( vit -> first );
	else if ( vit -> second == 'F' )
	  sout << *static_cast<float*>( vit -> first );
	else
	  sout << *static_cast<int*>( vit -> first );
	backpos = pos + 3;
      }
      sout << ccuts.substr( backpos );

      if ( General::StringParser::BareEvaluate( sout.str() ) )
	treeEntries.push_back( *it );
    }

  }
  else
    std::cerr << "ERROR: Expression for the category not valid: " << ccuts << std::endl;
  return treeEntries;
}
