///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 05/01/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class TreeCategory. It allows to manage subsets of data
//  located in TTree objects that satisfy a given set of cuts. Also provides
//  a method to obtain a subset of the TreeCategory object itself, returning
//  the index of the entries.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "StringParser.h"
#include "TreeCategory.h"
#include "TreeExpression.h"

#include "TDirectory.h"
#include "TH1D.h"
#include "TH1F.h"
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

  // Checks for the variable type of the branch. In case the variable is an int,
  // the generated histogram will have a double type.
  if ( brtitle.find( "/D" ) != std::string::npos ) {
    hist  = new TH1D( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
    vtype = 'D';
  }
  else if ( brtitle.find( "/F" ) != std::string::npos ) {
    hist  = new TH1F( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
    vtype = 'F';
  }
  else if ( brtitle.find( "/I" ) != std::string::npos ) {
    hist  = new TH1D( hname.c_str(), hname.c_str(), nbins, vmin, vmax );
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
// Makes a new histogram for a expression
TH1* Analysis::TreeCategory::MakeHistogram( std::string name,
					    std::string expr,
					    size_t      nbins,
					    double      vmin,
					    double      vmax,
					    std::string cuts,
					    bool        check ) {
  std::string hname = ( name + "_" + fName ).c_str();
  // The returned histogram is always double-valued
  TH1 *hist  = new TH1D( hname.c_str(), hname.c_str(), nbins, vmin, vmax );;
  std::vector<size_t> ventries;
  if ( cuts.size() )
    ventries = this -> MakeSlice( cuts, check );
  else
    ventries = fTreeEntries;
  Analysis::TreeDoubleExpression texpr( expr, fTree, check );
  for ( auto it = ventries.begin(); it != ventries.end(); it++ ) {
    fTree -> GetEntry( *it );
    hist  -> Fill( texpr.Calculate() );
  }
  return hist;
}

//_______________________________________________________________________________
// Generates a vector with all the indices of this category that satisfy the
// given cuts
std::vector<size_t> Analysis::TreeCategory::MakeSlice( std::string cuts, bool check ) {
  std::vector<size_t> treeEntries;
  Analysis::TreeBoolExpression texpr( cuts, fTree, check );
  for ( auto it = fTreeEntries.begin(); it != fTreeEntries.end(); it++ ) {
    fTree -> GetEntry( *it );
    if ( texpr.Evaluate() )
      treeEntries.push_back( *it );
  }
  return treeEntries;
}
