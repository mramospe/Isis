///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Abstract class to add priors to the CLs method
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_PRIOR
#define CLS_PRIOR

#include <iostream>


//_______________________________________________________________________________

namespace Isis {

  struct CLsPrior {

  public:

    // Constructor
    CLsPrior() { };

    // Virtual destructor
    virtual ~CLsPrior() { };

    // Evaluate the prior given the position, the mean and the given value from the
    // observation
    virtual double Evaluate( const size_t &pos,
			     const double &mean,
			     const double &value ) = 0;

  };

}

#endif
