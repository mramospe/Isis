///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 29/06/2017
//
// ------------------------------------------------------------------------
//
//  Description:
//
//  Implements the LorentzVector class, that allows to compute and perform
//  operations concerning the 4-vector physics of particles.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef LORENTZ_VECTOR
#define LORENTZ_VECTOR

#include <limits>

#include "Vector.h"


//_______________________________________________________________________________

namespace isis {

  class LorentzVector {

  public:

    // Main constructor
    LorentzVector();

    // Constructor given all the coordinates
    LorentzVector( const double &px,
		   const double &py,
		   const double &pz,
		   const double &pe ) :
      fP(px, py, pz), fE(pe) { }

    // Contructor given the momentum and the energy
    LorentzVector( const Vector &vec, const double &pe ) :
      fP(vec), fE(pe) { }

    // Copy constructor
    LorentzVector( const LorentzVector &vec ) :
      fP(vec.fP), fE(vec.fE) { }

    // Destructor
    ~LorentzVector() { };

    // Gets the Armenteros-Podolanski parameter alpha
    double armAlpha( const LorentzVector &vec1,
		     const LorentzVector &vec2 ) const;

    // Gets the Armenteros-Podolanski transverse momentum
    double armPt( const LorentzVector &vec ) const;

    // Gets the Armenteros-Podolanski longitudinal momentum
    double armPl( const LorentzVector &vec ) const;

    // Gets the value of beta for the particle
    inline double beta() const {

      return fP.mod()/fE;
    }

    // Gets the vector concerning the parameter beta
    inline Vector vbeta() const {

      return fP/fE;
    }

    // Calculates the value of the cosine of the helicity angle of the particle
    // given by <vec> in the direction <dir>
    double cosHelAngle( const LorentzVector &vec, const Vector &dir ) const;
    
    // Gets the value of gamma for the particle
    inline double gamma() const {

      return fE/std::sqrt(fE*fE - fP.mod2());
    }

    // Gets the value of the helicity angle of the particle <vec> in the direction <dir>
    inline double helAngle( const LorentzVector &vec, const Vector &dir ) const {

      return std::acos(this->cosHelAngle(vec, dir));
    }

    // Gets the transformed vector of the particle in the reference frame of the
    // class vector
    LorentzVector lorentzTransf( const LorentzVector &vec ) const;

    // Gets the mass of the particle
    inline double mass() const {

      return std::sqrt(this->mass());
    }

    // Gets the squared mass of the particle
    inline double mass2() const {

      return fE*fE - fP.mod2();
    }

    // Gets the pseudorapidity of the particle
    double pseudoRapidity() const;

    // Gets the rapidity of the particle
    inline double rapidity() const {

      return fE - fP.Z() != 0 ?
	0.5*std::log((fE + fP.Z())/(fE - fP.Z())) :
	std::numeric_limits<double>::max();
    }

    // Set the mass
    inline void setM( const double &val ) {

      fE = std::sqrt(fP.mod2() + val*val);
    }

    // Set the energy
    inline void setE( const double &val ) {

      fE = val;
    }

    // Set the four coordinates of this vector
    inline void setPxPyPzE( const double &px,
			    const double &py,
			    const double &pz,
			    const double &pe ) {

      fP.setXYZ( px, py, pz );
      fE = pe;
    }

    // Set the three spatial coordinates and the energy using the mass
    inline void setPxPyPzM( const double &px,
			    const double &py,
			    const double &pz,
			    const double &m ) {

      fP.setXYZ(px, py, pz);
      fE = std::sqrt(fP.mod2() + m*m);
    }

    // Get the energy
    inline double E() const {

      return fE;
    }

    // Get path of the energy
    inline double* pathToE() {

      return &fE;
    }

    // Get the vector associated with the momentum
    inline Vector momentum() const {
      
      return fP;
    }

    // Get path to the vector associated with the momentum
    inline Vector* pathToMomentum() {

      return &fP;
    }

    // Directly add the values from another vector
    inline LorentzVector& operator += ( const LorentzVector &vec ) {

      fP += vec.fP;
      fE += vec.fE;
      return *this;
    }

    // Directly substract the values from another vector
    inline LorentzVector& operator -= ( const LorentzVector &vec ) {

      fP -= vec.fP;
      fE -= vec.fE;
      return *this;
    }

    // Add two vectors
    inline LorentzVector operator + ( const LorentzVector &vec ) const {

      return LorentzVector( fP + vec.fP,
			    fE + vec.fE );
    }

    // Substract two vectors
    inline LorentzVector operator - ( const LorentzVector &vec ) const {

      return LorentzVector( fP - vec.fP,
			    fE - vec.fE );
    }

    // Represent this vector as a stream
    inline std::ostream& operator << ( std::ostream &os ) const {

      os << "[ "
	 << fP.X() << ", "
	 << fP.Y() << ", "
	 << fP.Z() << ", "
	 << fE << " ]";
      
      return os;
    }

  protected:
    
    // 3D vector
    Vector fP;

    // Energy
    double fE;
    
  };

}

#endif
