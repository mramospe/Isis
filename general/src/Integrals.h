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
//  Define functions to perform integrals using function proxies
//
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////


#ifndef INTEGRALS_H
#define INTEGRALS_H

#include "Messenger.h"

#include <cmath>
#include <iostream>


//________________________________________________________________________

namespace isis {

  namespace integrals {

    //______________________________________________________________________
    // Do the integral of the given function using the rectangle rule
    template<class type>
    double rectangle_rule( const type &function,
			   const double &v0,
			   const double &vf,
			   const size_t &nsteps ) {

      const double fext = (function(v0) + function(vf))/2.;
      const double step = (vf - v0)/nsteps;

      double sum = 0.;
      for ( size_t i = 1; i < nsteps; ++i )
	sum += function(v0 + i*step);
      
      double integral = step*(fext + sum);

      return integral;
    }
    
    //______________________________________________________________________
    // Do the integral of the given function using the Simpsons' formula
    template<class type>
    double simpson( const type &function,
		    const double &v0,
		    const double &vf,
		    const size_t &nsteps ) {

      if ( nsteps % 2 != 0 )
	IWarning << "Number of steps should be even" << IEndMsg;
  
      const double h = (vf - v0)/nsteps;
    
      double sum_even = 0.;
      for ( size_t i = 2; i <= nsteps - 1; i += 2 )
	sum_even += function(v0 + i*h);

      double sum_odd = 0.;
      for ( size_t i = 1; i <= nsteps; i += 2 )
	sum_odd += function(v0 + i*h);
    
      const double f0 = function(v0);
      const double ff = function(vf);
    
      double integral = h/3.*(f0 + 2*sum_even + 4*sum_odd + ff);
    
      return integral;
    }

    //______________________________________________________________________
    // This method assumes the function vanishes in infinite. The check to
    // see if we are at an infinite must be done with "d2".
    template<class type>
    inline double inf_quad( const type &function, const double &t ) {

      double t2 = t*t;
      double d  = 1 - t2;
      double d2 = d*d;

      return d2 == 0 ? 0 : function(t/d)*(1 + t2)/d2;
    }
    
    //______________________________________________________________________
    // Calculate the left infinite bounded integral using quadrature rules
    template<class type>
    inline double left_inf_quad( const type &function,
				 const double &v0, const double &t ) {

      double t2 = t*t;
      double d  = 1 - t;
      
      return t2 == 0 ? 0 : function(v0 - d/t)/t2;
    }
    
    //______________________________________________________________________
    // Calculate the right infinite bounded integral using quadrature rules
    template<class type>
    inline double right_inf_quad( const type &function,
				  const double &v0, const double &t ) {

      double d  = 1 - t;
      double d2 = d*d;
      
      return d2 == 0 ? 0 : function(v0 + t/d)/d2;
    }
    
    //______________________________________________________________________
    // Calculate the infinite bounded integral using quadrature rules
    template<class type>
    double infinite( const type &function, const size_t &nsteps ) {
      
      ModFuncProxy<const double&, double> proxy(function, inf_quad);

      double integral = simpson(proxy, -1, +1, nsteps);

      return integral;
    }

    //______________________________________________________________________
    // Calculate an one-side-bounded infinite integral
    
#define INFINITE_INTEGRAL(name)					\
    template<class type>					\
    inline double name ## _infinite( const type &function,	\
				     const double &v0,		\
				     const size_t &nsteps ) {	\
								\
      struct InfWrapper {					\
								\
	InfWrapper( const type &function, const double &v0 ) :	\
	  func(function), v0(v0) { };				\
								\
	double operator () ( const double &t ) const {		\
	  return name ## _inf_quad(func, v0, t);		\
	};							\
								\
	const type func = 0;					\
	const double v0;					\
								\
      } wrapper(function, v0);					\
								\
      double integral = simpson(wrapper, 0, +1, nsteps);	\
								\
      return integral;						\
    }
    
    INFINITE_INTEGRAL(left)  // define "left_infinite"
    INFINITE_INTEGRAL(right) // define "right_infinite"

    #undef INFINITE_INTEGRAL
    
  }
  
}

#endif
