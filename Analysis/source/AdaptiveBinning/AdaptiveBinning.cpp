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
//  Implements the abstract class to generate adaptive binned        //
//  histograms. Generally the structure of the output histogram is   //
//  obtained calling the < GetStruct > method, while < GetHist >     //
//  returns this structured histogram filled with the data used to   //
//  make it.					                     //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning/AdaptiveBinning.h"


//______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//______________________________________________________________________________
// Constructor
Analysis::AdaptiveBinning::AdaptiveBinning() { }

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning::~AdaptiveBinning() { }

//______________________________________________________________________________


// -- METHODS

//______________________________________________________________________________
// Virtual method to construct the class once filled the vectors of data
void Analysis::AdaptiveBinning::Construct( const size_t &occ ) { }

//______________________________________________________________________________
// Displays the information of the construction
void Analysis::AdaptiveBinning::DisplayInfo( const size_t &entries,
					     const double &sw,
					     const size_t &nbins,
					     const double &occ ) {
  std::cout << "- Entries used:      " << entries << std::endl;
  std::cout << "- Sum of weights:    " << sw      << std::endl;
  std::cout << "- Number of bins:    " << nbins   << std::endl;
  std::cout << "- Occupancy per bin: " << occ     << std::endl;
}

//______________________________________________________________________________
// Virtual method to get the adaptive binned histogram filled with the data used
// to construct it
TObject* Analysis::AdaptiveBinning::GetHist( const char *name, const char *title ) { return 0; }

//______________________________________________________________________________
// Virtual method to get the a histogram with the adaptive binned structure
TObject* Analysis::AdaptiveBinning::GetStruct( const char *name, const char *title ) { return 0; }
