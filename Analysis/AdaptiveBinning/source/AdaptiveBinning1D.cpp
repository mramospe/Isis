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
//  Last update: 20/04/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the class to make one-dimensional adaptive binning    //
//  histograms. The construction can be made given a set of vectors  //
//  or a TTree object and the name of the leaves.                    //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning1D.h"

#include "TLeaf.h"

#include <algorithm>
#include <cmath>
#include <vector>


//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Main constructor
Analysis::AdaptiveBinning1D::AdaptiveBinning1D() : AdaptiveBinning() { }

//______________________________________________________________________________
// Constructor given vectors of values
Analysis::AdaptiveBinning1D::AdaptiveBinning1D( size_t  occ,
						double  vmin,
						double  vmax,
						std::vector<double> *values,
						std::vector<double> *weights ) :
  AdaptiveBinning(),
  fMax( vmax ),
  fMin( vmin ) {

  auto itv = values -> begin();
  if ( weights ) {
    if ( values -> size() != weights -> size() )
      std::cerr <<
	"ERROR: The lengths of the vectors containing the values and the weights do no match"
		<< std::endl;
    auto itw = weights -> begin();
    while( itv != values -> end() )
      if ( *itv >= vmin && *itv <= vmax )
	fData.push_back( std::make_pair( *itv++, *itw++ ) );
    this -> Construct( occ );
  }
  else {
    std::vector<double> wvals( values -> size(), 1 );
    auto itw = wvals.begin();
    while( itv != values -> end() )
      if ( *itv >= vmin && *itv <= vmax )
	fData.push_back( std::make_pair( *itv++, *itw++ ) );
    this -> Construct( occ );
  }
}

//______________________________________________________________________________
// Constructor given a TTree object
Analysis::AdaptiveBinning1D::AdaptiveBinning1D( size_t  occ,
						double  vmin,
						double  vmax,
						TTree  *tree,
						const char *vname,
						const char *wname ) :
  AdaptiveBinning(),
  fMax( vmax ),
  fMin( vmin ) {

  TLeaf *leaf = tree -> GetLeaf( vname );
  if ( wname ) {
    TLeaf *wleaf = tree -> GetLeaf( wname );
    for ( long int ievt = 0; ievt < tree -> GetEntries(); ievt++ ) {
      tree -> GetEntry( ievt );
      if ( leaf -> GetValue() >= vmin && leaf -> GetValue() <= vmax )
	fData.push_back( std::make_pair( leaf -> GetValue(), wleaf -> GetValue() ) );
    }
    fWeighted = true;
    this -> Construct( occ );
  }
  else {
    for ( long int ievt = 0; ievt < tree -> GetEntries(); ievt++ ) {
      tree -> GetEntry( ievt );
      if ( leaf -> GetValue() >= vmin && leaf -> GetValue() <= vmax )
	fData.push_back( std::make_pair( leaf -> GetValue(), 1 ) );
    }
    fWeighted = false;
    this -> Construct( occ );
  }
}

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning1D::~AdaptiveBinning1D() { }

//______________________________________________________________________________


// -- PUBLIC METHODS

//______________________________________________________________________________
// Generates a new branch with name < brname > in the tree < itree >, and fills
// it with the bin corresponding to each event.
void Analysis::AdaptiveBinning1D::BinsToTree( std::string        brname,
					      TTree             *itree,
					      const std::string &datavar ) {
  int ibin;
  itree -> SetBranchStatus( "*", false );
  itree -> SetBranchStatus( datavar.c_str(), true );
  TH1     *hist   = this -> GetStruct( brname.c_str(), brname.c_str() );
  TLeaf   *leaf   = itree -> GetLeaf( datavar.c_str() );
  TBranch *branch = itree -> Branch( brname.c_str(), &ibin, ( brname + "/I" ).c_str() );
  for ( Long64_t ievt = 0; ievt < itree -> GetEntries(); ++ievt ) {
    itree -> GetEntry( ievt );
    ibin = hist -> Fill( leaf -> GetValue() );
    branch -> Fill();
  }
  itree -> AutoSave();
  itree -> SetBranchStatus( "*", true );
  delete hist;
}

