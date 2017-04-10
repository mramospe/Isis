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
//  Configuration of the functions to manage classes and functions from the
//  package Analysis
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/shared_ptr.hpp>

#include "GlobalWrappers.h"
#include "InitModule.h"
#include "PyAdaptiveBinning.h"
#include "PyAnalysisBase.h"
#include "PyStatistics.h"

#include <string>

namespace py = boost::python;
namespace np = boost::python::numpy;


//_______________________________________________________________________________

BOOST_PYTHON_MODULE( analysis ) {

  // Initialize python and numpy (to prevent a segmentation fault)
  I_INIT_MODULE;

  // Wrapper from AdaptiveBinning.h
  py::class_<isis::AdaptiveBinning>("AdaptiveBinning", py::init<>())
    .def("getStruct" , &AdBin::getStruct)
    .def("getBinList", &AdBin::getBinList)
    .def("getNbins"  , &isis::AdaptiveBinning::getNbins)
    ;

  // Wrapper from AdaptiveBinning1D.h
  py::class_<isis::AdaptiveBinning1D, py::bases<isis::AdaptiveBinning> >
    ("AdaptiveBinning1D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin1D::constructor))
    .def("__init__"     , py::make_constructor(&AdBin1D::constructor_NoWgts))
    .def("getStruct"    , &AdBin1D::getStruct, AdBin1D::getStruct_Overloads())
    .def_readonly("Max" , &isis::AdaptiveBinning1D::getMax)
    .def_readonly("Min" , &isis::AdaptiveBinning1D::getMin)
    ;

  // Wrapper from AdaptiveBinning2D.h
  py::class_<isis::AdaptiveBinning2D, py::bases<isis::AdaptiveBinning> >
    ("AdaptiveBinning2D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin2D::constructor))
    .def("__init__"     , py::make_constructor(&AdBin2D::constructor_NoWgts))
    .def("getAdjStruct" , &AdBin2D::getAdjStruct, AdBin2D::getAdjStruct_Overloads())
    .def("getStruct"    , &AdBin2D::getStruct, AdBin2D::getStruct_Overloads())
    .def_readonly("Xmax", &isis::AdaptiveBinning2D::getXmax)
    .def_readonly("Xmin", &isis::AdaptiveBinning2D::getXmin)
    .def_readonly("Ymax", &isis::AdaptiveBinning2D::getYmax)
    .def_readonly("Ymin", &isis::AdaptiveBinning2D::getYmin)
    ;

  // Wrapper from Bin.h. This class is meant to be used in read-only mode.
  py::class_<isis::Bin>("Bin", py::no_init)
    .def("getEntries"     , &isis::Bin::getEntries)
    .def("getSumOfWeights", &isis::Bin::getSumOfWeights)
    ;

  // Wrapper from Bin1D.h. This class is meant to be used in read-only mode.
  py::class_<isis::Bin1D, py::bases<isis::Bin> >("Bin1D", py::no_init)
    .def_readonly("Min", &isis::Bin1D::getMin)
    ;

  // Wrapper from Bin2D.h. This class is meant to be used in read-only mode.
  py::class_<isis::Bin2D, py::bases<isis::Bin> >("Bin2D", py::no_init)
    .def_readonly("Xmax", &isis::Bin2D::getXmax)
    .def_readonly("Xmin", &isis::Bin2D::getXmin)
    .def_readonly("Ymax", &isis::Bin2D::getYmax)
    .def_readonly("Ymin", &isis::Bin2D::getYmin)
    ;

  // Wrapper from CLsFluctuator
  py::class_<CLsFluct::CLsFluctWrap, boost::noncopyable>("CLsFluctuator")
    .def("fluctuate", py::pure_virtual(&CLsFluct::CLsFluctWrap::fluctuate))
    ;

  // Wrapper from CLsPrior
  py::class_<CLsPrior::CLsPriorWrap, boost::noncopyable>("CLsPrior")
    .def("evaluate", py::pure_virtual(&CLsPrior::CLsPriorWrap::evaluate))
    ;

  // Wrapper from CLsResult
  py::class_<isis::CLsResult>("CLsResult",
			      py::init<
			      const double&,
			      const double&,
			      const double&,
			      const double&,
			      const double&>())
    .def_readonly("Alpha"       , &isis::CLsResult::alpha)
    .def_readonly("Beta"        , &isis::CLsResult::beta)
    .def_readonly("CLb"         , &isis::CLsResult::CLb)
    .def_readonly("CLs"         , &isis::CLsResult::CLs)
    .def_readonly("CLsb"        , &isis::CLsResult::CLsb)
    .def_readonly("TestStat"    , &isis::CLsResult::testStat)
    .def_readonly("Significance", &isis::CLsResult::significance)
    .def("gaussSigPos"          , &isis::CLsResult::gaussSigPos)
    .def("isDiscovery"          , &isis::CLsResult::isDiscovery)
    .def("isEvidence"           , &isis::CLsResult::isEvidence)
    ;
    
  // Wrapper from CLsHypothesis
  py::class_<isis::CLsHypothesis>("CLsHypothesis", py::init<>())
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor))
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor_NoPrior))
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor_NoFluctNoPrior))
    .def("pValue"          , &isis::CLsHypothesis::pValue)
    .def("getTSVals"       , &CLsHyp::getTSVals)
    .def("getHyp"          , &CLsHyp::getHyp)
    .def("generate"        , &isis::CLsHypothesis::generate)
    .def("poissonProb"     , &CLsHyp::poissonProb)
    .def("setFactory"      , &isis::CLsHypothesis::setFactory)
    .def("setFluctuator"   , &isis::CLsHypothesis::setFluctuator)
    .def("setHyp"          , &CLsHyp::setHyp, CLsHyp::setHyp_Overloads())
    .def("setPrior"        , &isis::CLsHypothesis::setPrior)
    .def("testStatFromProb", &isis::CLsHypothesis::testStatFromProb)
    ;

  py::enum_<isis::CLsHypTypes>("CLsHypTypes")
    .value("aNone"  , isis::CLsHypTypes::aNone)
    .value("aNull"  , isis::CLsHypTypes::aNull)
    .value("aSignal", isis::CLsHypTypes::aSignal)
    ;
  
  // Wrapper from CLsFactory
  py::class_<isis::CLsFactory>("CLsFactory", py::init<>())
    .def(py::init<isis::CLsHypothesis&, isis::CLsHypothesis&>())
    .def("alpha"           , &isis::CLsFactory::alpha)
    .def("beta"            , &isis::CLsFactory::beta)
    .def("calculate"       , &CLsFact::calculateFromArray)
    .def("calculate"       , CLsFact::calculateFromDouble)
    .def("CLb"             , &isis::CLsFactory::CLb)
    .def("CLs"             , &isis::CLsFactory::CLs)
    .def("CLsb"            , &isis::CLsFactory::CLsb)
    .def("generate"        , &isis::CLsFactory::generate)
    .def("testStat"        , &CLsFact::testStat)
    .add_property("NullHyp", &CLsFact::getNullHyp, &isis::CLsFactory::setNullHyp)
    .add_property("SigHyp" , &CLsFact::getSigHyp, &isis::CLsFactory::setSigHyp)
    ;
  
  // Wrappers from RootUtils
  py::def("getSafeObject", &RootUtils::getSafeObject);
}
