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
//
//  Description:
//
//  Implements the class Vector, that allows to compute and perform operations
//  that concern three-dimensional vectors.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>
#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  class Vector {

  public:
    
    // Main constructor
    Vector();

    // Constructor given the three components of the vector
    Vector( const double &x, const double &y, const double &z );

    // Copy constructor
    Vector( const Vector &vec );

    // Destructor
    ~Vector();
    
    // Return the angle between two vectors
    inline double angle( const Vector &vec ) const;

    // Return the cosine of the angle between two vectors
    inline double cosAngle( const Vector &vec ) const;

    // Return the cosine of the phi angle in the x, y, z coordinate system
    inline double cosPhi() const;

    // Return the cosine of the theta angle in the x, y, z coordinate system
    inline double cosTheta() const;

    // Performs the cross product with a given vector
    inline Vector cross( const Vector &vec ) const;

    // Computes the dot product with a given vector
    inline double dot( const Vector &vec ) const;

    // Gets the longitudinal vector
    Vector getLong( const Vector &vec ) const;

    // Gets the transversal vector
    Vector getTran( const Vector &vec ) const;

    // Return the module of the vector
    inline double mod() const;

    // Return the squared value of the module of the vector
    inline double mod2() const;

    // Return the phi angle in the X, Y, Z coordinate system
    double phi() const;

    // Return the value of the transverse component
    inline double modT() const;

    // Rotate the vector in the X axis
    void rotateX( const double &angle );

    // Rotate the vector in the Y axis
    void rotateY( const double &angle );

    // Rotate the vector in the Z axis
    void rotateZ( const double &angle );

    // Set the X coordinate
    inline void setX( const double &val );

    // Set the Y coordinate
    inline void setY( const double &val );

    // Set the Z coordinate
    inline void setZ( const double &val );

    // Set all the coordinates
    inline void setXYZ( const double &x, const double &y, const double &z );

    // Return the sine of the phi angle in the x, y, z coordinate system
    inline double sinPhi() const;

    // Return the sine of the theta angle in the x, y, z coordinate system
    inline double sinTheta() const;

    // Return the theta angle in the X, Y, Z coordinate system
    double theta() const;

    // Return the unitary vector
    Vector unitary() const;

    // Return the X coordinate
    inline double X() const;

    // Return a pointer to the X coordinate
    inline double* pathToX();

    // Return the Y coordinate
    inline double Y() const;

    // Return a pointer to the Y coordinate
    inline double* pathToY();

    // Return the Z coordinate
    inline double Z() const;

    // Return a pointer to the Y coordinate
    inline double* pathToZ();

    // Assign values using another vector
    inline Vector& operator = ( const Vector &vec );

    // Multiply by an scalar
    inline Vector& operator *= ( const double &val );

    // Divide by an scalar
    inline Vector& operator /= ( const double &val );

    // Add another vector
    inline Vector& operator += ( const Vector &vec );

    // Substract another vector
    inline Vector& operator -= ( const Vector &vec );

    // Add two vectors
    inline Vector operator + ( const Vector &vec ) const;

    // Substract two vectors
    inline Vector operator - ( const Vector &vec ) const;

    // Divide by an scalar
    inline Vector operator / ( const double &val ) const;

    // Multiply scalar*vector
    inline friend Vector operator * ( const double &val , const Vector &vec );

    // Multiply vector*scalar
    inline friend Vector operator * ( const Vector &vec, const double &val  );

    // Return vector as a stream
    inline std::ostream& operator << ( std::ostream &os );

  protected:

    // X position
    double fX;

    // Y position
    double fY;

    // Z position
    double fZ;

  };

  //_______________________________________________________________________________
  //
  inline double Vector::angle( const Vector &vec ) const {
    return std::acos( this->cosAngle( vec ) );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::cosAngle( const Vector &vec ) const {
    
    return this->dot( vec )/( this->mod()*vec.mod() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::cosPhi() const {
    
    return fX/std::sqrt( fX*fX + fY*fY );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::cosTheta() const {
    
    return fZ/( this->mod() );
  }

  //_______________________________________________________________________________
  //
  inline Vector Vector::cross( const Vector &vec ) const {
    
    return Vector( fY*vec.fZ - fZ*vec.fY,
		   fZ*vec.fX - fX*vec.fZ,
		   fX*vec.fY - fY*vec.fX );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::dot( const Vector &vec ) const {
    
    return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::mod() const {
    
    return std::sqrt( this->mod2() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::mod2() const {
    
    return fX*fX + fY*fY + fZ*fZ;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::modT() const {
    
    return std::sqrt( fX*fX + fY*fY );
  }

  //_______________________________________________________________________________
  //
  inline void Vector::setX( const double &val ) { fX = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::setY( const double &val ) { fY = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::setZ( const double &val ) { fZ = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::setXYZ( const double &x, const double &y, const double &z ) {
    fX = x, fY = y; fZ = z;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::sinPhi() const {
    return fY/( this->modT() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::sinTheta() const {
    
    return ( this->modT() )/( this->mod() );
  }

  //_______________________________________________________________________________
  //
  inline double  Vector::X() const { return fX; }

  //_______________________________________________________________________________
  //
  inline double* Vector::pathToX() { return &fX; }

  //_______________________________________________________________________________
  //
  inline double  Vector::Y() const { return fY; }

  //_______________________________________________________________________________
  //
  inline double* Vector::pathToY() { return &fY; }

  //_______________________________________________________________________________
  //
  inline double  Vector::Z() const { return fZ; }

  //_______________________________________________________________________________
  //
  inline double* Vector::pathToZ() { return &fZ; }

  //_______________________________________________________________________________
  //
  inline Vector& Vector::operator = ( const Vector &vec ) {

    fX = vec.fX; fY = vec.fY; fZ = vec.fZ;
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline Vector& Vector::operator *= ( const double &val ) {

    fX *= val; fY *= val; fZ *= val;
    
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline Vector& Vector::operator /= ( const double &val ) {

    fX /= val; fY /= val; fZ /= val;
    
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline Vector& Vector::operator += ( const Vector &vec ) {

    fX += vec.fX; fY += vec.fX; fZ += vec.fZ;
    
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline Vector& Vector::operator -= ( const Vector &vec ) {

    fX -= vec.fX; fY -= vec.fX; fZ -= vec.fZ;
    
    return *this;
  }

  //_______________________________________________________________________________
  //
  inline Vector Vector::operator + ( const Vector &vec ) const {

    return Vector( fX + vec.fX, fY + vec.fY, fZ + vec.fZ );
  }

  //_______________________________________________________________________________
  //
  inline Vector Vector::operator - ( const Vector &vec ) const {

    return Vector( fX - vec.fX, fY - vec.fY, fZ - vec.fZ );
  }

  //_______________________________________________________________________________
  //
  inline Vector Vector::operator / ( const double &val ) const {

    return Vector( fX/val, fY/val, fZ/val );
  }
  
  //_______________________________________________________________________________
  //
  inline Vector operator * ( const double &val , const Vector &vec ) {

    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }

  //_______________________________________________________________________________
  //
  inline Vector operator * ( const Vector &vec, const double &val  ) {
    
    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }

  //_______________________________________________________________________________
  //
  inline std::ostream& Vector::operator << ( std::ostream &os ) {
    
    os << "[ " << fX << ", " << fY << ", " << fZ << " ]";
    
    return os;
  }

}

#endif
