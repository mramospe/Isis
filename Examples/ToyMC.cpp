//////////////////////////////////////////////////////
//  Example of generation of ToyMontecarlo samples  //
//////////////////////////////////////////////////////

#include "RooFit/ToyMCgenerator.h"
#include "TFile.h"

int main() {

  // Gets the input tree
  TFile *ifile = TFile::Open( "files/ToyMCin.root" );
  TTree *itree = (TTree*) ifile -> Get( "DecayTree" );

  // Creates the different particles of the taking part of the generation
  Analysis::DecayParticle
    Bs0( "B_s0" ),
    D0( "D0" ),
    pi0( "pi0" ),
    Kplus( "K+" ),
    piminus( "pi-" ),
    piplus( "pi+" );

  // Opens the output file
  TFile *ofile = TFile::Open( "ToyMCout.root", "RECREATE" );

  // Initializes the class
  Analysis::ToyMCgenerator gen( Bs0,
				itree,
				"B0_PX",
				"B0_PY",
				"B0_PZ",
				"B0_PE" );

  // Adds the daughters to the different mother particles. The masses are
  // taken in this case as those of the PDG database.
  Bs0.AddDaughter( D0 );
  Bs0.AddDaughter( pi0 );
  D0.AddDaughter( Kplus );
  D0.AddDaughter( piminus );

  // Creates one tree and generates 10000 events
  gen.CreateTree( "Main" );
  gen.Generate( 10000 );

  // Removes a particle from the decay tree and adds another one
  Bs0.Remove( pi0 );
  Bs0.AddDaughter( piplus );
  
  // Repeats the same process as above
  gen.CreateTree( "Sec" );
  gen.Generate( 1000 );

  ofile -> Close();
}
