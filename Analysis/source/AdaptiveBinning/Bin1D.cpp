///////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------- //
//						                     //
//  Analysis package	  			                     //
//        					                     //
// ----------------------------------------------------------------- //
//						                     //
//  AUTHOR: Miguel Ramos Pernas		                             //
//  e-mail: miguel.ramos.pernas@cern.ch		                     //
//						                     //
//  Last update: 12/01/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the one-dimensional bin class to work together with   //
//  AdaptiveBinning1D to create adaptive binned histograms.          //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning/Bin1D.h"


//______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//______________________________________________________________________________
// Constructor
Analysis::Bin1D::Bin1D() : Bin(), fMax( 0 ), fMin( 0 ) { }

//______________________________________________________________________________
// Destructor
Analysis::Bin1D::~Bin1D() { }

//______________________________________________________________________________


// -- METHOD

//______________________________________________________________________________
// Fills the bin if the point given is inside the considered range
void Analysis::Bin1D::Fill( const double &pos, const double &weight ) {
  if ( pos < fMin )
    fMin = pos;
  else if ( pos > fMax )
    fMax = pos;
  fEntries++;
  fSumOfWeights += weight;
}
