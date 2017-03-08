///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/03/2017
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


//_______________________________________________________________________________

namespace Analysis {

  class CLsFactory {

  public:

    // Constructor
    CLsFactory();

    // Constructor given the two hypothesis as arrays
    CLsFactory( const General::Doubles &h0, const General::Doubles &h1 );

    // Destructor
    ~CLsFactory();
    
    // Return the p-value of the null hypothesis
    inline double Alpha( const double &t ) const;

    // Return the p-value of the signal hypothesis
    inline double Beta( const double &t ) const;

    // Return a class with all the information concerning the CLs method
    CLsResult Calculate( const General::Doubles &array ) const;

    // Calculate CLb
    inline double CLb( const double &t ) const;

    // Calculate CLs
    inline double CLs( const double &t ) const;

    // Calculate CLsb
    inline double CLsb( const double &t ) const;

    // Generate < n > events for each of the hypotheses
    void Generate( const size_t &n = 10000 );
    
    // Return the null hypothesis
    inline CLsHypothesis& GetNullHyp();
    
    // Return the signal hypothesis
    inline CLsHypothesis& GetSigHyp();

    // Set the null hypothesis
    inline void SetNullHyp( CLsHypothesis &hyp );

    // Set the signal hypothesis
    inline void SetSigHyp( CLsHypothesis &hyp );

    // Return the test statistics for a given array of values
    double TestStat( const General::Doubles &values ) const;

  protected:

    // Null hypothesis
    CLsHypothesis fNullHyp;

    // Signal hypothesis
    CLsHypothesis fSigHyp;

  };
  
  //_______________________________________________________________________________
  //
  inline double CLsFactory::Alpha( const double &t ) const {
    
    return fNullHyp.PValue(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::Beta( const double &t ) const {

    return fSigHyp.PValue(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLb( const double &t ) const {

    return 1. - this->Alpha(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLs( const double &t ) const {
    
    return this->CLsb(t)/this->CLb(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLsb( const double &t ) const {

    return this->Beta(t);
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis& CLsFactory::GetNullHyp() {

    return fNullHyp;
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis& CLsFactory::GetSigHyp() {

    return fSigHyp;
  }

  //_______________________________________________________________________________
  //
  inline void CLsFactory::SetNullHyp( CLsHypothesis &hyp ) {

    fNullHyp = hyp;
  }
  
  //_______________________________________________________________________________
  //
  inline void CLsFactory::SetSigHyp( CLsHypothesis &hyp ) {

    fSigHyp = hyp;
  }
}

#endif
