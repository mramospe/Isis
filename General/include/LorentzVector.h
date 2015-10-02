///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //       
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 29/07/2015                                                      //
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

#include "Vector3.h"


//_______________________________________________________________________________

namespace General {

  class LorentzVector {

  public:

    // Constructor
    LorentzVector();
    LorentzVector( double px, double py, double pz, double pe );
    LorentzVector( Vector3 vec, double pe );
    LorentzVector( const LorentzVector &vec );

    // Destructor
    ~LorentzVector();

    // Methods
    inline double   Angle( LorentzVector vec );
    double          ArmAlpha( LorentzVector vec1, LorentzVector vec2 );
    double          ArmPt( LorentzVector vec );
    double          ArmPl( LorentzVector vec );
    inline double   Beta();
    inline Vector3  VBeta();
    inline double   CosAngle( LorentzVector vec );
    inline double   CosAngle( Vector3 vec );
    double          CosHelAngle( LorentzVector vec, Vector3 dir );
    inline double   CosHelAngle( LorentzVector vec, LorentzVector dir );
    inline double   CosPhi();
    inline double   CosTheta();
    inline double   Gamma();
    inline Vector3  GetLong( LorentzVector vec );
    inline Vector3  GetLong( Vector3 vec );
    inline Vector3  GetTran( LorentzVector vec );
    inline Vector3  GetTran( Vector3 vec );
    inline double   HelAngle( LorentzVector vec, Vector3 dir );
    inline double   HelAngle( LorentzVector vec, LorentzVector dir );
    LorentzVector   LorentzTransf( LorentzVector vec );
    inline double   Mass();
    inline double   Mass2();
    inline double   Phi();
    inline double   P();
    double          PseudoRapidity();
    inline double   Pt();
    inline double   Rapidity();
    void            RotateX( double angle );
    void            RotateY( double angle );
    void            RotateZ( double angle );
    inline void     SetPx( double val );
    inline void     SetPy( double val );
    inline void     SetPz( double val );
    inline void     SetE( double val );
    inline void     SetPxPyPzE( double px, double py, double pz, double pe );
    inline void     SetPxPyPzM( double px, double py, double pz, double m );
    inline double   Theta();
    inline Vector3  Unitary();
    inline double   Px() const;
    inline double*  PathToPx();
    inline double   Py() const;
    inline double*  PathToPy();
    inline double   Pz() const;
    inline double*  PathToPz();
    inline double   E() const;
    inline double*  PathToE();
    inline Vector3  Momentum() const;
    inline Vector3* PathToMomentum();

    // Operators
    inline LorentzVector& operator += ( LorentzVector vec );
    inline LorentzVector& operator -= ( LorentzVector vec );
    inline LorentzVector  operator +  ( LorentzVector vec );
    inline LorentzVector  operator -  ( LorentzVector vec );
    inline LorentzVector  operator /  ( double val  );

  protected:
    
    // Attributes
    Vector3 fP;
    double  fE;
    
  };

  //______________________________
  // INLINE METHODS

