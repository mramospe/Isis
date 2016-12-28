//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 28/12/2016
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Configuration of the functions to manage classes and functions from the
//  package General
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/shared_ptr.hpp>

#include "GlobalWrappers.h"

#include "AdaptiveBinning.h"
#include "AdaptiveBinning1D.h"
#include "AdaptiveBinning2D.h"
#include "Bin.h"

#include "TPython.h"

#include <string>

namespace py = boost::python;
namespace an = Analysis;


//_______________________________________________________________________________

// Wrappers for the class AdaptiveBinning
namespace AdBin {

  py::list GetBinList( const an::AdaptiveBinning &adbin ) {
  
    return IBoost::StdVecToBoostList( adbin.GetBinList() );
  }

  PyObject* GetStruct( const an::AdaptiveBinning &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TObject",
					     false );
  }  
}

// Wrappers for the class AdaptiveBinning1D
namespace AdBin1D {
  
  boost::shared_ptr<an::AdaptiveBinning1D> Constructor( size_t occ,
							double vmin,
							double vmax,
							py::list values,
							py::list weights ) {
  
    auto vec_values  = IBoost::BoostListToStdVec<double>( values );
    auto vec_weights = IBoost::BoostListToStdVec<double>( weights );
    auto adbin       = new an::AdaptiveBinning1D(occ, vmin, vmax,
						 vec_values, vec_weights);
  
    return boost::shared_ptr<an::AdaptiveBinning1D>( adbin );
  }

  boost::shared_ptr<an::AdaptiveBinning1D> Constructor_NoWgts( size_t occ,
							       double vmin,
							       double vmax,
							       py::list values ) {
    return Constructor(occ, vmin, vmax, values, py::list());
  }

  PyObject* GetStruct( const an::AdaptiveBinning1D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TH1D",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetStruct_Overloads, GetStruct, 1, 3);
}

// Wrappers for the class AdaptiveBinning2D
namespace AdBin2D {
  
  boost::shared_ptr<an::AdaptiveBinning2D> Constructor( size_t occ,
							double xmin, double xmax,
							double ymin, double ymax,
							py::list xvalues,
							py::list yvalues,
							py::list weights ) {
  
    auto vec_xvalues = IBoost::BoostListToStdVec<double>( xvalues );
    auto vec_yvalues = IBoost::BoostListToStdVec<double>( yvalues );
    auto vec_weights = IBoost::BoostListToStdVec<double>( weights );
    auto adbin       = new an::AdaptiveBinning2D(occ,
						 xmin, xmax,
						 ymin, ymax,
						 vec_xvalues, vec_yvalues,
						 vec_weights);
  
    return boost::shared_ptr<an::AdaptiveBinning2D>( adbin );
  }

  boost::shared_ptr<an::AdaptiveBinning2D> Constructor_NoWgts( size_t occ,
							       double xmin,
							       double xmax,
							       double ymin,
							       double ymax,
							       py::list xvalues,
							       py::list yvalues ) {
  
    return Constructor(occ, xmin, xmax, ymin, ymax, xvalues, yvalues, py::list());
  }

  PyObject* GetAdjStruct( const an::AdaptiveBinning2D &adbin,
			  const char *name = "",
			  const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetAdjStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetAdjStruct_Overloads, GetAdjStruct, 1, 3);
  
  PyObject* GetStruct( const an::AdaptiveBinning2D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetStruct_Overloads, GetStruct, 1, 3);
}


BOOST_PYTHON_MODULE( PyAnalysis ) {

  // Wrapper from AdaptiveBinning.h
  py::class_<an::AdaptiveBinning>("AdaptiveBinning", py::init<>())
    .def("GetStruct" , &AdBin::GetStruct)
    .def("GetBinList", &AdBin::GetBinList)
    .def("GetNbins"  , &an::AdaptiveBinning::GetNbins)
    ;

  // Wrapper from AdaptiveBinning1D.h
  py::class_<an::AdaptiveBinning1D, py::bases<an::AdaptiveBinning> >
    ("AdaptiveBinning1D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor_NoWgts))
    .def_readonly("Max" , &an::AdaptiveBinning1D::GetMax)
    .def_readonly("Min" , &an::AdaptiveBinning1D::GetMin)
    .def("GetStruct"    , &AdBin1D::GetStruct, AdBin1D::GetStruct_Overloads())
    ;

  // Wrapper from AdaptiveBinning2D.h
  py::class_<an::AdaptiveBinning2D, py::bases<an::AdaptiveBinning> >
    ("AdaptiveBinning2D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor_NoWgts))
    .def("GetAdjStruct" , &AdBin2D::GetAdjStruct, AdBin2D::GetAdjStruct_Overloads())
    .def("GetStruct"    , &AdBin2D::GetStruct, AdBin2D::GetStruct_Overloads())
    .def_readonly("Xmax", &an::AdaptiveBinning2D::GetXmax)
    .def_readonly("Xmin", &an::AdaptiveBinning2D::GetXmin)
    .def_readonly("Ymax", &an::AdaptiveBinning2D::GetYmax)
    .def_readonly("Ymin", &an::AdaptiveBinning2D::GetYmin)
    ;

  // Wrapper from Bin.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin>("Bin", py::no_init)
    .def("GetEntries"     , &an::Bin::GetEntries)
    .def("GetSumOfWeights", &an::Bin::GetSumOfWeights)
    ;

  // Wrapper from Bin1D.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin1D, py::bases<an::Bin> >("Bin1D", py::no_init)
    .def_readonly("Min", &an::Bin1D::GetMin)
    ;

  // Wrapper from Bin2D.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin2D, py::bases<an::Bin> >("Bin2D", py::no_init)
    .def_readonly("Xmax", &an::Bin2D::GetXmax)
    .def_readonly("Xmin", &an::Bin2D::GetXmin)
    .def_readonly("Ymax", &an::Bin2D::GetYmax)
    .def_readonly("Ymin", &an::Bin2D::GetYmin)
    ;

}
