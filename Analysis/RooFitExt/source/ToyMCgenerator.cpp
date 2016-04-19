/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//  Analysis package                                                               //
//                                                                                 //
// ------------------------------------------------------------------------------- //
//                                                                                 //
//  AUTHOR: Miguel Ramos Pernas                                                    //
//  e-mail: miguel.ramos.pernas@cern.ch                                            //
//                                                                                 //
//  Last update: 19/04/2016                                                        //
//                                                                                 //
// ------------------------------------------------------------------------------- //
//                                                                                 //
//  Description:                                                                   //
//                                                                                 //
//  Implements the two classes to generate Toy-MonteCarlo samples. Particles are   //
//  constructed with the <DecayParticle> class, which allows to define them given  //
//  its mass, width and/or resolution, or a TF1/PDF. All the particles and links   //
//  between them (daughters, granddaughters...) have to be specified before the    //
//  Monte-Carlo generation. It is performed via the <ToyMCgenerator> class, which  //
//  uses the information of a mother particle or the distributions of a given      //
//  TTree.                                                                         //
//                                                                                 //
// ------------------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////////////


#include "ToyMCgenerator.h"

#include "TLeaf.h"
#include "TDatabasePDG.h"
#include "RooArgList.h"
#include "RooBreitWigner.h"
#include "RooVoigtian.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include <map>


//_______________________________________________________________________________
//////////////////////////////////
//     DECAY PARTICLE CLASS     //
//////////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Constructor given the name and the possible resolution. The information of
// the particle is obtained through the TDatabasePDG class, but with the masses
// and widths in MeV, not in GeV
Analysis::DecayParticle::DecayParticle( const char *name, double sigma ) :
  fName( name ),
  fNdaughters( 0 ),
  fPDF( 0 ) {

  // Reads the data from the PDG database
  TDatabasePDG pdg;

  pdg.ReadPDGTable();

  TParticlePDG *particle( pdg.GetParticle( name ) );

  // Converts the mass and width in GeV to MeV
  double
    mass( particle -> Mass()*1000 ),
    width( particle -> Width()*1000 );

  // Checks if the width is so small to work with it
  width = ( width/mass < 1E-6 ? 0 : width );

  // Changes the symbols in the name
  std::map<std::string, std::string> mp;
  mp[ "+" ] = "plus";
  mp[ "-" ] = "minus";
  mp[ "*" ] = "st";

  size_t fpos;
  for ( std::map<std::string, std::string>::iterator it = mp.begin();
	it != mp.end(); it++ ) {

    fpos = fName.find( it -> first );
    if ( fpos != std::string::npos )
      fName.replace( fpos, fpos + 1, mp[ it -> first ] );
  }

  fMass = mass;

  // If does exist any or both width and sigma, it sets the mass distribution
  if ( width || sigma )
    this -> SetDistribution( width, sigma );
}

//_______________________________________________________________________________
// Constructor given a name, a PDF and its independent variable
Analysis::DecayParticle::DecayParticle( const char *name,
					RooAbsPdf  *pdf,
					RooRealVar &mass ) :
  fName( name ),
  fNdaughters( 0 ),
  fPDF( pdf -> asTF( RooArgList( mass ) ) ) { }

//_______________________________________________________________________________
// Constructor given a name and a PDF as a TF1
Analysis::DecayParticle::DecayParticle( const char *name,
					TF1        *pdf ) :
  fName( name ),
  fNdaughters( 0 ),
  fPDF( pdf ) { }

//_______________________________________________________________________________
// Constructor given a name, the mass, width and resolution for the particle
Analysis::DecayParticle::DecayParticle( const char *name,
					double      mass,
					double      width,
					double      sigma ) :
  fMass( mass ),
  fName( name ),
  fNdaughters( 0 ),
  fPDF( 0 ) {

  this -> SetDistribution( width, sigma );
}

//_______________________________________________________________________________
// Destructor
Analysis::DecayParticle::~DecayParticle() {

  if ( fPDF )
    delete fPDF;

  if ( fPDFbase.size() )
    for ( std::vector<RooAbsReal*>::iterator it = fPDFbase.begin();
	  it != fPDFbase.end(); it++ )
      delete ( *it );
}

