//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
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
#include <boost/python/numpy.hpp>
#include <boost/python/scope.hpp>

#include "CutManager.hpp"
#include "Definitions.hpp"
#include "GlobalWrappers.hpp"
#include "InitModule.hpp"
#include "Messenger.hpp"
#include "MessengerConfig.hpp"
#include "LoopArray.hpp"
#include "ProgressBar.hpp"

#include <string>

namespace py = boost::python;
namespace np = boost::python::numpy;


//_______________________________________________________________________________

// Wrappers for the class CutManager
namespace CutMgr {
  
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(bookCut_Overloads,
					 isis::CutManager::bookCut,
					 1, 2);

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(makeMergedCut_Overloads,
					 isis::CutManager::makeMergedCut,
					 0, 1);
}

// Wrappers for the class LoopArray
namespace LoopArray {

  // Convert vector to list
  inline py::list getItem( const isis::LoopArray &looper,
			   const size_t &idx ) {

    return iboost::stdContToBoostList(looper[idx]);
  }

  // Convert vector to list
  inline py::list values( const isis::LoopArray &looper ) {

    return iboost::stdContToBoostList(looper.values());
  }
}

// Thin functions to send messages using the class Messenger
namespace Msg {

  // Function to send a formatted message
  inline void sendFormattedMsg( const std::string &msg,
				const int &fgcol = isis::ANSIFormat::aNoColor,
				const int &bgcol = isis::ANSIFormat::aNoColor,
				const int &sty = isis::ANSIFormat::aNoStyle ) {

    isis::Messenger("", std::cout, fgcol, bgcol, sty) << msg << IEndMsg;
  }
  BOOST_PYTHON_FUNCTION_OVERLOADS(sendFormattedMsg_Overloads,
				  sendFormattedMsg,
				  1, 4);

  // Function to send an usual message
  inline void sendMsg( const std::string &msg ) { IBegMsg << msg << IEndMsg; }

  // Function to send an error message
  inline void sendErrorMsg( const std::string &msg ) { IError << msg << IEndMsg; }

  // Function to send an information message
  inline void sendInfoMsg( const std::string &msg ) { IInfo << msg << IEndMsg; }

  // Function to send a warning message
  inline void sendWarningMsg( const std::string &msg ) { IWarning << msg << IEndMsg; }
}

BOOST_PYTHON_MODULE( general ) {

  // Initialize python and numpy (to prevent a segmentation fault)
  I_INIT_MODULE;
  
  // Wrappers from CutManager.hpp
  py::class_<isis::CutManager>("CutManager", py::init<const std::string&>())
    .def(py::init<const isis::CutManager&>())
    .def("bookCut"      , &isis::CutManager::bookCut, CutMgr::bookCut_Overloads())
    .def("clear"        , &isis::CutManager::clear)
    .def("close"        , &isis::CutManager::close)
    .def("getCut"       , &isis::CutManager::getCut)
    .def("makeMergedCut", &isis::CutManager::makeMergedCut, CutMgr::makeMergedCut_Overloads())
    .def("open"         , &isis::CutManager::open)
    .def("display"      , &isis::CutManager::display)
    .def("remove"       , &isis::CutManager::remove)
    .def("__getitem__"  , &isis::CutManager::operator[])
    ;

  // Wrappers from Definitions.hpp
  py::scope().attr("ISIS_VERSION") = ISIS_VERSION;
  
  // Wrappers from GlobalMessenger.hpp
  py::def("sendFormattedMsg", &Msg::sendFormattedMsg, Msg::sendFormattedMsg_Overloads());
  py::def("sendMsg"         , &Msg::sendMsg);
  py::def("sendErrorMsg"    , &Msg::sendErrorMsg);
  py::def("sendInfoMsg"     , &Msg::sendInfoMsg);
  py::def("sendWarningMsg"  , &Msg::sendWarningMsg);

  { // ANSIFormat scope
    class ANSIFormatNSP { };
    py::scope ANSIFormat = py::class_<ANSIFormatNSP>("ANSIFormat")
      .def_readwrite("ErrorColor"    , &isis::ANSIFormat::ErrorColor)
      .def_readwrite("InfoColor"     , &isis::ANSIFormat::InfoColor)
      .def_readwrite("WarningColor"  , &isis::ANSIFormat::WarningColor)
      .def_readwrite("ErrorPrefix"   , &isis::ANSIFormat::ErrorPrefix)
      .def_readwrite("InfoPrefix"    , &isis::ANSIFormat::InfoPrefix)
      .def_readwrite("MsgPrefix"     , &isis::ANSIFormat::MsgPrefix)
      .def_readwrite("WarningPrefix" , &isis::ANSIFormat::WarningPrefix)
      ;
  
    py::enum_<isis::ANSIFormat::Colors>("Colors")
      .value("aBlack"  , isis::ANSIFormat::aBlack)
      .value("aRed"    , isis::ANSIFormat::aRed)
      .value("aGreen"  , isis::ANSIFormat::aGreen)
      .value("aYellow" , isis::ANSIFormat::aYellow)
      .value("aBlue"   , isis::ANSIFormat::aBlue)
      .value("aMagenta", isis::ANSIFormat::aMagenta)
      .value("aCyan"   , isis::ANSIFormat::aCyan)
      .value("aWhite"  , isis::ANSIFormat::aWhite)
      .value("aNoColor", isis::ANSIFormat::aNoColor)
      ;
    py::enum_<isis::ANSIFormat::Styles>("Styles")
      .value("aNormal"   , isis::ANSIFormat::aNormal)
      .value("aBold"     , isis::ANSIFormat::aBold)
      .value("aFaint"    , isis::ANSIFormat::aFaint)
      .value("aItalic"   , isis::ANSIFormat::aItalic)
      .value("aUnderline", isis::ANSIFormat::aUnderline)
      .value("aNoStyle"  , isis::ANSIFormat::aNoStyle)
      ;
  }

  // Wrappers from LoopArray.hpp
  py::class_<isis::LoopArray>("LoopArray", py::init<>())
    .def(py::init<const size_t&, const size_t&, const size_t&>())
    .def(py::init<const isis::LoopArray&>())
    .def("addIndex"   , &isis::LoopArray::addIndex)
    .def("getPos"     , &isis::LoopArray::getPos)
    .def("next"       , &isis::LoopArray::next)
    .def("nindices"   , &isis::LoopArray::nindices)
    .def("nloops"     , &isis::LoopArray::nloops)
    .def("start"      , &isis::LoopArray::start)
    .def("status"     , &isis::LoopArray::status)
    .def("values"     , &LoopArray::values)
    .def("__getitem__", &LoopArray::getItem)
    ;
  
  // Wrappers from ProgessBar.h
  py::class_<isis::ProgressBar>("ProgressBar", py::init<const char*>())
    .def("end"     , &isis::ProgressBar::end)
    .def("display" , &isis::ProgressBar::display)
    .def("setColor", &isis::ProgressBar::setColor)
    .def("start"   , &isis::ProgressBar::start)
    ;
}
