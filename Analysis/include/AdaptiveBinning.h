/////////////////////////////////////////////////////////
// --------------------------------------------------- //
//						       //
//  Analysis package  				       //
//        					       //
// --------------------------------------------------- //
//						       //
//  AUTHOR: Miguel Ramos Pernas		               //
//  e-mail: miguel.ramos.pernas@cern.ch		       //
//						       //
//  Last update: 11/01/2016			       //
//   						       //
// --------------------------------------------------- //
//						       //
//  Description:				       //
//						       //
//  Implements the class to make adaptive binned       //
//  histograms given a TTree and the name of the two   //
//  leaves to work with. The relation between the      //
//  number of entries and the occupancy of each bin    //
//  is a power of two.				       //
//						       //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING
#define ADAPTIVE_BINNING

#include <iostream>
#include <vector>

#include "TTree.h"
#include "TH2Poly.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning {
    
  public:
    
    // Constructor
    AdaptiveBinning();
    AdaptiveBinning( size_t      occ_min,
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

    // Destructor
    ~AdaptiveBinning();

    // Methods
    TH2Poly* GetAdjHist( const char *name, const char *title );
    TH2Poly* GetAdjStruct( const char *name, const char *title );
    TH2Poly* GetHist( const char *name, const char *title );
    TH2Poly* GetStruct( const char *name, const char *title );

    // Nested class
    class Bin {

      friend AdaptiveBinning;

    public:

      // Constructor
      Bin( const double &xmin, const double &xmax, const double &ymin, const double &ymax );

      // Destructor
      ~Bin();

      // Methods
      void AdjustBin( const double &xmin,
		      const double &xmax,
		      const double &ymin,
		      const double &ymax,
		      const double &delta );
      void Fill( const double &x, const double &y, const double &w );

      // Inline methods
      inline const double GetNpoints() const;
      inline const double GetXmin() const;
      inline const double GetXmax() const;
      inline const double GetYmin() const;
      inline const double GetYmax() const;
    
      // Attributes
      double              fNpoints;
      double              fXmax;
      double              fXmaxPoint;
      double              fXmedian;
      double              fXmin;
      double              fXminPoint;
      std::vector<double> fXpoints;
      double              fYmax;
      double              fYmaxPoint;
      double              fYmedian;
      double              fYmin;
      double              fYminPoint;
      std::vector<double> fYpoints;
      std::vector<double> fWpoints;

    protected:

      // Methods
      Bin Divide( double &xrange, double &yrange );
    
    private:

      // Methods
      void                CalcMedians();
      void                Clear();
      std::vector<double> Sort( std::vector<double> &d_vector,
				std::vector<double> &w_vector );

    };

    // Inline methods
    inline std::vector<AdaptiveBinning::Bin>::const_iterator AdjBinListBegin();
    inline std::vector<AdaptiveBinning::Bin>::const_iterator AdjBinListEnd();
    inline std::vector<AdaptiveBinning::Bin>::const_iterator BinListBegin();
    inline std::vector<AdaptiveBinning::Bin>::const_iterator BinListEnd();
    inline const std::vector<AdaptiveBinning::Bin>           GetAdjBinList() const;
    inline const std::vector<AdaptiveBinning::Bin>           GetBinList() const;
    inline size_t                                            GetNbins() const;

  protected:
    
    // Attributes
    std::vector<Bin>    fAdjBinList;
    std::vector<Bin>    fBinList;
    double              fDelta;
    size_t              fLength;
    std::vector<double> fXdata;
    double              fXmax;
    double              fXmin;
    std::vector<double> fYdata;
    double              fYmax;
    double              fYmin;
    std::vector<double> fWdata;

  };

  //_____________________________________________
  // INLINE METHODS FOR THE ADAPTIVEBINNING CLASS
  inline std::vector<AdaptiveBinning::Bin>::const_iterator AdaptiveBinning::AdjBinListBegin() {
    return fAdjBinList.cbegin();
  }
  inline std::vector<AdaptiveBinning::Bin>::const_iterator AdaptiveBinning::AdjBinListEnd() {
    return fAdjBinList.cend();
  }
  inline std::vector<AdaptiveBinning::Bin>::const_iterator AdaptiveBinning::BinListBegin() {
    return fBinList.cbegin();
  }
  inline std::vector<AdaptiveBinning::Bin>::const_iterator AdaptiveBinning::BinListEnd() {
    return fBinList.cend();
  }
  inline const std::vector<AdaptiveBinning::Bin> AdaptiveBinning::GetAdjBinList() const {
    return fAdjBinList;
  }
  inline const std::vector<AdaptiveBinning::Bin> AdaptiveBinning::GetBinList() const {
    return fBinList;
  }
  inline size_t AdaptiveBinning::GetNbins() const {
    return fBinList.size();
  }

  //_________________________________
  // INLINE METHODS FOR THE BIN CLASS
    
  inline const double AdaptiveBinning::Bin::GetNpoints() const { return fNpoints; }
  inline const double AdaptiveBinning::Bin::GetXmax() const    { return fXmax; }
  inline const double AdaptiveBinning::Bin::GetXmin() const    { return fXmin; }
  inline const double AdaptiveBinning::Bin::GetYmax() const    { return fYmax; }
  inline const double AdaptiveBinning::Bin::GetYmin() const    { return fYmin; }

}

#endif
