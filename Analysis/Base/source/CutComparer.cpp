///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 23/03/2016                                                      //
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
#include "Utils.h"

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
// Destructor
Analysis::CutComparer::~CutComparer() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Adds a new variable to cut over. The expression will be taken as the name
// itself.
void Analysis::CutComparer::AddCutVariable( std::string       name,
					    const std::string &dir,
					    const size_t      &npoints,
					    const double      &vmin,
					    const double      &vmax ) {
  this -> AddCutVariable( name, name, dir, npoints, vmin, vmax );
}

//_______________________________________________________________________________
// Adds a new variable to cut over. The name of the variable, the direction of
// the cut: "<", ">", "<=" or ">=", the number of points and the range of the cut
// have to be specified. One can oblige this class to make the same cuts to a set
// of variables writing in the < name > input, all the different names separated
// by < ; >.
void Analysis::CutComparer::AddCutVariable( const std::string &name,
					    std::string       expr,
					    const std::string &dir,
					    const size_t      &npoints,
					    const double      &vmin,
					    const double      &vmax ) {
  if ( dir != ">" && dir != "<" && dir != ">=" && dir != "<=" )
    std::cerr <<
      "ERROR: Unknown boolean operator for variable < " << name << " >"
							<< std::endl;
  // Removes the whitespaces that could be present in the name
  auto it = expr.begin();
  while ( it != expr.end() )
    if ( *it == ' ' )
      expr.erase( it );
    else
      ++it;
  // Adds the different variables
  std::vector<std::string> nvec;
  General::SplitString( nvec, expr, ";" );
  fCutVars.push_back( CutCompVar( name, expr, npoints, vmin, vmax, nvec.size() ) );
  // If more than one variable is present, then the expression is introduced between parentheses
  std::string add = nvec.front() + dir + "%%%";
  for ( auto itn = nvec.begin() + 1; itn != nvec.end(); ++itn )
    add += " && " + *itn + dir + "%%%";
  if ( fCutString.size() )
    fCutString += " && (" + add + ')';
  else
    fCutString += '(' + add + ')';
  fLoopArray.AddIndex( npoints );
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
  std::cout << "Variables to be compared (" << fCompVars.size() << "):" << std::endl;
  for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ )
    std::cout << " - " << itv -> fName << std::endl;;
  std::cout << "Histograms to be written: " << fCategories.size()*fCompVars.size()*fLoopArray.GetNloops() << std::endl;
  std::cout << "*** List of categories ***" << std::endl;
  for ( auto itc = fCategories.begin(); itc != fCategories.end(); itc++ ) {
    std::cout << "++ " << itc -> fName << " ++" << std::endl;
    std::cout << "  - Directory: " << itc -> fTree -> GetDirectory() -> GetPath() << std::endl;
    std::cout << "  - Tree:      " << itc -> fTree -> GetName() << std::endl;
    std::cout << "  - Cut:       " << ( itc -> fCut ? "True" : "False" ) << std::endl;
    if ( itc -> fWgtExpr.size() )
      std::cout << "  - Weight:    " << itc -> fWgtExpr << std::endl;
  }

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
    tree = itc -> fTree;
    name = itc -> fName;
    for ( auto itcmp = fCompVars.begin(); itcmp != fCompVars.end(); itcmp++ )
      if ( itcmp -> fExpr == "" ) {
	if ( !tree -> GetBranch( itcmp -> fName.c_str() ) ) {
	  std::cerr <<
	    "ERROR: Branch < " << itcmp -> fName << " > not known for category < " << name << " >" 
			       << std::endl;
	  return;
	}
      }
  }

  // Performs the loop over all the different cuts, generating the histograms and saving them
  // into the output directory
  size_t pos, backpos, imh;
  double step, sw, nmax, max = 0;
  std::string cutStr, var, cuts, vmin, vmax, nbins;
  std::stringstream sout, sname;
  std::vector<size_t> htodel;

  // Defines the colors to draw the histograms
  Color_t currcol, colors[ 6 ] = { kBlue, kRed, kOrange, kGreen, kMagenta, kCyan };
  unsigned short int icol;

  // Pointers for the folder and the histograms
  TCanvas *canvas = new TCanvas;
  TDirectoryFile *folder;
  TH1
    *hist   = 0,
    **vhist = new TH1*[ fCategories.size() ],
    **chist = new TH1*[ fCategories.size() ];
  TLegend    *legend = new TLegend( 2./3, 2./3, 0.9, 0.9 );
  TObjString *objStr;

  // Creates the iterators
  std::vector<CutCompVar>::iterator itv;
  std::vector<std::pair<size_t, size_t> >::const_iterator itl;

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
      step  = ( itv -> fMax - itv -> fMin )/( itv -> fNdiv - 1 );
      for ( size_t iv = 0; iv < itv -> fNvars; ++iv ) {
	pos = fCutString.find( "%%%", backpos );
	sout << fCutString.substr( backpos, pos - backpos );
	sout << step*( itl -> first ) + itv -> fMin;
	backpos = pos + 3;
      }
      sname << itl -> first;
      itv++;
      itl++;
    }
    sout << fCutString.substr( backpos );
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

    // Makes the loop over the different variables and categories to make the histograms
    for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ ) {
      name   = itv -> fName;
      nbins  = std::to_string( itv -> fNdiv );
      vmin   = std::to_string( itv -> fMin );
      vmax   = std::to_string( itv -> fMax );
      icol   = 0;

      // Redefines the canvas and the legend
      canvas -> Clear();
      canvas -> SetName( name.c_str() );
      canvas -> SetTitle( name.c_str() );
      canvas -> cd();
      legend -> Clear();

      for ( auto it = fCategories.begin(); it != fCategories.end(); it++, icol++ ) {
	cuts = it -> fWgtExpr;
	if ( cuts.size() ) {
	  if ( it -> fCut )
	    cuts = '(' + cuts + ")*(" + cutStr + ')';
	}
	else
	  if ( it -> fCut )
	    cuts = cutStr;
	tree = it -> fTree;
	if ( itv -> fExpr.size() )
	  var = itv -> fExpr;
	else
	  var = name;
	// Generates the histogram. The name has always to be set to avoid memory leak problems.
	tree -> Draw( ( var + ">>hist(" + nbins + "," +	vmin + "," + vmax + ")"	).c_str(), cuts.c_str() );
	hist = ( TH1* ) gDirectory -> Get( "hist" );
	var  = name + "_" + it -> fName;
	hist -> SetNameTitle( var.c_str(), var.c_str() );

	// The histogram is formatted only if it is not void
	if ( hist -> GetSumOfWeights() > 0 ) {
	  currcol = colors[ icol%6 ] + icol/6;
	  hist   -> SetMarkerStyle( 20 );
	  hist   -> SetLineColor( currcol );
	  hist   -> SetMarkerColor( currcol );
	  hist   -> SetTitle( "" );
	  hist   -> GetXaxis() -> SetTitle( name.c_str() );
	  legend -> AddEntry( hist, it -> fName.c_str() );
	  hist   -> Write();
	}
	vhist[ icol ] = hist;
      }
      
      // Draws the histograms in the canvas. The first histogram to be drawn is that with
      // the maximum value in the Y axis.
      max = 0;
      imh = 0;
      htodel.clear();
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
	hist = vhist[ imh ] -> DrawNormalized( "HE1" );
	for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	  if ( ih != imh && vhist[ ih ] -> GetSumOfWeights() > 0 ) {
	    chist[ ih ] = vhist[ ih ] -> DrawNormalized( "SAMEHE1" );
	    htodel.push_back( ih );
	  }
	legend -> Draw();
	canvas -> Write();
	folder -> Save();

	// Deletes the cloned histograms
	delete hist;
	for ( auto it = htodel.begin(); it != htodel.end(); it++ )
	  delete chist[ *it ];
      }
      std::cout << " - Variable < " << itv -> fName << " > processed" << std::endl;
      
      // Deletes the pointers
      for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	delete vhist[ ih ];
    }

    // Returns to the previous directory
    delete objStr;
    delete folder;
    
    currDir -> cd();
    std::cout << "Returned to the main directory" << std::endl;
  }

  // Deletes the last allocated memory
  delete legend;
  delete canvas;

  delete [] chist;
  delete [] vhist;
  
  std::cout << "*************************" << std::endl;
  std::cout << "*** Process finished! ***" << std::endl;
  std::cout << "*************************" << std::endl;
}