//______________________________________________________________________________
// Returns an histogram with the adaptive bins and filled with the data used to
// construct it
TH1D* Analysis::AdaptiveBinning1D::GetHist( const char *name, const char *title ) {
  TH1D *hist = this -> GetStruct( name, title );
  for ( auto it = fData.begin(); it != fData.end(); it++ )
    hist -> Fill( it -> first, it -> second );
  return hist;
}

//______________________________________________________________________________
// Returns an empty histogram with the adaptive binning structure
TH1D* Analysis::AdaptiveBinning1D::GetStruct( const char *name, const char *title ) {
 double *bins = new double[ fBinList.size() + 1 ];
  for ( size_t i = 0; i < fBinList.size(); i++ )
    bins[ i ] = fBinList[ i ].fMin;
  bins[ fBinList.size() ] = fBinList.back().fMax;
  TH1D *hist = new TH1D( name, title, fBinList.size(), bins );
  delete bins;
  return hist;
}

//______________________________________________________________________________


// -- PROTECTED METHOD

//______________________________________________________________________________
// Constructs the vector of one-dimensional adaptive bins
void Analysis::AdaptiveBinning1D::Construct( const size_t &occ ) {

  std::cout << "--- Making an adaptive binning division ---" << std::endl;

  // Calculates the sum of weights
  double sw = 0;
  for ( auto it = fData.begin(); it != fData.end(); it++ )
    sw += it -> second;

  // Calculates the minimum distance between events
  double new_delta, delta = 1;
  for ( auto it1 = fData.begin(); it1 != fData.end(); it1++ )
    for ( auto it2 = it1 + 1; it2 != fData.end(); it2++ ) {
      new_delta = std::abs( it2 -> first - it1 -> first );
      if ( new_delta < delta && new_delta != 0 )
	delta = new_delta;
    }
  delta /= 2;

  // Calculates the number of bins
  size_t nbins = size_t( sw )/occ;

  // If the number of bins is zero an error is displayed
  if ( nbins == 0 )
    std::cerr << "ERROR: Occupancy requested is too big: " << occ << std::endl;

  // Creates the vector of bins
  fBinList = std::vector<Bin1D>( nbins );

  // Sorts the data and the weights
  std::sort( fData.begin(), fData.end(), [] ( std::pair<double, double> it1,
					      std::pair<double, double> it2 ) {
	       return it1.first < it2.first; } );
  // Depending if it is working with weights or with entries it fills the bins
  auto id = fData.begin();
  if ( fWeighted ) {
    double swpb = sw/nbins;
    for ( auto ib = fBinList.begin(); ib != fBinList.end(); ib++, id++ ) {
      while ( ib -> GetSumOfWeights() < swpb && id != fData.end() )
	ib -> Fill( id -> first, id -> second );
      swpb = ( sw - swpb )/( --nbins );
    }
    // If the end of the data has not been reached, it fills the last bin with the rest of the events
    while ( id != fData.end() )
      fBinList.back().Fill( id -> first, id++ -> second );
  }
  else {
    size_t
      enpb = fData.size()/nbins,
      add1 = fData.size()%nbins;
    for ( auto ib = fBinList.begin(); ib != fBinList.end(); ib++ ) {
      if ( add1 ) {
	while ( ib -> GetEntries() < enpb + 1 )
	  ib -> Fill( id++ -> first );
	add1--;
      }
      else
	while ( ib -> GetEntries() < enpb )
	  ib -> Fill( id++ -> first );
    }
  }

  // Corrects the first and the last bins so the range is the specified in the constructor
  fBinList.front().fMin = fMin;
  fBinList.front().fMax = fBinList.front().fMax + delta;
  fBinList.back().fMax  = fMax;

  // Sets the different bin limits
  auto itlast = fBinList.end() - 1;
  for ( auto it = fBinList.begin() + 1; it != itlast; it++ )
    it -> SetBin( ( it - 1 ) -> fMax, it -> fMax + delta );
  fBinList.back().fMin = ( fBinList.end() - 2 ) -> fMax;

  // Displays the information of the process
  this -> DisplayInfo( fData.size(), sw, nbins, sw/nbins );
}
