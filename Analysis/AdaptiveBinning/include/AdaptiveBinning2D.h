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
//  Last update: 13/06/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the class to make two-dimensional adaptive binning    //
//  histograms. The construction can be made given a set of vectors  //
//  or a TTree object and the name of the leaves.                    //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING_2D
#define ADAPTIVE_BINNING_2D

#include "AdaptiveBinning.h"
#include "Bin2D.h"

#include <iostream>
#include <vector>

#include "TTree.h"
#include "TH2Poly.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning2D: public AdaptiveBinning {
    
  public:
    
    // Constructors and destructor
    AdaptiveBinning2D();
    AdaptiveBinning2D( size_t      min_occ,
		       double      xmin,
		       double      xmax,
		       double      ymin,
		       double      ymax,
		       const std::vector<double> &xvalues,
		       const std::vector<double> &yvalues,
		       const std::vector<double> &weights = std::vector<double>() );
    AdaptiveBinning2D( size_t      min_occ,
		       double      xmin,
		       double      xmax,
		       double      ymin,
		       double      ymax,
		       TTree      *tree,
		       const char *xleaf_name,
		       const char *yleaf_name,
		       const char *wleaf_name = 0,
		       double      xnorm = 1,
		       double      ynorm = 1 );
    ~AdaptiveBinning2D();

    // Methods
    TH2Poly* GetAdjHist( const char *name, const char *title );
    TH2Poly* GetAdjStruct( const char *name, const char *title );
    TH2Poly* GetHist( const char *name, const char *title );
    TH2Poly* GetStruct( const char *name, const char *title );

    // Inline methods
    inline std::vector<Analysis::Bin2D>::const_iterator AdjBinListBegin();
    inline std::vector<Analysis::Bin2D>::const_iterator AdjBinListEnd();
    inline std::vector<Analysis::Bin2D>::const_iterator BinListBegin();
    inline std::vector<Analysis::Bin2D>::const_iterator BinListEnd();
    inline const std::vector<Analysis::Bin2D>           GetAdjBinList() const;
    inline const std::vector<Analysis::Bin2D>           GetBinList() const;
    inline size_t                                       GetNbins() const;

  protected:
    
    // Attributes
    std::vector<Bin2D>  fAdjBinList;
    std::vector<Bin2D>  fBinList;
    std::vector<double> fXdata;
    double              fXmax;
    double              fXmin;
    std::vector<double> fYdata;
    double              fYmax;
    double              fYmin;
    std::vector<double> fWdata;

    // Method
    void Construct( const double &min_occ );

  };

  //_______________
  // INLINE METHODS

  // Different inline methods to get the iterators of the vector of bins
  inline std::vector<Analysis::Bin2D>::const_iterator AdaptiveBinning2D::AdjBinListBegin() {
    return fAdjBinList.cbegin();
  }
  inline std::vector<Analysis::Bin2D>::const_iterator AdaptiveBinning2D::AdjBinListEnd() {
    return fAdjBinList.cend();
  }
  inline std::vector<Analysis::Bin2D>::const_iterator AdaptiveBinning2D::BinListBegin() {
    return fBinList.cbegin();
  }
  inline std::vector<Analysis::Bin2D>::const_iterator AdaptiveBinning2D::BinListEnd() {
    return fBinList.cend();
  }
  // Returns a copy of the list of adjusted adaptive bins
  inline const std::vector<Analysis::Bin2D> AdaptiveBinning2D::GetAdjBinList() const {
    return fAdjBinList;
  }
  // Returns a copy of the list of adaptive bins
  inline const std::vector<Analysis::Bin2D> AdaptiveBinning2D::GetBinList() const {
    return fBinList;
  }
  // Returns the number of bins
  inline size_t AdaptiveBinning2D::GetNbins() const {
    return fBinList.size();
  }

}

#endif
