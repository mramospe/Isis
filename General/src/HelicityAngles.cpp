///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "HelicityAngles.h"


//______________________________________________________________________________

namespace Isis {

  //______________________________________________________________________________
  //
  HelicityAngles::HelicityAngles() :
    fMother( 0 ),
    fP1( 0 ), fP2( 0 ),
    fP11( 0 ), fP12( 0 ),
    fP21( 0 ), fP22( 0 ) { }

  //______________________________________________________________________________
  //
  HelicityAngles::HelicityAngles( LorentzVector &Mother,
				  LorentzVector &P1, LorentzVector &P2,
				  LorentzVector &P11, LorentzVector &P12,
				  LorentzVector &P21, LorentzVector &P22 ) :
    fMother( &Mother ),
    fP1( &P1 ), fP2( &P2 ),
    fP11( &P11 ), fP12( &P12 ),
    fP21( &P21 ), fP22( &P22 ) { }

  //______________________________________________________________________________
  //
  HelicityAngles::~HelicityAngles() { }

  //______________________________________________________________________________
  //
  void HelicityAngles::CalculateAngles( LorentzVector &Mother,
					LorentzVector &P1,
					LorentzVector &P2,
					LorentzVector &P11,
					LorentzVector &P12,
					LorentzVector &P21,
					LorentzVector &P22 ) {
  
    Vector 
      P1_MRF( Mother.LorentzTransf( P1 ).Unitary() ),
      P2_MRF( Mother.LorentzTransf( P2 ).Unitary() ),
      P11_P1RF( P1.LorentzTransf( P11 ).Unitary() ),
      P21_P2RF( P2.LorentzTransf( P21 ).Unitary() ),
      P11_MRF( Mother.LorentzTransf( P11 ).Unitary() ),
      P12_MRF( Mother.LorentzTransf( P12 ).Unitary() ),
      P21_MRF( Mother.LorentzTransf( P21 ).Unitary() ),
      P22_MRF( Mother.LorentzTransf( P22 ).Unitary() ),
      n1( P11_MRF.Cross( P12_MRF ) ),
      n2( P21_MRF.Cross( P22_MRF ) );

    fCosTheta1 = P11_P1RF.Dot( P1_MRF );
    fCosTheta2 = P21_P2RF.Dot( P2_MRF );

    fPhi = ( std::atan2( n1.Cross( n2 ).Dot( P1_MRF ), n1.Dot( n2 ) ) );
    fPhi = fPhi > 0 ? fPhi : fPhi + 2*M_PI;
  }

  //______________________________________________________________________________
  //
  void HelicityAngles::CalculateAngles( LorentzVector &P11,
					LorentzVector &P12,
					LorentzVector &P21,
					LorentzVector &P22 ) {

    LorentzVector
      P1( P11 + P12 ),
      P2( P21 + P22 ),
      Mother( P1 + P2 );

    this->CalculateAngles( Mother, P1, P2, P11, P12, P21, P22 );
  }

}
