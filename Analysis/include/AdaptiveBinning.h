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
//  Last update: 13/07/2015			       //
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
    AdaptiveBinning( unsigned int occ_min,
		     double       xmin,
		     double       xmax,
		     double       ymin,
		     double       ymax,
		     TTree       *tree,
		     const char  *xleaf_name,
		     const char  *yleaf_name,
		     const char  *wleaf_name = 0,
		     double       xnorm = 1,
		     double       ynorm = 1 );

    // Destructor
    ~AdaptiveBinning();

    // Methods
    TH2Poly*     GetAdjHist( const char *name, const char *title );
    TH2Poly*     GetAdjStruct( const char *name, const char *title );
    TH2Poly*     GetHist( const char *name, const char *title );
    unsigned int GetNbins() const;
    TH2Poly*     GetStruct( const char *name, const char *title );

  private:

    // Nested class
    class Bin {

    public:

      // Constructor
      Bin( double xmin, double xmax, double ymin, double ymax );

      // Destructor
      ~Bin();

      // Methods
      void AdjustBin( double xmin, double xmax, double ymin, double ymax, double delta );
      void CalcMedians();
      void Clear();
      void IfInFill( double x, double y, double w );
    
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
      std::vector<double> Sort( std::vector<double> &d_vector,
				std::vector<double> &w_vector );

    };

  protected:

    // Attributes
    std::vector<Bin>    fAdjBinsList;
    std::vector<Bin>    fBinsList;
    double              fDelta;
    unsigned int        fLength;
    std::vector<double> fXdata;
    double              fXmax;
    double              fXmin;
    std::vector<double> fYdata;
    double              fYmax;
    double              fYmin;
    std::vector<double> fWdata;

    };

}

#endif
