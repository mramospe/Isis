///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the abstract class to defines adaptive bins. Each
//  been has defined the limits, the number of entries and the sum
//  of weights.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef __BIN__
#define __BIN__

#include <iostream>


//______________________________________________________________________________

namespace Isis {

  class Bin {

  public:

    // Constructor
    Bin();

    // Destructor
    virtual ~Bin();
    
    // Return the number of entries stored in the bin
    inline const size_t GetEntries() const;

    // Return the sum of the weights of the entries in the bin
    inline const size_t GetSumOfWeights() const;

  protected:

    // Entries in the bin
    size_t fEntries;

    // Sum of weights in the bin
    double fSumOfWeights;

  };

  //______________________________________________________________________________
  //
  inline const size_t Bin::GetEntries() const { return fEntries; }

  //______________________________________________________________________________
  //
  inline const size_t Bin::GetSumOfWeights() const { return fSumOfWeights; }

}

#endif
