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
//
//  Description:
//
//  This class allows to perform a weighting process for the events of a tree
//  trying to reproduce the shape of the events of another one. The process is
//  performed generating a multivariable bin map where the information of the
//  entries on each bin is computed, as well as the associated weight.
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#ifndef VAR_WEIGHTER
#define VAR_WEIGHTER

#include "VarBin.h"

#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"
#include "TTree.h"

#include <vector>


//_______________________________________________________________________________

namespace Isis {

  class VarWeighter {

  public:

    // Main constructor. The reference tree and the tree to be weighed have to be
    // passed to the constructor.
    VarWeighter( TTree *rtree, TTree *wtree );

    // Destructor
    ~VarWeighter();

    // Adds a new variable to this class. There have to be provided the name, number
    // of bins, minimum and maximum values and the unit for it. The bins stored in
    // the class will automatically be splitted. The blank bins will be removed. This
    // will take a while, but will boost the other methods, as well as to reduce the
    // memory consumption.
    void AddVariable( const std::string &name,
			const size_t      &nbins,
			const double      &min,
			const double      &max,
			const std::string &units = "" );

    // Applies the weights to a given tree. Two branches will be added to it: one
    // containing the weights and the other the errors. The type of the branch has
    // also to be specified: D (double), F (float). The maximum allowed relative
    // error is an optional input. If specified, all the weights with an error
    // greater than that are going to be set to zero.
    void ApplyWeights( TTree             *tree,
			 const std::string &wvname,
			 const std::string &svname,
			 const char        &type );

    // Performs the calculation of the weights using the two attached trees. If a
    // precision is specified, the map of bins will be printed using such value.
    void CalculateWeights( const double &maxrelerr = 1., const size_t &prec = 0 );

    // Returns a list with the histograms for one of the variables in the tree given
    // its name, the number of bins and the minimum and maximumm value in the
    // histogram.
    TList* MakeHistograms( std::string   variable,
			   const size_t &nbins,
			   const double &vmin,
			   const double &vmax );

    // Displays the map of bins associated to this class
    void Print( const size_t &prec = 4 );

    // Returns the vector of bins of the class
    inline const std::vector<VarBin>& GetBinVector() const;

  protected:

    // Vector of bins
    std::vector<VarBin> fBinVector;

    // Tree used as a reference to make the bins
    TTree *fRefTree;

    // Tree to be weighted
    TTree *fWgtTree;

    // Map containing name and title for each variable
    std::map<std::string, std::string> fVariables;

  private:

    // Fills the output branches. It is important, in the < while > statement to do
    // not change the order of the conditionals.
    template<class type>
    void Fill( TTree   *tree,
	       TBranch *wbranch,
	       void    *waddress,
	       TBranch *sbranch,
	       void    *saddress,
	       std::map<std::string, double> &valuesmap,
	       const std::map<std::string, TLeaf*> &newleafmap );

    // Fills the bins in a vector given the tree, the map of leaves and the map of
    // values
    void FillBinVector( TTree *tree,
			std::map<std::string, TLeaf*> &wgtleafmap,
			std::map<std::string, double> &valuesmap,
			std::vector<VarBin> &binvector );

    // Disables all the branches that are not in the map of leaves. Only those to be
    // used are enabled. Also defines the keys for the given maps.
    void SetupTrees( std::map<std::string, TLeaf*> &refleafmap,
		     std::map<std::string, TLeaf*> &wgtleafmap,
		     std::map<std::string, double> &valuesmap );

  };

  //_______________________________________________________________________________
  //
  inline const std::vector<VarBin>& VarWeighter::GetBinVector() const {
    return fBinVector;
  }

  //_______________________________________________________________________________
  //
  template<class type>
  void VarWeighter::Fill( TTree   *tree,
			  TBranch *wbranch,
			  void    *waddress,
			  TBranch *sbranch,
			  void    *saddress,
			  std::map<std::string, double> &valuesmap,
			  const std::map<std::string, TLeaf*> &newleafmap ) {
    std::vector<VarBin>::iterator itw;
    for ( Long64_t ievt = 0; ievt < tree->GetEntries(); ievt++ ) {
      tree->GetEntry( ievt );
      for ( auto it = valuesmap.begin(); it != valuesmap.end(); it++ )
	it->second = newleafmap.at( it->first )->GetValue();
      itw = fBinVector.begin();
      while ( itw != fBinVector.end() && itw->IsOutside( valuesmap ) )
	itw++;
      if ( itw == fBinVector.end() ) {
	*static_cast<type>( waddress ) = 0;
	*static_cast<type>( saddress ) = 0;
      }
      else {
	*static_cast<type>( waddress ) = itw->GetWeight();
	*static_cast<type>( saddress ) = itw->GetError();
      }
      wbranch->Fill();
      sbranch->Fill();
      if ( ievt % 100000 == 0 )
	tree->AutoSave();
    }
  }

}

#endif
