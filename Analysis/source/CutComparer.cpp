///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 02/01/2016                                                      //
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


#include "TCanvas.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TH1.h"
#include "TObjString.h"
#include "TROOT.h"

#include "CutComparer.h"
#include "StringParser.h"
#include "TreeManagement.h"

#include <ctime>
#include <sstream>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::CutComparer::CutComparer() { }

//_______________________________________________________________________________
// Constructor given a vector of categories
Analysis::CutComparer::CutComparer( std::vector<Analysis::TreeCategory*> &catvec ) :
  fCategories( catvec ) { }

//_______________________________________________________________________________
// Destructor
Analysis::CutComparer::~CutComparer() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Adds a new variable to compare. The number of bins and the range have to be
// specified.
void Analysis::CutComparer::AddCompVariable( std::string var,
					     size_t      nbins,
					     double      vmin,
					     double      vmax ) {
  CutComparer::CatCompVar cvar;
  cvar.fMax = vmax;
  cvar.fMin = vmin;
  cvar.fN   = nbins;
  fCompVars.push_back( std::make_pair( var, cvar ) );
}

//_______________________________________________________________________________
// Adds a new variable to cut over. The name of the variable, the direction of
// the cut: "<", ">", "<=" or ">=", the number of points and the range of the cut
// have to be specified.
void Analysis::CutComparer::AddCutVariable( std::string var,
					    std::string dir,
					    size_t      npoints,
					    double      vmin,
					    double      vmax ) {
  CutComparer::CatCompVar cvar;
  cvar.fMax = vmax;
  cvar.fMin = vmin;
  cvar.fN   = npoints;
  fCutVars.push_back( std::make_pair( var, cvar ) );
  if ( dir != ">" && dir != "<" && dir != ">=" && dir != "<=" )
    std::cerr <<
      "ERROR: Unknown boolean operator for variable < " << var << " >"
							<< std::endl;
  if ( fCutString.size() )
    fCutString += " && " + var + dir + "%%%";
  else
    fCutString = var + dir + "%%%";
  fLoopArray.AddIndex( npoints );
}

//_______________________________________________________________________________
// Performs the comparison among the different categories making cuts on the
// specified variables, and saving the histograms of the stored comparison
// variables.
void Analysis::CutComparer::Compare() {

  // Gets the current directory to use it later when creating the folders
  TDirectory *currDir = gDirectory;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << "Initializing comparison among categories" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << "Working directory set to: " << currDir -> GetName() << std::endl;
  std::cout << "Number of categories:     " << fCategories.size() << std::endl;
  std::cout << "Number of cut loops:      " << fLoopArray.GetNloops() << std::endl;
  std::cout << "Variables to be compared: " << std::endl;
  for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ )
    std::cout << "- " << itv -> first << std::endl;

  // Sets Root in batch mode to do not display the canvases
  gROOT -> SetBatch();

  // Checks that all the categories have the variables to work with
  for ( auto itc = fCategories.begin(); itc != fCategories.end(); itc++ ) {
    for ( auto itcmp = fCompVars.begin(); itcmp != fCompVars.end(); itcmp++ )
      if ( !( *itc ) -> GetTree() -> GetBranch( itcmp -> first.c_str() ) ) {
	std::cerr <<
	  "ERROR: Branch < " << itcmp -> first.c_str() << " > not known for category < " <<
	  ( *itc ) -> GetName() << " >" << std::endl;
	return;
      }
    for ( auto itcut = fCutVars.begin(); itcut != fCutVars.end(); itcut++ )
      if ( !( *itc ) -> GetTree() -> GetBranch( itcut -> first.c_str() ) ) {
	std::cerr <<
	  "ERROR: Branch < " << itcut -> first.c_str() << " > not know for category < " <<
	  ( *itc ) -> GetName() << " >" << std::endl;
	return;
      }
  }

  // Performs the loop over all the different cuts, generating the histograms and saving them
  // into the output directory
  size_t pos, backpos;
  double step;
  std::string cutStr;
  std::stringstream sout, sname;
  CutComparer::CatCompVar *ccvar;

  // Pointers for the folder and the histograms
  TCanvas *canvas;
  TDirectoryFile *folder;
  TH1 *hist, **vhist = new TH1*[ fCategories.size() ];
  TObjString *objStr;

  // Creates the iterators
  std::vector<std::pair<std::string, CatCompVar> >::iterator itv;
  std::vector<std::pair<size_t, size_t> >::const_iterator    itl;  

  // Loops over all the configurations of cuts
  std::cout << "-- Starting to loop over the cuts --" << std::endl;
  for ( size_t nlp = 0; nlp < fLoopArray.GetNloops(); nlp++, fLoopArray++ ) {

    // Configures the cut
    std::cout << " - Initiating cut " << nlp + 1 << " -" << std::endl;
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
    std::cout << "Generating histograms in folder < " << sname.str() << " >" << std::endl;

    // Writes the string with the information about the cut in the folder
    objStr = new TObjString( cutStr.c_str() );
    objStr -> Write( "CutStr" );

    // Defines the colors to draw the histograms
    Color_t currcol, colors[ 6 ] = { kBlue, kRed, kOrange, kGreen, kMagenta, kCyan };
    unsigned short int icol;

    // Makes the loop over the different categories to make the histograms
    for ( auto itv = fCompVars.begin(); itv != fCompVars.end(); itv++ ) {
      canvas = new TCanvas( itv -> first.c_str(), itv -> first.c_str() );
      icol   = 0;
      for ( auto it = fCategories.begin(); it != fCategories.end(); it++, icol++ ) {
	hist = ( *it ) -> MakeHistogram( itv -> first.c_str(),
					 itv -> second.fN,
					 itv -> second.fMin,
					 itv -> second.fMax,
					 cutStr,
					 false );
	currcol = colors[ icol%6 ] + icol/6;
	hist   -> SetMarkerStyle( 20 );
	hist   -> SetLineColor( currcol );
	hist   -> SetMarkerColor( currcol );
	hist   -> Write();
	vhist[ icol ] = hist;
      }
      // Draws the pointers into the canvas
      for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	vhist[ ih ] -> Draw( "SAMEHE1" );
      canvas -> Write();
      folder -> Save();
      // Deletes the pointers
      for ( size_t ih = 0; ih < fCategories.size(); ih++ )
	delete vhist[ ih ];
      delete canvas;
    }
    // Returns to the previous directory
    delete folder;
    delete objStr;
    currDir -> cd();
  }
  delete [] vhist;
  std::cout << "-- Finished! --" << std::endl;
}

