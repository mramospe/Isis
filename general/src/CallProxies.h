////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/06/2017
//
// -------------------------------------------------------------------------
//
//  Description:
//
//  Set of classes to represent proxies of functions. All this proxies have
//  their functionality based on the operator "()".
//
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////


#ifndef CALL_PROXIES_H
#define CALL_PROXIES_H


namespace isis {

  //________________________________________________________________________
  // Types referring to function pointers
  template<class input, class output>
  using FuncPtr = output (*) ( input );

  template<class cl, class input, class output>
  using MethPtr = output (cl::*) ( input ) const;

  template<class input, class output>
  using ModFuncPtr = output (*) ( const FuncPtr<input, output>, input );
  
  //________________________________________________________________________
  // Abstract class to define a proxy for a "type -> type" function/method
  template<class input, class output>
  class CallProxy {

  public:
    
    // Constructor
    CallProxy() { }

    // Destructor
    virtual ~CallProxy() { }

    // Virtual declaration
    virtual output operator () ( input ) const = 0;
  };

  //________________________________________________________________________
  // Proxy for a function returning a value with the same type as the input
  template<class input, class output>
  class FuncProxy : public CallProxy<input, output> {

  public:
    
    // Constructor
    FuncProxy( const FuncPtr<input, output> func ) :
      CallProxy<input, output>(), fFunc(func) { };

    // Destructor
    virtual ~FuncProxy() { };
    
    // Override operator()
    output operator () ( input v ) const override {

      return (*fFunc)(v);
    }

  protected:

    // Pointer to the function
    FuncPtr<input, output> fFunc = 0;
  };

  //________________________________________________________________________
  // Proxy for a method returning a value with the same type as the input
  template<class cl, class input, class output>
  class MethProxy : public CallProxy<input, output> {

  public:
    
    // Constructor
    MethProxy( const cl *obj, const MethPtr<cl, input, output> meth ) :
      CallProxy<input, output>(), fClass(obj), fMeth(meth) { }

    // Destructor
    virtual ~MethProxy() { }

    // Override operator()
    output operator () ( input v ) const override {

      return (fClass->*fMeth)(v);
    }
    
  protected:
    
    // Pointer to the class
    const cl *fClass = 0;

    // Class method
    const MethPtr<cl, input, output> fMeth = 0;
    
  };

  //________________________________________________________________________
  // Proxy modifying the behaviour of a given function
  template<class input, class output>
  class ModFuncProxy : public FuncProxy<input, output> {

  public:
    
    // Constructor
    ModFuncProxy( const FuncPtr<input, output> func,
		  const ModFuncPtr<input, output> mod ) :
      FuncProxy<input, output>(func), fMod(mod) { };

    // Destructor
    virtual ~ModFuncProxy() { };
    
    // Override operator()
    output operator () ( input v ) const override {

      return (*fMod)(this->fFunc, v);
    }

  protected:

    // Pointer to the function
    ModFuncPtr<input, output> fMod = 0;
  };
  
}

#endif
