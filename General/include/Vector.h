///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 26/10/2015                                                      //
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
    Vector( double x, double y, double z );
    Vector( const Vector &vec );

    // Destructor
    ~Vector();
    
    // Methods
    inline double  Angle( Vector vec );
    inline double  CosAngle( Vector vec );
    inline double  CosPhi();
    inline double  CosTheta();
    inline Vector  Cross( Vector vec );
    inline double  Dot( Vector vec );
    Vector         GetLong( Vector vec );
    Vector         GetTran( Vector vec );
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
    Vector         Unitary();

    inline double  X() const;
    inline double* PathToX();
    inline double  Y() const;
    inline double* PathToY();
    inline double  Z() const;
    inline double* PathToZ();

    // Operators
    inline Vector& operator =  ( Vector vec );
    inline Vector& operator *= ( double val  );
    inline Vector& operator /= ( double val  );
    inline Vector& operator += ( Vector vec );
    inline Vector& operator -= ( Vector vec );
    inline Vector  operator +  ( Vector vec );
    inline Vector  operator -  ( Vector vec );
    inline Vector  operator /  ( double val  );

    // Friend operators
    inline friend Vector operator * ( double val , Vector vec );
    inline friend Vector operator * ( Vector vec, double val  );

  protected:

    // Attributes
    double fX;
    double fY;
    double fZ;

  };

  //______________________________
  // INLINE METHODS

  // Gets the angle between two vectors
  inline double Vector::Angle( Vector vec )   {
    return std::acos( this -> CosAngle( vec ) );
  }
  // Gets the cosine of the angle between two vectors
  inline double Vector::CosAngle( Vector vec )   {
    return this -> Dot( vec )/( this -> Mod()*vec.Mod() );
  }
  // Gets the cosine of the phi angle in the x, y, z coordinate system
  inline double Vector::CosPhi()   {
    return fX/std::sqrt( fX*fX + fY*fY );
  }
  // Gets the cosine of the theta angle in the x, y, z coordinate system
  inline double Vector::CosTheta() {
    return fZ/( this -> Mod() );
  }
  // Performs the cross product with a given vector
  inline Vector Vector::Cross( Vector vec ) {
    return Vector( fY*vec.fZ - fZ*vec.fY,
		    fZ*vec.fX - fX*vec.fZ,
		    fX*vec.fY - fY*vec.fX );
  }
  // Computes the dot product with a given vector
  inline double Vector::Dot( General::Vector vec ) {
    return fX*vec.fX + fY*vec.fY + fZ*vec.fZ;
  }
  // Gets the module of the vector
  inline double Vector::Mod() {
    return std::sqrt( this -> Mod2() );
  }
  // Gets the squared value of the module of the vector
  inline double Vector::Mod2() {
    return fX*fX + fY*fY + fZ*fZ;
  }
  // Gets the value of the transverse momentum
  inline double Vector::Pt() {
    return std::sqrt( fX*fX + fY*fY );
  }
  // Methods to set the different parameters of the class
  inline void Vector::SetX( double val ) { fX = val; }
  inline void Vector::SetY( double val ) { fY = val; }
  inline void Vector::SetZ( double val ) { fZ = val; }
  inline void Vector::SetXYZ( double x, double y, double z ) {
    fX = x, fY = y; fZ = z;
  }
  // Gets the sine of the phi angle in the x, y, z coordinate system
  inline double Vector::SinPhi() {
    return fY/( this -> Pt() );
  }
  // Gets the sine of the theta angle in the x, y, z coordinate system
  inline double Vector::SinTheta() {
    return ( this -> Pt() )/( this -> Mod() );
  }
  // Methods to get the value or the path of the different components of the vector
  inline double  Vector::X() const { return fX; }
  inline double* Vector::PathToX()   { return &fX; }
  inline double  Vector::Y() const { return fY; }
  inline double* Vector::PathToY()   { return &fY; }
  inline double  Vector::Z() const { return fZ; }
  inline double* Vector::PathToZ()   { return &fZ; }

  //______________________________
  // OPERATORS
  
  inline Vector& Vector::operator = ( Vector vec ) {
    fX = vec.fX; fY = vec.fY; fZ = vec.fZ;
    return *this;
  }
  inline Vector& Vector::operator *= ( double val ) {
    fX *= val; fY *= val; fZ *= val;
    return *this;
  }
  inline Vector& Vector::operator /= ( double val ) {
    fX /= val; fY /= val; fZ /= val;
    return *this;
  }
  inline Vector& Vector::operator += ( Vector vec ) {
    fX += vec.fX; fY += vec.fX; fZ += vec.fZ;
    return *this;
  }
  inline Vector& Vector::operator -= ( Vector vec ) {
    fX -= vec.fX; fY -= vec.fX; fZ -= vec.fZ;
    return *this;
  }
  inline Vector Vector::operator + ( Vector vec ) {
    return Vector( fX + vec.fX, fY + vec.fY, fZ + vec.fZ );
  }
  inline Vector Vector::operator - ( Vector vec ) {
    return Vector( fX - vec.fX, fY - vec.fY, fZ - vec.fZ );
  }
  inline Vector Vector::operator / ( double val ) {
    return General::Vector( fX/val, fY/val, fZ/val );
  }
  inline Vector operator * ( double val , Vector vec ) {
    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline Vector operator * ( Vector vec, double val  ) {
    return Vector( vec.fX*val, vec.fY*val, vec.fZ*val );
  }
  inline std::ostream& operator << ( std::ostream &os,
				     Vector        vec ) {
      
    os << "[ " << vec.X() << ", " << vec.Y() << ", " << vec.Z() << " ]";

    return os;
  }

}

#endif
