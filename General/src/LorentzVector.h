///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
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


#ifndef LORENTZ_VECTOR
#define LORENTZ_VECTOR

#include <limits>

#include "Vector.h"


//_______________________________________________________________________________

namespace General {

  class LorentzVector {

  public:

    // Main constructor
    LorentzVector();

    // Constructor given all the coordinates
    LorentzVector( const double &px, const double &py, const double &pz, const double &pe );

    // Contructor given the momentum and the energy
    LorentzVector( const Vector &vec, const double &pe );

    // Copy constructor
    LorentzVector( const LorentzVector &vec );

    // Destructor
    ~LorentzVector();

    // Gets the angle between two vectors
    inline double Angle( const LorentzVector &vec ) const;

    // Gets the Armenteros-Podolanski parameter alpha
    double ArmAlpha( const LorentzVector &vec1, const LorentzVector &vec2 ) const;

    // Gets the Armenteros-Podolanski transverse momentum
    double ArmPt( const LorentzVector &vec ) const;

    // Gets the Armenteros-Podolanski longitudinal momentum
    double ArmPl( const LorentzVector &vec ) const;

    // Gets the value of beta for the particle
    inline double Beta() const;

    // Gets the vector concerning the parameter beta
    inline Vector VBeta() const;

    // Gets the cosine of the angle between two LorentzVectors
    inline double CosAngle( const LorentzVector &vec ) const;

    // Gets the cosine of the angle respect to a 3D vector
    inline double CosAngle( const Vector &vec ) const;

    // Calculates the value of the cosine of the helicity angle of the particle
    // given by <vec> in the direction <dir>
    double CosHelAngle( const LorentzVector &vec, const Vector &dir ) const;

    // Gets the value of the cosine of the helicity angle of the particle <vec>
    // in the direction <dir>
    inline double CosHelAngle( const LorentzVector &vec, const LorentzVector &dir ) const;
    
    // Gets the phi angle in the x, y, z coordinate system
    inline double CosPhi() const;

    // Gets the theta angle in the x, y, z coordinate system
    inline double CosTheta() const;

    // Gets the value of gamma for the particle
    inline double Gamma() const;

    // Gets the longitudinal component of the given LorentzVector regarding to this one
    inline Vector GetLong( const LorentzVector &vec ) const;

    // Gets the longitudinal component of the given 3D vector regarding to this one
    inline Vector GetLong( const Vector &vec ) const;

    // Gets the transversal component of the given LorentzVector regarding to this one
    inline Vector GetTran( const LorentzVector &vec ) const;

    // Gets the transversal component of the given 3D vector regarding to this one
    inline Vector GetTran( const Vector &vec ) const;

    // Gets the value of the helicity angle of the particle <vec> in the direction <dir>
    inline double HelAngle( const LorentzVector &vec, const Vector &dir ) const;

    // Gets the value of the helicity angle of the particle <vec> in the direction in <dir>
    inline double HelAngle( const LorentzVector &vec, const LorentzVector &dir ) const;

    // Gets the transformed vector of the particle in the reference frame of the
    // class vector
    LorentzVector LorentzTransf( const LorentzVector &vec ) const;

    // Gets the mass of the particle
    inline double Mass() const;

    // Gets the squared mass of the particle
    inline double Mass2() const;

    // Gets the value of the phi angle in the x, y, z coordinate system
    inline double Phi() const;

    // Gets the module of the momentum
    inline double P() const;

    // Gets the pseudorapidity of the particle
    double PseudoRapidity() const;

    // Gets the value of the transverse momentum
    inline double Pt() const;

    // Gets the rapidity of the particle
    inline double Rapidity() const;

    // Methods to rotate this vector a given angle respect to the X axis
    void RotateX( const double &angle );

    // Methods to rotate this vector a given angle respect to the Y axis
    void RotateY( const double &angle );

    // Methods to rotate this vector a given angle respect to the Z axis
    void RotateZ( const double &angle );

    // Set the mass
    inline void SetM( const double &val );

    // Set the momentum on the X direction
    inline void SetPx( const double &val );

    // Set the momentum on the Y direction
    inline void SetPy( const double &val );

    // Set the momentum on the Y direction
    inline void SetPz( const double &val );

    // Set the energy
    inline void SetE( const double &val );

    // Set the four coordinates of this vector
    inline void SetPxPyPzE( const double &px,
			    const double &py,
			    const double &pz,
			    const double &pe );

    // Set the three spatial coordinates and the energy using the mass
    inline void SetPxPyPzM( const double &px,
			    const double &py,
			    const double &pz,
			    const double &m );

    // Gets the value of the theta angle in the x, y, z coordinate system
    inline double Theta() const;

    // Gets the unitary vector
    inline Vector Unitary() const;

    // Get the momentum on the X direction
    inline double Px() const;

    // Get path of the momentum on the X direction
    inline double* PathToPx();

    // Get the momentum on the Y direction
    inline double Py() const;

    // Get path of the momentum on the Y direction
    inline double* PathToPy();

    // Get the momentum on the Z direction
    inline double Pz() const;

    // Get path of the momentum on the Z direction
    inline double* PathToPz();

    // Get the energy
    inline double E() const;

    // Get path of the energy
    inline double* PathToE();

    // Get the vector associated with the momentum
    inline Vector Momentum() const;

