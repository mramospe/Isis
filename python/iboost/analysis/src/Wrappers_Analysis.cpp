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
#include "PyAdaptiveBinning.h"
#include "PyAnalysisBase.h"
#include "PyStatistics.h"

#include <string>

namespace py = boost::python;


//_______________________________________________________________________________

BOOST_PYTHON_MODULE( analysis ) {

  // Initialize python and numpy (to prevent a segmentation fault)
  Py_Initialize();
  py::numpy::initialize();

  // Wrapper from AdaptiveBinning.h
  py::class_<Isis::AdaptiveBinning>("AdaptiveBinning", py::init<>())
    .def("getStruct" , &AdBin::getStruct)
    .def("getBinList", &AdBin::getBinList)
    .def("getNbins"  , &Isis::AdaptiveBinning::getNbins)
    ;

  // Wrapper from AdaptiveBinning1D.h
  py::class_<Isis::AdaptiveBinning1D, py::bases<Isis::AdaptiveBinning> >
    ("AdaptiveBinning1D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin1D::constructor))
    .def("__init__"     , py::make_constructor(&AdBin1D::constructor_NoWgts))
    .def("getStruct"    , &AdBin1D::getStruct, AdBin1D::getStruct_Overloads())
    .def_readonly("Max", &Isis::AdaptiveBinning1D::getMax)
    .def_readonly("Min", &Isis::AdaptiveBinning1D::getMin)
    ;

  // Wrapper from AdaptiveBinning2D.h
  py::class_<Isis::AdaptiveBinning2D, py::bases<Isis::AdaptiveBinning> >
    ("AdaptiveBinning2D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin2D::constructor))
    .def("__init__"     , py::make_constructor(&AdBin2D::constructor_NoWgts))
    .def("getAdjStruct" , &AdBin2D::getAdjStruct, AdBin2D::getAdjStruct_Overloads())
    .def("getStruct"    , &AdBin2D::getStruct, AdBin2D::getStruct_Overloads())
    .def_readonly("Xmax", &Isis::AdaptiveBinning2D::getXmax)
    .def_readonly("Xmin", &Isis::AdaptiveBinning2D::getXmin)
    .def_readonly("Ymax", &Isis::AdaptiveBinning2D::getYmax)
    .def_readonly("Ymin", &Isis::AdaptiveBinning2D::getYmin)
    ;

  // Wrapper from Bin.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin>("Bin", py::no_init)
    .def("getEntries"     , &Isis::Bin::getEntries)
    .def("getSumOfWeights", &Isis::Bin::getSumOfWeights)
    ;

  // Wrapper from Bin1D.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin1D, py::bases<Isis::Bin> >("Bin1D", py::no_init)
    .def_readonly("Min", &Isis::Bin1D::getMin)
    ;

  // Wrapper from Bin2D.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin2D, py::bases<Isis::Bin> >("Bin2D", py::no_init)
    .def_readonly("Xmax", &Isis::Bin2D::getXmax)
    .def_readonly("Xmin", &Isis::Bin2D::getXmin)
    .def_readonly("Ymax", &Isis::Bin2D::getYmax)
    .def_readonly("Ymin", &Isis::Bin2D::getYmin)
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
  py::class_<Isis::CLsResult>("CLsResult",
			      py::init<
			      const double&,
			      const double&,
			      const double&,
			      const double&,
			      const double&>())
    .def_readonly("Alpha"       , &Isis::CLsResult::alpha)
    .def_readonly("Beta"        , &Isis::CLsResult::beta)
    .def_readonly("CLb"         , &Isis::CLsResult::CLb)
    .def_readonly("CLs"         , &Isis::CLsResult::CLs)
    .def_readonly("CLsb"        , &Isis::CLsResult::CLsb)
    .def_readonly("TestStat"    , &Isis::CLsResult::testStat)
    .def_readonly("Significance", &Isis::CLsResult::significance)
    .def("gaussSigPos"          , &Isis::CLsResult::gaussSigPos)
    .def("isDiscovery"          , &Isis::CLsResult::isDiscovery)
    .def("isEvidence"           , &Isis::CLsResult::isEvidence)
    ;
    
  // Wrapper from CLsHypothesis
  py::class_<Isis::CLsHypothesis>("CLsHypothesis", py::init<>())
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor))
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor_NoPrior))
    .def("__init__"        , py::make_constructor(&CLsHyp::constructor_NoFluctNoPrior))
    .def("pValue"          , &Isis::CLsHypothesis::pValue)
    .def("getTSVals"       , &CLsHyp::getTSVals)
    .def("getHyp"          , &CLsHyp::getHyp)
    .def("generate"        , &Isis::CLsHypothesis::generate)
    .def("poissonProb"     , &CLsHyp::poissonProb)
    .def("setFactory"      , &Isis::CLsHypothesis::setFactory)
    .def("setFluctuator"   , &Isis::CLsHypothesis::setFluctuator)
    .def("setHyp"          , &CLsHyp::setHyp, CLsHyp::setHyp_Overloads())
    .def("setPrior"        , &Isis::CLsHypothesis::setPrior)
    .def("testStatFromProb", &Isis::CLsHypothesis::testStatFromProb)
    ;

  py::enum_<Isis::CLsHypTypes>("CLsHypTypes")
    .value("aNone"  , Isis::CLsHypTypes::aNone)
    .value("aNull"  , Isis::CLsHypTypes::aNull)
    .value("aSignal", Isis::CLsHypTypes::aSignal)
    ;
  
  // Wrapper from CLsFactory
  py::class_<Isis::CLsFactory>("CLsFactory", py::init<>())
    .def(py::init<Isis::CLsHypothesis&, Isis::CLsHypothesis&>())
    .def("alpha"           , &Isis::CLsFactory::alpha)
    .def("beta"            , &Isis::CLsFactory::beta)
    .def("calculate"       , &CLsFact::calculateFromArray)
    .def("calculate"       , CLsFact::calculateFromDouble)
    .def("CLb"             , &Isis::CLsFactory::CLb)
    .def("CLs"             , &Isis::CLsFactory::CLs)
    .def("CLsb"            , &Isis::CLsFactory::CLsb)
    .def("generate"        , &Isis::CLsFactory::generate)
    .def("testStat"        , &CLsFact::testStat)
    .add_property("NullHyp", &CLsFact::getNullHyp, &Isis::CLsFactory::setNullHyp)
    .add_property("SigHyp" , &CLsFact::getSigHyp, &Isis::CLsFactory::setSigHyp)
    ;
  
  // Wrappers from RootUtils
  py::def("getSafeObject", &RootUtils::getSafeObject);
}
