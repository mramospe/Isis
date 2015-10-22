///////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------- //
//                                                                               //
//  General package      			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR : Miguel Ramos Pernas		                                 //
//  e-mail: miguel.ramos.pernas@cern.ch		                                 //
//                                                                               //
//  Last Update: 22/10/2015			                                 //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:				                                 //
//						                                 //
//  Implements different functions. All functions inherit from < Functor >       //
//  class. If the class is created ( when no static method is used ) the         //
//  < Calculate > method has to perform the calculations to get the desired      //
//  variable. All the input and output variables are given to the class by       //
//  reference, while the static method takes them by value.                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __FUNCTORS__
#define __FUNCTORS__

#include "LorentzVector.h"


//_______________________________________________________________________________

namespace General {

  //______________________________________________________________________________
  // Virtual class to describe general class-functions with static methods
  class Functor {
    
  public:

    // Constructor and destructor
    Functor() { };
    ~Functor() { };

    // Method
    virtual void Calculate() { };
    
  };

  //______________________________________________________________________________
  // Functor to calculate the half-life multiplied by the speed of light
  class CTAU: public Functor {

  public:
    
    // Constructor and destructor
    CTAU( double &ctau, LorentzVector &TLV, Vector3 &OwnPV, Vector3 &EndV ) : 
      fCTAU( &ctau ),
      fTLV( &TLV ),
      fOwnPV( &OwnPV ),
      fEndV( &EndV ) { };
    ~CTAU() { };

    // Methods
    void          Calculate() { *fCTAU = this -> Eval( *fTLV, *fOwnPV, *fEndV ); }
    static double Eval( LorentzVector TLV, Vector3 OwnPV, Vector3 EndV );

  protected:
    double        *fCTAU;
    LorentzVector *fTLV;
    Vector3       *fOwnPV;
    Vector3       *fEndV;

  };

  //______________________________________________________________________________
  // Functor to calculate the determinant of three Vector3 variables
  class Det: public Functor {

  public:
    
    // Constructor and destructor
    Det( double &res, Vector3 &vec1, Vector3 &vec2, Vector3 &vec3 ) :
      fRes( &res ),
      fVec1( &vec1 ),
      fVec2( &vec2 ),
      fVec3( &vec3 ) { };
    ~Det() { };

    // Methods
    void          Calculate() { *fRes = this -> Eval( *fVec1, *fVec2, *fVec3 ); }
    static double Eval( Vector3 vec1, Vector3 vec2, Vector3 vec3 );

  protected:
    double  *fRes;
    Vector3 *fVec1;
    Vector3 *fVec2;
    Vector3 *fVec3;

  };

  //______________________________________________________________________________
  // Functor to calculate the impact parameter given the momentum, the primary
  // and the secondary vertex positions
  class IP: public Functor {

  public:
    
    // Constructor and destructor
    IP( double &ip, Vector3 &mom, Vector3 &pv, Vector3 &sv ) :
      fIP( &ip ),
      fMom( &mom ),
      fPV( &pv ),
      fSV( &sv ) { };
    ~IP() { };

    // Methods
    void          Calculate() { *fIP = this -> Eval( *fMom, *fPV, *fSV ); }
    static double Eval( Vector3 mom, Vector3 pv, Vector3 sv );

  protected:
    double  *fIP;
    Vector3 *fMom;
    Vector3 *fPV;
    Vector3 *fSV;

  };

  //______________________________________________________________________________
  // Functor to calculate the distance of closest approach between two particles
  class DOCA: public Functor {

  public:
    
    // Constructor and destructor
    DOCA( double &doca, Vector3 &vec1, Vector3 &pos1, Vector3 &vec2,Vector3 &pos2 ) :
      fDOCA( &doca ),
      fPos1( &pos1 ),
      fPos2( &pos2 ),
      fVec1( &vec1 ),
      fVec2( &vec2 ) { };
    ~DOCA() { };

    // Methods
    void          Calculate() { *fDOCA = this -> Eval( *fVec1, *fPos1, *fVec2, *fPos2 ); }
    static double Eval( Vector3 vec1, Vector3 pos1, Vector3 vec2, Vector3 pos2 );

  protected:
    double  *fDOCA;
    Vector3 *fPos1;
    Vector3 *fPos2;
    Vector3 *fVec1;
    Vector3 *fVec2;

  };

}

#endif
