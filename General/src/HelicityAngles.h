///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class HelicityAngles, that allows to calculate the helicity
//  angles of 4-body decays.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef HELICITY_ANGLES
#define HELICITY_ANGLES

#include "LorentzVector.h"


//______________________________________________________________________________

namespace General {

  class HelicityAngles {

  public:

    // Main constructor
    HelicityAngles();

    // Constructor given the paths to the different Lorentz vectors
    HelicityAngles( LorentzVector &Mother,
		    LorentzVector &P1, LorentzVector &P2,
		    LorentzVector &P11, LorentzVector &P12,
		    LorentzVector &P21, LorentzVector &P22 );

    // Destructor
    ~HelicityAngles();

    // Return the angle < fCosTheta1 >
    inline double GetCosTheta1() const;

    // Return the angle < fCosTheta2 >
    inline double GetCosTheta2() const;

    // Return the phi angle
    inline double GetPhi() const;

    // Calculates the angles
    inline void CalculateAngles();

    // Calculates the helicity angles of a four-body decay given all the 4-vectors
    // of the particles
    void CalculateAngles( LorentzVector &Mother,
			  LorentzVector &P1,  LorentzVector &P2,
			  LorentzVector &P11, LorentzVector &P12,
			  LorentzVector &P21, LorentzVector &P22 );
    
    // Calculates the helicity angles of a four-body decay given the 4-vectors of
    // the daughters
    void CalculateAngles( LorentzVector &P11, LorentzVector &P12,
			  LorentzVector &P21, LorentzVector &P22 );

    // Grants access to the path of < fCosTheta1 >
    inline double* PathToCosTheta1();

    // Grants access to the path of < fCosTheta2 >
    inline double* PathToCosTheta2();

    // Grants access to the path of < fPhi >
    inline double* PathToPhi();

    // Sets the pointers to the 4-vectors of the particles
    inline void SetParticles( LorentzVector &Mother,
			      LorentzVector &P1,  LorentzVector &P2,
			      LorentzVector &P11, LorentzVector &P12,
			      LorentzVector &P21, LorentzVector &P22 );

    // Calculates the triple product < U > variable
    inline double TripleProdU();

    // Calculates the triple product < V > variable
    inline double TripleProdV();

  protected:
      
    // Mother particle
    LorentzVector *fMother;

    // First daughter
    LorentzVector *fP1;

    // Second daughter
    LorentzVector *fP2;

    // First-first granddaughter
    LorentzVector *fP11;

    // First-second graddaughter
    LorentzVector *fP12;

    // Second-first granddaughter
    LorentzVector *fP21;

    // Second-second granddaughter
    LorentzVector *fP22;

    // Angle between first daughter particle respect to one of its
    // own daughter particles in the mother reference frame
    double fCosTheta1;

    // Angle between second daughter particle respect to one of its
    // own daughter particles in the mother reference frame
    double fCosTheta2;

    // Angle between the planes formed by the granddaughters
    double fPhi;
    
  };

  //______________________________________________________________________________
  //
  inline void HelicityAngles::CalculateAngles() {
    this->CalculateAngles( *fMother,
			   *fP1, *fP2,
			   *fP11, *fP12,
			   *fP21, *fP22 );
  }

  //______________________________________________________________________________
  //
  inline double  HelicityAngles::GetCosTheta1() const { return fCosTheta1; }

  //______________________________________________________________________________
  //
  inline double  HelicityAngles::GetCosTheta2() const { return fCosTheta2; }

  //______________________________________________________________________________
  //
  inline double  HelicityAngles::GetPhi() const { return fPhi; }

  //______________________________________________________________________________
  //
  inline double* HelicityAngles::PathToCosTheta1() { return &fCosTheta1; }

  //______________________________________________________________________________
  //
  inline double* HelicityAngles::PathToCosTheta2() { return &fCosTheta2; }

  //______________________________________________________________________________
  //
  inline double* HelicityAngles::PathToPhi() { return &fPhi; }

  //______________________________________________________________________________
  //
  inline void HelicityAngles::SetParticles( LorentzVector &Mother,
					    LorentzVector &P1,  LorentzVector &P2,
					    LorentzVector &P11, LorentzVector &P12,
					    LorentzVector &P21, LorentzVector &P22 ) {

    fMother = &Mother;
    fP1 = &P1; fP2 = &P2;
    fP11 = &P11; fP12 = &P12;
    fP21 = &P21; fP22 = &P22;
  }

  //______________________________________________________________________________
  //
  inline double  HelicityAngles::TripleProdU() { return std::sin( 2*fPhi ); }

  //______________________________________________________________________________
  //
  inline double  HelicityAngles::TripleProdV() {
    return fCosTheta1*fCosTheta2 > 0 ? std::sin( fPhi ) : -std::sin( fPhi );
  }

}

#endif
