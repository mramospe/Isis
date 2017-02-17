/////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ----------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// ----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////


#include "DecayParticle.h"
#include "ToyMCgenerator.h"

#include "TLeaf.h"
#include <map>


//_______________________________________________________________________________
//
Analysis::ToyMCgenerator::ToyMCgenerator( Analysis::DecayParticle &mother,
					  const char              *tree_name ) :
  fInputTree( 0 ), fMother( &mother ), fOutputTree( 0 ) {

  this->StartMessage();

  if ( tree_name )
    fOutputTree = new TTree( tree_name, tree_name, 0 );
}

//_______________________________________________________________________________
//
Analysis::ToyMCgenerator::ToyMCgenerator( Analysis::DecayParticle &mother,
					  TTree                   *tree,
					  const char              *px,
					  const char              *py,
					  const char              *pz,
					  const char              *pe,
					  const char              *tree_name ) :
  fInputTree( tree ),
  fMother( &mother ),
  fOutputTree( 0 ) {

  this->StartMessage();

  std::cout <<
    " Obtaining distributions for the mother particle" << "\n"
	    << " Input tree:\t" << tree->GetName() << "\n"
	    << " X-momentum:\t" << px << "\n"
	    << " Y-momentum:\t" << py << "\n"
	    << " Z-momentum:\t" << pz << "\n"
	    << " Energy:    \t" << pe
	    << std::endl;

  fVarNames.push_back( px );
  fVarNames.push_back( py );
  fVarNames.push_back( pz );
  fVarNames.push_back( pe );

  if ( tree_name )
    fOutputTree = new TTree( tree_name, tree_name, 0 );
}

//_______________________________________________________________________________
//
Analysis::ToyMCgenerator::~ToyMCgenerator() { }

//_______________________________________________________________________________
//
void Analysis::ToyMCgenerator::Generate( long int nevts ) {

  unsigned int w( ( this->GetDecayTree() ).size() + 30 );

  std::cout << "\n";
  std::cout.width( w );
  std::cout.fill( '=' );
  std::cout << std::left << " ";
  std::cout << "\n === Generating decay < " <<
    this->GetDecayTree() << " > ===\n";
  std::cout.width( w );
  std::cout.fill( '=' );
  std::cout << std::left << " " << std::endl;

  std::cout << "\n  - Number of events:    " << nevts;
  std::cout << "\n  - Number of particles: " << fMother->GetNparticles();
  std::cout << "\n  - Output tree name:    " << fOutputTree->GetName();
  std::cout << std::endl;

  std::cout << "\n Branches attached to the output tree:\n" << std::endl;
  fMother->SetBranches( fOutputTree );

  if ( fInputTree ) {

    std::cout << "\n Generating phase-space from input tree" << std::endl;

    fMother->fPDF = 0;

    if ( nevts > fInputTree->GetEntries() ) {

      nevts = fInputTree->GetEntries();
      std::cout << " Given entries are greater than that of input tree, set to " <<
	nevts << std::endl;
    }

    TLeaf *leaves[ 4 ];

    for ( unsigned int i = 0; i < 4; i++ )
      leaves[ i ] = fInputTree->GetLeaf( fVarNames[ i ] );

    for ( long int ievt = 0; ievt < nevts; ievt++ ) {

      fInputTree->GetEntry( ievt );

      for ( unsigned int i = 0; i < 4; i++ )
	fMother->fVector[ i ] = leaves[ i ]->GetValue();

      fMother->fMass = fMother->fVector.M();

      fMother->GenPhaseSpace();

    fOutputTree->Fill();
    }
  }
  else
    for ( long int ievt = 0; ievt < nevts; ievt++ ) {
   
      fMother    ->GetMass();
      fMother    ->GenPhaseSpace();
      fOutputTree->Fill();
    }
  
  fOutputTree->Write();

  std::cout << "\n Generation finished\n" << std::endl;
}
