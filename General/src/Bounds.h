////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -----------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
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

namespace isis {

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
    inline const type& getMin() const;

    // Return the upper bound
    inline const type& getMax() const;

    // Return whether the given value is inside the bounds or not
    template<class vtype>
    inline bool isInside( const vtype &value ) const;

    // Set the upper bound
    template<class vtype>
    inline void setMax( const vtype &value );

    // Set the lower bound
    template<class vtype>
    inline void setMin( const vtype &value );

    // Set both upper and lower bounds
    template<class vtype>
    inline void setMinMax( const vtype &min, const vtype &max );

    // Assignment operator
    template<class vtype>
    inline Bounds<type>& operator = ( const Bounds<vtype> &other ) {
      
      fMin = other.getMin();
      fMax = other.getMax();

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
  inline const type& Bounds<type>::getMin() const { return fMin; }

  //_______________________________________________________________________________
  //
  template<class type>
  inline const type& Bounds<type>::getMax() const { return fMax; }

  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline void Bounds<type>::setMax( const vtype &value ) { fMax = value; }

  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline void Bounds<type>::setMin( const vtype &value ) { fMin = value; }

  template<class type> template<class vtype>
  inline void Bounds<type>::setMinMax( const vtype &min, const vtype &max ) {

    this->setMin(min);
    this->setMax(max);
  }
  
  //_______________________________________________________________________________
  //
  template<class type> template<class vtype>
  inline bool Bounds<type>::isInside( const vtype &value ) const {

    return (value >= fMin && value < fMax);
  }
  
}

#endif
