///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 13/06/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to make one-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
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
						const std::vector<double> &values,
						const std::vector<double> &weights ) :
  AdaptiveBinning(),
  fMax( vmax ),
  fMin( vmin ) {

  fData = new std::vector< std::pair<double, double> >;

  auto itv = values.begin();
  if ( weights.size() ) {
    if ( values.size() != weights.size() )
      std::cerr <<
	"ERROR: The lengths of the vectors containing the values and the weights do no match"
		<< std::endl;
    auto itw = weights.begin();
    while( itv != values.end() )
      if ( *itv >= vmin && *itv < vmax )
	fData -> push_back( std::make_pair( *itv++, *itw++ ) );
    fWeighted = true;
    this -> Construct( occ );
  }
  else {
    while( itv != values.end() )
      if ( *itv >= vmin && *itv < vmax )
	fData -> push_back( std::make_pair( *itv++, 1 ) );
    fWeighted = false;
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

  fData = new std::vector< std::pair<double, double> >;

  TLeaf *leaf = tree -> GetLeaf( vname );
  if ( wname ) {
    TLeaf *wleaf = tree -> GetLeaf( wname );
    for ( long int ievt = 0; ievt < tree -> GetEntries(); ++ievt ) {
      tree -> GetEntry( ievt );
      if ( leaf -> GetValue() >= vmin && leaf -> GetValue() < vmax )
	fData -> push_back( std::make_pair( leaf -> GetValue(), wleaf -> GetValue() ) );
    }
    fWeighted = true;
    this -> Construct( occ );
  }
  else {
    for ( long int ievt = 0; ievt < tree -> GetEntries(); ++ievt ) {
      tree -> GetEntry( ievt );
      if ( leaf -> GetValue() >= vmin && leaf -> GetValue() < vmax )
	fData -> push_back( std::make_pair( leaf -> GetValue(), 1 ) );
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
// it with the bin number corresponding to each event.
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
// Returns an empty histogram with the adaptive binning structure
TH1D* Analysis::AdaptiveBinning1D::GetStruct( const char *name, const char *title ) {
 double *bins = new double[ fBinList.size() + 1 ];
  for ( size_t i = 0; i < fBinList.size(); ++i )
    bins[ i ] = fBinList[ i ].fMin;
  bins[ fBinList.size() ] = fMax;
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
  for ( auto it = fData -> begin(); it != fData -> end(); ++it )
    sw += it -> second;
    
  // Calculates the number of bins
  size_t nbins = size_t( sw )/occ;
  
  // If the number of bins is zero an error is displayed
  if ( nbins == 0 )
    std::cerr << "ERROR: Occupancy requested is too big: " << occ << std::endl;
  
  // Creates the vector of bins
  fBinList = std::vector<Bin1D>( nbins, Bin1D( fMax ) );
  
  // Sorts the data and the weights
  std::sort( fData -> begin(), fData -> end(), [] ( std::pair<double, double> it1,
					      std::pair<double, double> it2 ) {
	       return it1.first < it2.first; } );
  
  // Depending if it is working with weights or with entries it fills the bins
  auto id = fData -> begin();
  if ( fWeighted ) {
    double auxsw = sw, swpb = 0;
    size_t binsout = 0;
    for ( auto ib = fBinList.begin(); ib != fBinList.end(); ++ib ) {
      swpb = auxsw/( nbins - binsout++ );
      while ( ib -> GetSumOfWeights() < swpb && id != fData -> end() ) {
	ib -> Fill( id -> first, id -> second );
	++id;
      }
      auxsw -= ib -> GetSumOfWeights();
    }
    // If the end of the data has not been reached, it fills the last bin with the rest of the events
    while ( id != fData -> end() )
      fBinList.back().Fill( id -> first, id++ -> second );
  }
  else {
    size_t
      enpb = fData -> size() / nbins,
      add1 = fData -> size() % nbins;
    for ( auto ib = fBinList.begin(); ib != fBinList.end(); ++ib ) {
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
  
  // Sets the different bin limits
  fBinList.front().fMin = fMin;
  
  // Displays the information of the process
  this -> DisplayInfo( fData -> size(), sw, nbins, sw/nbins );

  // Deletes the data allocated by the class
  delete fData;
}
