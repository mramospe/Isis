//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  General package                                                             //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  AUTHOR: Miguel Ramos Pernas                                                 //
//  e-mail: miguel.ramos.pernas@cern.ch                                         //
//                                                                              //
//  Last update: 28/06/2016                                                     //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  Description:                                                                //
//                                                                              //
//  Main class to store any kind of primitive data types. Different objects of  //
//  this type can be stored in the same container, while refering to different  //
//  data types. It results very useful when dealing with buffers were each      //
//  variable has a different type.                                              //
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
    
    // Constructor and destructor
    BufferVariable( const char &type = '\0' );
    ~BufferVariable();

    // Methods
    std::string ToString() const;

    // Inline methods
    template<typename type> inline void ExtractValue( type &value );
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
  void BufferVariable::ExtractValue( type &value ) {
    switch( fType ) {
    case 'B': value = *static_cast<char*>                   ( fPath ); break;
    case 'b': value = *static_cast<unsigned char*>          ( fPath ); break;
    case 'S': value = *static_cast<short int*>              ( fPath ); break;
    case 's': value = *static_cast<unsigned short int*>     ( fPath ); break;
    case 'I': value = *static_cast<int*>                    ( fPath ); break;
    case 'i': value = *static_cast<unsigned int*>           ( fPath ); break;
    case 'F': value = *static_cast<float*>                  ( fPath ); break;
    case 'D': value = *static_cast<double*>                 ( fPath ); break;
    case 'L': value = *static_cast<long long int*>          ( fPath ); break;
    case 'l': value = *static_cast<unsigned long long int*> ( fPath ); break;
    case 'O': value = *static_cast<bool*>                   ( fPath ); break;
    default :
      std::cerr << "ERROR: The type of the buffer variable has not been specified yet" << std::endl;
      break;
    }
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
    switch( fType ) {
    case 'B': *static_cast<char*>                   ( fPath ) = value; break;
    case 'b': *static_cast<unsigned char*>          ( fPath ) = value; break;
    case 'S': *static_cast<short int*>              ( fPath ) = value; break;
    case 's': *static_cast<unsigned short int*>     ( fPath ) = value; break;
    case 'I': *static_cast<int*>                    ( fPath ) = value; break;
    case 'i': *static_cast<unsigned int*>           ( fPath ) = value; break;
    case 'F': *static_cast<float*>                  ( fPath ) = value; break;
    case 'D': *static_cast<double*>                 ( fPath ) = value; break;
    case 'L': *static_cast<long long int*>          ( fPath ) = value; break;
    case 'l': *static_cast<unsigned long long int*> ( fPath ) = value; break;
    case 'O': *static_cast<bool*>                   ( fPath ) = value; break;
    default :
      std::cerr << "ERROR: The type of the buffer variable has not been specified yet" << std::endl;
      break;
    }
  }
  
  // ____________________
  // NON-MEMBER OPERATOR
  std::ostream& operator << ( std::ostream &os, const BufferVariable &var );

}

#endif
