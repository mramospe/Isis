//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 30/06/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Wrappers from the Statistics library
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/shared_ptr.hpp>

#include "GlobalWrappers.hpp"

#include "CLsFactory.hpp"
#include "CLsFluctuator.hpp"
#include "CLsHypothesis.hpp"
#include "CLsPrior.hpp"
#include "CLsResult.hpp"
#include "Definitions.hpp"

#include "TPython.h"


//_______________________________________________________________________________

// Wrappers for the class CLsFactory
namespace CLsFact {
  
  //_______________________________________________________________________________
  //
  inline isis::CLsResult calculateFromArray( const isis::CLsFactory &factory,
					     const np::ndarray &array ) {

    auto vector = iboost::numpyArrayToStdCont<isis::Doubles>( array );

    return factory.calculate( vector );
  }

  //_______________________________________________________________________________
  //
  isis::CLsResult
  (isis::CLsFactory::*calculateFromDouble)( const double &tstat ) const =
    &isis::CLsFactory::calculate;

  //_______________________________________________________________________________
  //
  inline isis::CLsHypothesis getNullHyp( isis::CLsFactory &factory ) {

    return *(factory.getNullHyp());
  }

  //_______________________________________________________________________________
  //
  inline isis::CLsHypothesis getSigHyp( isis::CLsFactory &factory ) {

    return *(factory.getSigHyp());
  }

  //_______________________________________________________________________________
  //
  inline double testStat( const isis::CLsFactory &factory,
			  const np::ndarray &values ) {

    auto vector = iboost::numpyArrayToStdCont<isis::Doubles>( values );

    return factory.testStat( vector );
  }
}

// Wrappers for the class CLsFluctuator
namespace CLsFluct {

  struct CLsFluctWrap : isis::CLsFluctuator, py::wrapper<isis::CLsFluctuator> {
    
    double fluctuate( const size_t &pos, const double &value ) {

      return this->get_override("fluctuate")(pos, value);
    }
  };
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::CLsHypothesis>
  constructor( const np::ndarray &lst,
	       isis::CLsFluctuator *fluct,
	       isis::CLsPrior *prior ) {

    auto array = iboost::numpyArrayToStdCont<isis::Doubles>( lst );
    auto hyp   = new isis::CLsHypothesis(array, fluct, prior);

    return boost::shared_ptr<isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::CLsHypothesis>
  constructor_NoPrior( const np::ndarray &lst, isis::CLsFluctuator *fluct ) {

    auto array = iboost::numpyArrayToStdCont<isis::Doubles>( lst );
    auto hyp   = new isis::CLsHypothesis(array, fluct);

    return boost::shared_ptr<isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::CLsHypothesis>
  constructor_NoFluctNoPrior( const np::ndarray &lst ) {

    auto array = iboost::numpyArrayToStdCont<isis::Doubles>( lst );
    auto hyp   = new isis::CLsHypothesis(array);

    return boost::shared_ptr<isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline np::ndarray getHyp( const isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getHyp();

    return iboost::stdContToNumpyArray( vec );
  }

  //_______________________________________________________________________________
  //
  inline double poissonProb( const isis::CLsHypothesis &hyp,
			     const np::ndarray &values ) {

    auto vec = iboost::numpyArrayToStdCont<isis::Doubles>( values );

    return hyp.poissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  inline np::ndarray getTSVals( const isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getTSVals();

    return iboost::stdContToNumpyArray( vec );
  }

  //_______________________________________________________________________________
  //
  inline void setHyp( isis::CLsHypothesis &hyp,
		      const np::ndarray &lst,
		      isis::CLsFluctuator *fluct = 0,
		      isis::CLsPrior *prior = 0 ) {

    auto vec = iboost::numpyArrayToStdCont<isis::Doubles>( lst );

    hyp.setHyp(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(setHyp_Overloads, setHyp, 2, 4);
}

// Wrappers for the class CLsPrior
namespace CLsPrior {

  struct CLsPriorWrap : isis::CLsPrior, py::wrapper<isis::CLsPrior> {

    double evaluate( const size_t &pos, const double &mean, const double &value ) {

      return this->get_override("evaluate")(pos, mean, value);
    }
  };
}
