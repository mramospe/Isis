///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 28/02/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class to perform comparisons among different TreeCategory     //
//  classes. One has to define the different variables that are going to be      //
//  compared together with those to cut over. This class will work in the        //
//  current directory so one has to construct a TFile or a TDirectoryFile        //
//  object in order to use it. All the histograms will be saved in the           //
//  different folders, together with the cut information and the canvas with     //
//  all the histograms.                                                          //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "CutComparer.h"

#include "TCanvas.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TStyle.h"

#include <ctime>
#include <sstream>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::CutComparer::CutComparer() { }

//_______________________________________________________________________________
// Constructor given a vector of categories
Analysis::CutComparer::CutComparer( const std::map<std::string,
						   std::pair<TTree*, std::string> > &catvec ) :
  fCategories( catvec ) { }

//_______________________________________________________________________________
// Destructor
Analysis::CutComparer::~CutComparer() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Adds a new variable to cut over. The name of the variable, the direction of
// the cut: "<", ">", "<=" or ">=", the number of points and the range of the cut
// have to be specified.
void Analysis::CutComparer::AddCutVariable( const std::string &name,
					    const std::string &dir,
					    const size_t      &npoints,
					    const double      &vmin,
					    const double      &vmax ) {
  fCutVars.push_back( std::make_pair( name, CutCompVar( name, npoints, vmin, vmax ) ) );
  if ( dir != ">" && dir != "<" && dir != ">=" && dir != "<=" )
    std::cerr <<
      "ERROR: Unknown boolean operator for variable < " << name << " >"
							<< std::endl;
  if ( fCutString.size() )
    fCutString += " && " + name + dir + "%%%";
  else
    fCutString = name + dir + "%%%";
  fLoopArray.AddIndex( npoints );
}

//_______________________________________________________________________________
// Adds a new variable to cut over as an expression
void Analysis::CutComparer::AddCutVariable( const std::string &name,
					    const std::string &expr,
					    const std::string &dir,
					    const size_t      &npoints,
					    const double      &vmin,
					    const double      &vmax ) {
  this -> AddCutVariable( expr, dir, npoints, vmin, vmax );
  fCutVars.back().first = name;
}

