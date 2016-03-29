///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 29/03/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  A set of different classes and functions to manage variables are defined     //
//  here. All these objects are used by the other classes in the Analysis        //
//  package, to generate hisograms, graphs, perform analysis in ranges, etc.     //
//  The functions < VariableType > and < ExtractValue > are linked together, in  //
//  such a way that the behaviour for the different types have to be the same.   //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef __VARIABLES__
#define __VARIABLES__

#include <iostream>
#include <string>
#include <typeinfo>


//_______________________________________________________________________________

namespace Analysis {

  //_______________________________________________________________________________
  // FUNCTION: VARIABLE TYPE
  //
  // This an auxiliar function needed by the different variable classes. It can be
  // used in two different ways: the first one is to call this function with no
  // argument < VariableType<type>() >, and the second one is calling it with the
  // address of a variable < VariableType( &var ) >. In both cases it will work
  // with the variable type, which is defined in the template call.
  template<typename type> char VariableType( const type *var = 0 ) {
    const std::type_info &tp = typeid( type );
    if ( tp == typeid( const char* ) )
      return 'C';
    else if ( tp == typeid( char ) )
      return 'B';
    else if ( tp == typeid( unsigned char ) )
      return 'b';
    else if ( tp == typeid( short int ) )
      return 'S';
    else if ( tp == typeid( unsigned short int ) )
      return 's';
    else if ( tp == typeid( int ) )
      return 'I';
    else if ( tp == typeid( unsigned int ) )
      return 'i';
    else if ( tp == typeid( float ) )
      return 'F';
    else if ( tp == typeid( double ) )
      return 'D';
    else if ( tp == typeid( long long int ) )
      return 'L';
    else if ( tp == typeid( unsigned long long int ) )
      return 'l';
    else if ( tp == typeid( bool ) )
      return 'O';
    else {
      std::cout << "WARNING: Type for one of the variables not known. Type id: " <<
	tp.name() << std::endl;
      return '\0';
    }
  }

  
  //_______________________________________________________________________________
  // CLASS: NAMED VARIABLE
  //
  // This class is the base class for all the different variable classes. It only
  // has a name, an expression and the value type. The first of these variables
  // is used as the identifier, while the second is that which will be computed.
  // The value type is necessary if one wants to use polimorphism with this class.

  class NamedVariable {
    
  public:

    // Constructors and destructor
    NamedVariable();
    NamedVariable( const std::string &name,
		   const std::string &expr = std::string(),
		   const char        &type = 'D' );
    virtual ~NamedVariable();

    // Inline methods
    inline const std::string BuildTitle();
    inline const std::string GetExpr() const;
    inline const std::string GetName() const;
    inline const char        GetType() const;
    inline void              SetExpr( const std::string &expr );
    inline void              SetName( const std::string &name );

  protected:

    // Attributes
    std::string fExpr;
    std::string fName;
    char        fType;
  };

  // ______________
  // INLINE METHODS

  // Constructs the title using the name and the type
  inline const std::string NamedVariable::BuildTitle() { return fName + '/' + fType; }
  // Returns the expression attached to the class
  inline const std::string NamedVariable::GetExpr() const { return fExpr; }
  // Returns the name of the variable
  inline const std::string NamedVariable::GetName() const { return fName; }
  // Returns the type of the variable
  inline const char NamedVariable::GetType() const { return fType; }
  // Sets the expression to the current string
  inline void NamedVariable::SetExpr( const std::string &expr ) { fExpr = expr; }
  // Sets a new name for the variable
  inline void NamedVariable::SetName( const std::string &name ) { fName = name; }

  
  //_______________________________________________________________________________
  // CLASS: BASIC VARIABLE
  //
  // This class defines the main variable object to store a value. It is meant to
  // be used with boolean, integer, float and double variables.

  template<typename type>
  class BasicVariable : public NamedVariable {

  public:

    // Constructors and destructor
    BasicVariable();
    BasicVariable( const std::string &name, const std::string &expr = std::string() );
    virtual ~BasicVariable();

    // Inline methods
    inline const type GetValue() const;
    inline type*      PathToValue();
    inline void       SetValue( const type &value );
    
  protected:

    // Attribute
    type fValue;
  };
  
  // ____________________________
  // CONSTRUCTORS AND DESTRUCTORS

  // Constructor with no arguments
  template<typename type>
  BasicVariable<type>::BasicVariable() : BasicVariable<type>( std::string(), std::string() ) { }

  // Constructor given the name and the expression of the variable. All the variables accepted by
  // the class TTree are considered.
  template<typename type>
  BasicVariable<type>::BasicVariable( const std::string &name, const std::string &expr ) :
    NamedVariable( name, expr, VariableType<type>() ) { }
    
  // Destructor
  template<typename type>
  BasicVariable<type>::~BasicVariable() { }

  // ______________
  // INLINE METHODS

  // Returns the current value of the variable
  template<typename type>
  inline const type BasicVariable<type>::GetValue() const { return fValue; }
  // Returns the path to the value of the class
  template<typename type>
  inline type* BasicVariable<type>::PathToValue() { return &fValue; }
  // Sets a new value for the variable
  template<typename type>
  inline void BasicVariable<type>::SetValue( const type &value ) { fValue = value; }


  //_______________________________________________________________________________
  // CLASS: VARIABLE
  //
  // This is the main variable class. It stores the name, expression, a value and
  // the limits for it. If it is used with boolean variables, one has to take into
  // account that, since the limits are taken as ">=" for the minimum value and
  // "<" for the maximum, when working with booleans a < 0 > is always considered
  // to be inside while < 1 > is always outside.
  