//______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Gets the string with the decay structure of the particle
std::string Analysis::DecayParticle::GetDecay() {

  std::string out( fName );

  if ( fParticles.size() != 0 ) {

    out += "->(";
    out += fParticles[ 0 ] -> GetDecay();

    for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin() + 1;
	  it != fParticles.end(); it++ ) {
    
      out += ", ";
      out += ( *it ) -> Name();
    }

    out += ")";
  }

  return out;
}

//_______________________________________________________________________________
// Gets the mass of the particle
double Analysis::DecayParticle::GetMass() {

  if ( fPDF )
    fMass = fPDF -> GetRandom();

  return fMass;
}

//_______________________________________________________________________________
// Returns the number of particles attached to this one
unsigned int Analysis::DecayParticle::GetNparticles() {

  unsigned int np( 1 );

  for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin();
	it != fParticles.end(); it++ )
    np += ( *it ) -> GetNparticles();

  return np;
}

//_______________________________________________________________________________
// Removes one particle from the list of particles
void Analysis::DecayParticle::Remove( Analysis::DecayParticle &particle ) {

  bool dec( true );

  for ( std::vector<DecayParticle*>::iterator it = fParticles.begin();
	it != fParticles.end() && dec ; it++ )
    if ( ( *it ) == &particle ) {
	
      dec = false;
	
      fParticles.erase( it );

      fNdaughters--;
    }
}

//_______________________________________________________________________________
// Sets the branches for a given tree, with the kinematic information of every
// particle attached to this one
void Analysis::DecayParticle::SetBranches( TTree *tree ) {

  std::string
    br_name,
    mag[ 6 ] = { "_M", "_PX", "_PY", "_PZ", "_PE", "_GenWeight" };

  br_name = fName + mag[ 0 ];
  tree -> Branch( br_name.c_str(), &fMass, ( br_name + "/D" ).c_str() );
  std::cout << " " << br_name;

  for ( unsigned int i = 1; i < 5; i++ ) {

    br_name = fName + mag[ i ];
    tree -> Branch( br_name.c_str(), &fVector( i - 1 ), ( br_name + "/D" ).c_str() );
    std::cout << ", " << br_name;
  }
  if ( fNdaughters != 0 ) {

    br_name = fName + mag[ 5 ];
    tree -> Branch( br_name.c_str(), &fGenWeight, ( br_name + "/D" ).c_str() );
    std::cout << ", " << br_name;
  } 
  std::cout << std::endl;

  for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin();
	it != fParticles.end(); it++ ) ( *it ) -> SetBranches( tree );
}

//_______________________________________________________________________________
// Sets the distribution of a particle given its width and resolution values
void Analysis::DecayParticle::SetDistribution( double width, double sigma ) {

  RooAbsPdf  *pdf;
  RooRealVar *rmass = new RooRealVar( "rmass", "rmass",
				      fMass - 3*( width + sigma ),
				      fMass + 3*( width + sigma ) );

  if ( width || sigma ) {
    if ( width && !sigma )
      pdf = new RooBreitWigner( "BMass", "BMass",
				*rmass,
				RooFit::RooConst( fMass ),
				RooFit::RooConst( width ) );
    else if ( !width && sigma )
      pdf = new RooGaussian( "GMass", "GMass",
			     *rmass,
			     RooFit::RooConst( fMass ),
			     RooFit::RooConst( sigma ) );
    else if ( width && sigma )
      pdf = new RooVoigtian( "VMass", "VMass",
			     *rmass,
			     RooFit::RooConst( fMass ),
			     RooFit::RooConst( width ),
			     RooFit::RooConst( sigma ) );

    fPDF = pdf -> asTF( RooArgList( *rmass ) );

    fPDFbase.push_back( pdf );
    fPDFbase.push_back( rmass );
  }
  else
    fVector.SetXYZM( 0, 0, 0, fMass );
}

