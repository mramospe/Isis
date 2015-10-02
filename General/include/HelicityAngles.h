///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //       
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 22/07/2015                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class HelicityAngles, that allows to calculate the helicity   //
//  angles of 4-body decays.                                                     //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef HELICITY_ANGLES
#define HELICITY_ANGLES

#include "LorentzVector.h"

//______________________________________________________________________________

namespace General {

  class HelicityAngles {

  public:

    // Constructors
    HelicityAngles();
    HelicityAngles( LorentzVector &Mother,
		    LorentzVector &P1, LorentzVector &P2,
		    LorentzVector &P11, LorentzVector &P12,
		    LorentzVector &P21, LorentzVector &P22 );

    // Destructor
    ~HelicityAngles();

    // Methods
    inline double  GetCosTheta1() const;
    inline double  GetCosTheta2() const;
    inline double  GetPhi() const;
    inline void    CalculateAngles();
    void           CalculateAngles( LorentzVector &Mother,
				    LorentzVector &P1,  LorentzVector &P2,
				    LorentzVector &P11, LorentzVector &P12,
				    LorentzVector &P21, LorentzVector &P22 );
    void           CalculateAngles( LorentzVector &P11, LorentzVector &P12,
				    LorentzVector &P21, LorentzVector &P22 );
    inline double* PathToCosTheta1();
    inline double* PathToCosTheta2();
    inline double* PathToPhi();
    inline void    SetParticles( LorentzVector &Mother,
				 LorentzVector &P1,  LorentzVector &P2,
				 LorentzVector &P11, LorentzVector &P12,
				 LorentzVector &P21, LorentzVector &P22 );
    inline double  TripleProdU();
    inline double  TripleProdV();

  protected:
      
    // Attributes
    LorentzVector *fMother;
    LorentzVector *fP1;
    LorentzVector *fP2;
    LorentzVector *fP11;
    LorentzVector *fP12;
    LorentzVector *fP21;
    LorentzVector *fP22;
    double fCosTheta1;
    double fCosTheta2;
    double fPhi;

  };

  // Calculates the angles
  inline void    HelicityAngles::CalculateAngles() {
    this -> CalculateAngles( *fMother,
			     *fP1, *fP2,
			     *fP11, *fP12,
			     *fP21, *fP22 );
  }
  // Gets the different variables
  inline double  HelicityAngles::GetCosTheta1() const { return fCosTheta1; }
  inline double  HelicityAngles::GetCosTheta2() const { return fCosTheta2; }
  inline double  HelicityAngles::GetPhi() const { return fPhi; }
  // Grants access to the path of the variables
  inline double* HelicityAngles::PathToCosTheta1() { return &fCosTheta1; }
  inline double* HelicityAngles::PathToCosTheta2() { return &fCosTheta2; }
  inline double* HelicityAngles::PathToPhi() { return &fPhi; }
  // Sets the pointers to the particles 4-vectors
  inline void    HelicityAngles::SetParticles( LorentzVector &Mother,
					       LorentzVector &P1,  LorentzVector &P2,
					       LorentzVector &P11, LorentzVector &P12,
					       LorentzVector &P21, LorentzVector &P22 ) {

    fMother = &Mother;
    fP1 = &P1; fP2 = &P2;
    fP11 = &P11; fP12 = &P12;
    fP21 = &P21; fP22 = &P22;
  }
  // Calculates the different helicity variables
  inline double  HelicityAngles::TripleProdU() { return std::sin( 2*fPhi ); }
  inline double  HelicityAngles::TripleProdV() {
    return fCosTheta1*fCosTheta2 > 0 ? std::sin( fPhi ) : -std::sin( fPhi );
  }

}

#endif
