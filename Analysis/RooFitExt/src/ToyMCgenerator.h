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
//  Implements the class to generate Toy-MonteCarlo samples. Particles are
//  constructed with the <DecayParticle> class. All the particles and links
//  between them (daughters, granddaughters...) have to be specified before the
//  Monte-Carlo generation. It is performed via the <ToyMCgenerator> class, which
//  uses the information of a mother particle or the distributions of a given
//  TTree.
//
// ----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////


#ifndef TOY_MC_GENERATOR
#define TOY_MC_GENERATOR

#include "DecayParticle.h"

#include "TF1.h"
#include "TLorentzVector.h"
#include "TTree.h"

#include <vector>
#include <string>


//_______________________________________________________________________________

namespace Analysis {

  class ToyMCgenerator {

  public:

    // Constructor given the mother particle and the possible output tree name
    ToyMCgenerator( DecayParticle &mother, const char *tree_name = 0 );

    // Constructor given the mother particle, the paths to the tree and the different
    // names of the kinematic variables of it, as well as a possible output tree name
    ToyMCgenerator( DecayParticle &mother,
		    TTree         *tree,
		    const char    *px,
		    const char    *py,
		    const char    *pz,
		    const char    *pe,
		    const char    *tree_name = 0 );

    // Destructor
    ~ToyMCgenerator();

    // Generates the Monte-Carlo data
    void Generate( long int nevts = 100000 );

    // Get the decay structure
    inline std::string GetDecayTree();

    // Creates another tree with the given name
    inline void CreateTree( const char *tree_name );

  protected:

    // Input tree
    TTree *fInputTree;

    // Mother particle to generate the decay
    DecayParticle *fMother;

    // Output tree
    TTree *fOutputTree;

    // Name of the variables
    std::vector<const char*> fVarNames;

  private:
    
    // Displays the startup message
    inline void StartMessage();

  };

  //_______________________________________________________________________________
  //
  inline std::string ToyMCgenerator::GetDecayTree() {
    return fMother->GetDecay();
  }

  //_______________________________________________________________________________
  //
  inline void ToyMCgenerator::CreateTree( const char *tree_name ) {
    fOutputTree = new TTree( tree_name, tree_name, 0 );
  }

  //_______________________________________________________________________________
  //
  inline void ToyMCgenerator::StartMessage() {

    unsigned int w( ( fMother->fName ).size() + 48 );

    std::cout << "\n";
    std::cout.width( w );
    std::cout.fill( '*' );
    std::cout << std::left << " ";
    std::cout << "\n *** ToyMCgenerator for mother particle < " <<
      fMother->fName << " > ***\n";
    std::cout.width( w );
    std::cout.fill( '*' );
    std::cout << std::left << " " << std::endl;
    std::cout << std::endl;
  }

}

#endif
