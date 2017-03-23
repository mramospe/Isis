//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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
#include <boost/python/scope.hpp>

#include "CutManager.h"
#include "Definitions.h"
#include "Messenger.h"
#include "MessengerConfig.h"
#include "LoopArray.h"
#include "ProgressBar.h"

#include <string>

namespace py = boost::python;


//_______________________________________________________________________________

// Wrappers for the class CutManager
namespace CutMgr {
  
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(BookCut_Overloads,
					 Isis::CutManager::BookCut,
					 1, 2);

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MakeMergedCut_Overloads,
					 Isis::CutManager::MakeMergedCut,
					 0, 1);
}

// Thin functions to send messages using the class Messenger
namespace Msg {

  // Function to send a formatted message
  void sendFormattedMsg( const std::string &msg,
			 const int &fgcol = Isis::ANSIFormat::aNoColor,
			 const int &bgcol = Isis::ANSIFormat::aNoColor,
			 const int &sty = Isis::ANSIFormat::aNoStyle ) {

    Isis::Messenger("", std::cout, fgcol, bgcol, sty) << msg << IEndMsg;
  }
  BOOST_PYTHON_FUNCTION_OVERLOADS(sendFormattedMsg_Overloads,
				  sendFormattedMsg,
				  1, 4);

  // Function to send an usual message
  void sendMsg( const std::string &msg ) { IBegMsg << msg << IEndMsg; }

  // Function to send an error message
  void sendErrorMsg( const std::string &msg ) { IError << msg << IEndMsg; }

  // Function to send an information message
  void sendInfoMsg( const std::string &msg ) { IInfo << msg << IEndMsg; }

  // Function to send a warning message
  void sendWarningMsg( const std::string &msg ) { IWarning << msg << IEndMsg; }
}

BOOST_PYTHON_MODULE( PyGeneral ) {

  // Wrappers from CutManager.h
  py::class_<Isis::CutManager>("CutManager", py::init<const std::string&>())
    .def(py::init<const Isis::CutManager&>())
    .def("BookCut"      , &Isis::CutManager::BookCut, CutMgr::BookCut_Overloads())
    .def("Clear"        , &Isis::CutManager::Clear)
    .def("Close"        , &Isis::CutManager::Close)
    .def("GetCut"       , &Isis::CutManager::GetCut)
    .def("MakeMergedCut", &Isis::CutManager::MakeMergedCut, CutMgr::MakeMergedCut_Overloads())
    .def("Open"         , &Isis::CutManager::Open)
    .def("Print"        , &Isis::CutManager::Print)
    .def("Remove"       , &Isis::CutManager::Remove)
    .def("__getitem__"  , &Isis::CutManager::operator[])
    ;

  // Wrappers from Definitions.h
  py::scope().attr("ISIS_VERSION") = ISIS_VERSION;
  
  // Wrappers from GlobalMessenger.h
  py::def("sendFormattedMsg", &Msg::sendFormattedMsg, Msg::sendFormattedMsg_Overloads());
  py::def("sendMsg"         , &Msg::sendMsg);
  py::def("sendErrorMsg"    , &Msg::sendErrorMsg);
  py::def("sendInfoMsg"     , &Msg::sendInfoMsg);
  py::def("sendWarningMsg"  , &Msg::sendWarningMsg);

  { // ANSIFormat scope
    class ANSIFormatNSP { };
    py::scope ANSIFormat = py::class_<ANSIFormatNSP>("ANSIFormat")
      .def_readwrite("ErrorColor"  , &Isis::ANSIFormat::ErrorColor)
      .def_readwrite("InfoColor"   , &Isis::ANSIFormat::InfoColor)
      .def_readwrite("WarningColor", &Isis::ANSIFormat::WarningColor)
      ;
  
    py::enum_<Isis::ANSIFormat::Colors>("Colors")
      .value("aBlack"  , Isis::ANSIFormat::aBlack)
      .value("aRed"    , Isis::ANSIFormat::aRed)
      .value("aGreen"  , Isis::ANSIFormat::aGreen)
      .value("aYellow" , Isis::ANSIFormat::aYellow)
      .value("aBlue"   , Isis::ANSIFormat::aBlue)
      .value("aMagenta", Isis::ANSIFormat::aMagenta)
      .value("aCyan"   , Isis::ANSIFormat::aCyan)
      .value("aWhite"  , Isis::ANSIFormat::aWhite)
      .value("aNoColor", Isis::ANSIFormat::aNoColor)
      ;
    py::enum_<Isis::ANSIFormat::Styles>("Styles")
      .value("aNormal"   , Isis::ANSIFormat::aNormal)
      .value("aBold"     , Isis::ANSIFormat::aBold)
      .value("aFaint"    , Isis::ANSIFormat::aFaint)
      .value("aItalic"   , Isis::ANSIFormat::aItalic)
      .value("aUnderline", Isis::ANSIFormat::aUnderline)
      .value("aNoStyle"  , Isis::ANSIFormat::aNoStyle)
      ;
  }

  // Wrappers from LoopArray.h
  py::class_<Isis::LoopArray>("LoopArray", py::init<>())
    .def(py::init<const size_t&, const size_t&, const size_t&>())
    .def(py::init<const Isis::LoopArray&>())
    .def("AddIndex"   , &Isis::LoopArray::AddIndex)
    .def("GetNindices", &Isis::LoopArray::GetNindices)
    .def("GetNloops"  , &Isis::LoopArray::GetNloops)
    .def("GetPos"     , &Isis::LoopArray::GetPos)
    .def("Next"       , &Isis::LoopArray::Next)
    .def("Start"      , &Isis::LoopArray::Start)
    .def("Status"     , &Isis::LoopArray::Status)
    .def("__getitem__", &Isis::LoopArray::operator[])
    ;
  
  // Wrappers from ProgessBar.h
  py::class_<Isis::ProgressBar>("ProgressBar", py::init<const char*>())
    .def("End"     , &Isis::ProgressBar::End)
    .def("Print"   , &Isis::ProgressBar::Print)
    .def("SetColor", &Isis::ProgressBar::SetColor)
    .def("Start"   , &Isis::ProgressBar::Start)
    ;
}
