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


#ifndef TREE_EXPRESSION
#define TREE_EXPRESSION

#include "TTree.h"

#include <string>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class TreeExpression {

  public:

    // Constructor and destructor
    TreeExpression( std::string expr, TTree *itree );
    virtual ~TreeExpression();

    // Static methods
    static bool CheckCalcExpression( std::string expr, TTree *itree );
    static bool CheckEvalExpression( std::string expr, TTree *itree );

  protected:
    std::string                          fExpression;
    std::vector<std::pair<void*, char> > fVarPtrs;

  private:
    static void ReplaceBranchNames( std::string &expr, TTree *itree );
  };


  class TreeBoolExpression: public TreeExpression {

  public:

    // Constructor and destructor
    TreeBoolExpression( std::string expr, TTree *itree, bool check = true );
    ~TreeBoolExpression();
    
    // Method
    bool Evaluate();
  };


  class TreeDoubleExpression: public TreeExpression {

  public:

    // Constructor and destructor
    TreeDoubleExpression( std::string expr, TTree *itree, bool check = true );
    ~TreeDoubleExpression();
    
    // Method
    double Calculate();
  };

}

#endif
