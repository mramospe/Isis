///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 02/03/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the LorentzVector class, that allows to compute and perform       //
//  operations concerning the 4-vector physics of particles                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef LORENTZ_VECTOR
#define LORENTZ_VECTOR

#include <limits>

#include "Vector.h"


//_______________________________________________________________________________

namespace General {

  class LorentzVector {

  public:

    // Constructor
    LorentzVector();
    LorentzVector( const double &px, const double &py, const double &pz, const double &pe );
    LorentzVector( const Vector &vec, const double &pe );
    LorentzVector( const LorentzVector &vec );

    // Destructor
    ~LorentzVector();

    // Methods
    inline double   Angle( const LorentzVector &vec ) const;
    double          ArmAlpha( const LorentzVector &vec1, const LorentzVector &vec2 ) const;
    double          ArmPt( const LorentzVector &vec ) const;
    double          ArmPl( const LorentzVector &vec ) const;
    inline double   Beta() const;
    inline Vector   VBeta() const;
    inline double   CosAngle( const LorentzVector &vec ) const;
    inline double   CosAngle( const Vector &vec ) const;
    double          CosHelAngle( const LorentzVector &vec, const Vector &dir ) const;
    inline double   CosHelAngle( const LorentzVector &vec, const LorentzVector &dir ) const;
    inline double   CosPhi() const;
    inline double   CosTheta() const;
    inline double   Gamma() const;
    inline Vector   GetLong( const LorentzVector &vec ) const;
    inline Vector   GetLong( const Vector &vec ) const;
    inline Vector   GetTran( const LorentzVector &vec ) const;
    inline Vector   GetTran( const Vector &vec ) const;
    inline double   HelAngle( const LorentzVector &vec, const Vector &dir ) const;
    inline double   HelAngle( const LorentzVector &vec, const LorentzVector &dir ) const;
    LorentzVector   LorentzTransf( const LorentzVector &vec ) const;
    inline double   Mass() const;
    inline double   Mass2() const;
    inline double   Phi() const;
    inline double   P() const;
    double          PseudoRapidity() const;
    inline double   Pt() const;
    inline double   Rapidity() const;
    void            RotateX( const double &angle );
    void            RotateY( const double &angle );
    void            RotateZ( const double &angle );
    inline void     SetM( const double &val );
    inline void     SetPx( const double &val );
    inline void     SetPy( const double &val );
    inline void     SetPz( const double &val );
    inline void     SetE( const double &val );
    inline void     SetPxPyPzE( const double &px,
				const double &py,
				const double &pz,
				const double &pe );
    inline void     SetPxPyPzM( const double &px,
				const double &py,
				const double &pz,
				const double &m );
    inline double   Theta() const;
    inline Vector   Unitary() const;
    inline double   Px() const;
    inline double*  PathToPx();
    inline double   Py() const;
    inline double*  PathToPy();
    inline double   Pz() const;
    inline double*  PathToPz();
    inline double   E() const;
    inline double*  PathToE();
    inline Vector   Momentum() const;
    inline Vector*  PathToMomentum();

    // Operators
    inline LorentzVector& operator += ( const LorentzVector &vec );
    inline LorentzVector& operator -= ( const LorentzVector &vec );
    inline LorentzVector  operator +  ( const LorentzVector &vec ) const;
    inline LorentzVector  operator -  ( const LorentzVector &vec ) const;
    inline LorentzVector  operator /  ( const double &val ) const;

  protected:
    
    // Attributes
    Vector fP;
    double fE;
    
  };

  //______________________________
  // INLINE METHODS

