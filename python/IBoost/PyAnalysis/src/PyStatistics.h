//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/03/2017
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

namespace py = boost::python;
namespace an = Analysis;


//_______________________________________________________________________________

// Wrappers for the class CLsFactory
namespace CLsFact {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsFactory> Constructor( py::list h0, py::list h1 ) {

    auto vh0 = IBoost::BoostListToStdVec<double>( h0 );
    auto vh1 = IBoost::BoostListToStdVec<double>( h1 );

    auto factory = new an::CLsFactory(vh0, vh1);

    return boost::shared_ptr<an::CLsFactory>( factory );
  }

  //_______________________________________________________________________________
  //
  an::CLsResult Calculate( const an::CLsFactory &factory, py::list array ) {

    auto vector = IBoost::BoostListToStdVec<double>( array );

    return factory.Calculate( vector );
  }

  //_______________________________________________________________________________
  //
  an::CLsHypothesis GetNullHyp( an::CLsFactory &factory ) {

    return factory.GetNullHyp();
  }

  //_______________________________________________________________________________
  //
  an::CLsHypothesis GetSigHyp( an::CLsFactory &factory ) {

    return factory.GetSigHyp();
  }

  //_______________________________________________________________________________
  //
  double TestStat( const an::CLsFactory &factory, py::list values ) {

    auto vector = IBoost::BoostListToStdVec<double>( values );

    return factory.TestStat( vector );
  }
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor( int type,
	       an::CLsFactory *factory,
	       py::list lst,
	       an::CLsFluctuator *fluct,
	       an::CLsPrior *prior ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(type, factory, array, fluct, prior);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor_NoPrior( int type,
		       an::CLsFactory *factory,
		       py::list lst,
		       an::CLsFluctuator *fluct ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(type, factory, array, fluct);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor_NoFluctNoPrior( int type,
			      an::CLsFactory *factory,
			      py::list lst ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(type, factory, array);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  py::list GetHyp( const an::CLsHypothesis &hyp ) {

    auto vec = hyp.GetHyp();

    return IBoost::StdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  double PoissonProb( const an::CLsHypothesis &hyp, py::list values ) {

    auto vec = IBoost::BoostListToStdVec<double>( values );

    return hyp.PoissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  py::list GetTSVals( const an::CLsHypothesis &hyp ) {

    auto vec = hyp.GetTSVals();

    return IBoost::StdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  void SetHypothesis( an::CLsHypothesis &hyp,
		      py::list lst,
		      an::CLsFluctuator *fluct = 0,
		      an::CLsPrior *prior = 0 ) {

    auto vec = IBoost::BoostListToStdVec<double>( lst );

    hyp.SetHypothesis(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(SetHypothesis_Overloads, SetHypothesis, 2, 4);
}
