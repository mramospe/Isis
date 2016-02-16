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
//  This class allows to perform a weighting process for the events of a tree     //
//  trying to reproduce the shape of the events of another one. The process is    //
//  performed generating a multivariable bin map where the information of the     //
//  entries on each bin is computed, as well as the associated weight.            //
//                                                                                //
// ------------------------------------------------------------------------------ //
////////////////////////////////////////////////////////////////////////////////////


#ifndef VAR_WEIGHTER
#define VAR_WEIGHTER

#include "Weights/VarBin.h"

#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"
#include "TTree.h"

#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class VarWeighter {

  public:

    // Constructor and destructor
    VarWeighter( TTree *rtree, TTree *wtree );
    ~VarWeighter();

    // Methods
    void   AddVariable( const std::string &name,
			const size_t      &nbins,
			const double      &min,
			const double      &max,
			const std::string &units = "" );
    void   ApplyWeights( TTree             *tree,
			 const std::string &wvname,
			 const std::string &svname,
			 const char        &type );
    void   CalculateWeights( const double &maxrelerr = 1., const size_t &prec = 0 );
    TList* MakeHistograms( std::string   variable,
			   const size_t &nbins,
			   const double &vmin,
			   const double &vmax );
    void   Print( const size_t &prec = 4 );

    // Inline method
    inline const std::vector<Analysis::VarBin>& GetBinVector() const;

  protected:

    // Attributes
    std::vector<Analysis::VarBin>       fBinVector;
    TTree                              *fRefTree;
    TTree                              *fWhtTree;
    std::map<std::string, std::string>  fVariables;

  private:

    // Method
    template<class type>
    void Fill( TTree   *tree,
	       TBranch *wbranch,
	       void    *waddress,
	       TBranch *sbranch,
	       void    *saddress,
	       std::map<std::string, double> &valuesmap,
	       const std::map<std::string, TLeaf*> &newleafmap );
    void FillBinVector( TTree *tree,
			std::map<std::string, TLeaf*> &whtleafmap,
			std::map<std::string, double> &valuesmap,
			std::vector<Analysis::VarBin> &binvector );
    void SetupTrees( std::map<std::string, TLeaf*> &refleafmap,
		     std::map<std::string, TLeaf*> &whtleafmap,
		     std::map<std::string, double> &valuesmap );

  };

  //_________________
  // -- INLINE METHOD

  // Returns the vector of bins of the class
  inline const std::vector<Analysis::VarBin>& VarWeighter::GetBinVector() const {
    return fBinVector;
  }

  //___________________________
  // -- PRIVATE TEMPLATE METHOD

  // Fills the output branches. It is important, in the < while > statement to do not change
  // the order of the conditionals.
  template<class type>
  void VarWeighter::Fill( TTree   *tree,
			  TBranch *wbranch,
			  void    *waddress,
			  TBranch *sbranch,
			  void    *saddress,
			  std::map<std::string, double> &valuesmap,
			  const std::map<std::string, TLeaf*> &newleafmap ) {
    std::vector<Analysis::VarBin>::iterator itw;
    for ( Long64_t ievt = 0; ievt < tree -> GetEntries(); ievt++ ) {
      tree -> GetEntry( ievt );
      for ( auto it = valuesmap.begin(); it != valuesmap.end(); it++ )
	it -> second = newleafmap.at( it -> first ) -> GetValue();
      itw = fBinVector.begin();
      while ( itw != fBinVector.end() && itw -> IsOutside( valuesmap ) )
	itw++;
      if ( itw == fBinVector.end() ) {
	*static_cast<type>( waddress ) = 0;
	*static_cast<type>( saddress ) = 0;
      }
      else {
	*static_cast<type>( waddress ) = itw -> GetWeight();
	*static_cast<type>( saddress ) = itw -> GetError();
      }
      wbranch -> Fill();
      sbranch -> Fill();
      if ( ievt % 100000 == 0 )
	tree -> AutoSave();
    }
  }

}

#endif
