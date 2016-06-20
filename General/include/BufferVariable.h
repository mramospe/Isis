//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  General package                                                             //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  AUTHOR: Miguel Ramos Pernas                                                 //
//  e-mail: miguel.ramos.pernas@cern.ch                                         //
//                                                                              //
//  Last update: 20/06/2016                                                     //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  Description:                                                                //
//                                                                              //
//  Main class to store any kind of primitive data types. Different objects of  //
//  this type can be stored in the same container, while refering to different  //
//  data types. It results very useful when dealing with buffers were each      //
//  variable has a different type. Since it uses a < union >, the size of this  //
//  objects is defined as that of the larger type for the computer.             //
//                                                                              //
// ---------------------------------------------------------------------------- //
//////////////////////////////////////////////////////////////////////////////////


#ifndef BUFFER_VARIABLE
#define BUFFER_VARIABLE

#include <iostream>
#include <string>


//_______________________________________________________________________________

namespace General {

  class BufferVariable {

  public:
    
    // Constructors and destructor
    BufferVariable();
    BufferVariable( const char &type );
    ~BufferVariable();

    // Methods
    void*       PathToValue();
    std::string ToString() const;

    // Inline methods
    template<typename type> inline void ExtractValue( type &value );
    inline const char&                  GetType() const;
    inline void                         SetType( const char &type );
    template<typename type> inline void SetValue( const type &value );

  protected:

    // Attributes
    char                     fType;
    union {
      char                   fCharValue;
      unsigned char          fUCharValue;
      short int              fSIntValue;
      unsigned short int     fUSIntValue;
      unsigned int           fUIntValue;
      int                    fIntValue;
      float                  fFloatValue;
      double                 fDoubleValue;
      long long int          fLLIntValue;
      unsigned long long int fULLIntValue;
      bool                   fBoolValue;
    };

  };

  // ______________
  // INLINE METHODS
  
  // Sets the given variable as the value stored in the class
  template<typename type>
  void BufferVariable::ExtractValue( type &value ) {
    switch( fType ) {
    case 'B': value = fCharValue;   break;
    case 'b': value = fUCharValue;  break;
    case 'S': value = fSIntValue;   break;
    case 's': value = fUSIntValue;  break;
    case 'I': value = fIntValue;    break;
    case 'i': value = fUIntValue;   break;
    case 'F': value = fFloatValue;  break;
    case 'D': value = fDoubleValue; break;
    case 'L': value = fLLIntValue;  break;
    case 'l': value = fULLIntValue; break;
    case 'O': value = fBoolValue;   break;
    }
  }
  // Returns the type of the class
  inline const char& BufferVariable::GetType() const { return fType; }
  // Sets the type of this variable. The information stored in it is lost.
  inline void BufferVariable::SetType( const char &type ) { fType = type; }
  // Sets the value stored in the class as that of the given variable
  template<typename type>
  void BufferVariable::SetValue( const type &value ) {
    switch( fType ) {
    case 'B': fCharValue   = value; break;
    case 'b': fUCharValue  = value; break;
    case 'S': fSIntValue   = value; break;
    case 's': fUSIntValue  = value; break;
    case 'I': fIntValue    = value; break;
    case 'i': fUIntValue   = value; break;
    case 'F': fFloatValue  = value; break;
    case 'D': fDoubleValue = value; break;
    case 'L': fLLIntValue  = value; break;
    case 'l': fULLIntValue = value; break;
    case 'O': fBoolValue   = value; break;
    }
  }
  
  // ____________________
  // NON-MEMBER OPERATOR
  std::ostream& operator << ( std::ostream &os, const BufferVariable &var );

}

#endif
