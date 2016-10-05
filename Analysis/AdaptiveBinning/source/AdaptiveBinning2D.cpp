///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/06/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to make two-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning2D.h"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning2D::AdaptiveBinning2D() : AdaptiveBinning() { }

//______________________________________________________________________________
// Constructor given vectors of values
Analysis::AdaptiveBinning2D::AdaptiveBinning2D( size_t      min_occ,
						double      xmin,
						double      xmax,
						double      ymin,
						double      ymax,
						const std::vector<double> &xvalues,
						const std::vector<double> &yvalues,
						const std::vector<double> &weights ) :
  AdaptiveBinning(),
  fXmax( xmax ),
  fXmin( xmin ),
  fYmax( ymax ),
  fYmin( ymin ),
  fWdata( 0 ) {

  fWeighted = weights.size();

  if ( weights.size() )
    fWdata = &weights;
  fXdata = &xvalues;
  fYdata = &yvalues;
  this -> Construct( min_occ );
}

//______________________________________________________________________________
// Constructor using a TTree object
Analysis::AdaptiveBinning2D::AdaptiveBinning2D( size_t      min_occ,
						double      xmin,
						double      xmax,
						double      ymin,
						double      ymax,
						TTree      *tree,
						const char *xleaf_name,
						const char *yleaf_name,
						const char *wleaf_name ) :
  AdaptiveBinning(),
  fXmax( xmax ),
  fXmin( xmin ),
  fYmax( ymax ),
  fYmin( ymin ),
  fWdata( 0 ) {
  
  fWeighted = wleaf_name;
  
  std::vector<double>
    *xdata = new std::vector<double>,
    *ydata = new std::vector<double>,
    *wdata;
  
  size_t nentries = tree -> GetEntries();
  TLeaf
    *xleaf = tree -> GetLeaf( xleaf_name ),
    *yleaf = tree -> GetLeaf( yleaf_name );
  double x, y, w;
  // Gets the data and fills the vectors that contain the points and the weights
  if ( wleaf_name ) {
    wdata = new std::vector<double>;
    TLeaf *wleaf( tree -> GetLeaf( wleaf_name ) );
    for ( size_t ievt = 0; ievt < nentries; ievt++ ) {
      tree -> GetEntry( ievt );
      x = xleaf -> GetValue();
      y = yleaf -> GetValue();
      w = wleaf -> GetValue();
      if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {
	xdata -> push_back( x );
	ydata -> push_back( y );
	wdata -> push_back( w );
      }
    }
  }
  else {
    for ( size_t ievt = 0; ievt < nentries; ievt++ ) {
      tree -> GetEntry( ievt );
      x = xleaf -> GetValue();
      y = yleaf -> GetValue();
      if ( x > fXmin && x < fXmax && y > fYmin && y < fYmax ) {
	xdata -> push_back( x );
	ydata -> push_back( y );
      }
    }
    wdata = new std::vector<double> ( xdata -> size(), 1 );
  }

  fXdata = xdata;
  fYdata = ydata;
  fWdata = wdata;

  this -> Construct( min_occ );

  // Deletes the memory allocated by the class
  delete fXdata;
  delete fYdata;
  delete fWdata;
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning2D::~AdaptiveBinning2D() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Generates a new branch with name < brname > in the tree < itree >, and fills
// it with the bin number corresponding to each event.
void Analysis::AdaptiveBinning2D::BinsToTree( std::string        brname,
					      TTree             *itree,
					      const std::string &xvar,
					      const std::string &yvar ) {
  int ibin;
  itree -> SetBranchStatus( "*", false );
  itree -> SetBranchStatus( xvar.c_str(), true );
  itree -> SetBranchStatus( yvar.c_str(), true );
  
  TH2 *hist = this -> GetStruct( brname.c_str(), brname.c_str() );
  
  TLeaf
    *xleaf = itree -> GetLeaf( xvar.c_str() ),
    *yleaf = itree -> GetLeaf( yvar.c_str() );
    
  TBranch *branch = itree -> Branch( brname.c_str(), &ibin, ( brname + "/I" ).c_str() );
  
  for ( Long64_t ievt = 0; ievt < itree -> GetEntries(); ++ievt ) {
    itree -> GetEntry( ievt );
    ibin = hist -> Fill( xleaf -> GetValue(), yleaf -> GetValue() );
    branch -> Fill();
  }
  itree -> AutoSave();
  itree -> SetBranchStatus( "*", true );
  
  delete hist;
}

//______________________________________________________________________________
// Makes an adjusted adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetAdjStruct( const char *name, const char *title ) {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fAdjBinList.begin(); it != fAdjBinList.end();	it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );
  return hist;
}

