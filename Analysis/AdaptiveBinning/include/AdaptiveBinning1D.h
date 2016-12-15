///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/12/2016
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


#ifndef ADAPTIVE_BINNING_1D
#define ADAPTIVE_BINNING_1D

#include "AdaptiveBinning.h"
#include "Bin1D.h"

#include "TH1D.h"
#include "TTree.h"

#include <iostream>


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning1D: public AdaptiveBinning {

  public:

    // Constructors and destructor
    AdaptiveBinning1D();
    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       const std::vector<double> &values,
		       const std::vector<double> &weights = std::vector<double>() );
    ~AdaptiveBinning1D();

    // Methods
    void  BinsToTree( std::string brname, TTree *itree, const std::string &datavar );
    TH1D* GetStruct( const char *name, const char *title );

    // Inline Methods
    inline const std::vector<Bin1D>& GetBinList() const;
    inline const double              GetMax() const;
    inline const double              GetMin() const;
    inline const size_t              GetNbins() const;
    inline std::vector<Bin1D>::const_iterator Begin();
    inline std::vector<Bin1D>::const_iterator End();

  protected:

    // Attributes
    std::vector<Bin1D>                        fBinList;
    double                                    fMax;
    double                                    fMin;
    std::vector< std::pair<double, double> > *fData;

    // Method
    void Construct( const size_t &occ );
  
  };

  //_______________
  // INLINE METHODS

  inline std::vector<Bin1D>::const_iterator AdaptiveBinning1D::Begin() { return fBinList.begin(); }
  inline std::vector<Bin1D>::const_iterator AdaptiveBinning1D::End() { return fBinList.end(); }
  // Returns the vector with the one-dimensional bins
  inline const std::vector<Bin1D>& AdaptiveBinning1D::GetBinList() const { return fBinList; }
  // Returns the maximum value for the variable of this class
  inline const double AdaptiveBinning1D::GetMax() const { return fMax; }
  // Returns the minimum value for the variable of this class
  inline const double AdaptiveBinning1D::GetMin() const { return fMin; }
  // Returns the number of bins
  inline const size_t AdaptiveBinning1D::GetNbins() const { return fBinList.size(); }
}

#endif
