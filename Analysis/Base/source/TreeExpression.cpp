///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 09/11/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the classes to make calculations among variables from a tree
//  given a expression. The < TreeBoolExpression > class allows to evaluate a
//  given expression for each tree entry using the < Evaluate > method. On the
//  other hand, the < TreeDoubleExpression > class allows to calculate the
//  value of an expression for a tree entry using the < Calculate > method. To
//  compute the quantities, one has to loop over the input tree. When getting
//  an entry, the aforementioned methods allow to calculate the expressions.
//  these classes are usefull when one doesn't want to calculate the variables
//  by hand ( creating all the subsequent calculations ) and doesn't want to
//  create new branches neither.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "TreeExpression.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "StringParser.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR FOR THE "TreeExpression" CLASS

//_______________________________________________________________________________
// Main constructor
Analysis::TreeExpression::TreeExpression( std::string expr, TTree *itree ) :
  fExpression( expr ) {

  std::vector<size_t> treeEntries;
  TObjArray *brlist = itree -> GetListOfBranches();

  // Gets all the posible branches that could appear in the expression
  std::string brname;
  size_t backpos, pos;
  std::vector<std::pair<std::string, size_t> > brvect;
  for ( long int ibr = 0; ibr < brlist -> GetEntries(); ibr++ ) {
    brname  = brlist -> At( ibr ) -> GetName();
    backpos = 0;
    while ( ( pos = fExpression.find( brname, backpos ) ) != std::string::npos ) {
      brvect.push_back( std::make_pair( brname, pos ) );
      backpos = pos + brname.size();
    }
  }

  // If no variables are found in the expression given, an error message is sent
  if ( !brvect.size() )
    throw std::logic_error( "ERROR: No variables found in the current expression" );

  // Sorts the variables, so the largest appear first
  std::sort( brvect.begin(), brvect.end(),
	     [] ( std::pair<std::string, size_t> it1,
		  std::pair<std::string, size_t> it2 ) {
	       return it1.first.size() > it2.first.size(); } );

  // Gets only the true variables that appear in the input expression
  for ( auto it = brvect.begin(); it != brvect.end(); it++ )
    if ( ( pos = fExpression.find( it -> first ) ) != std::string::npos )
      fExpression.replace( pos, ( it -> first ).size(), "%%%" );
    else {
      brvect.erase( it );
      it--;
    }

  // Sorts the branches given the position in the expression
  std::sort( brvect.begin(), brvect.end(), [] ( std::pair<std::string, size_t> it1,
						std::pair<std::string, size_t> it2 ) {
	       return it1.second < it2.second; } );

  // Creates the vector of pointers
  fVarPtrs = std::vector<std::pair<void*, char> >( brvect.size() );

  // Gets the pointers to the different variables, saving their type
  std::string brtitle;
  auto itp = fVarPtrs.begin();
  for ( auto itb = brvect.begin(); itb != brvect.end(); itb++, itp++ ) {
    brname  = itb -> first;
    itree -> SetBranchStatus( brname.c_str(), true );
    brtitle = itree -> GetBranch( brname.c_str() ) -> GetTitle();
    if ( brtitle.find( "/O" ) != std::string::npos )
      *itp = std::make_pair( itree -> GetLeaf( brname.c_str() ) -> GetValuePointer(), 'O' );
    else if ( brtitle.find( "/D" ) != std::string::npos )
      *itp = std::make_pair( itree -> GetLeaf( brname.c_str() ) -> GetValuePointer(), 'D' );
    else if ( brtitle.find( "/F" ) != std::string::npos )
      *itp = std::make_pair( itree -> GetLeaf( brname.c_str() ) -> GetValuePointer(), 'F' );
    else if ( brtitle.find( "/I" ) != std::string::npos )
      *itp = std::make_pair( itree -> GetLeaf( brname.c_str() ) -> GetValuePointer(), 'I' );
    else
      throw std::logic_error( "ERROR: Type for leaf < " + brname + " > not found" );
  }
}

//_______________________________________________________________________________
// Destructor
Analysis::TreeExpression::~TreeExpression() { }

//_______________________________________________________________________________


// -- STATIC MEMBERS FOR THE "TreeExpression" CLASS

//_______________________________________________________________________________
// Checks that the current expression can be correctly calculated, looking for
// the branch names in the given tree
bool Analysis::TreeExpression::CheckCalcExpression( std::string expr, TTree *itree ) {
  TreeExpression::ReplaceBranchNames( expr, itree );
  return General::StringParser::CheckCalcExpression( expr );
}

//_______________________________________________________________________________
// Checks that the current expression can be correctly evaluated, looking for
// the branch names in the given tree
bool Analysis::TreeExpression::CheckEvalExpression( std::string expr, TTree *itree ) {
  TreeExpression::ReplaceBranchNames( expr, itree );
  return General::StringParser::CheckEvalExpression( expr );
}

