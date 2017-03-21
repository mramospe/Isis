////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#include "Messenger.h"
#include "Utils.h"
#include "VarWeighter.h"

#include "TBranch.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TLeaf.h"
#include "TList.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  VarWeighter::VarWeighter( TTree *rtree, TTree *wtree ) :
    fBinVector( 1 ), fRefTree( rtree ), fWgtTree( wtree ) {
    std::cout << "***********************************" << std::endl;
    std::cout << "*** Initializing weighter class ***" << std::endl;
    std::cout << "***********************************" << std::endl;
    std::cout << " - Reference tree:   " << rtree->GetName() << " ( " <<
      rtree->GetDirectory()->GetName() << " )" << std::endl;
    std::cout << " - Reweighting tree: " << wtree->GetName() << " ( " <<
      wtree->GetDirectory()->GetName() << " )" << std::endl;
  }

  //_______________________________________________________________________________
  //
  VarWeighter::~VarWeighter() { }

  //_______________________________________________________________________________
  //
  void VarWeighter::AddVariable( const std::string &name,
				 const size_t      &nbins,
				 const double      &min,
				 const double      &max,
				 const std::string &unit ) {
    std::cout << "*** Adding new variable < " << name << " > ***" << std::endl;
    std::cout << " - Number of bins: " << nbins << std::endl;
    std::cout << " - Minimum value:  " << min << std::endl;
    std::cout << " - Maximum value:  " << max << std::endl;
    if ( unit.size() )
      fVariables[ name ] = name + " ( " + unit + " )";
    else
      fVariables[ name ] = "";
    std::vector<VarBin> vbvector, appvector;
    for ( auto it = fBinVector.begin(); it != fBinVector.end(); it++ ) {
      vbvector = it->Split( name, nbins, min, max );
      appvector.insert( appvector.end(), vbvector.begin(), vbvector.end() );
    }

    // Makes the setup of the trees
    std::map<std::string, TLeaf*> refleafmap, wgtleafmap;
    std::map<std::string, double> valuesmap;
    this->SetupTrees( refleafmap, wgtleafmap, valuesmap );
  
    // Fills the bins with the reference data and removes the blank ones
    this->FillBinVector( fRefTree, refleafmap, valuesmap, appvector );
    for ( auto itb = appvector.begin(); itb != appvector.end(); itb++ )
      if ( itb->GetEntries() == 0 )
	appvector.erase( itb-- );
    for ( auto itb = appvector.begin(); itb != appvector.end(); itb++ )
      itb->fNentries = 0;

    // Fills the bins with the data to be weighted and removes the blank ones
    this->FillBinVector( fWgtTree, wgtleafmap, valuesmap, appvector );
    for ( auto itb = appvector.begin(); itb != appvector.end(); itb++ )
      if ( itb->GetEntries() == 0 )
	appvector.erase( itb-- );
    for ( auto itb = appvector.begin(); itb != appvector.end(); itb++ )
      itb->fNentries = 0;

    // Finally redefines the vector of bins of the class
    fBinVector = appvector;
    std::cout << " - Bin-list size:  " << fBinVector.size() << std::endl;
  }


  //_______________________________________________________________________________
  //
  void VarWeighter::ApplyWeights( TTree             *tree,
				  const std::string &wvname,
				  const std::string &svname,
				  const char        &type ) {
    std::cout << "*** Initializing weighting process ***" << std::endl;
    std::cout << "Working with tree: " << tree->GetName() <<
      " ( " << tree->GetDirectory()->GetName() << " )" << std::endl;
    // Generates the maps of values and with the leaves of the tree to be used
    std::map<std::string, TLeaf*> newleafmap;
    std::map<std::string, double> valuesmap;
    tree->SetBranchStatus( "*", false );
    for ( auto it = fVariables.begin(); it != fVariables.end(); it++ ) {
      tree->SetBranchStatus( it->first.c_str(), true );
      newleafmap[ it->first ] = tree->GetLeaf( it->first.c_str() );
      valuesmap[ it->first ]  = 0;
    }

    // Defines the variable to be added and its branch
    void *saddress, *waddress;
    TBranch *wbranch, *sbranch;
    if ( type == 'D' ) {
      waddress = new double;
      saddress = new double;
    }
    else if ( type == 'F' ) {
      waddress = new float;
      saddress = new float;
    }
    else {
      IError <<
	"Wrong variable type for the branch of weights < " <<
	type << " > ( F/D )" << IEndMsg;
      return;
    }

    // Creates the new branches
    std::cout << "Created output branches:" << std::endl;
    wbranch = tree->Branch( wvname.c_str(), waddress, ( wvname + '/' + type ).c_str() );
    std::cout << " - Weights < " << wbranch->GetName() << " >" << std::endl;
    sbranch = tree->Branch( svname.c_str(), saddress, ( svname + '/' + type ).c_str() );
    std::cout << " - Errors  < " << sbranch->GetName() << " >" << std::endl;

    // Fills the output branches
    std::cout << "Filling the output branch" << std::endl;
    if ( type == 'D' )
      this->Fill<double*>( tree, wbranch, waddress, sbranch, saddress, valuesmap, newleafmap );
    else
      this->Fill<float*>( tree, wbranch, waddress, sbranch, saddress, valuesmap, newleafmap );
    tree->SetBranchStatus( "*", true );

    // Writes the output tree
    tree->AutoSave();
    std::cout << "Written output tree < " << tree->GetName() << " > in file: " <<
      tree->GetDirectory()->GetName() << std::endl;

    // Deletes the allocated memory
    if ( type == 'D' ) {
      delete static_cast<double*>( waddress );
      delete static_cast<double*>( saddress );
    }
    else {
      delete static_cast<float*>( waddress );
      delete static_cast<float*>( saddress );
    }

    std::cout << "*** Weighting process finished ***" << std::endl;
  }

  //_______________________________________________________________________________
  //
  void VarWeighter::CalculateWeights( const double &maxrelerr,
				      const size_t &prec ) {
  
    std::cout << "***************************" << std::endl;
    std::cout << "*** Calculating weights ***" << std::endl;
    std::cout << "***************************" << std::endl;
    std::cout << "Working with < " << fBinVector.size() << " > bins" << std::endl;
    std::cout << "Generating a copy of the bin vector" << std::endl;
    std::vector<VarBin> refvector( fBinVector );
    double
      wentries = fWgtTree->GetEntries(),
      rentries = fRefTree->GetEntries(),
      ratio    = wentries/rentries,
      sratio   = std::sqrt( wentries*( rentries + wentries )/rentries )/rentries;

    // Makes the setup of the trees
    std::map<std::string, TLeaf*> refleafmap, wgtleafmap;
    std::map<std::string, double> valuesmap;
    std::cout << "Making maps of leaves from the input trees" << std::endl;
    this->SetupTrees( refleafmap, wgtleafmap, valuesmap );
  
    // Fills the bins for both the reference bins and the bins to be weighted
    std::cout << "Filling the reference bins" << std::endl;
    this->FillBinVector( fRefTree, refleafmap, valuesmap, refvector );
    std::cout << "Filling the bins to be weighted" << std::endl;
    this->FillBinVector( fWgtTree, wgtleafmap, valuesmap, fBinVector );

    // Calculates the weight for each bin. If the number of entries in the bin is
    // null, that region will be set with null weight.
    std::cout << "Calculating weights" << std::endl;
    size_t nowb = 0, nweights = 0, nwentries = 0, nrentries = 0;
    auto itr = refvector.begin(), itw = fBinVector.begin();
    while ( itr != refvector.end() ) {
      itw->SetWeight( itr->GetEntries(), ratio, sratio, maxrelerr );
      if ( itw->GetWeight() > 0. ) {
	nweights  += itw->GetWeight();
	nwentries += itw->GetEntries();
	nrentries += itr->GetEntries();
      }
      else
	nowb++;
      itw++;
      itr++;
    }
    std::cout << "Results:" << std::endl;
    std::cout << " - Number of entries (reference): " <<
      nrentries << " ( " << fRefTree->GetEntries() << " )" << std::endl;
    std::cout << " - Number of entries (weighted):  " <<
      nwentries << " ( " << fWgtTree->GetEntries() << " )" << std::endl;
    std::cout << " - Total sum of weights:          " << nweights << std::endl;
    std::cout << " - Number of null-weighted bins:  " <<
      nowb << " ( " << fBinVector.size() << " )" << std::endl;
  
    fRefTree->SetBranchStatus( "*", true );
    fWgtTree->SetBranchStatus( "*", true );

    // If a precision is specified, it displays the map of bins
    if ( prec )
      this->Print( prec );

    std::cout << "**************************" << std::endl;
    std::cout << "*** Weights calculated ***" << std::endl;
    std::cout << "**************************" << std::endl;
  }

  //_______________________________________________________________________________
  //
  TList* VarWeighter::MakeHistograms( std::string   variable,
				      const size_t &nbins,
				      const double &vmin,
				      const double &vmax ) {
    // Defines the names of the output histograms
    std::string
      hrwn = variable + "_RawWhist",
      hrrn = variable + "_RawRhist",
      hfwn = variable + "_Whist",
      hfrn = variable + "_Rhist";

    // These are the different histograms that are returned
    TH1D
      *hrw = new TH1D( hrwn.c_str(), hrwn.c_str(), nbins, vmin, vmax ),
      *hrr = new TH1D( hrrn.c_str(), hrrn.c_str(), nbins, vmin, vmax ),
      *hfw = new TH1D( hfwn.c_str(), hfwn.c_str(), nbins, vmin, vmax ),
      *hfr = new TH1D( hfrn.c_str(), hfrn.c_str(), nbins, vmin, vmax );

    // Makes the setup of the trees
    std::map<std::string, TLeaf*> refleafmap, wgtleafmap;
    std::map<std::string, double> valuesmap;
    this->SetupTrees( refleafmap, wgtleafmap, valuesmap );
  
    std::vector<std::string> variables;
    TLeaf *leaf;
    for ( auto it = fVariables.begin(); it != fVariables.end(); it++ )
      variables.push_back( it->first );
    if ( std::find( variables.begin(), variables.end(), variable ) == variables.end() ) {
      variables.push_back( variable );
      fRefTree->SetBranchStatus( variable.c_str(), true );
      fWgtTree->SetBranchStatus( variable.c_str(), true );
    }
  
    // This is the iterator of the vector of bins
    std::vector<VarBin>::iterator itb;

    // Fills the histograms from the tree to be weighted
    leaf = fWgtTree->GetLeaf( variable.c_str() );
    for ( Long64_t ievt = 0; ievt < fWgtTree->GetEntries(); ievt++ ) {
      fWgtTree->GetEntry( ievt );
      for ( auto it = valuesmap.begin(); it != valuesmap.end(); it++ )
	it->second = wgtleafmap[ it->first ]->GetValue();
      hrw->Fill( leaf->GetValue() );
      itb = fBinVector.begin();
      while( itb != fBinVector.end() && itb->IsOutside( valuesmap ) )
	itb++;
      if ( itb != fBinVector.end() && itb->GetWeight() )
	hfw->Fill( leaf->GetValue(), itb->GetWeight() );
    }
  
    // Fills the histograms from the reference tree
    leaf = fRefTree->GetLeaf( variable.c_str() );
    for ( Long64_t ievt = 0; ievt < fRefTree->GetEntries(); ievt++ ) {
      fRefTree->GetEntry( ievt );
      for ( auto it = valuesmap.begin(); it != valuesmap.end(); it++ )
	it->second = refleafmap[ it->first ]->GetValue();
      hrr->Fill( leaf->GetValue() );
      itb = fBinVector.begin();
      while( itb != fBinVector.end() && itb->IsOutside( valuesmap ) )
	itb++;
      if ( itb != fBinVector.end() && itb->GetWeight() > 0 )
	hfr->Fill( leaf->GetValue() );
    }

    // Enables again all the variables of the trees
    fRefTree->SetBranchStatus( "*", true );
    fWgtTree->SetBranchStatus( "*", true );

    // Builds the list to be returned
    TList *list = new TList;
    list->Add( hrw );
    list->Add( hrr );
    list->Add( hfw );
    list->Add( hfr );

    return list;
  }

  //_______________________________________________________________________________
  //
  void VarWeighter::Print( const size_t &prec ) {

    size_t
      n = 0,
      maxnsize = std::to_string( fBinVector.size() ).size(),
      maxvsize = 2*( prec + 10 ),
      maxesize = ( maxvsize + 1 )/2;

    // The variable < maxnsize > must have a size equal or greater than the bin
    // number identifier
    if ( maxnsize < 2 )
      maxnsize = 2;

    // Gets the size of the larger variable name
    for ( auto it = fVariables.begin(); it != fVariables.end(); it++ )
      if ( it->second.size() > maxvsize )
	maxvsize = it->second.size();
    std::string separator( maxnsize + 2*maxesize + 8 + fVariables.size()*( maxvsize + 2 ), '*' );

    // Displays the names of the variables
    std::cout << separator << std::setprecision( prec ) << std::endl;
    std::cout << std::right << "| " << CenterString( "No", maxnsize ) << " |";
    std::cout << CenterString( "Weight", ( maxvsize + 1 ) /2 ) << " |";
    std::cout << CenterString( "Error", ( maxvsize + 1 ) /2 ) << " |";
    for ( auto it = fVariables.begin(); it != fVariables.end(); it++ )
      std::cout << CenterString( it->second, maxvsize ) << " |";
    std::cout << std::endl;
    std::cout << separator << std::endl;

    // Displays the information of each bin
    for ( auto itb = fBinVector.begin(); itb != fBinVector.end(); itb++ ) {
      std::cout << "| " << std::setw( maxnsize ) << n++ << " |";
      std::cout << std::setw( maxesize ) << itb->GetWeight() << " |";
      std::cout << std::setw( maxesize ) << itb->GetError() << " |";
      itb->Print( maxesize );
    }
    std::cout << separator << std::endl;
  }

  //_______________________________________________________________________________
  //
  void VarWeighter::FillBinVector( TTree *tree,
				   std::map<std::string, TLeaf*> &leafmap,
				   std::map<std::string, double> &valuesmap,
				   std::vector<VarBin> &binvector ) {
    for ( Long64_t ievt = 0; ievt < tree->GetEntries(); ievt++ ) {
      tree->GetEntry( ievt );
      for ( auto it = valuesmap.begin(); it != valuesmap.end(); it++ )
	it->second = leafmap[ it->first ]->GetValue();
      for ( auto itb = binvector.begin(); itb != binvector.end(); itb++ )
	itb->IfInsideAdd( valuesmap );
    }  
  }

  //_______________________________________________________________________________
  //
  void VarWeighter::SetupTrees( std::map<std::string, TLeaf*> &refleafmap,
				std::map<std::string, TLeaf*> &wgtleafmap,
				std::map<std::string, double> &valuesmap ) {
    fRefTree->SetBranchStatus( "*", false );
    fWgtTree->SetBranchStatus( "*", false );
    for ( auto it = fVariables.begin(); it != fVariables.end(); it++ ) {
      fRefTree->SetBranchStatus( it->first.c_str(), true );
      fWgtTree->SetBranchStatus( it->first.c_str(), true );
      refleafmap[ it->first ] = fRefTree->GetLeaf( it->first.c_str() );
      wgtleafmap[ it->first ] = fWgtTree->GetLeaf( it->first.c_str() );
      valuesmap[ it->first ]  = 0;
    }
  }

}
