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
    inline double Angle( const Vector &vec ) const;

    // Return the cosine of the angle between two vectors
    inline double CosAngle( const Vector &vec ) const;

    // Return the cosine of the phi angle in the x, y, z coordinate system
    inline double CosPhi() const;

    // Return the cosine of the theta angle in the x, y, z coordinate system
    inline double CosTheta() const;

    // Performs the cross product with a given vector
    inline Vector Cross( const Vector &vec ) const;

    // Computes the dot product with a given vector
    inline double Dot( const Vector &vec ) const;

    // Gets the longitudinal vector
    Vector GetLong( const Vector &vec ) const;

    // Gets the transversal vector
    Vector GetTran( const Vector &vec ) const;

    // Return the module of the vector
    inline double Mod() const;

    // Return the squared value of the module of the vector
    inline double Mod2() const;

    // Return the phi angle in the X, Y, Z coordinate system
    double Phi() const;

    // Return the value of the transverse component
    inline double ModT() const;

    // Rotate the vector in the X axis
    void RotateX( const double &angle );

    // Rotate the vector in the Y axis
    void RotateY( const double &angle );

    // Rotate the vector in the Z axis
    void RotateZ( const double &angle );

    // Set the X coordinate
    inline void SetX( const double &val );

    // Set the Y coordinate
    inline void SetY( const double &val );

    // Set the Z coordinate
    inline void SetZ( const double &val );

    // Set all the coordinates
    inline void SetXYZ( const double &x, const double &y, const double &z );

    // Return the sine of the phi angle in the x, y, z coordinate system
    inline double SinPhi() const;

    // Return the sine of the theta angle in the x, y, z coordinate system
    inline double SinTheta() const;

    // Return the theta angle in the X, Y, Z coordinate system
    double Theta() const;

    // Return the unitary vector
    Vector Unitary() const;

    // Return the X coordinate
    inline double X() const;

    // Return a pointer to the X coordinate
    inline double* PathToX();

    // Return the Y coordinate
    inline double Y() const;

    // Return a pointer to the Y coordinate
    inline double* PathToY();

    // Return the Z coordinate
    inline double Z() const;

    // Return a pointer to the Y coordinate
    inline double* PathToZ();

    // Assign values using another vector
    inline Vector& operator =  ( const Vector &vec );

    // Multiply by an scalar
    inline Vector& operator *= ( const double &val );

    // Divide by an scalar
    inline Vector& operator /= ( const double &val );

    // Add another vector
    inline Vector& operator += ( const Vector &vec );

    // Substract another vector
    inline Vector& operator -= ( const Vector &vec );

    // Add two vectors
    inline Vector operator +  ( const Vector &vec ) const;

    // Substract two vectors
    inline Vector operator -  ( const Vector &vec ) const;

    // Divide by an scalar
    inline Vector operator /  ( const double &val ) const;

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
  inline double Vector::Angle( const Vector &vec ) const {
    return std::acos( this->CosAngle( vec ) );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::CosAngle( const Vector &vec ) const {
    return this->Dot( vec )/( this->Mod()*vec.Mod() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::CosPhi() const {
    return fX/std::sqrt( fX*fX + fY*fY );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::CosTheta() const {
    return fZ/( this->Mod() );
  }

  //_______________________________________________________________________________
  //
  inline Vector Vector::Cross( const Vector &vec ) const {
    return Vector( fY*vec.fZ - fZ*vec.fY,
		   fZ*vec.fX - fX*vec.fZ,
		   fX*vec.fY - fY*vec.fX );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::Dot( const Vector &vec ) const {
    return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::Mod() const {
    return std::sqrt( this->Mod2() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::Mod2() const {
    return fX*fX + fY*fY + fZ*fZ;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::ModT() const {
    return std::sqrt( fX*fX + fY*fY );
  }

  //_______________________________________________________________________________
  //
  inline void Vector::SetX( const double &val ) { fX = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::SetY( const double &val ) { fY = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::SetZ( const double &val ) { fZ = val; }

  //_______________________________________________________________________________
  //
  inline void Vector::SetXYZ( const double &x, const double &y, const double &z ) {
    fX = x, fY = y; fZ = z;
  }

  //_______________________________________________________________________________
  //
  inline double Vector::SinPhi() const {
    return fY/( this->ModT() );
  }

  //_______________________________________________________________________________
  //
  inline double Vector::SinTheta() const {
    return ( this->ModT() )/( this->Mod() );
  }

  //_______________________________________________________________________________
  //
  inline double  Vector::X() const { return fX; }

  //_______________________________________________________________________________
  //
  inline double* Vector::PathToX() { return &fX; }

  //_______________________________________________________________________________
  //
  inline double  Vector::Y() const { return fY; }

  //_______________________________________________________________________________
  //
  inline double* Vector::PathToY() { return &fY; }

  //_______________________________________________________________________________
  //
  inline double  Vector::Z() const { return fZ; }

  //_______________________________________________________________________________
  //
  inline double* Vector::PathToZ() { return &fZ; }

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