//_______________________________________________________________________________
// Replaces the names of the branches located in the given expression by a < 0 >
void Analysis::TreeExpression::ReplaceBranchNames( std::string &expr, TTree *itree ) {

  std::vector<size_t> treeEntries;
  TObjArray *brlist = itree -> GetListOfBranches();

  // Gets all the posible branches that could appear in the expression
  std::string brname;
  size_t backpos, pos;
  std::vector<std::string> brvect;
  for ( long int ibr = 0; ibr < brlist -> GetEntries(); ibr++ ) {
    brname  = brlist -> At( ibr ) -> GetName();
    backpos = 0;
    while ( ( pos = expr.find( brname, backpos ) ) != std::string::npos ) {
      brvect.push_back( brname );
      backpos = pos + brname.size();
    }
  }

  // If no variables are found in the expression given, an error message is sent
  if ( !brvect.size() )
    throw std::logic_error( "ERROR: No variables found in the current expression" );

  // Sorts the variables, so the largest appear first
  std::sort( brvect.begin(), brvect.end(),
	     [] ( std::string it1, std::string it2 ) {
	       return it1.size() > it2.size(); } );

  // Gets only the true variables that appear in the input expression
  for ( auto it = brvect.begin(); it != brvect.end(); it++ )
    if ( ( pos = expr.find( *it ) ) != std::string::npos )
      expr.replace( pos, it -> size(), "0" );
    else {
      brvect.erase( it );
      it--;
    }
}

//_______________________________________________________________________________


// -- CONSTRUCTOR, DESTRUCTOR AND METHOD FOR THE "TreeBoolExpression" CLASS

//_______________________________________________________________________________
// After constructing the TreeExpression class, checks that the given expression
// is correctly written
Analysis::TreeBoolExpression::TreeBoolExpression( std::string expr, TTree *itree, bool check ) 
  : TreeExpression( expr, itree ) {
  if ( check )
    if ( !General::StringParser::CheckEvalExpression( fExpression, "%%%" ) )
      throw std::invalid_argument( "ERROR: Expression not valid: " + fExpression );
}

//_______________________________________________________________________________
// Destructor
Analysis::TreeBoolExpression::~TreeBoolExpression() { }

//_______________________________________________________________________________
// Evaluates the attached expression
bool Analysis::TreeBoolExpression::Evaluate() {

  size_t pos, backpos = 0;
  std::stringstream sout;

  for ( auto itp = fVarPtrs.begin(); itp != fVarPtrs.end(); itp++ ) {
    pos = fExpression.find( "%%%", backpos );
    sout << fExpression.substr( backpos, pos - backpos );
    if ( itp -> second == 'O' )
      sout << *static_cast<bool*>( itp -> first );
    else if ( itp -> second == 'D' )
      sout << *static_cast<double*>( itp -> first );
    else if ( itp -> second == 'F' )
      sout << *static_cast<float*>( itp -> first );
    else
      sout << *static_cast<int*>( itp -> first );
    backpos = pos + 3;
  }
  sout << fExpression.substr( backpos );
  return General::StringParser::BareEvaluate( sout.str() );
}

//_______________________________________________________________________________


// -- CONSTRUCTOR, DESTRUCTOR AND METHOD FOR THE "TreeDoubleExpression" CLASS

//_______________________________________________________________________________
// After constructing the TreeExpression class, checks that the given expression
// is correctly written
Analysis::TreeDoubleExpression::TreeDoubleExpression( std::string expr, TTree *itree, bool check ) 
  : TreeExpression( expr, itree ) {
  if ( check )
    if ( !General::StringParser::CheckCalcExpression( fExpression, "%%%" ) )
      throw std::invalid_argument( "ERROR: Expression not valid: " + fExpression );
}

//_______________________________________________________________________________
// Destructor
Analysis::TreeDoubleExpression::~TreeDoubleExpression() { }

//_______________________________________________________________________________
// Calculates the value of the attached expression
double Analysis::TreeDoubleExpression::Calculate() {

  size_t pos, backpos = 0;
  std::stringstream sout;

  for ( auto itp = fVarPtrs.begin(); itp != fVarPtrs.end(); itp++ ) {
    pos = fExpression.find( "%%%", backpos );
    sout << fExpression.substr( backpos, pos - backpos );
    if ( itp -> second == 'O' )
      sout << *static_cast<bool*>( itp -> first );
    else if ( itp -> second == 'D' )
      sout << *static_cast<double*>( itp -> first );
    else if ( itp -> second == 'F' )
      sout << *static_cast<float*>( itp -> first );
    else
      sout << *static_cast<int*>( itp -> first );
    backpos = pos + 3;
  }
  sout << fExpression.substr( backpos );
  return General::StringParser::BareCalculate( sout.str() );
}
