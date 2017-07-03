//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 03/07/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Wrappers from the AdaptiveBinning library
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "GlobalWrappers.h"

#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/numpy.hpp>

#include "AdaptiveBinning.h"
#include "AdaptiveBinning1D.h"
#include "AdaptiveBinning2D.h"
#include "Bin.h"
#include "Bin1D.h"
#include "Bin2D.h"
#include "Definitions.h"

#include "TPython.h"

namespace py = boost::python;
namespace np = boost::python::numpy;


//_______________________________________________________________________________

// Wrappers for the class AdaptiveBinning
namespace AdBin {

  struct AdaptiveBinningWrap : isis::AdaptiveBinning,
			       py::wrapper<isis::AdaptiveBinning> {

    inline TObject* getStruct( const char *name = "",
			       const char *title = "" ) const {
    
      return this->get_override("getStruct")(name, title);
    }

    inline PyObject* getStructWrap( const isis::AdaptiveBinning1D &adbin,
				    const char *name = "",
				    const char *title = "" ) const {
      
      return TPython::ObjectProxy_FromVoidPtr( this->getStruct(name, title),
					       "TObject",
					       false );
    }

    inline py::list getBinListWrap() const {
  
      return iboost::stdContToBoostList(this->getBinList());
    }
    
  };
    
}

// Wrappers for the class AdaptiveBinning1D
namespace AdBin1D {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::AdaptiveBinning1D>
  constructor( size_t occ,
	       double vmin,
	       double vmax,
	       np::ndarray values,
	       np::ndarray weights ) {
  
    auto vec_values  = iboost::numpyArrayToStdCont<isis::Doubles>( values );
    auto vec_weights = iboost::numpyArrayToStdCont<isis::Doubles>( weights );
    auto adbin       = new isis::AdaptiveBinning1D(occ, vmin, vmax,
						   vec_values, vec_weights);
  
    return boost::shared_ptr<isis::AdaptiveBinning1D>( adbin );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::AdaptiveBinning1D>
  constructor_NoWgts( size_t occ,
		      double vmin,
		      double vmax,
		      np::ndarray values ) {
    
    return constructor(occ, vmin, vmax, values, np::array(py::list()));
  }

  //_______________________________________________________________________________
  //
  inline PyObject* getStruct( const isis::AdaptiveBinning1D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    
    return TPython::ObjectProxy_FromVoidPtr( adbin.getStruct(name, title),
					     "TH1D",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getStruct_Overloads, getStruct, 1, 3);
}

// Wrappers for the class AdaptiveBinning2D
namespace AdBin2D {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::AdaptiveBinning2D>
  constructor( size_t occ,
	       double xmin, double xmax,
	       double ymin, double ymax,
	       np::ndarray xvalues,
	       np::ndarray yvalues,
	       np::ndarray weights ) {
  
    auto vec_xvalues = iboost::numpyArrayToStdCont<isis::Doubles>( xvalues );
    auto vec_yvalues = iboost::numpyArrayToStdCont<isis::Doubles>( yvalues );
    auto vec_weights = iboost::numpyArrayToStdCont<isis::Doubles>( weights );
    auto adbin       = new isis::AdaptiveBinning2D(occ,
						   xmin, xmax,
						   ymin, ymax,
						   vec_xvalues, vec_yvalues,
						   vec_weights);
  
    return boost::shared_ptr<isis::AdaptiveBinning2D>( adbin );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<isis::AdaptiveBinning2D>
  constructor_NoWgts( size_t occ,
		      double xmin,
		      double xmax,
		      double ymin,
		      double ymax,
		      np::ndarray xvalues,
		      np::ndarray yvalues ) {
  
    return constructor(occ, xmin, xmax, ymin, ymax, xvalues, yvalues, np::array(py::list()));
  }

  //_______________________________________________________________________________
  //
  inline PyObject* getAdjStruct( const isis::AdaptiveBinning2D &adbin,
				 const char *name = "",
				 const char *title = "" ) {

    return TPython::ObjectProxy_FromVoidPtr( adbin.getAdjStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getAdjStruct_Overloads, getAdjStruct, 1, 3);

  //_______________________________________________________________________________
  //
  inline PyObject* getStruct( const isis::AdaptiveBinning2D &adbin,
			      const char *name = "",
			      const char *title = "" ) {
    
    return TPython::ObjectProxy_FromVoidPtr( adbin.getStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getStruct_Overloads, getStruct, 1, 3);
}