  // Gets the angle between two vectors
  inline double LorentzVector::Angle( const LorentzVector &vec ) const { return fP.Angle( vec.fP ); }
  // Gets the value of beta for the particle
  inline double LorentzVector::Beta() const { return fP.Mod()/fE; }
  // Gets the vector concerning the parameter beta
  inline Vector LorentzVector::VBeta() const { return fP/fE; }
  // Gets the cosine of the angle between two vectors
  inline double LorentzVector::CosAngle( const LorentzVector &vec ) const {
    return fP.CosAngle( vec.fP );
  }
  inline double LorentzVector::CosAngle( const Vector &vec ) const { return fP.CosAngle( vec ); }
  // Gets the value of the cosine of the helicity angle of the particle <vec>
  // in the direction <dir>
  inline double LorentzVector::CosHelAngle( const LorentzVector &vec,
					    const LorentzVector &dir ) const {
    return this -> CosHelAngle( vec, dir.Momentum() );
  }
  // Gets the phi angle in the x, y, z coordinate system
  inline double LorentzVector::CosPhi() const { return fP.CosPhi(); }
  // Gets the theta angle in the x, y, z coordinate system
  inline double LorentzVector::CosTheta() const { return fP.CosTheta(); }
  // Gets the value of gamma for the particle
  inline double LorentzVector::Gamma() const { return fE/std::sqrt( fE*fE - fP.Mod2() ); }
  // Gets the longitudinal component of the given vector regarding to this one
  inline Vector LorentzVector::GetLong( const LorentzVector &vec ) const {
    return fP.GetLong( vec.fP );
  }
  inline Vector LorentzVector::GetLong( const Vector &vec ) const { return fP.GetLong( vec ); }
  // Gets the transversal component of the given vector regarding to this one
  inline Vector LorentzVector::GetTran( const LorentzVector &vec ) const {
    return fP.GetTran( vec.fP );
  }
  inline Vector LorentzVector::GetTran( const Vector &vec ) const {
    return fP.GetTran( vec );
  }
  // Gets the value of the helicity angle of the particle <vec> in the direction <dir>
  inline double LorentzVector::HelAngle( const LorentzVector &vec, const Vector &dir ) const {
    return std::acos( this -> CosHelAngle( vec, dir ) ); }
  inline double LorentzVector::HelAngle( const LorentzVector &vec, const LorentzVector &dir ) const {
    return std::acos( this -> CosHelAngle( vec, dir ) ); }
  // Gets the mass of the particle
  inline double LorentzVector::Mass() const { return std::sqrt( fE*fE - fP.Mod2() ); }
  // Gets the squared mass of the particle
  inline double LorentzVector::Mass2() const { return fE*fE - fP.Mod2(); }
  // Gets the value of the phi angle in the x, y, z coordinate system
  inline double LorentzVector::Phi() const { return fP.Phi(); }
  // Gets the value of the transverse momentum
  inline double LorentzVector::Pt() const { return fP.Pt(); }
  // Methods to rotate this vector a given angle
  inline void LorentzVector::RotateX( const double &angle ) { fP.RotateX( angle ); }
  inline void LorentzVector::RotateY( const double &angle ) { fP.RotateY( angle ); }
  inline void LorentzVector::RotateZ( const double &angle ) { fP.RotateZ( angle ); }
  // Methods to set the different components of the vector
  inline void LorentzVector::SetM( const double &val ) {
    fE = std::sqrt( fP.Mod2() + val*val );
  }
  inline void LorentzVector::SetPx( const double &val ) { fP.SetX( val ); }
  inline void LorentzVector::SetPy( const double &val ) { fP.SetY( val ); }
  inline void LorentzVector::SetPz( const double &val ) { fP.SetZ( val ); }
  inline void LorentzVector::SetE( const double &val ) { fE = val; }
  inline void LorentzVector::SetPxPyPzE( const double &px,
					     const double &py,
					     const double &pz,
					     const double &pe ) {
    fP.SetXYZ( px, py, pz ); fE = pe;
  }
  inline void LorentzVector::SetPxPyPzM( const double &px,
					     const double &py,
					     const double &pz,
					     const double &m ) {
    fP.SetXYZ( px, py, pz ); fE = std::sqrt( fP.Mod2() + m*m );
  }
  // Gets the value of the theta angle in the x, y, z coordinate system
  inline double LorentzVector::Theta() const { return fP.Theta(); }
  // Gets the unitary vector
  inline Vector LorentzVector::Unitary() const { return fP.Unitary(); }
  // Gets the module of the momentum
  inline double LorentzVector::P() const { return fP.Mod(); }
  // Methods to get the value or the path of the different components of the vector
  inline double LorentzVector::Px() const { return fP.X(); }
  inline double* LorentzVector::PathToPx() { return fP.PathToX(); }
  inline double LorentzVector::Py() const { return fP.Y(); }
  inline double* LorentzVector::PathToPy() { return fP.PathToY(); }
  inline double LorentzVector::Pz() const { return fP.Z(); }
  inline double* LorentzVector::PathToPz() { return fP.PathToZ(); }
  inline double LorentzVector::E() const { return fE; }
  inline double* LorentzVector::PathToE() { return &fE; }
  inline Vector LorentzVector::Momentum() const { return fP; }
  inline Vector* LorentzVector::PathToMomentum() { return &fP; }
  // Gets the rapidity of the particle
  inline double   LorentzVector::Rapidity() const {
    return fE - fP.Z() != 0 ?
      0.5*std::log( ( fE + fP.Z() ) / ( fE - fP.Z() ) ) :
      std::numeric_limits<double>::max();
  }

  //______________________________
  // OPERATORS

  inline LorentzVector& LorentzVector::operator += ( const LorentzVector &vec ) {
    fP += vec.fP;
    fE += vec.fE;
    return *this;
  }
  inline LorentzVector& LorentzVector::operator -= ( const LorentzVector &vec ) {
    fP -= vec.fP;
    fE -= vec.fE;
    return *this;
  }
  inline LorentzVector LorentzVector::operator + ( const LorentzVector &vec ) const {
    return LorentzVector( fP.X() + vec.Px(),
			  fP.Y() + vec.Py(),
			  fP.Z() + vec.Pz(),
			  fE     + vec.E() );
  }
  inline LorentzVector LorentzVector::operator - ( const LorentzVector &vec ) const {
    return LorentzVector( fP.X() - vec.Px(),
			  fP.Y() - vec.Py(),
			  fP.Z() - vec.Pz(),
			  fE     - vec.E() );
  }
  inline std::ostream& operator << ( std::ostream &os,
				     const LorentzVector &vec ) {
    os << "[ "
       << vec.Px() << ", "
       << vec.Py() << ", "
       << vec.Pz() << ", "
       << vec.E() << " ]";
    return os;
  }

}

#endif
