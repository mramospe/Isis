///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //       
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 28/07/2015                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class Vector3, that allows to compute and perform operations  //
//  that concern three-dimensional vectors.                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __VECTOR3__
#define __VECTOR3__

#include <iostream>
#include <cmath>


//_______________________________________________________________________________

namespace General {

  class Vector3 {

  public:
    
    // Constructors
    Vector3();
    Vector3( double x, double y, double z );
    Vector3( const Vector3 &vec );

    // Destructor
    ~Vector3();
    
    // Methods
    inline double  Angle( Vector3 vec );
    inline double  CosAngle( Vector3 vec );
    inline double  CosPhi();
    inline double  CosTheta();
    inline Vector3 Cross( Vector3 vec );
    inline double  Dot( Vector3 vec );
    Vector3        GetLong( Vector3 vec );
    Vector3        GetTran( Vector3 vec );
    inline double  Mod();
    inline double  Mod2();
    double         Phi();
    inline double  Pt();
    void           RotateX( double angle );
    void           RotateY( double angle );
    void           RotateZ( double angle );
    inline void    SetX( double val );
    inline void    SetY( double val );
    inline void    SetZ( double val );
    inline void    SetXYZ( double x, double y, double z );
    inline double  SinPhi();
    inline double  SinTheta();
    double         Theta();
    Vector3        Unitary();

    inline double  X() const;
    inline double* PathToX();
    inline double  Y() const;
    inline double* PathToY();
    inline double  Z() const;
    inline double* PathToZ();

    // Operators
    inline Vector3& operator =  ( Vector3 vec );
    inline Vector3& operator *= ( double val  );
    inline Vector3& operator /= ( double val  );
    inline Vector3& operator += ( Vector3 vec );
    inline Vector3& operator -= ( Vector3 vec );
    inline Vector3  operator +  ( Vector3 vec );
    inline Vector3  operator -  ( Vector3 vec );
    inline Vector3  operator /  ( double val  );

    // Friend operators
    inline friend Vector3 operator * ( double val , Vector3 vec );
    inline friend Vector3 operator * ( Vector3 vec, double val  );

  protected:

    // Attributes
    double fX;
    double fY;
    double fZ;

  };

  //______________________________
  // INLINE METHODS

  // Gets the angle between two vectors
  inline double Vector3::Angle( Vector3 vec )   {
    return std::acos( this -> CosAngle( vec ) );
  }
  // Gets the cosine of the angle between two vectors
  inline double Vector3::CosAngle( Vector3 vec )   {
    return this -> Dot( vec )/( this -> Mod()*vec.Mod() );
  }
  // Gets the cosine of the phi angle in the x, y, z coordinate system
  inline double Vector3::CosPhi()   {
    return fX/std::sqrt( fX*fX + fY*fY );
  }
  // Gets the cosine of the theta angle in the x, y, z coordinate system
  inline double Vector3::CosTheta() {
    return fZ/( this -> Mod() );
  }
  // Performs the cross product with a given vector
  inline Vector3 Vector3::Cross( Vector3 vec ) {
    return Vector3( fY*vec.fZ - fZ*vec.fY,
		    fZ*vec.fX - fX*vec.fZ,
		    fX*vec.fY - fY*vec.fX );
  }
  // Computes the dot product with a given vector
  inline double Vector3::Dot( General::Vector3 vec ) {
    return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
  }
  // Gets the module of the vector
  inline double Vector3::Mod() {
    return std::sqrt( this -> Mod2() );
  }
  // Gets the squared value of the module of the vector
  inline double Vector3::Mod2() {
    return fX*fX + fY*fY + fZ*fZ;
  }
  // Gets the value of the transverse momentum
  inline double Vector3::Pt() {
    return std::sqrt( fX*fX + fY*fY );
  }
  // Methods to set the different parameters of the class
  inline void Vector3::SetX( double val ) { fX = val; }
  inline void Vector3::SetY( double val ) { fY = val; }
  inline void Vector3::SetZ( double val ) { fZ = val; }
  inline void Vector3::SetXYZ( double x, double y, double z ) {
    fX = x, fY = y; fZ = z;
  }
  // Gets the sine of the phi angle in the x, y, z coordinate system
  inline double Vector3::SinPhi() {
    return fY/( this -> Pt() );
  }
  // Gets the sine of the theta angle in the x, y, z coordinate system
  inline double Vector3::SinTheta() {
    return ( this -> Pt() )/( this -> Mod() );
  }
  // Methods to get the value or the path of the different components of the vector
  inline double  Vector3::X() const { return fX; }
  inline double* Vector3::PathToX()   { return &fX; }
  inline double  Vector3::Y() const { return fY; }
  inline double* Vector3::PathToY()   { return &fY; }
  inline double  Vector3::Z() const { return fZ; }
  inline double* Vector3::PathToZ()   { return &fZ; }

  //______________________________
  // OPERATORS
  
  inline Vector3& Vector3::operator = ( Vector3 vec ) {
    fX = vec.fX; fY = vec.fY; fZ = vec.fZ;
    return *this;
  }
  inline Vector3& Vector3::operator *= ( double val ) {
    fX *= val; fY *= val; fZ *= val;
    return *this;
  }
  inline Vector3& Vector3::operator /= ( double val ) {
    fX /= val; fY /= val; fZ /= val;
    return *this;
  }
  inline Vector3& Vector3::operator += ( Vector3 vec ) {
    fX += vec.fX; fY += vec.fX; fZ += vec.fZ;
    return *this;
  }
  inline Vector3& Vector3::operator -= ( Vector3 vec ) {
    fX -= vec.fX; fY -= vec.fX; fZ -= vec.fZ;
    return *this;
  }
  inline Vector3 Vector3::operator + ( Vector3 vec ) {
    return Vector3( fX + vec.fX, fY + vec.fY, fZ + vec.fZ );
  }
  inline Vector3 Vector3::operator - ( Vector3 vec ) {
    return Vector3( fX - vec.fX, fY - vec.fY, fZ - vec.fZ );
  }
  inline Vector3 Vector3::operator / ( double val ) {
    return General::Vector3( fX/val, fY/val, fZ/val );
  }
  inline Vector3 operator * ( double val , Vector3 vec ) {
    return Vector3( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline Vector3 operator * ( Vector3 vec, double val  ) {
    return Vector3( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline std::ostream& operator << ( std::ostream &os,
				     Vector3       vec ) {
      
    os << "[ " << vec.X() << ", " << vec.Y() << ", " << vec.Z() << " ]";

    return os;
  }

}

#endif
