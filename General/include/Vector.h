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
//  Implements the class Vector, that allows to compute and perform operations   //
//  that concern three-dimensional vectors.                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>
#include <cmath>


//_______________________________________________________________________________

namespace General {

  class Vector {

  public:
    
    // Constructors
    Vector();
    Vector( const double &x, const double &y, const double &z );
    Vector( const Vector &vec );

    // Destructor
    ~Vector();
    
    // Methods
    inline double  Angle( const Vector &vec ) const;
    inline double  CosAngle( const Vector &vec ) const;
    inline double  CosPhi() const;
    inline double  CosTheta() const;
    inline Vector  Cross( const Vector &vec ) const;
    inline double  Dot( const Vector &vec ) const;
    Vector         GetLong( const Vector &vec ) const;
    Vector         GetTran( const Vector &vec ) const;
    inline double  Mod() const;
    inline double  Mod2() const;
    double         Phi() const;
    inline double  Pt() const;
    void           RotateX( const double &angle );
    void           RotateY( const double &angle );
    void           RotateZ( const double &angle );
    inline void    SetX( const double &val );
    inline void    SetY( const double &val );
    inline void    SetZ( const double &val );
    inline void    SetXYZ( const double &x, const double &y, const double &z );
    inline double  SinPhi() const;
    inline double  SinTheta() const;
    double         Theta() const;
    Vector         Unitary() const;

    inline double  X() const;
    inline double* PathToX();
    inline double  Y() const;
    inline double* PathToY();
    inline double  Z() const;
    inline double* PathToZ();

    // Operators
    inline Vector& operator =  ( const Vector &vec );
    inline Vector& operator *= ( const double &val );
    inline Vector& operator /= ( const double &val );
    inline Vector& operator += ( const Vector &vec );
    inline Vector& operator -= ( const Vector &vec );
    inline Vector  operator +  ( const Vector &vec ) const;
    inline Vector  operator -  ( const Vector &vec ) const;
    inline Vector  operator /  ( const double &val ) const;

    // Friend operators
    inline friend Vector operator * ( const double &val , const Vector &vec );
    inline friend Vector operator * ( const Vector &vec, const double &val  );

  protected:

    // Attributes
    double fX;
    double fY;
    double fZ;

  };

  //______________________________
  // INLINE METHODS

  // Gets the angle between two vectors
  inline double Vector::Angle( const Vector &vec ) const {
    return std::acos( this -> CosAngle( vec ) );
  }
  // Gets the cosine of the angle between two vectors
  inline double Vector::CosAngle( const Vector &vec ) const {
    return this -> Dot( vec )/( this -> Mod()*vec.Mod() );
  }
  // Gets the cosine of the phi angle in the x, y, z coordinate system
  inline double Vector::CosPhi() const {
    return fX/std::sqrt( fX*fX + fY*fY );
  }
  // Gets the cosine of the theta angle in the x, y, z coordinate system
  inline double Vector::CosTheta() const {
    return fZ/( this -> Mod() );
  }
  // Performs the cross product with a given vector
  inline Vector Vector::Cross( const Vector &vec ) const {
    return Vector( fY*vec.fZ - fZ*vec.fY,
		    fZ*vec.fX - fX*vec.fZ,
		    fX*vec.fY - fY*vec.fX );
  }
  // Computes the dot product with a given vector
  inline double Vector::Dot( const General::Vector &vec ) const {
    return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
  }
  // Gets the module of the vector
  inline double Vector::Mod() const {
    return std::sqrt( this -> Mod2() );
  }
  // Gets the squared value of the module of the vector
  inline double Vector::Mod2() const {
    return fX*fX + fY*fY + fZ*fZ;
  }
  // Gets the value of the transverse momentum
  inline double Vector::Pt() const {
    return std::sqrt( fX*fX + fY*fY );
  }
  // Methods to set the different parameters of the class
  inline void Vector::SetX( const double &val ) { fX = val; }
  inline void Vector::SetY( const double &val ) { fY = val; }
  inline void Vector::SetZ( const double &val ) { fZ = val; }
  inline void Vector::SetXYZ( const double &x, const double &y, const double &z ) {
    fX = x, fY = y; fZ = z;
  }
  // Gets the sine of the phi angle in the x, y, z coordinate system
  inline double Vector::SinPhi() const {
    return fY/( this -> Pt() );
  }
  // Gets the sine of the theta angle in the x, y, z coordinate system
  inline double Vector::SinTheta() const {
    return ( this -> Pt() )/( this -> Mod() );
  }
  // Methods to get the value or the path of the different components of the vector
  inline double  Vector::X() const { return fX; }
  inline double* Vector::PathToX() { return &fX; }
  inline double  Vector::Y() const { return fY; }
  inline double* Vector::PathToY() { return &fY; }
  inline double  Vector::Z() const { return fZ; }
  inline double* Vector::PathToZ() { return &fZ; }

  //______________________________
  // OPERATORS
  
  inline Vector& Vector::operator = ( const Vector &vec ) {
    fX = vec.fX; fY = vec.fY; fZ = vec.fZ;
    return *this;
  }
  inline Vector& Vector::operator *= ( const double &val ) {
    fX *= val; fY *= val; fZ *= val;
    return *this;
  }
  inline Vector& Vector::operator /= ( const double &val ) {
    fX /= val; fY /= val; fZ /= val;
    return *this;
  }
  inline Vector& Vector::operator += ( const Vector &vec ) {
    fX += vec.fX; fY += vec.fX; fZ += vec.fZ;
    return *this;
  }
  inline Vector& Vector::operator -= ( const Vector &vec ) {
    fX -= vec.fX; fY -= vec.fX; fZ -= vec.fZ;
    return *this;
  }
  inline Vector Vector::operator + ( const Vector &vec ) const {
    return Vector( fX + vec.fX, fY + vec.fY, fZ + vec.fZ );
  }
  inline Vector Vector::operator - ( const Vector &vec ) const {
    return Vector( fX - vec.fX, fY - vec.fY, fZ - vec.fZ );
  }
  inline Vector Vector::operator / ( const double &val ) const {
    return General::Vector( fX/val, fY/val, fZ/val );
  }
  inline Vector operator * ( const double &val , const Vector &vec ) {
    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline Vector operator * ( const Vector &vec, const double &val  ) {
    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline std::ostream& operator << ( std::ostream &os,
				     const Vector &vec ) {
    os << "[ " << vec.X() << ", " << vec.Y() << ", " << vec.Z() << " ]";
    return os;
  }

}

#endif
