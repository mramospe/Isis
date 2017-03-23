///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
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

namespace Isis {

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
    inline double angle( const LorentzVector &vec ) const;

    // Gets the Armenteros-Podolanski parameter alpha
    double armAlpha( const LorentzVector &vec1, const LorentzVector &vec2 ) const;

    // Gets the Armenteros-Podolanski transverse momentum
    double armPt( const LorentzVector &vec ) const;

    // Gets the Armenteros-Podolanski longitudinal momentum
    double armPl( const LorentzVector &vec ) const;

    // Gets the value of beta for the particle
    inline double beta() const;

    // Gets the vector concerning the parameter beta
    inline Vector vbeta() const;

    // Gets the cosine of the angle between two LorentzVectors
    inline double cosAngle( const LorentzVector &vec ) const;

    // Gets the cosine of the angle respect to a 3D vector
    inline double cosAngle( const Vector &vec ) const;

    // Calculates the value of the cosine of the helicity angle of the particle
    // given by <vec> in the direction <dir>
    double cosHelAngle( const LorentzVector &vec, const Vector &dir ) const;

    // Gets the value of the cosine of the helicity angle of the particle <vec>
    // in the direction <dir>
    inline double cosHelAngle( const LorentzVector &vec, const LorentzVector &dir ) const;
    
    // Gets the phi angle in the x, y, z coordinate system
    inline double cosPhi() const;

    // Gets the theta angle in the x, y, z coordinate system
    inline double cosTheta() const;

    // Gets the value of gamma for the particle
    inline double gamma() const;

    // Gets the longitudinal component of the given LorentzVector regarding to this one
    inline Vector getLong( const LorentzVector &vec ) const;

    // Gets the longitudinal component of the given 3D vector regarding to this one
    inline Vector getLong( const Vector &vec ) const;

    // Gets the transversal component of the given LorentzVector regarding to this one
    inline Vector getTran( const LorentzVector &vec ) const;

    // Gets the transversal component of the given 3D vector regarding to this one
    inline Vector getTran( const Vector &vec ) const;

    // Gets the value of the helicity angle of the particle <vec> in the direction <dir>
    inline double helAngle( const LorentzVector &vec, const Vector &dir ) const;

    // Gets the value of the helicity angle of the particle <vec> in the direction in <dir>
    inline double helAngle( const LorentzVector &vec, const LorentzVector &dir ) const;

    // Gets the transformed vector of the particle in the reference frame of the
    // class vector
    LorentzVector lorentzTransf( const LorentzVector &vec ) const;

    // Gets the mass of the particle
    inline double mass() const;

    // Gets the squared mass of the particle
    inline double mass2() const;

    // Gets the value of the phi angle in the x, y, z coordinate system
    inline double phi() const;

    // Gets the module of the momentum
    inline double P() const;

    // Gets the pseudorapidity of the particle
    double pseudoRapidity() const;

    // Gets the value of the transverse momentum
    inline double Pt() const;

    // Gets the rapidity of the particle
    inline double rapidity() const;

    // Methods to rotate this vector a given angle respect to the X axis
    void rotateX( const double &angle );

    // Methods to rotate this vector a given angle respect to the Y axis
    void rotateY( const double &angle );

    // Methods to rotate this vector a given angle respect to the Z axis
    void rotateZ( const double &angle );

    // Set the mass
    inline void setM( const double &val );

    // Set the momentum on the X direction
    inline void setPx( const double &val );

    // Set the momentum on the Y direction
    inline void setPy( const double &val );

    // Set the momentum on the Y direction
    inline void setPz( const double &val );

    // Set the energy
    inline void setE( const double &val );

    // Set the four coordinates of this vector
    inline void setPxPyPzE( const double &px,
			    const double &py,
			    const double &pz,
			    const double &pe );

    // Set the three spatial coordinates and the energy using the mass
    inline void setPxPyPzM( const double &px,
			    const double &py,
			    const double &pz,
			    const double &m );

    // Gets the value of the theta angle in the x, y, z coordinate system
    inline double theta() const;

    // Gets the unitary vector
    inline Vector unitary() const;

    // Get the momentum on the X direction
    inline double Px() const;

    // Get path of the momentum on the X direction
    inline double* pathToPx();

    // Get the momentum on the Y direction
    inline double Py() const;