//_______________________________________________________________________________
// Performs the comparison among the different categories making cuts on the
// specified variables, and saving the histograms of the stored comparison
// variables.
void Analysis::CutComparer::Compare() {

  // Sets Root in batch mode to do not show the canvases
  gROOT -> SetBatch();

  // If no variables to cut over or not variables to performe the comparisons have
  // been specified, an error message is displayed
  if ( !fCutVars.size() ) {
    std::cerr << "ERROR: No cut variables have been specified" << std::endl;
    return;
  }
  if ( !fCompVars.size() ) {
    std::cerr << "ERROR: No variables to compare have been specified" << std::endl;
    return;
  }

  // Gets the current directory to use it later when creating the folders
  TDirectory *currDir = gDirectory;
  std::cout << "************************************************" << std::endl;
  std::cout << "*** Initializing comparison among categories ***" << std::endl;
  std::cout << "************************************************" << std::endl;
  std::cout << "Working directory set to: " << currDir -> GetName() << std::endl;
  std::cout << "Number of categories:     " << fCategories.size() << std::endl;
  std::cout << "Number of cut loops:      " << fLoopArray.GetNloops() << std::endl;
  std::cout << "Variables to be compared:"  << std::endl;
  for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ )
    std::cout << " - " << itv -> first << std::endl;;
  std::cout << "Histograms to be written: " << fCategories.size()*fLoopArray.GetNloops() << std::endl;

  // Sets Root in batch mode to do not display the canvases
  gROOT -> SetBatch();

  // Removes the title and the stats box ( for the canvases only )
  gStyle -> SetOptTitle( 0 );
  gStyle -> SetOptStat( 0 );

  // Checks that all the categories have the variables to work with
  std::cout << "***********************" << std::endl;
  std::cout << "*** Process started ***" << std::endl;
  std::cout << "***********************" << std::endl;
  TTree *tree;
  std::string name;
  for ( auto itc = fCategories.begin(); itc != fCategories.end(); itc++ ) {
    tree = itc -> second.first;
    name = itc -> first;
    for ( auto itcmp = fCompVars.begin(); itcmp != fCompVars.end(); itcmp++ )
      if ( itcmp -> second.fExpr == "" ) {
	if ( !tree -> GetBranch( itcmp -> first.c_str() ) ) {
	  std::cerr <<
	    "ERROR: Branch < " << itcmp -> first << " > not known for category < " << name << " >" 
			       << std::endl;
	  return;
	}
      }
  }

  // Performs the loop over all the different cuts, generating the histograms and saving them
  // into the output directory
  size_t pos, backpos;
  double step;
  std::string cutStr;
  std::stringstream sout, sname;
  CutComparer::CutCompVar *ccvar;

  // Pointers for the folder and the histograms
  TCanvas *canvas;
  TDirectoryFile *folder;
  TH1 *hist( 0 ), **vhist = new TH1*[ fCategories.size() ];
  TLegend *legend;
  TObjString *objStr;

  // Creates the iterators
  std::vector<std::pair<std::string, CutCompVar> >::iterator itv;
  std::vector<std::pair<size_t, size_t> >::const_iterator    itl;  

  // Loops over all the configurations of cuts
  for ( size_t nlp = 0; nlp < fLoopArray.GetNloops(); nlp++, fLoopArray++ ) {

    // Configures the cut
    std::cout << "*** Initiating cut " << nlp + 1 << " ***" << std::endl;
    sout.str( "" );
    sname.str( "" );
    sname << "CutSet_";
    backpos = 0;
    itv = fCutVars.begin();
    itl = fLoopArray.cBegin();
    while ( itv != fCutVars.end() && itl != fLoopArray.cEnd() ) {
      pos = fCutString.find( "%%%", backpos );
      sout << fCutString.substr( backpos, pos - backpos );
      ccvar = &( itv -> second );
      step  = ( ccvar -> fMax - ccvar -> fMin )/( ccvar -> fN - 1 );
      sout << step*( itl -> first ) + ccvar -> fMin;
      sname << itl -> first;
      backpos = pos + 3;
      itv++;
      itl++;
    }
    cutStr = sout.str();
    std::cout << "Generated new cut: " << cutStr << std::endl;

    // Generates the folder to save the histograms
    folder = new TDirectoryFile( sname.str().c_str(), sname.str().c_str() );
    folder -> Write();
    folder -> cd();
    std::cout << "Generating histograms in folder < " << sname.str() << " >:" << std::endl;

    // Writes the string with the information about the cut in the folder
    objStr = new TObjString( cutStr.c_str() );
    objStr -> Write( "CutStr" );

    // Defines the colors to draw the histograms
    Color_t currcol, colors[ 6 ] = { kBlue, kRed, kOrange, kGreen, kMagenta, kCyan };
    unsigned short int icol;

    // Makes the loop over the different variables and categories to make the histograms
    std::string var, cuts, vmin, vmax, nbins;
    for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ ) {
      name   = itv -> first;
      nbins  = std::to_string( itv -> second.fN );
      vmin   = std::to_string( itv -> second.fMin );
      vmax   = std::to_string( itv -> second.fMax );
      canvas = new TCanvas( name.c_str(), name.c_str() );
      legend = new TLegend( 2./3, 2./3, 0.9, 0.9 );
      icol   = 0;
      for ( auto it = fCategories.begin(); it != fCategories.end(); it++, icol++ ) {
	if ( it -> second.second.size() )
	  cuts = "(" + it -> second.second + ") && (" + cutStr + ")";
	else
	  cuts = cutStr;
	tree = it -> second.first;
	if ( itv -> second.fExpr.size() )
	  var = itv -> second.fExpr;
	else
	  var = name;
	// Generates the histogram. The name has always to be set to avoid memory leak problems.
	tree -> Draw( ( var + ">>hist(" + nbins + "," +	vmin + "," + vmax + ")"	).c_str(), cuts.c_str() );
	hist = ( TH1* ) gDirectory -> Get( "hist" );
	var  = name + "_" + it -> first;
	hist -> SetNameTitle( var.c_str(), var.c_str() );

	// The histogram is formatted only if it is not void
	if ( hist -> GetSumOfWeights() > 0 ) {
	  currcol = colors[ icol%6 ] + icol/6;
	  hist   -> SetMarkerStyle( 20 );
	  hist   -> SetLineColor( currcol );
	  hist   -> SetMarkerColor( currcol );
	  hist   -> SetTitle( "" );
	  hist   -> GetXaxis() -> SetTitle( itv -> first.c_str() );
	  legend -> AddEntry( hist, it -> first.c_str() );
	  hist   -> Write();
	}
	vhist[ icol ] = hist;
      }
      // Draws the pointers into the canvas. The first histogram to be drawn is that with the
      // maximum value in the Y axis.
      double sw, nmax, max = 0;
      size_t imh = 0;
      for ( size_t ih = 0; ih < fCategories.size(); ih++ ) {
	sw = vhist[ ih ] -> GetSumOfWeights();
	if ( sw > 0 ) {
	  nmax = vhist[ ih ] -> GetMaximum()/sw;
	  if ( nmax > max ) {
	    max = nmax;
	    imh = ih;
	  }
	}
      }
      if ( vhist[ imh ] -> GetSumOfWeights() > 0 ) {
	vhist[ imh ] -> DrawNormalized( "HE1" );
	for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	  if ( ih != imh && vhist[ ih ] -> GetSumOfWeights() > 0 )
	    vhist[ ih ] -> DrawNormalized( "SAMEHE1" );
	legend -> Draw();
	canvas -> Write();
	folder -> Save();
      }
      std::cout << " - Variable < " << itv -> first << " > processed" << std::endl;
      
      // Deletes the pointers
      for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	delete vhist[ ih ];
      delete canvas;
      delete legend;
    }

    // Returns to the previous directory
    delete folder;
    delete objStr;
    currDir -> cd();
  }

  delete [] vhist;
  std::cout << "*************************" << std::endl;
  std::cout << "*** Process finished! ***" << std::endl;
  std::cout << "*************************" << std::endl;
}

