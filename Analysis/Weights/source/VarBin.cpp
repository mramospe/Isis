////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//  Analysis package                                                              //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  AUTHOR: Miguel Ramos Pernas                                                   //
//  e-mail: miguel.ramos.pernas@cern.ch                                           //
//                                                                                //
//  Last update: 16/02/2016                                                       //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  Description:                                                                  //
//                                                                                //
//  This class defines a multivariable bin. The bin has information about the     //
//  variables, the number of entries and an associated weight. This is a friend   //
//  class of < VarWeighter >.                                                     //
//                                                                                //
// ------------------------------------------------------------------------------ //
////////////////////////////////////////////////////////////////////////////////////


#include "Weights/VarBin.h"

#include <iomanip>
#include <iostream>
#include <cmath>


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::VarBin::VarBin() : fError( 0 ), fNentries( 0 ), fWeight( 0 ) { }

//_______________________________________________________________________________
// Destructor
Analysis::VarBin::~VarBin() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Increases the number of entries in one unit for this bin if the values given
// are inside its limits
void Analysis::VarBin::IfInsideAdd( const std::map<std::string, double> &values ) {
  for ( auto it = values.begin(); it != values.end(); it++ )
    if ( this -> IsOutside( it -> first, it -> second ) )
      return;
  fNentries++;
}

//_______________________________________________________________________________
// Checks if the value for the variable given is inside this bin
bool Analysis::VarBin::IsOutside( const std::string &vname, const double &value ) {
  std::pair<double, double> *range = &fVarRanges[ vname ];
  if ( value < range -> first || value >= range -> second )
    return true;
  return false;
}

//_______________________________________________________________________________
// Checks if the values given are inside this bin
bool Analysis::VarBin::IsOutside( const std::map<std::string, double> &values ) {
  for ( auto it = values.begin(); it != values.end(); it++ )
    if ( this -> IsOutside( it -> first, it -> second ) )
      return true;
  return false;
}

//_______________________________________________________________________________
// Displays the information of this bin, where the width of the columns is
// specified as an input
void Analysis::VarBin::Print( const size_t &width ) {
  std::pair<double, double> *pair;
  for ( auto itm = fVarRanges.begin(); itm != fVarRanges.end(); itm++ ) {
    pair = &itm -> second;
    std::cout << std::right << std::setw( width ) <<
      pair -> first << std::setw( width ) << pair -> second << " |";
  }
  std::cout << std::endl;
}

//_______________________________________________________________________________
// Sets the weight for this bin. The input parameters are the number of entries
// for the reference bin, the ratio between the entries for each tree and its
// error. Both the weight and the error are computed in this method.
void Analysis::VarBin::SetWeight( const size_t &rentries,
				  const double &ratio,
				  const double &sratio ) {
  if ( rentries && fNentries ) {
    double
      rw = rentries*1./fNentries,
      ir = 1./rentries,
      iw = 1./fNentries;
    fError  = rw*std::sqrt( ratio*ratio*( ir + iw ) + sratio*sratio );
    fWeight = rw*ratio;
  }
  else {
    fWeight = 0;
    fError  = 0;
  }
}

//_______________________________________________________________________________


// -- PROTECTED METHOD

//_______________________________________________________________________________
// Divides this bin using the information given
std::vector<Analysis::VarBin> Analysis::VarBin::Split( const std::string &name,
						       const size_t      &nbins,
						       const double      &min,
						       const double      &max ) {
  double step = ( max - min )/nbins;
  std::vector<Analysis::VarBin> outvector( nbins, *this );
  for ( size_t i = 0; i < nbins; i++ )
    outvector[ i ].fVarRanges[ name ] = std::make_pair( min + i*step, min + ( i + 1 )*step );
  outvector.back().fVarRanges[ name ].second = max;
  return outvector;
}