//_______________________________________________________________________________
// Generates the phase-space for the decay of this particle
void Analysis::DecayParticle::GenPhaseSpace( double px, double py, double pz ) {

  double masses[ fNdaughters ];

  for ( unsigned int i = 0; i < fNdaughters; i++ )
    masses[ i ] = fParticles[ i ] -> GetMass();
  
  fPHSP.SetDecay( fVector, fNdaughters, masses );

  fGenWeight = fPHSP.Generate();

  for ( unsigned int i = 0; i < fNdaughters; i++ ) {

    fParticles[ i ] -> SetPxPyPzE( fPHSP.GetDecay( i ) -> Px(),
				   fPHSP.GetDecay( i ) -> Py(),
				   fPHSP.GetDecay( i ) -> Pz(),
				   fPHSP.GetDecay( i ) -> E() );

    if ( fParticles[ i ] -> fNdaughters != 0 )
      fParticles[ i ] -> GenPhaseSpace();
  }
}

//_______________________________________________________________________________



//_______________________________________________________________________________
////////////////////////////////////
//     TOY MC GENERATOR CLASS     //
////////////////////////////////////


// -- CONSTRUCTORS

//_______________________________________________________________________________
// Constructor given the mother particle and the possible output tree name
Analysis::ToyMCgenerator::ToyMCgenerator( Analysis::DecayParticle &mother,
					  const char              *tree_name ) :
  fInputTree( 0 ), fMother( &mother ), fOutputTree( 0 ) {

  this -> StartMessage();

  if ( tree_name )
    fOutputTree = new TTree( tree_name, tree_name, 0 );
}

//_______________________________________________________________________________
// Constructor given the mother particle, the paths to the tree and the different
// names of the kinematic variables of it, as well as a possible output tree name
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

  this -> StartMessage();

  std::cout <<
    " Obtaining distributions for the mother particle" << "\n"
	    << " Input tree:\t" << tree -> GetName() << "\n"
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
// Destructor
Analysis::ToyMCgenerator::~ToyMCgenerator() { }

//_______________________________________________________________________________


// -- PUBLIC METHOD

//_______________________________________________________________________________
// Generates the Monte-Carlo data
void Analysis::ToyMCgenerator::Generate( long int nevts ) {

  unsigned int w( ( this -> GetDecayTree() ).size() + 30 );

  std::cout << "\n";
  std::cout.width( w );
  std::cout.fill( '=' );
  std::cout << std::left << " ";
  std::cout << "\n === Generating decay < " <<
    this -> GetDecayTree() << " > ===\n";
  std::cout.width( w );
  std::cout.fill( '=' );
  std::cout << std::left << " " << std::endl;

  std::cout << "\n  - Number of events:    " << nevts;
  std::cout << "\n  - Number of particles: " << fMother -> GetNparticles();
  std::cout << "\n  - Output tree name:    " << fOutputTree -> GetName();
  std::cout << std::endl;

  std::cout << "\n Branches attached to the output tree:\n" << std::endl;
  fMother -> SetBranches( fOutputTree );

  if ( fInputTree ) {

    std::cout << "\n Generating phase-space from input tree" << std::endl;

    fMother -> fPDF = 0;

    if ( nevts > fInputTree -> GetEntries() ) {

      nevts = fInputTree -> GetEntries();
      std::cout << " Given entries are greater than that of input tree, set to " <<
	nevts << std::endl;
    }

    TLeaf *leaves[ 4 ];

    for ( unsigned int i = 0; i < 4; i++ )
      leaves[ i ] = fInputTree -> GetLeaf( fVarNames[ i ] );

    for ( long int ievt = 0; ievt < nevts; ievt++ ) {

      fInputTree -> GetEntry( ievt );

      for ( unsigned int i = 0; i < 4; i++ )
	fMother -> fVector[ i ] = leaves[ i ] -> GetValue();

      fMother -> fMass = fMother -> fVector.M();

      fMother -> GenPhaseSpace();

    fOutputTree -> Fill();
    }
  }
  else
    for ( long int ievt = 0; ievt < nevts; ievt++ ) {
   
      fMother     -> GetMass();
      fMother     -> GenPhaseSpace();
      fOutputTree -> Fill();
    }
  
  fOutputTree -> Write();

  std::cout << "\n Generation finished\n" << std::endl;
}
