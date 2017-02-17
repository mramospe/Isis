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
//
//  Description:
//
//  This class allows to define a particle given its mass, width and/or resolution,
//  or a PDF, to generate Toy-MonteCarlo samples.
//
// ----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////


#ifndef DECAY_PARTICLE
#define DECAY_PARTICLE

#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"
#include "TParticlePDG.h"
#include "TTree.h"

#include <string>
#include <vector>


namespace Analysis {

  class ToyMCgenerator;

  class DecayParticle {

    friend class ToyMCgenerator;

  public:

    // Constructor given the name and the possible resolution. The information of
    // the particle is obtained through the TDatabasePDG class, but with the masses
    // and widths in MeV, not in GeV
    DecayParticle( const char *name, double sigma = 0 );

    // Constructor given a name, a PDF and its independent variable
    DecayParticle( const char *name, RooAbsPdf *pdf, RooRealVar &mass );

    // Constructor given a name and a PDF as a TF1
    DecayParticle( const char *name, TF1 *pdf );

    // Constructor given a name, the mass, width and resolution for the particle
    DecayParticle( const char *name, double mass, double width, double sigma );

    // Destructor
    ~DecayParticle();

    // Adds a new daughter to the decay of this particle
    inline void AddDaughter( DecayParticle &particle );

    // Clears the list of particles attached to this one
    inline void Clear();

    // Returns the name of the particle
    inline std::string Name() const;

    // Gets the string with the decay structure of the particle
    std::string GetDecay();

    // Gets the mass of the particle
    double GetMass();

    // Returns the number of particles attached to this one
    unsigned int GetNparticles();

    // Removes one particle from the list of particles
    void Remove( DecayParticle &particle );

    // Sets the distribution of a particle given its width and resolution values
    void SetDistribution( double width, double sigma );

    // Sets the kinematic variables of the particle
    inline void SetPxPyPzE( double px, double py, double pz, double pe );

  protected:

    // Mass of the particle
    double fMass;

    // Name of the particle
    std::string fName;

    // Number of daughters
    unsigned int fNdaughters;

    // Vector with the paths to all the particles attached to this one
    std::vector<DecayParticle*> fParticles;

    // PDF to do the generation
    TF1 *fPDF;

    // Vector with PDFs for all the daughters
    std::vector<RooAbsReal*> fPDFbase;

    // Main class to generate the phase space
    TGenPhaseSpace fPHSP;

    // Vector of this particle
    TLorentzVector fVector;

    // Generator weight
    double fGenWeight;

  private:

    // Generates the phase-space for the decay of this particle
    void GenPhaseSpace( double px = 0, double py = 0, double pz = 0 );

    // Sets the branches for a given tree, with the kinematic information of every
    // particle attached to this one
    void SetBranches( TTree *tree );

  };

  //_______________________________________________________________________________
  //
  inline void DecayParticle::AddDaughter( DecayParticle &particle ) {
    fParticles.push_back( &particle );
    fNdaughters++;
  }

  //_______________________________________________________________________________
  //
  inline void DecayParticle::Clear() { fParticles.clear(); }

  //_______________________________________________________________________________
  //
  inline std::string DecayParticle::Name() const  { return fName; }

  //_______________________________________________________________________________
  //
  inline void DecayParticle::SetPxPyPzE( double px, double py, double pz, double pe ) {
    fVector.SetPxPyPzE( px, py, pz, pe );
  }
}

#endif
