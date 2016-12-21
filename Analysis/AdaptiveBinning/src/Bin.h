///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 12/01/2016
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

namespace Analysis {

  class Bin {

  public:

    // Constructor and destructor
    Bin();
    virtual ~Bin();
    
    // Inline methods
    inline const size_t GetEntries() const;
    inline const size_t GetSumOfWeights() const;

  protected:

    // Attributes
    size_t fEntries;
    double fSumOfWeights;

  };

  //_______________
  // INLINE METHODS

  // Returns the number of entries stored in the bin
  inline const size_t Bin::GetEntries() const { return fEntries; }
  // Returns the sum of the weights of the entries in the bin
  inline const size_t Bin::GetSumOfWeights() const { return fSumOfWeights; }

}

#endif
