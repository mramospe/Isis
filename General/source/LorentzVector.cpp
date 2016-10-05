///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 02/03/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the LorentzVector class, that allows to compute and perform
//  operations concerning the 4-vector physics of particles.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "LorentzVector.h"

#include <cmath>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::LorentzVector::LorentzVector() { }

//_______________________________________________________________________________
// Constructor given all the coordinates
General::LorentzVector::LorentzVector( const double &px,
				       const double &py,
				       const double &pz,
				       const double &pe ) :
  fP( px, py, pz ), fE( pe ) { }

//_______________________________________________________________________________
// Contructor given the momentum and the energy
General::LorentzVector::LorentzVector( const Vector &vec, const double &pe ) :
  fP( vec ), fE( pe ) { }

//_______________________________________________________________________________
// Copy constructor
General::LorentzVector::LorentzVector( const LorentzVector &vec ) :
  fP( vec.fP ), fE( vec.fE ) { }

//_______________________________________________________________________________
// Destructor
General::LorentzVector::~LorentzVector() { }

//_______________________________________________________________________________


// -- METHODS

//_______________________________________________________________________________
// Gets the Armenteros-Podolanski parameter alpha
double General::LorentzVector::ArmAlpha( const General::LorentzVector &vec1,
					 const General::LorentzVector &vec2 ) const {
  double
    pl1( this -> ArmPl( vec1 ) ),
    pl2( this -> ArmPl( vec2 ) );
  return pl1 + pl2 != 0 ? ( pl1 - pl2 )/( pl1 + pl2 ) : 1;
}

//_______________________________________________________________________________
// Gets the Armenteros-Podolanski transverse momentum
double  General::LorentzVector::ArmPt( const General::LorentzVector &vec ) const {
  return std::sqrt( vec.fP.Mod2() - this -> ArmPl( vec )*this -> ArmPl( vec ) );
}

//_______________________________________________________________________________
// Gets the Armenteros-Podolanski longitudinal momentum
double  General::LorentzVector::ArmPl( const General::LorentzVector &vec ) const {
  return vec.fP.Dot( fP.Unitary() );
}

//_______________________________________________________________________________
// Calculates the value of the cosine of the helicity angle of the particle
// given by <vec> in the direction <dir>
double General::LorentzVector::CosHelAngle( const LorentzVector &vec, const Vector &dir ) const {
  General::LorentzVector vec_transf( this -> LorentzTransf( vec ) );
  return vec_transf.CosAngle( dir );
}

//_______________________________________________________________________________
// Gets the pseudorapidity of the particle
double General::LorentzVector::PseudoRapidity() const {
  double p( fP.Mod() );
  return 
    p - fP.Z() != 0 ? 0.5*std::log( ( p + fP.Z() ) / ( p - fP.Z() ) ) : 
    std::numeric_limits<double>::max();
}

//_______________________________________________________________________________
// Gets the transformed vector of the particle in the reference frame of the
// class vector
General::LorentzVector General::LorentzVector::LorentzTransf( const General::LorentzVector &vec ) const {

  General::Vector 
    vec_long( this -> GetLong( vec ) ),
    vec_tran( this -> GetTran( vec ) ),
    u       ( vec_long.Unitary() );

  double
    mod  ( vec_long.Mod()   ),
    gamma( this -> Gamma() ),
    btgm ( gamma*this -> Beta()  );

  return General::LorentzVector( vec_tran + ( gamma*mod - btgm*vec.fE )*u,
				 gamma*vec.fE - btgm*mod );
}
