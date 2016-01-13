//////////////////////////////////////////////////////
//  Example of generation of ToyMontecarlo samples  //
//////////////////////////////////////////////////////

#include "RooFitExt/ToyMCgenerator.h"
#include "TFile.h"
#include "TRandom.h"

#include <cmath>

int main() {

  // Generates the input sample
  TFile *ifile = TFile::Open( "files/ToyMCin.root", "RECREATE" );
  TTree *itree = new TTree( "DecayTree", "DecayTree", 0 );
  
  double B0_M, B0_PX, B0_PY, B0_PZ, B0_PE;

  itree -> Branch( "B0_M" , &B0_M , "B0_M/D"  );
  itree -> Branch( "B0_PX", &B0_PX, "B0_PX/D" );
  itree -> Branch( "B0_PY", &B0_PY, "B0_PY/D" );
  itree -> Branch( "B0_PZ", &B0_PZ, "B0_PZ/D" );
  itree -> Branch( "B0_PE", &B0_PE, "B0_PE/D" );

  for ( size_t i = 0; i < 10000; i++ ) {

    B0_M = gRandom -> Gaus( 5279.53, 20 );
    
    if   ( gRandom -> Uniform() < 0.5 )
      B0_PX = gRandom -> Gaus( +10, 5 );
    else
      B0_PX = gRandom -> Gaus( -10, 5 );

    if   ( gRandom -> Uniform() < 0.5 )
      B0_PY = gRandom -> Gaus( +10, 5 );
    else
      B0_PY = gRandom -> Gaus( -10, 5 );

    B0_PZ = gRandom -> Exp( 1000 );

    B0_PE = std::sqrt( B0_M*B0_M + B0_PX*B0_PX + B0_PY*B0_PY + B0_PZ*B0_PZ );

    itree -> Fill();
  }
  itree -> Write();

  ifile -> Close();

  // Creates the toy-montecarlo sample
  ifile = TFile::Open( "files/ToyMCin.root", "READ" );
  itree = (TTree*) ifile -> Get( "DecayTree" );

  // Creates the different particles of the taking part of the generation
  Analysis::DecayParticle
    Bs0( "B_s0" ),
    D0( "D0" ),
    pi0( "pi0" ),
    Kplus( "K+" ),
    piminus( "pi-" ),
    piplus( "pi+" );

  // Opens the output file
  TFile *ofile = TFile::Open( "files/ToyMCout.root", "RECREATE" );

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