//______________________________________________________________________________
// Makes an adaptive binned histogram
TH2Poly* Analysis::AdaptiveBinning2D::GetStruct( const char *name, const char *title ) {
  TH2Poly *hist = new TH2Poly( name, title, fXmin, fXmax, fYmin, fYmax );
  for ( auto it = fBinList.begin(); it != fBinList.end(); it++ )
    hist -> AddBin( it -> fXmin, it -> fYmin, it -> fXmax, it -> fYmax );
  return hist;
}

//______________________________________________________________________________


// -- PROTECTED METHOD

//______________________________________________________________________________
// Constructs the vector of two-dimensional adaptive bins
void Analysis::AdaptiveBinning2D::Construct( const double &min_occ ) {

  std::cout << "--- Making an adaptive binning division ---" << std::endl;

  // Gets the minimum distance between points
  double
    delta_x = std::abs( fXdata -> front() - fXdata -> back() ),
    delta_y = std::abs( fYdata -> front() - fYdata -> back() ),
    delta,
    new_delta;

  for ( auto it1 = fXdata -> begin(); it1 != fXdata -> end(); it1++ )
    for ( auto it2 = it1 + 1; it2 != fXdata -> end(); it2++ ) {	  
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_x )
	delta_x = new_delta;
    }

  for ( auto it1 = fYdata -> begin(); it1 != fYdata -> end(); it1++ )
    for ( auto it2 = it1 + 1; it2 != fYdata -> end(); it2++ ) {
      new_delta = std::abs( *it2 - *it1 );
      if ( new_delta < delta_y )
	delta_y = new_delta;
    }
  delta = std::min( delta_x, delta_y )/2;

  // Modifies the minimum and maximum values of the axis to properly construct the histograms
  fXmin -= delta;
  fXmax += delta;
  fYmin -= delta;
  fYmax += delta;

  // Initializes the bins list
  fBinList = std::vector<Analysis::Bin2D>( 1, Bin2D( fXmin, fXmax, fYmin, fYmax ) );

  // Gets the sum of the weights (to get the number of true entries)
  double sum_of_evts = 0;
  for ( auto it = fWdata -> begin(); it != fWdata -> end(); it++ )
    sum_of_evts += *it;

  // Makes the adaptive bins
  size_t
    max_iter = std::floor( std::log( sum_of_evts/min_occ )/std::log( 2 ) ),
    nbins    = 1;

  if ( max_iter == 0 )
    std::cerr << " ERROR: minimum occupancy is so big, decrease it." << std::endl;

  double
    xrange( *std::max_element( fXdata -> begin(), fXdata -> end() ) -
	     *std::min_element( fXdata -> begin(), fXdata -> end() ) ),
    yrange( *std::max_element( fYdata -> begin(), fYdata -> end() ) -
	     *std::min_element( fYdata -> begin(), fYdata -> end() ) );

  for ( size_t i = 0; i < max_iter; i++ ) {
    for ( size_t ibin = 0; ibin < nbins; ibin++ ) {
      for ( size_t ievt = 0; ievt < fWdata -> size(); ievt++ )
	fBinList[ ibin ].Fill( fXdata -> at( ievt ), fYdata -> at( ievt ), fWdata -> at( ievt ) );
      fBinList.push_back( fBinList[ ibin ].Divide( xrange, yrange ) );
    }
    // Sets the new number of bins
    nbins *= 2;
  }

  // Fills the data for the last time to get the limits of the bins
  for ( auto itbin = fBinList.begin(); itbin != fBinList.end(); itbin++ )
    for ( size_t ievt = 0; ievt < fWdata -> size(); ievt++ )
      itbin -> Fill( fXdata -> at( ievt ), fYdata -> at( ievt ), fWdata -> at( ievt ) );

  // Makes the list of adjusted bins
  fAdjBinList = std::vector<Analysis::Bin2D>( fBinList );
  for ( auto itbin = fAdjBinList.begin(); itbin != fAdjBinList.end(); itbin++ )
    itbin -> AdjustBin( fXmin, fXmax, fYmin, fYmax, delta );
  
  // Displays the information of the process
  this -> DisplayInfo( fWdata -> size(), sum_of_evts, nbins, sum_of_evts );

}
