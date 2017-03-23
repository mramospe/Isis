///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Main class to perform analysis using the CLs method
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_FACTORY
#define CLS_FACTORY

#include "CLsHypothesis.h"
#include "CLsPrior.h"
#include "CLsResult.h"
#include "Definitions.h"


//_______________________________________________________________________________

namespace Isis {

  class CLsFactory {

  public:

    // Constructor
    CLsFactory();

    // Constructor given the two hypothesis as arrays
    CLsFactory( CLsHypothesis &h0, CLsHypothesis &h1 );

    // Destructor
    ~CLsFactory();
    
    // Return the p-value of the null hypothesis
    inline double alpha( const double &t ) const;

    // Return the p-value of the signal hypothesis
    inline double beta( const double &t ) const;

    // Return information concerning the CLs method given the test statistics value
    CLsResult calculate( const double &tstat ) const;
    
    // Return information concerning the CLs method given an array
    inline CLsResult calculate( const Doubles &array ) const;

    // Calculate CLb
    inline double CLb( const double &t ) const;

    // Calculate CLs
    inline double CLs( const double &t ) const;

    // Calculate CLsb
    inline double CLsb( const double &t ) const;

    // Generate < n > events for each of the hypotheses
    void generate( const size_t &n = 10000 );
    
    // Return the null hypothesis
    inline CLsHypothesis* getNullHyp();
    
    // Return the signal hypothesis
    inline CLsHypothesis* getSigHyp();

    // Set the null hypothesis
    inline void setNullHyp( CLsHypothesis &hyp );

    // Set the signal hypothesis
    inline void setSigHyp( CLsHypothesis &hyp );

    // Return the test statistics for a given array of values
    double testStat( const Doubles &values ) const;

  protected:

    // Null hypothesis
    CLsHypothesis *fNullHyp;

    // Signal hypothesis
    CLsHypothesis *fSigHyp;

  };
  
  //_______________________________________________________________________________
  //
  inline double CLsFactory::alpha( const double &t ) const {
    
    return fNullHyp->pValue(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::beta( const double &t ) const {

    return fSigHyp->pValue(t);
  }

  //_______________________________________________________________________________
  //
  inline CLsResult CLsFactory::calculate( const Doubles &array ) const {

    double t = this->testStat(array);

    return this->calculate(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLb( const double &t ) const {

    return 1. - this->alpha(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLs( const double &t ) const {
    
    return this->CLsb(t)/this->CLb(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLsb( const double &t ) const {

    return this->beta(t);
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis* CLsFactory::getNullHyp() {

    return fNullHyp;
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis* CLsFactory::getSigHyp() {

    return fSigHyp;
  }

  //_______________________________________________________________________________
  //
  inline void CLsFactory::setNullHyp( CLsHypothesis &hyp ) {

    fNullHyp = &hyp;

    fNullHyp->setFactory(this);
    fNullHyp->setType(CLsHypTypes::aNull);
  }
  
  //_______________________________________________________________________________
  //
  inline void CLsFactory::setSigHyp( CLsHypothesis &hyp ) {

    fSigHyp = &hyp;

    fSigHyp->setFactory(this);
    fSigHyp->setType(CLsHypTypes::aSignal);
  }
}

#endif
