/////////////////////////////////////////////////////////
// --------------------------------------------------- //
//						       //
//  Analysis package	  			       //
//        					       //
// --------------------------------------------------- //
//						       //
//  AUTHOR: Miguel Ramos Pernas		               //
//  e-mail: miguel.ramos.pernas@cern.ch		       //
//						       //
//  Last update: 13/06/2016			       //
//   						       //
// --------------------------------------------------- //
//						       //
//  Description:				       //
//						       //
//  Implements the class to calculate the asymmetries  //
//  bin to bin using two-dimensional adaptive binning  //
//  histograms. Be careful using sWeighted data        //
//  because for large amount of bins they could        //
//  appear variation in the number of entries in each  //
//  bin.					       //
//						       //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#ifndef MIRANDA_ANALYSIS
#define MIRANDA_ANALYSIS

#include "AdaptiveBinning2D.h"

#include "TTree.h"
#include "TLeaf.h"
#include "TH2Poly.h"

#include "RooRealVar.h"
#include "RooDataSet.h"


//______________________________________________________________________________

namespace Analysis {

  class MirandaAnalysis {

  public:

    // Constructor
    MirandaAnalysis( unsigned int occ_min,
		     double       xmin,
		     double       xmax,
		     double       ymin,
		     double       ymax );

    // Destructor
    ~MirandaAnalysis();

    // Methods
    TH2Poly*    GetAdjAsymHist( const char *name, const char *title );
    TH2Poly*    GetAdjBotHist( const char *name, const char *title );
    TH2Poly*    GetAdjStruct( const char *name, const char *title );
    TH2Poly*    GetAdjTopHist( const char *name, const char *title );
    TH2Poly*    GetAsymHist( const char *name, const char *title );
    RooDataSet* GetAsymData( const char *name, const char *title, RooRealVar &var );
    TH2Poly*    GetTopHist( const char *name, const char *title );
    TH2Poly*    GetBotHist( const char *name, const char *title );
    TH2Poly*    GetStruct( const char *name, const char *title );
    void        SetTopTree( TTree      *tree,
			    const char *xleaf_name,
			    const char *yleaf_name,
			    const char *wleaf_name = 0 );
    void        SetBotTree( TTree      *tree,
			    const char *xleaf_name,
			    const char *yleaf_name,
			    const char *wleaf_name = 0 );
    void        SetStructTree( TTree      *tree,
			       const char *xleaf_name,
			       const char *yleaf_name,
			       const char *wleaf_name = 0 );
    
  protected:
    
    // Attributes
    unsigned int        fMinOcc;
    TTree              *fTopTree;
    TLeaf             **fTopLeaves;
    TTree              *fBotTree;
    TLeaf             **fBotLeaves;
    AdaptiveBinning2D   fStructure;
    double              fXmax;
    double              fXmin;
    double              fYmax;
    double              fYmin;

  };

}

#endif