    // Get path of the momentum on the Y direction
    inline double* pathToPy();

    // Get the momentum on the Z direction
    inline double Pz() const;

    // Get path of the momentum on the Z direction
    inline double* pathToPz();

    // Get the energy
    inline double E() const;

    // Get path of the energy
    inline double* pathToE();

    // Get the vector associated with the momentum
    inline Vector momentum() const;

    // Get path to the vector associated with the momentum
    inline Vector* pathToMomentum();

    // Directly add the values from another vector
    inline LorentzVector& operator += ( const LorentzVector &vec );

    // Directly substract the values from another vector
    inline LorentzVector& operator -= ( const LorentzVector &vec );

    // Add two vectors
    inline LorentzVector operator + ( const LorentzVector &vec ) const;

    // Substract two vectors
    inline LorentzVector operator - ( const LorentzVector &vec ) const;

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
  inline double LorentzVector::angle( const LorentzVector &vec ) const {

    return fP.angle( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::beta() const { return fP.mod()/fE; }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::vbeta() const { return fP/fE; }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::cosAngle( const LorentzVector &vec ) const {
    
    return fP.cosAngle( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::cosAngle( const Vector &vec ) const {

    return fP.cosAngle( vec );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::cosHelAngle( const LorentzVector &vec,
					    const LorentzVector &dir ) const {
    return this->cosHelAngle( vec, dir.momentum() );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::cosPhi() const { return fP.cosPhi(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::cosTheta() const { return fP.cosTheta(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::gamma() const {

    return fE/std::sqrt( fE*fE - fP.mod2() );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::getLong( const LorentzVector &vec ) const {

    return fP.getLong( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::getLong( const Vector &vec ) const {

    return fP.getLong( vec );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::getTran( const LorentzVector &vec ) const {
    
    return fP.getTran( vec.fP );
  }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::getTran( const Vector &vec ) const {
    
    return fP.getTran( vec );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::helAngle( const LorentzVector &vec, const Vector &dir ) const {
    
    return std::acos( this -> cosHelAngle( vec, dir ) );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::helAngle( const LorentzVector &vec, const LorentzVector &dir ) const {
    
    return std::acos( this -> cosHelAngle( vec, dir ) );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::mass() const { return std::sqrt( fE*fE - fP.mod2() ); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::mass2() const { return fE*fE - fP.mod2(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::phi() const { return fP.phi(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Pt() const { return fP.modT(); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::rotateX( const double &angle ) { fP.rotateX( angle ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::rotateY( const double &angle ) { fP.rotateY( angle ); }
  
  //_______________________________________________________________________________
  //
  inline void LorentzVector::rotateZ( const double &angle ) { fP.rotateZ( angle ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setM( const double &val ) {
    
    fE = std::sqrt( fP.mod2() + val*val );
  }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setPx( const double &val ) { fP.setX( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setPy( const double &val ) { fP.setY( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setPz( const double &val ) { fP.setZ( val ); }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setE( const double &val ) { fE = val; }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setPxPyPzE( const double &px,
					 const double &py,
					 const double &pz,
					 const double &pe ) {
    fP.setXYZ( px, py, pz ); fE = pe;
  }

  //_______________________________________________________________________________
  //
  inline void LorentzVector::setPxPyPzM( const double &px,
					 const double &py,
					 const double &pz,
					 const double &m ) {
    fP.setXYZ( px, py, pz ); fE = std::sqrt( fP.mod2() + m*m );
  }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::theta() const { return fP.theta(); }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::unitary() const { return fP.unitary(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::P() const { return fP.mod(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Px() const { return fP.X(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::pathToPx() { return fP.pathToX(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Py() const { return fP.Y(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::pathToPy() { return fP.pathToY(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::Pz() const { return fP.Z(); }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::pathToPz() { return fP.pathToZ(); }

  //_______________________________________________________________________________
  //
  inline double LorentzVector::E() const { return fE; }

  //_______________________________________________________________________________
  //
  inline double* LorentzVector::pathToE() { return &fE; }

  //_______________________________________________________________________________
  //
  inline Vector LorentzVector::momentum() const { return fP; }

  //_______________________________________________________________________________
  //
  inline Vector* LorentzVector::pathToMomentum() { return &fP; }

  //_______________________________________________________________________________
  //
  inline double   LorentzVector::rapidity() const {
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
