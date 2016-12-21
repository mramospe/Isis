/////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ----------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 24/10/2015
//
// ----------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the two classes to generate Toy-MonteCarlo samples. Particles are
//  constructed with the <DecayParticle> class, which allows to define them given
//  its mass, width and/or resolution, or a TF1/PDF. All the particles and links
//  between them (daughters, granddaughters...) have to be specified before the
//  Monte-Carlo generation. It is performed via the <ToyMCgenerator> class, which
//  uses the information of a mother particle or the distributions of a given
//  TTree.
//
// ----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////


#ifndef TOY_MC_GENERATOR
#define TOY_MC_GENERATOR

#include <vector>
#include <string>

#include "TTree.h"
#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "TF1.h"


//_______________________________________________________________________________

namespace Analysis {

  class DecayParticle {

    friend class ToyMCgenerator;

  public:

    // Constructors
    DecayParticle( const char *name, double sigma = 0 );
    DecayParticle( const char *name, RooAbsPdf *pdf, RooRealVar &mass );
    DecayParticle( const char *name, TF1 *pdf );
    DecayParticle( const char *name, double mass, double width, double sigma );

    // Destructor
    ~DecayParticle();

    // Methods
    inline void        AddDaughter( DecayParticle &particle );
    inline void        Clear();
    inline std::string Name() const;
    std::string        GetDecay();
    double             GetMass();
    unsigned int       GetNparticles();
    void               Remove( DecayParticle &particle );
    void               SetDistribution( double width, double sigma );
    inline void        SetPxPyPzE( double px, double py, double pz, double pe );

  protected:

    // Attributes
    double                       fMass;
    std::string                  fName;
    unsigned int                 fNdaughters;
    std::vector<DecayParticle*>  fParticles;
    TF1                         *fPDF;
    std::vector<RooAbsReal*>     fPDFbase;
    TGenPhaseSpace               fPHSP;
    TLorentzVector               fVector;
    double                       fGenWeight;

  private:

    // Methods
    void GenPhaseSpace( double px = 0, double py = 0, double pz = 0 );
    void SetBranches( TTree *tree );

  };

  //______________________________
  // INLINE METHODS

  // Adds a new daughter to the decay of this particle
  inline void DecayParticle::AddDaughter( DecayParticle &particle ) {
    fParticles.push_back( &particle );
    fNdaughters++;
  }
  // Clears the list of particles attached to this one
  inline void DecayParticle::Clear() { fParticles.clear(); }
  // Returns the name of the particle
  inline std::string DecayParticle::Name() const  { return fName; }
  // Sets the kinematic variables of the particle
  inline void DecayParticle::SetPxPyPzE( double px, double py, double pz, double pe ) {
    fVector.SetPxPyPzE( px, py, pz, pe );
  }


  class ToyMCgenerator {

  public:

    // Constructors
    ToyMCgenerator( DecayParticle &mother, const char *tree_name = 0 );
    ToyMCgenerator( DecayParticle &mother,
		    TTree         *tree,
		    const char    *px,
		    const char    *py,
		    const char    *pz,
		    const char    *pe,
		    const char    *tree_name = 0 );

    // Destructor
    ~ToyMCgenerator();

    // Methods
    void               Generate( long int nevts = 100000 );
    inline std::string GetDecayTree();
    inline void        CreateTree( const char *tree_name );

  protected:

    // Attributes
    TTree                    *fInputTree;
    DecayParticle            *fMother;
    TTree                    *fOutputTree;
    std::vector<const char*>  fVarNames;

  private:
    
    // Method
    inline void StartMessage();

  };

  //______________________________
  // INLINE METHODS

  // Gets the decay structure
  inline std::string ToyMCgenerator::GetDecayTree() {
    return fMother -> GetDecay();
  }
  // Creates another tree with the given name
  inline void ToyMCgenerator::CreateTree( const char *tree_name ) {
    fOutputTree = new TTree( tree_name, tree_name, 0 );
  }
  // Displays the startup message
  inline void ToyMCgenerator::StartMessage() {

    unsigned int w( ( fMother -> fName ).size() + 48 );

    std::cout << "\n";
    std::cout.width( w );
    std::cout.fill( '*' );
    std::cout << std::left << " ";
    std::cout << "\n *** ToyMCgenerator for mother particle < " <<
      fMother -> fName << " > ***\n";
    std::cout.width( w );
    std::cout.fill( '*' );
    std::cout << std::left << " " << std::endl;
    std::cout << std::endl;
  }

}

#endif
