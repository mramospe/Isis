//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Main class to store any kind of primitive data types. Different objects of
//  this type can be stored in the same container, while refering to different
//  data types. It results very useful when dealing with buffers were each
//  variable has a different type.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef BUFFER_VARIABLE
#define BUFFER_VARIABLE

#include <iostream>
#include <string>

#include "Messenger.h"
#include "ValueTypeDef.h"


//_______________________________________________________________________________

namespace Isis {

  class BufferVariable {

  public:
    
    // Constructor given the type as a character
    BufferVariable( const char &type = '\0' );

    // Destructor
    ~BufferVariable();

    // Converts this class to a string
    std::string ToString() const;

    // Sets the given variable as the value stored in the class
    template<typename type> inline void ExtractValue( type &value ) const;

    // Returns the type of the class
    inline const char& GetType() const;

    // Returns the void pointer to the value stored by this class
    inline void* PathToValue();

    // Sets the type of this variable. The information stored in it is lost.
    inline void SetType( const char &type );

    // Sets the value stored in the class as that of the given variable
    template<typename type> inline void SetValue( const type &value );

    // Convert to string and add it to the stream
    std::ostream& operator << ( std::ostream &os );

  protected:

    // Type of the variable stored
    char fType;

    // Pointer to the variable stored
    void *fPath;

  private:
    
    // Method to construct this class, given the type
    void Construct();

    // Method to delete the value stored
    void Delete();

  };

  //_______________________________________________________________________________
  //
  template<typename type>
  void BufferVariable::ExtractValue( type &value ) const {
    
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_GET_PTR_VALUE,
			  
			  IError << "The type of the buffer variable "
			  "has not been specified yet" << IEndMsg;
			  );
  }

  //_______________________________________________________________________________
  //
  inline const char& BufferVariable::GetType() const { return fType; }

  //_______________________________________________________________________________
  //
  inline void* BufferVariable::PathToValue() { return fPath; }

  //_______________________________________________________________________________
  //
  inline void BufferVariable::SetType( const char &type ) {
    
    this->Delete();
    fType = type;
    this->Construct();
  }

  //_______________________________________________________________________________
  //
  template<typename type>
  void BufferVariable::SetValue( const type &value ) {
    
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_SET_PTR_VALUE,
			  
			  IError << "The type of the buffer variable "
			  "has not been specified yet" << IEndMsg;
			  );
  }
  
  //_______________________________________________________________________________
  //
  inline std::ostream& BufferVariable::operator << ( std::ostream &os ) {
    
    os << this->ToString();
    return os;
  }

}

#endif