  template<typename type>
  class Variable : public BasicVariable<type> {

  public:

    // Constructors and destructor
    Variable();
    Variable( const std::string &name,
	      const std::string &expr,
	      const type        &min,
	      const type        &max,
	      const size_t      &ndiv = 100 );
    ~Variable();
    
    // Inline methods
    inline const type   GetMax() const;
    inline const type   GetMin() const;
    inline const size_t GetNdiv() const;
    inline const double GetStep( const bool &tolim = false ) const;
    inline bool         IsInside() const;
    inline bool         IsInside( const type &value ) const;
    inline bool         IsOutside() const;
    inline bool         IsOutside( const type &value ) const;
    inline void         SetMax( const type &max );
    inline void         SetMin( const type &min );
    inline void         SetMinMax( const type &min, const type &max );
    inline void         SetNdiv( const size_t &ndiv );

  protected:

    // Attributes
    type   fMax;
    type   fMin;
    size_t fNdiv;
    char   fType;
    type   fValue;
  };

  // ___________________________
  // CONSTRUCTORS AND DESTRUCTOR

  // Constructor with no arguments
  template<typename type>
  Variable<type>::Variable() { }

  // Constructor given the name, the expression and the limits for the variable
  template<typename type>
  Variable<type>::Variable( const std::string &name,
			    const std::string &expr,
			    const type        &min,
			    const type        &max,
			    const size_t      &ndiv ) :
    BasicVariable<type>( name, expr ), fMax( max ), fMin( min ), fNdiv( ndiv ) { }
  
  // Destructor
  template<typename type>
  Variable<type>::~Variable() { }

  // ______________
  // INLINE METHODS

  // Returns the upper limit of the variable
  template<typename type>
  inline const type Variable<type>::GetMax() const { return fMax; }
  // Returns the lower limit of the variable
  template<typename type>
  inline const type Variable<type>::GetMin() const { return fMin; }
  // Returns the number of divisions for this class
  template<typename type>
  inline const size_t Variable<type>::GetNdiv() const { return fNdiv; }
  // Calculates the step using the current limit and number of divisions. If the input is set
  // to true, it will return the step in such a way that for the fNdiv iteration the upper
  // limit is reached (otherwise it will never be reached).
  template<typename type>
  inline const double Variable<type>::GetStep( const bool &tolim ) const {
    return tolim ? double( fMax - fMin )/( fNdiv - 1 ) : double( fMax - fMin )/fNdiv;
  }
  // Checks whether the current value is inside of the variable of not
  template<typename type>
  inline bool Variable<type>::IsInside() const {
    return ( fValue >= fMin && fValue < fMax );
  }
  // Checks whether the given value is inside of the limits
  template<typename type>
  inline bool Variable<type>::IsInside( const type &value ) const {
    return ( value >= fMin && value < fMax );
  }
  // Checks whether the current value is outside of the limits
  template<typename type>
  inline bool Variable<type>::IsOutside() const {
    return ( fValue < fMin && fValue >= fMax );
  }
  // Checks whether the given value is outside of the limits
  template<typename type>
  inline bool Variable<type>::IsOutside( const type &value ) const {
    return ( value < fMin && value >= fMax );
  }
  // Sets the upper limit of the variable
  template<typename type>
  inline void Variable<type>::SetMax( const type &max ) { fMax = max; }
  // Sets the lower limit of the variable
  template<typename type>
  inline void Variable<type>::SetMin( const type &min ) { fMin = min; }
  // Sets both limits of the variable
  template<typename type>
  inline void Variable<type>::SetMinMax( const type &min, const type &max ) {
    fMin = min;
    fMax = max;
  }
  // Sets the number of divisions of this class
  template<typename type>
  inline void Variable<type>::SetNdiv( const size_t &ndiv ) { fNdiv = ndiv; }


  //_______________________________________________________________________________
  // FUNCTION: EXTRACT VALUE
  //
  // This function sets the value stored in a < BasicVariable > class into another
  // variable. The user must know if the two types are compatible.
  template<typename type> void ExtractValue( type &out, const NamedVariable *var ) {
    const char tp = var -> GetType();
    if ( tp == 'C' )
      out = static_cast<BasicVariable<const char*>*>( var ) -> GetValue();
    else if ( tp == 'B' )
      out = static_cast<BasicVariable<char>*>( var ) -> GetValue();
    else if ( tp == 'b' )
      out = static_cast<BasicVariable<unsigned char>*>( var ) -> GetValue();
    else if ( tp == 'S' )
      out = static_cast<BasicVariable<short int>*>( var ) -> GetValue();
    else if ( tp == 's' )
      out = static_cast<BasicVariable<unsigned short int>*>( var ) -> GetValue();
    else if ( tp == 'I' )
      out = static_cast<BasicVariable<int>*>( var ) -> GetValue();
    else if ( tp == 'i' )
      out = static_cast<BasicVariable<unsigned int>*>( var ) -> GetValue();
    else if ( tp == 'F' )
      out = static_cast<BasicVariable<float>*>( var ) -> GetValue();
    else if ( tp == 'D' )
      out = static_cast<BasicVariable<double>*>( var ) -> GetValue();
    else if ( tp == 'L' )
      out = static_cast<BasicVariable<long long int>*>( var ) -> GetValue();
    else if ( tp == 'l' )
      out = static_cast<BasicVariable<unsigned long long int>*>( var ) -> GetValue();
    else // ( tp == 'O' )
      out = static_cast<BasicVariable<bool>*>( var ) -> GetValue();
  }
  
}

#endif
