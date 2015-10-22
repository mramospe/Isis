////////////////////////////////////////////
//                                        //
//  Mu-scan example for CLs calculations  //
//                                        //
////////////////////////////////////////////

#include "CLsAnalysis.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"

using namespace Analysis;

int main() {

  double oldH[3] = { 1.5, 1., 0.5 };
  double stepH[3] = { 2., 4., 2. };
  double obs[3]  = {1., 1., 1.};

  // Arrays have to be given as CLsArray objects
  CLsArray
    oldHarray( oldH, 3 ),
    stepHarray( stepH, 3 ),
    OBSarray( obs, 3 );

  // Opens the output file
  TFile *file = new TFile( "Results_CLs.root", "RECREATE" );

  std::cout << "\nInitializing..." << std::endl;

  // Initializes the analyser setting the old hypothesis and the number of
  // iterations fot each step
  CLsAnalyser Analyser;
  Analyser.SetOldHypothesis( oldHarray );
  Analyser.SetNPoints( 100000 );

  // Number of steps, and different < mu > values for the scan
  int    np( 10 );
  double
    mu,
    mu_min( 0.1 ),
    mu_max( 1.  ),
    step( ( mu_max - mu_min )/( np - 1 ) );

  // Creates the output histograms and graphs
  TH1D *h1, *h2;
  TGraph
    *roc,
    *gr     = new TGraph(),
    *gr_med = new TGraph(),
    *gr_1s  = new TGraph();

  TString
    nname( "New_" ),
    oname( "Old_" ),
    aname;

  // Performs the loop over all the number of steps
  for ( int i = 0; i < np; i++ ) {

    // Calculates the mu value of the step
    mu = mu_min + i*step;

    std::cout << "\n---------------------------" << std::endl;
    std::cout << "Loop:\t" << i << "\t mu = " << mu << std::endl;
    std::cout << "---------------------------" << std::endl;

    // Sets the new hypothesis considering the new value for mu
    Analyser.SetNewHypothesis( oldHarray + mu*stepHarray );
    Analyser.Evaluate();

    // Generates the histograms with the distribution of the new and old hypothesis
    // test-statistics
    aname  = nname;
    aname += i;
    h1     = Analyser.GetNewHypHist( aname, 300);

    aname  = oname;
    aname += i;
    h2     = Analyser.GetOldHypHist( aname, 300);

    // Creates the ROC curve
    roc = Analyser.GetROC( 200 );

    h1 -> SetLineColor( kRed );
    h2 -> SetLineColor( kBlue );

    std::cout << " t0:\t\t"      << Analyser.TestStat( OBSarray )  << std::endl;
    std::cout << " CLs:\t\t"     << Analyser.GetCLs( OBSarray )    << std::endl;
    std::cout << " CLs_med:\t"   << Analyser.GetQCLs( 0.5, "old" ) << std::endl;
    std::cout << " CLs_1smin:\t" << Analyser.GetQCLs( 0.8413 )     << std::endl;
    std::cout << " CLs_1smax:\t" << Analyser.GetQCLs( 1 - 0.8413 ) << std::endl;
    std::cout << " Alpha:\t\t"   << Analyser.GetAlpha( OBSarray )  << std::endl;
    std::cout << " Beta:\t\t"    << Analyser.GetBeta( OBSarray )   << std::endl;

    // Sets the points for the different graphs
    gr     -> SetPoint( i, mu, Analyser.GetCLs( OBSarray ) );
    gr_med -> SetPoint( i, mu, Analyser.GetQCLs( 0.5 ) );
    gr_1s  -> SetPoint( i, mu, Analyser.GetQCLs( 1 - 0.8413 ) );
    gr_1s  -> SetPoint( 2*np - i - 1, mu, Analyser.GetQCLs( 0.8413 ) );

    // Writes the output objects
    h1  -> Write();
    h2  -> Write();

    aname  = "ROC_";
    aname += i;
    roc -> Write( aname );

    delete h1;
    delete h2;
    delete roc;
  }

  // Creates a canvas for the given step with the CLs results
  TCanvas *canvas = new TCanvas( "c", "c", 1000, 1000 );
  gr_1s  -> GetXaxis() -> SetTitle( "#mu" );
  gr_1s  -> GetYaxis() -> SetTitle( "CLs" );
  gr_1s  -> SetFillColor   ( kYellow-7 );

  gr     -> SetMarkerColor ( kRed      );
  gr_med -> SetMarkerColor ( kBlue     );
  gr_1s  -> SetMarkerColor ( kBlue-10  );

  gr     -> SetLineColor   ( kRed      );
  gr_med -> SetLineColor   ( kBlue     );
  gr_1s  -> SetLineColor   ( kBlue-10  );

  gr_1s  -> GetXaxis() -> SetRangeUser( mu_min, mu_max );

  gr_1s  -> Draw(  "AF"    );
  gr     -> Draw( "SAMECP" );
  gr_med -> Draw( "SAMECP" );

  canvas -> Print( "RESULT.pdf" );

  std::cout << "\nEnding...\n" << std::endl;

  file -> Close();

  return 0;
}
