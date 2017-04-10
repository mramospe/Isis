//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
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

#include "GlobalWrappers.h"

#include "CLsFactory.h"
#include "CLsFluctuator.h"
#include "CLsHypothesis.h"
#include "CLsPrior.h"
#include "CLsResult.h"

#include "TPython.h"


//_______________________________________________________________________________

// Wrappers for the class CLsFactory
namespace CLsFact {
  
  //_______________________________________________________________________________
  //
  inline Isis::CLsResult calculateFromArray( const Isis::CLsFactory &factory,
					     const np::ndarray &array ) {

    auto vector = iboost::numpyArrayToStdVec<double>( array );

    return factory.calculate( vector );
  }

  //_______________________________________________________________________________
  //
  Isis::CLsResult
  (Isis::CLsFactory::*calculateFromDouble)( const double &tstat ) const =
    &Isis::CLsFactory::calculate;

  //_______________________________________________________________________________
  //
  inline Isis::CLsHypothesis getNullHyp( Isis::CLsFactory &factory ) {

    return *(factory.getNullHyp());
  }

  //_______________________________________________________________________________
  //
  inline Isis::CLsHypothesis getSigHyp( Isis::CLsFactory &factory ) {

    return *(factory.getSigHyp());
  }

  //_______________________________________________________________________________
  //
  inline double testStat( const Isis::CLsFactory &factory,
			  const np::ndarray &values ) {

    auto vector = iboost::numpyArrayToStdVec<double>( values );

    return factory.testStat( vector );
  }
}

// Wrappers for the class CLsFluctuator
namespace CLsFluct {

  struct CLsFluctWrap : Isis::CLsFluctuator, py::wrapper<Isis::CLsFluctuator> {
    
    double fluctuate( const size_t &pos, const double &value ) {

      return this->get_override("fluctuate")(pos, value);
    }
  };
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor( const np::ndarray &lst,
	       Isis::CLsFluctuator *fluct,
	       Isis::CLsPrior *prior ) {

    auto array = iboost::numpyArrayToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct, prior);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor_NoPrior( const np::ndarray &lst, Isis::CLsFluctuator *fluct ) {

    auto array = iboost::numpyArrayToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor_NoFluctNoPrior( const np::ndarray &lst ) {

    auto array = iboost::numpyArrayToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline np::ndarray getHyp( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getHyp();

    return iboost::stdVecToNumpyArray( vec );
  }

  //_______________________________________________________________________________
  //
  inline double poissonProb( const Isis::CLsHypothesis &hyp,
			     const np::ndarray &values ) {

    auto vec = iboost::numpyArrayToStdVec<double>( values );

    return hyp.poissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  inline np::ndarray getTSVals( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getTSVals();

    return iboost::stdVecToNumpyArray( vec );
  }

  //_______________________________________________________________________________
  //
  inline void setHyp( Isis::CLsHypothesis &hyp,
		      const np::ndarray &lst,
		      Isis::CLsFluctuator *fluct = 0,
		      Isis::CLsPrior *prior = 0 ) {

    auto vec = iboost::numpyArrayToStdVec<double>( lst );

    hyp.setHyp(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(setHyp_Overloads, setHyp, 2, 4);
}

// Wrappers for the class CLsPrior
namespace CLsPrior {

  struct CLsPriorWrap : Isis::CLsPrior, py::wrapper<Isis::CLsPrior> {

    double evaluate( const size_t &pos, const double &mean, const double &value ) {

      return this->get_override("evaluate")(pos, mean, value);
    }
  };
}
