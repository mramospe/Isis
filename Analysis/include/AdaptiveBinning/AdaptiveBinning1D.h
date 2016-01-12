
#ifndef ADAPTIVE_BINNING_1D
#define ADAPTIVE_BINNING_1D

#include "AdaptiveBinning/AdaptiveBinning.h"
#include "Bin1D.h"

#include "TH1D.h"
#include "TTree.h"

#include <iostream>


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning1D: public AdaptiveBinning {

  public:

    // Constructors and destructor
    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       std::vector<double> *values,
		       std::vector<double> *weights = 0 );

    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       TTree  *tree,
		       const char *vname,
		       const char *wname = 0 );
    ~AdaptiveBinning1D();

    // Methods
    TH1D* GetHist( const char *name, const char *title );
    TH1D* GetStruct( const char *name, const char *title );

    // Inline Methods
    inline std::vector<Bin1D> GetBinList() const;
    inline const size_t       GetNbins() const;
    inline std::vector<Bin1D>::const_iterator Begin();
    inline std::vector<Bin1D>::const_iterator End();

  protected:

    // Attributes
    std::vector<Bin1D>                      fBinList;
    double                                  fMax;
    double                                  fMin;
    std::vector<std::pair<double, double> > fData;

    // Method
    void Construct( const size_t &occ );
  
  };

  //_______________
  // INLINE METHODS

  inline std::vector<Bin1D>::const_iterator AdaptiveBinning1D::Begin() { return fBinList.begin(); }
  inline std::vector<Bin1D>::const_iterator AdaptiveBinning1D::End() { return fBinList.end(); }
  // Returns the vector with the one-dimensional bins
  inline std::vector<Bin1D> AdaptiveBinning1D::GetBinList() const { return fBinList; }
  // Returns the number of bins
  inline const size_t AdaptiveBinning1D::GetNbins() const { return fBinList.size(); }
}

#endif
