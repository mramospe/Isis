////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#include "VarBin.h"

#include <iomanip>
#include <iostream>
#include <cmath>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  VarBin::VarBin() : fError( 0 ), fNentries( 0 ), fWeight( 0 ) { }

  //_______________________________________________________________________________
  //
  VarBin::~VarBin() { }

  //_______________________________________________________________________________
  //
  void VarBin::ifInsideAdd( const std::map<std::string, double> &values ) {
    
    for ( auto it = values.begin(); it != values.end(); it++ )
      if ( this->isOutside( it -> first, it -> second ) )
	return;
    
    fNentries++;
  }

  //_______________________________________________________________________________
  //
  bool VarBin::isOutside( const std::string &vname, const double &value ) {
    
    std::pair<double, double> *range = &fVarRanges[ vname ];
    if ( value < range -> first || value >= range -> second )
      return true;
    
    return false;
  }

  //_______________________________________________________________________________
  //
  bool VarBin::isOutside( const std::map<std::string, double> &values ) {
    
    for ( auto it = values.begin(); it != values.end(); it++ )
      if ( this->isOutside( it -> first, it -> second ) )
	return true;
    
    return false;
  }

  //_______________________________________________________________________________
  //
  void VarBin::display( const size_t &width ) {
    
    std::pair<double, double> *pair;
    for ( auto itm = fVarRanges.begin(); itm != fVarRanges.end(); itm++ ) {
      pair = &itm -> second;
      std::cout << std::right << std::setw( width ) <<
	pair -> first << std::setw( width ) << pair -> second << " |";
    }
    std::cout << std::endl;
  }

  //_______________________________________________________________________________
  //
  void VarBin::setWeight( const size_t &rentries,
			  const double &ratio,
			  const double &sratio,
			  const double &maxrelerr ) {
    
    if ( rentries && fNentries ) {
      double
	rw = rentries*1./fNentries,
	ir = 1./rentries,
	iw = 1./fNentries;
      fError  = rw*std::sqrt( ratio*ratio*( ir + iw ) + sratio*sratio );
      fWeight = rw*ratio;
      if ( fError/fWeight > maxrelerr ) {
	fError  = 0;
	fWeight = 0;
      }
    }
    else {
      fWeight = 0;
      fError  = 0;
    }
  }

  //_______________________________________________________________________________
  //
  std::vector<VarBin> VarBin::split( const std::string &name,
				     const size_t      &nbins,
				     const double      &min,
				     const double      &max ) {
    
    double step = ( max - min )/nbins;
    std::vector<VarBin> outvector( nbins, *this );
    for ( size_t i = 0; i < nbins; i++ )
      outvector[ i ].fVarRanges[ name ] = std::make_pair( min + i*step, min + ( i + 1 )*step );
    outvector.back().fVarRanges[ name ].second = max;
    return outvector;
  }

}
