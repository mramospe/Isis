///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Abstract class to fluctuate the means of the CLs hypotheses
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_FLUCTUATOR
#define CLS_FLUCTUATOR

#include <iostream>


//_______________________________________________________________________________

namespace isis {

  struct CLsFluctuator {

  public:

    // Constructor
    CLsFluctuator() { };

    // Virtual destructor
    virtual ~CLsFluctuator() { };

    // Given the position and the value, fluctuate it using an user-defined function
    virtual double fluctuate( const size_t &pos, const double &value ) = 0;

  };
  
}

#endif
