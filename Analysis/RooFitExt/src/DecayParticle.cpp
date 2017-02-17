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

#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooBreitWigner.h"
#include "RooVoigtian.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooRealVar.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TParticlePDG.h"
#include "TTree.h"

#include <map>
#include <string>


//_______________________________________________________________________________
//
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
    mass( particle->Mass()*1000 ),
    width( particle->Width()*1000 );

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

    fpos = fName.find( it->first );
    if ( fpos != std::string::npos )
      fName.replace( fpos, fpos + 1, mp[ it->first ] );
  }

  fMass = mass;

  // If does exist any or both width and sigma, it sets the mass distribution
  if ( width || sigma )
    this->SetDistribution( width, sigma );
}

//_______________________________________________________________________________
//
Analysis::DecayParticle::DecayParticle( const char *name,
					RooAbsPdf  *pdf,
					RooRealVar &mass ) :
  fName( name ),
  fNdaughters( 0 ),
  fPDF( pdf->asTF( RooArgList( mass ) ) ) { }

//_______________________________________________________________________________
//
Analysis::DecayParticle::DecayParticle( const char *name,
					TF1        *pdf ) :
  fName( name ),
  fNdaughters( 0 ),
  fPDF( pdf ) { }

//_______________________________________________________________________________
//
Analysis::DecayParticle::DecayParticle( const char *name,
					double      mass,
					double      width,
					double      sigma ) :
  fMass( mass ),
  fName( name ),
  fNdaughters( 0 ),
  fPDF( 0 ) {

  this->SetDistribution( width, sigma );
}

//_______________________________________________________________________________
//
Analysis::DecayParticle::~DecayParticle() {

  if ( fPDF )
    delete fPDF;

  if ( fPDFbase.size() )
    for ( std::vector<RooAbsReal*>::iterator it = fPDFbase.begin();
	  it != fPDFbase.end(); it++ )
      delete ( *it );
}

//_______________________________________________________________________________
//
std::string Analysis::DecayParticle::GetDecay() {

  std::string out( fName );

  if ( fParticles.size() != 0 ) {

    out += "->(";
    out += fParticles[ 0 ]->GetDecay();

    for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin() + 1;
	  it != fParticles.end(); it++ ) {
    
      out += ", ";
      out += ( *it )->Name();
    }

    out += ")";
  }

  return out;
}

//_______________________________________________________________________________
//
double Analysis::DecayParticle::GetMass() {

  if ( fPDF )
    fMass = fPDF->GetRandom();

  return fMass;
}

//_______________________________________________________________________________
//
unsigned int Analysis::DecayParticle::GetNparticles() {

  unsigned int np( 1 );

  for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin();
	it != fParticles.end(); it++ )
    np += ( *it )->GetNparticles();

  return np;
}

//_______________________________________________________________________________
//
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
//
void Analysis::DecayParticle::SetBranches( TTree *tree ) {

  std::string
    br_name,
    mag[ 6 ] = { "_M", "_PX", "_PY", "_PZ", "_PE", "_GenWeight" };

  br_name = fName + mag[ 0 ];
  tree->Branch( br_name.c_str(), &fMass, ( br_name + "/D" ).c_str() );
  std::cout << " " << br_name;

  for ( unsigned int i = 1; i < 5; i++ ) {

    br_name = fName + mag[ i ];
    tree->Branch( br_name.c_str(), &fVector( i - 1 ), ( br_name + "/D" ).c_str() );
    std::cout << ", " << br_name;
  }
  if ( fNdaughters != 0 ) {

    br_name = fName + mag[ 5 ];
    tree->Branch( br_name.c_str(), &fGenWeight, ( br_name + "/D" ).c_str() );
    std::cout << ", " << br_name;
  } 
  std::cout << std::endl;

  for ( std::vector<Analysis::DecayParticle*>::iterator it = fParticles.begin();
	it != fParticles.end(); it++ ) ( *it )->SetBranches( tree );
}

//_______________________________________________________________________________
//
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

    fPDF = pdf->asTF( RooArgList( *rmass ) );

    fPDFbase.push_back( pdf );
    fPDFbase.push_back( rmass );
  }
  else
    fVector.SetXYZM( 0, 0, 0, fMass );
}

//_______________________________________________________________________________
//
void Analysis::DecayParticle::GenPhaseSpace( double px, double py, double pz ) {

  double masses[ fNdaughters ];

  for ( unsigned int i = 0; i < fNdaughters; i++ )
    masses[ i ] = fParticles[ i ]->GetMass();
  
  fPHSP.SetDecay( fVector, fNdaughters, masses );

  fGenWeight = fPHSP.Generate();

  for ( unsigned int i = 0; i < fNdaughters; i++ ) {

    fParticles[ i ]->SetPxPyPzE( fPHSP.GetDecay( i )->Px(),
				   fPHSP.GetDecay( i )->Py(),
				   fPHSP.GetDecay( i )->Pz(),
				   fPHSP.GetDecay( i )->E() );

    if ( fParticles[ i ]->fNdaughters != 0 )
      fParticles[ i ]->GenPhaseSpace();
  }
}
