///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the two-dimensional bin class to work together with
//  AdaptiveBinning2D to create adaptive binned histograms.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef BIN_2D
#define BIN_2D

#include "Bin.h"

#include <vector>


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning2D;

  class Bin2D: public Bin {

    friend AdaptiveBinning2D;

  public:

    // Constructor
    Bin2D( const double &xmin, const double &xmax,
	   const double &ymin, const double &ymax );

    // Destructor
    ~Bin2D();

    // If the bin is in a border of the histogram, adjusts it to the data
    void AdjustBin( const double &xmin,
		    const double &xmax,
		    const double &ymin,
		    const double &ymax,
		    const double &delta );

    // Fills the bin if the point is inside it
    void Fill( const double &x, const double &y, const double &w );

    // Return number of points in the bin
    inline const double GetNpoints() const;

    // Get the maximum value in the X direction
    inline const double GetXmax() const;

    // Get the minimum value in the X direction
    inline const double GetXmin() const;

    // Get the maximum value in the Y direction
    inline const double GetYmax() const;

    // Get the minimum value in the Y direction
    inline const double GetYmin() const;

  protected:
    
    // Maximum value in the X direction
    double fXmax;

    // Value of the X position for the point with its maximum value
    double fXmaxPoint;

    // Median in the X direction
    double fXmedian;

    // Minimum value in the X direction
    double fXmin;

    // Value of the X position for the point with its minimum value
    double fXminPoint;

    // Vector with the values in the X direction
    std::vector<double> fXpoints;

    // Maximum value in the Y direction
    double fYmax;

    // Value of the Y position for the point with its maximum value
    double fYmaxPoint;

    // Median in the Y direction
    double fYmedian;

    // Minimum value in the Y direction
    double fYmin;

    // Value of the Y position for the point with its minimum value
    double fYminPoint;

    // Vector with the values in the Y direction
    std::vector<double> fYpoints;

    // Vector with the values for the weights of each point
    std::vector<double> fWpoints;

  protected:

    // Divides the bin in two, given the range of the X and Y axis
    Bin2D* Divide( const double &xrange, const double &yrange );
    
  private:

    // If the bin is in a border of the histogram, adjusts it to the data
    void CalcMedians();
    
    // Clears the content of the bin but the limits
    void Clear();
    
    // Sorts the data and the weights and returns the sorted vector of weights for
    // that data sample
    std::vector<double> Sort( std::vector<double> &dvector,
			      std::vector<double> &wvector );

  };

  //______________________________________________________________________________
  //
  inline const double Analysis::Bin2D::GetXmax() const { return fXmax; }

  //______________________________________________________________________________
  //
  inline const double Analysis::Bin2D::GetXmin() const { return fXmin; }

  //______________________________________________________________________________
  //
  inline const double Analysis::Bin2D::GetYmax() const { return fYmax; }

  //______________________________________________________________________________
  //
  inline const double Analysis::Bin2D::GetYmin() const { return fYmin; }

}

#endif
