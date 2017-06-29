///////////////////////////////////////////////////////////////////////////////////
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
//  Implements the class Vector, that allows to compute and perform operations
//  that concern three-dimensional vectors.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>
#include <cmath>


//_________________________________________________________________________

namespace isis {

  class Vector {

  public:
    
    // Main constructor
    Vector() { };

    // Constructor given the three components of the vector
    Vector( const double &x, const double &y, const double &z ) :
      fX(x),
      fY(y),
      fZ(z) { }

    // Copy constructor
    Vector( const Vector &vec ) :
      fX(vec.fX),
      fY(vec.fY),
      fZ(vec.fZ) { }

    // Destructor
    ~Vector() { };
    
    // Return the angle between two vectors
    inline double angle( const Vector &vec ) const {

      return std::acos(this->cosAngle(vec));
    }

    // Return the cosine of the angle between two vectors
    inline double cosAngle( const Vector &vec ) const {

      return this->dot(vec)/(this->mod()*vec.mod());
    }

    // Return the cosine of the phi angle in the x, y, z coordinate system
    inline double cosPhi() const {

      return fX/std::sqrt(fX*fX + fY*fY);
    }

    // Return the cosine of the theta angle in the x, y, z coordinate system
    inline double cosTheta() const {

      return fZ/( this->mod() );
    }

    // Performs the cross product with a given vector
    inline Vector cross( const Vector &vec ) const {

      return Vector( fY*vec.fZ - fZ*vec.fY,
		     fZ*vec.fX - fX*vec.fZ,
		     fX*vec.fY - fY*vec.fX );
    }

    // Computes the dot product with a given vector
    inline double dot( const Vector &vec ) const {
      
      return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
    }

    // Gets the longitudinal vector
    inline Vector getLong( const Vector &vec ) const {

      Vector u{this->unitary()};
      return (u.dot(vec))*u;
    }

    // Gets the transversal vector
    inline Vector getTran( const Vector &vec ) const {

      Vector u{this->unitary()};
      return (vec - (u.dot(vec))*u);
    }

    // Return the module of the vector
    inline double mod() const {

      return std::sqrt(this->mod2());
    }

    // Return the squared value of the module of the vector
    inline double mod2() const {

      return fX*fX + fY*fY + fZ*fZ;
    }

    // Return the phi angle in the X, Y, Z coordinate system
    inline double phi() const {

      double phi = std::atan2(this->sinPhi(), this->cosPhi());
      return phi > 0 ? phi : phi + 2*M_PI;
    }

    // Return the value of the transverse component
    inline double modT() const {

      return std::sqrt(fX*fX + fY*fY);
    }

    // Rotate the vector in the X axis
    void rotateX( const double &angle );

    // Rotate the vector in the Y axis
    void rotateY( const double &angle );

    // Rotate the vector in the Z axis
    void rotateZ( const double &angle );

    // Set the X coordinate
    inline void setX( const double &val ) {

      fX = val;
    }

    // Set the Y coordinate
    inline void setY( const double &val ) {

      fY = val;
    }

    // Set the Z coordinate
    inline void setZ( const double &val ) {

      fZ = val;
    }

    // Set all the coordinates
    inline void setXYZ( const double &x, const double &y, const double &z ) {

      fX = x;
      fY = y;
      fZ = z;
    }

    // Return the sine of the phi angle in the x, y, z coordinate system
    inline double sinPhi() const {

      return fY/(this->modT());
    }

    // Return the sine of the theta angle in the x, y, z coordinate system
    inline double sinTheta() const {

      return (this->modT())/(this->mod());
    }

    // Return the theta angle in the X, Y, Z coordinate system
    double theta() const;

    // Return the unitary vector
    Vector unitary() const;

    // Return the X coordinate
    inline double X() const {

      return fX;
    }

    // Return a pointer to the X coordinate
    inline double* pathToX() {

      return &fX;
    }

    // Return the Y coordinate
    inline double Y() const {

      return fY;
    }

    // Return a pointer to the Y coordinate
    inline double* pathToY() {

      return &fY;
    }

    // Return the Z coordinate
    inline double Z() const {

      return fZ;
    }

    // Return a pointer to the Y coordinate
    inline double* pathToZ() {

      return &fZ;
    }

    // Assign values using another vector
    inline Vector& operator = ( const Vector &vec ) {

      fX = vec.fX;
      fY = vec.fY;
      fZ = vec.fZ;
      
      return *this;
    }

    // Multiply by an scalar
    inline Vector& operator *= ( const double &val ) {

      fX *= val;
      fY *= val;
      fZ *= val;
      
      return *this;
    }

    // Divide by an scalar
    inline Vector& operator /= ( const double &val ) {

      fX /= val;
      fY /= val;
      fZ /= val;

      return *this;
    }

    // Add another vector
    inline Vector& operator += ( const Vector &vec ) {

      fX += vec.fX;
      fY += vec.fX;
      fZ += vec.fZ;
    
      return *this;
    }

    // Substract another vector
    inline Vector& operator -= ( const Vector &vec ) {

      fX -= vec.fX;
      fY -= vec.fX;
      fZ -= vec.fZ;
    
      return *this;
    }

    // Add two vectors
    inline Vector operator + ( const Vector &vec ) const {

      return Vector(fX + vec.fX, fY + vec.fY, fZ + vec.fZ);
    }

    // Substract two vectors
    inline Vector operator - ( const Vector &vec ) const {

      return Vector(fX - vec.fX, fY - vec.fY, fZ - vec.fZ);
    }

    // Divide by an scalar
    inline Vector operator / ( const double &val ) const {

      return Vector(fX/val, fY/val, fZ/val);
    }

    // Multiply scalar*vector
    inline friend Vector operator * ( const double &val , const Vector &vec ) {

      return Vector(vec.fX*val, vec.fY*val, vec.fZ*val);
    }

    // Multiply vector*scalar
    inline friend Vector operator * ( const Vector &vec, const double &val  ) {

      return val*vec;
    }

    // Return vector as a stream
    inline std::ostream& operator << ( std::ostream &os ) {

      os << "[ " << fX << ", " << fY << ", " << fZ << " ]";

      return os;
    }

  protected:

    // X position
    double fX;

    // Y position
    double fY;

    // Z position
    double fZ;

  };

}

#endif
