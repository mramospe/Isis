////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -----------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 22/03/2017
//
// -----------------------------------------------------------------------------
//
//  Description:
//
//  Class to define a lower and an upper bound. The convention is to accept
//  that a value is inside it if "value >= lower and value < upper".
//
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////


#ifndef __BOUNDS__
#define __BOUNDS__


//_______________________________________________________________________________

namespace Isis {

  template<class type>
  class Bounds {

  public:

    // Constructor
    Bounds() { };

    // Constructor given the two bounds
    Bounds( const type &min, const type &max ) : fMin( min ), fMax( max ) { }

    // Copy constructor
    template<class vtype>
    Bounds( const Bounds<vtype> &other ) { *this = other; }

    // Destructor
    virtual ~Bounds() { }

    // Return the lower bound
    inline const type& GetMin() const;

    // Return the upper bound
    inline const type& GetMax() const;

    // Return whether the given value is inside the bounds or not
    template<class vtype>
    inline bool IsInside( const vtype &value ) const;

    // Set the upper bound
    template<class vtype>
    inline void SetMax( const vtype &value );

    // Set the lower bound
    template<class vtype>
    inline void SetMin( const vtype &value );

    // Set both upper and lower bounds
    template<class vtype>
    inline void SetMinMax( const vtype &min, const vtype &max );

    // Assignment operator
    template<class vtype>
    inline Bounds<type>& operator = ( const Bounds<vtype> &other ) {
      
      fMin = other.GetMin();
      fMax = other.GetMax();

      return *this;
    }

  protected:

    // Lower bound
    type fMin;

    // Upper bound
    type fMax;
    
  };

  //_______________________________________________________________________________
  //
  template<class type>
  inline const type& Bounds<type>::GetMin() const { return fMin; }

  //_______________________________________________________________________________
  //
  template<class type>
  inline const type& Bounds<type>::GetMax() const { return fMax; }

  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline void Bounds<type>::SetMax( const vtype &value ) { fMax = value; }

  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline void Bounds<type>::SetMin( const vtype &value ) { fMin = value; }

  template<class type> template<class vtype>
  inline void Bounds<type>::SetMinMax( const vtype &min, const vtype &max ) {

    this->SetMin(min);
    this->SetMax(max);
  }
  
  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline bool Bounds<type>::IsInside( const vtype &value ) const {

    return (value >= fMin && value < fMax);
  }
  
}

#endif