  // Gets the angle between two vectors
  inline double   LorentzVector::Angle( LorentzVector vec ) {
    return fP.Angle( vec.fP );
  }
  // Gets the value of beta for the particle
  inline double   LorentzVector::Beta()     { return fP.Mod()/fE; }
  // Gets the vector concerning the parameter beta
  inline Vector3  LorentzVector::VBeta()    { return fP/fE; }
  // Gets the cosine of the angle between two vectors
  inline double   LorentzVector::CosAngle( LorentzVector vec ) {
    return fP.CosAngle( vec.fP );
  }
  inline double   LorentzVector::CosAngle( Vector3 vec ) {
    return fP.CosAngle( vec );
  }
  // Gets the value of the cosine of the helicity angle of the particle <vec>
  // in the direction <dir>
  inline double   LorentzVector::CosHelAngle( LorentzVector vec, LorentzVector dir ) {
    return this -> CosHelAngle( vec, dir.Momentum() ); }
  // Gets the phi angle in the x, y, z coordinate system
  inline double   LorentzVector::CosPhi()   { return fP.CosPhi(); }
  // Gets the theta angle in the x, y, z coordinate system
  inline double   LorentzVector::CosTheta() { return fP.CosTheta(); }
  // Gets the value of gamma for the particle
  inline double   LorentzVector::Gamma()    { return fE/std::sqrt( fE*fE - fP.Mod2() ); }
  // Gets the longitudinal component of the given vector regarding to this one
  inline Vector3  LorentzVector::GetLong( LorentzVector vec ) {
    return fP.GetLong( vec.fP );
  }
  inline Vector3  LorentzVector::GetLong( Vector3 vec ) {
    return fP.GetLong( vec );
  }
  // Gets the transversal component of the given vector regarding to this one
  inline Vector3  LorentzVector::GetTran( LorentzVector vec ) {
    return fP.GetTran( vec.fP );
  }
  inline Vector3  LorentzVector::GetTran( Vector3 vec ) {
    return fP.GetTran( vec );
  }
  // Gets the value of the helicity angle of the particle <vec> in the direction <dir>
  inline double   LorentzVector::HelAngle( LorentzVector vec, Vector3 dir ) {
    return std::acos( this -> CosHelAngle( vec, dir ) ); }
  inline double   LorentzVector::HelAngle( LorentzVector vec, LorentzVector dir ) {
    return std::acos( this -> CosHelAngle( vec, dir ) ); }
  // Gets the mass of the particle
  inline double   LorentzVector::Mass()     { return std::sqrt( fE*fE - fP.Mod2() ); }
  // Gets the squared mass of the particle
  inline double   LorentzVector::Mass2()    { return fE*fE - fP.Mod2(); }
  // Gets the value of the phi angle in the x, y, z coordinate system
  inline double   LorentzVector::Phi()      { return fP.Phi(); }
  // Gets the value of the transverse momentum
  inline double   LorentzVector::Pt()       { return fP.Pt(); }
  // Methods to rotate this vector a given angle
  inline void     LorentzVector::RotateX( double angle ) { fP.RotateX( angle ); }
  inline void     LorentzVector::RotateY( double angle ) { fP.RotateY( angle ); }
  inline void     LorentzVector::RotateZ( double angle ) { fP.RotateZ( angle ); }
  // Methods to set the different components of the vector
  inline void     LorentzVector::SetPx( double val ) { fP.SetX( val ); }
  inline void     LorentzVector::SetPy( double val ) { fP.SetY( val ); }
  inline void     LorentzVector::SetPz( double val ) { fP.SetZ( val ); }
  inline void     LorentzVector::SetE( double val )  { fE = val;       }
  inline void     LorentzVector::SetPxPyPzE( double px,
					     double py,
					     double pz,
					     double pe ) {
    fP.SetXYZ( px, py, pz ); fE = pe;
  }
  inline void     LorentzVector::SetPxPyPzM( double px,
					     double py,
					     double pz,
					     double m ) {
    fP.SetXYZ( px, py, pz ); fE = std::sqrt( fP.Mod2() + m*m );
  }
  // Gets the value of the theta angle in the x, y, z coordinate system
  inline double   LorentzVector::Theta()    { return fP.Theta(); }
  // Gets the unitary vector
  inline Vector3  LorentzVector::Unitary()  { return fP.Unitary(); }
  // Gets the module of the momentum
  inline double   LorentzVector::P()        { return fP.Mod(); }
  // Methods to get the value or the path of the different components of the vector
  inline double   LorentzVector::Px() const { return fP.X();       }
  inline double*  LorentzVector::PathToPx()   { return fP.PathToX();   }
  inline double   LorentzVector::Py() const { return fP.Y();       }
  inline double*  LorentzVector::PathToPy()   { return fP.PathToY();   }
  inline double   LorentzVector::Pz() const { return fP.Z();       }
  inline double*  LorentzVector::PathToPz()   { return fP.PathToZ();   }
  inline double   LorentzVector::E() const  { return fE;           }
  inline double*  LorentzVector::PathToE()    { return &fE;          }
  inline Vector3  LorentzVector::Momentum() const { return fP; }
  inline Vector3* LorentzVector::PathToMomentum() { return &fP; }
  // Gets the rapidity of the particle
  inline double   LorentzVector::Rapidity() {
    return fE - fP.Z() != 0 ?
      0.5*std::log( ( fE + fP.Z() ) / ( fE - fP.Z() ) ) :
      std::numeric_limits<double>::max();
  }

  //______________________________
  // OPERATORS

  inline LorentzVector& LorentzVector::operator += ( LorentzVector vec ) {
    fP += vec.fP;
    fE += vec.fE;
    return *this;
  }
  inline LorentzVector& LorentzVector::operator -= ( LorentzVector vec ) {
    fP -= vec.fP;
    fE -= vec.fE;
    return *this;
  }
  inline LorentzVector LorentzVector::operator + ( LorentzVector vec ) {
    return LorentzVector( fP.X() + vec.Px(),
			  fP.Y() + vec.Py(),
			  fP.Z() + vec.Pz(),
			  fE     + vec.E() );
  }
  inline LorentzVector LorentzVector::operator - ( LorentzVector vec ) {
    return LorentzVector( fP.X() - vec.Px(),
			  fP.Y() - vec.Py(),
			  fP.Z() - vec.Pz(),
			  fE     - vec.E() );
  }
  inline std::ostream& operator << ( std::ostream &os,
				     LorentzVector vec ) {
      
    os << "[ "
       << vec.Px() << ", "
       << vec.Py() << ", "
       << vec.Pz() << ", "
       << vec.E() << " ]";

    return os;
  }

}

#endif
