/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 06/06/2016                            //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  Description:                                       //
//                                                     //
//  Implements the class to perform CLs analysis,      //
//  providing the tools to make a mu-scan, get the     //
//  p-values and the ROC curve, etc.                   //
//                                                     //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#include "CLsAnalyser.h"

#include <algorithm>
#include <cmath>
#include <cstring>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::CLsAnalyser::CLsAnalyser() { }

//_______________________________________________________________________________
// Constructor that allocates the distribution of the two samples in two vectors
Analysis::CLsAnalyser::CLsAnalyser( const Analysis::CLsArray &old_hyp,
				    const Analysis::CLsArray &new_hyp,
				    const int                &npoints ) {

  fNewHyp  = new_hyp;
  fOldHyp  = old_hyp;
  fNPoints = npoints;

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &Analysis::CLsArray::GetPoissonProb;
  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Destructor
Analysis::CLsAnalyser::~CLsAnalyser() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Gets the distribution of the old and new hypothesis
void Analysis::CLsAnalyser::Evaluate() {
  
  fNewHypArray = std::vector<double>( fNPoints );
  fOldHypArray = std::vector<double>( fNPoints );

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fNewHypArray[ ievt ] = this -> TestStat( fNewHyp.GenerateGaussian() );
  else
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fNewHypArray[ ievt ] = this -> TestStat( fNewHyp.GeneratePoisson() );
  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fOldHypArray[ ievt ] = this -> TestStat( fOldHyp.GenerateGaussian() );
  else
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fOldHypArray[ ievt ] = this -> TestStat( fOldHyp.GeneratePoisson() );

  std::sort( fNewHypArray.begin(), fNewHypArray.end() );
  std::sort( fOldHypArray.begin(), fOldHypArray.end() );
}

//_______________________________________________________________________________
// Gets the new hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetNewHypHist( const char *name, const int &nbins ) {
  double
    step( ( fNewHypArray[ fNPoints - 1 ] - fOldHypArray[ 0 ] )/2 ),
    xmin( fOldHypArray[ 0 ] - step ),
    xmax( fNewHypArray[ fNPoints - 1 ] + step );

  TH1D *hist = new TH1D( name,
			 name,
			 nbins,
			 xmin,
			 xmax );

  for ( int i = 0; i < fNPoints; i++ )
    hist -> Fill( fNewHypArray[ i ] );
  return hist;
}

//_______________________________________________________________________________
// Gets the old hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetOldHypHist( const char *name, const int &nbins ) {
  double
    step( ( fNewHypArray[ fNPoints - 1 ] - fOldHypArray[ 0 ] )/2 ),
    xmin( fOldHypArray[ 0 ] - step ),
    xmax( fNewHypArray[ fNPoints - 1 ] + step );
  TH1D *hist = new TH1D( name,
			 name,
			 nbins,
			 xmin,
			 xmax );
  for ( int i = 0; i < fNPoints; i++ )
    hist -> Fill( fOldHypArray[ i ] );
  return hist;
}

//_______________________________________________________________________________
// Gets the p-value for a given hypothesis
double Analysis::CLsAnalyser::GetPValue( const std::vector<double> &list,
					 const double              &t0,
					 const char                type ) {
  int evts( 0 );
  std::vector<double>::iterator it;
  if ( type == 'N' ) {
    for ( auto it = list.begin(); it != list.end(); it++ )
      if ( *it < t0 )
	evts++;
  }
  else {
    for ( auto it = list.begin(); it != list.end(); it++ )
      if ( *it > t0 )
	evts++;
  }
  return ( double ) evts/fNPoints;
}

//_______________________________________________________________________________
// Gets the CLs for a given confidence level ( introduce 0.8413 and 1 - 0.8413
// to get the region inside 1 sigma ).
double Analysis::CLsAnalyser::GetQCLs( const double &q, const char type ) {
  double tq;
  if ( type == 'N' )
    tq = fNewHypArray[ int( q*fNPoints ) ];
  else
    tq = fOldHypArray[ int( q*fNPoints ) ];
  return this -> GetBeta( tq )/( 1 - this -> GetAlpha( tq ) );    
}

//_______________________________________________________________________________
// Gets the ROC curve
TGraph* Analysis::CLsAnalyser::GetROC( const int &npoints ) {
  double
    it_min( fOldHypArray[ 0 ] ),
    step( ( fNewHypArray[ fNPoints - 1 ] - it_min )/npoints );
  TGraph *ROC = new TGraph();
  for ( int i = 0; i < npoints; i++ )
    ROC -> SetPoint( i,
		     this -> GetAlpha( it_min + i*step ),
		     1 - this -> GetBeta( it_min + i*step ) );
  return ROC;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The arrays containing the distributions of the old
// hypothesis are cleared.
void Analysis::CLsAnalyser::SetHypothesis( const Analysis::CLsArray &old_hyp,
					   const Analysis::CLsArray &new_hyp,
					   const int                &npoints ) {
  this -> SetNewHypothesis( new_hyp );
  this -> SetOldHypothesis( old_hyp );
  fNPoints = npoints;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old new
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetNewHypothesis( const Analysis::CLsArray &new_hyp ) {
  fNewHyp = new_hyp;
  fNewHypArray.clear();
  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old old
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetOldHypothesis( const Analysis::CLsArray &old_hyp ) {

  fOldHyp = old_hyp;
  fOldHypArray.clear();

  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &Analysis::CLsArray::GetPoissonProb;
}