    // Get path to the vector associated with the momentum
    inline Vector* PathToMomentum();

    // Directly add the values from another vector
    inline LorentzVector& operator += ( const LorentzVector &vec );

    // Directly substract the values from another vector
    inline LorentzVector& operator -= ( const LorentzVector &vec );

    // Add two vectors
    inline LorentzVector operator +  ( const LorentzVector &vec ) const;

    // Substract two vectors
    inline LorentzVector operator -  ( const LorentzVector &vec ) const;

    // Represent this vector as a stream
    std::ostream& operator << ( std::ostream &os ) const;

  protected:
    
    // 3D vector
    Vector fP;

    // Energy
    double fE;
    
  };

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Angle( const LorentzVector &vec ) const { return fP.Angle( vec.fP ); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Beta() const { return fP.Mod()/fE; }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::VBeta() const { return fP/fE; }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::CosAngle( const LorentzVector &vec ) const {
    return fP.CosAngle( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::CosAngle( const Vector &vec ) const { return fP.CosAngle( vec ); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::CosHelAngle( const LorentzVector &vec,
					    const LorentzVector &dir ) const {
    return this -> CosHelAngle( vec, dir.Momentum() );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::CosPhi() const { return fP.CosPhi(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::CosTheta() const { return fP.CosTheta(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Gamma() const { return fE/std::sqrt( fE*fE - fP.Mod2() ); }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::GetLong( const LorentzVector &vec ) const {
    return fP.GetLong( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::GetLong( const Vector &vec ) const { return fP.GetLong( vec ); }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::GetTran( const LorentzVector &vec ) const {
    return fP.GetTran( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::GetTran( const Vector &vec ) const {
    return fP.GetTran( vec );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::HelAngle( const LorentzVector &vec, const Vector &dir ) const {
    return std::acos( this -> CosHelAngle( vec, dir ) );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::HelAngle( const LorentzVector &vec, const LorentzVector &dir ) const {
    return std::acos( this -> CosHelAngle( vec, dir ) ); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Mass() const { return std::sqrt( fE*fE - fP.Mod2() ); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Mass2() const { return fE*fE - fP.Mod2(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Phi() const { return fP.Phi(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Pt() const { return fP.ModT(); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::RotateX( const double &angle ) { fP.RotateX( angle ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::RotateY( const double &angle ) { fP.RotateY( angle ); }
  
  //_______________________________________________________________________________
  //
  inline void LorentzVector::RotateZ( const double &angle ) { fP.RotateZ( angle ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetM( const double &val ) {
    fE = std::sqrt( fP.Mod2() + val*val );
  }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetPx( const double &val ) { fP.SetX( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetPy( const double &val ) { fP.SetY( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetPz( const double &val ) { fP.SetZ( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetE( const double &val ) { fE = val; }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetPxPyPzE( const double &px,
					 const double &py,
					 const double &pz,
					 const double &pe ) {
    fP.SetXYZ( px, py, pz ); fE = pe;
  }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::SetPxPyPzM( const double &px,
					 const double &py,
					 const double &pz,
					 const double &m ) {
    fP.SetXYZ( px, py, pz ); fE = std::sqrt( fP.Mod2() + m*m );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Theta() const { return fP.Theta(); }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::Unitary() const { return fP.Unitary(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::P() const { return fP.Mod(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Px() const { return fP.X(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::PathToPx() { return fP.PathToX(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Py() const { return fP.Y(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::PathToPy() { return fP.PathToY(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Pz() const { return fP.Z(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::PathToPz() { return fP.PathToZ(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::E() const { return fE; }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::PathToE() { return &fE; }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::Momentum() const { return fP; }

  //_______________________________________________________________________________
  //
  inline Vector* LorentzVector::PathToMomentum() { return &fP; }

  //_______________________________________________________________________________
  //
  inline double   LorentzVector::Rapidity() const {
    return fE - fP.Z() != 0 ?
      0.5*std::log( ( fE + fP.Z() ) / ( fE - fP.Z() ) ) :
      std::numeric_limits<double>::max();
  }

  //_______________________________________________________________________________
  //
  inline LorentzVector& LorentzVector::operator += ( const LorentzVector &vec ) {
    fP += vec.fP;
    fE += vec.fE;
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline LorentzVector& LorentzVector::operator -= ( const LorentzVector &vec ) {
    fP -= vec.fP;
    fE -= vec.fE;
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline LorentzVector LorentzVector::operator + ( const LorentzVector &vec ) const {
    return LorentzVector( fP.X() + vec.Px(),
			  fP.Y() + vec.Py(),
			  fP.Z() + vec.Pz(),
			  fE     + vec.E() );
  }

  //_______________________________________________________________________________
  //
  inline LorentzVector LorentzVector::operator - ( const LorentzVector &vec ) const {
    return LorentzVector( fP.X() - vec.Px(),
			  fP.Y() - vec.Py(),
			  fP.Z() - vec.Pz(),
			  fE     - vec.E() );
  }

  //_______________________________________________________________________________
  //
  inline std::ostream& LorentzVector::operator << ( std::ostream &os ) const {
    
    os << "[ "
       << this->Px() << ", "
       << this->Py() << ", "
       << this->Pz() << ", "
       << this->E() << " ]";
    return os;
  }

}

#endif
