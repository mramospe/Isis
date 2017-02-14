//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 14/02/2017
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

#include "ValueTypeDef.h"


//_______________________________________________________________________________

namespace General {

  class BufferVariable {

  public:
    
    // Constructor and destructor
    BufferVariable( const char &type = '\0' );
    ~BufferVariable();

    // Methods
    std::string ToString() const;

    // Inline methods
    template<typename type> inline void ExtractValue( type &value ) const;
    inline const char&                  GetType() const;
    inline void*                        PathToValue();
    inline void                         SetType( const char &type );
    template<typename type> inline void SetValue( const type &value );

  protected:

    // Attributes
    char  fType;
    void *fPath;

  private:
    
    // Methods
    void Construct();
    void Delete();

  };

  // ______________
  // INLINE METHODS
  
  // Sets the given variable as the value stored in the class
  template<typename type>
  void BufferVariable::ExtractValue( type &value ) const {
    
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_GET_PTR_VALUE,
			  
			  std::cerr << "ERROR: The type of the buffer variable "
			  "has not been specified yet" << std::endl;
			  );
  }
  
  // Returns the type of the class
  inline const char& BufferVariable::GetType() const { return fType; }

  // Returns the void pointer to the value stored by this class
  inline void* BufferVariable::PathToValue() { return fPath; }

  // Sets the type of this variable. The information stored in it is lost.
  inline void BufferVariable::SetType( const char &type ) {
    
    this -> Delete();
    fType = type;
    this -> Construct();
  }

  // Sets the value stored in the class as that of the given variable
  template<typename type>
  void BufferVariable::SetValue( const type &value ) {
    
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_SET_PTR_VALUE,
			  
			  std::cerr << "ERROR: The type of the buffer variable "
			  "has not been specified yet" << std::endl;
			  );
  }
  
  // ____________________
  // NON-MEMBER OPERATOR
  std::ostream& operator << ( std::ostream &os, const BufferVariable &var );

}

#endif
