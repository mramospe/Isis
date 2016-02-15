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
//  Last update: 15/02/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the two-dimensional bin class to work together with   //
//  AdaptiveBinning2D to create adaptive binned histograms.          //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#ifndef BIN_2D
#define BIN_2D

#include "AdaptiveBinning/Bin.h"

#include <vector>


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning2D;

  class Bin2D: public Bin {

    friend AdaptiveBinning2D;

  public:

    // Constructor
    Bin2D( const double &xmin, const double &xmax, const double &ymin, const double &ymax );

    // Destructor
    ~Bin2D();

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
    Bin2D Divide( const double &xrange, const double &yrange );
    
  private:

    // Methods
    void                CalcMedians();
    void                Clear();
    std::vector<double> Sort( std::vector<double> &dvector, std::vector<double> &wvector );

  };

  //_______________
  // INLINE METHODS
  
  inline const double Analysis::Bin2D::GetXmax() const { return fXmax; }
  inline const double Analysis::Bin2D::GetXmin() const { return fXmin; }
  inline const double Analysis::Bin2D::GetYmax() const { return fYmax; }
  inline const double Analysis::Bin2D::GetYmin() const { return fYmin; }

}

#endif
