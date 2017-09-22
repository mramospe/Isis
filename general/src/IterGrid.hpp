///////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 30/06/2017
//
// ------------------------------------------------------------------------
//
//  Description:
//
//  Define a virtual grid of double-type values. Instead of defining all
//  the points, only the values for each dimension are stored, and the
//  actual point is returned when looping over it.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef ITERGRID_H
#define ITERGRID_H

#include "Bounds.hpp"
#include "Definitions.hpp"
#include "Looper.hpp"
#include "LoopArray.hpp"

#include <algorithm>
#include <vector>


//_________________________________________________________________________

namespace isis {

  template<class type>
  class IterGrid : public Looper {

  public:

    // Empty constructor
    IterGrid() : Looper() { };

    // Copy constructor
    IterGrid( const IterGrid &other ) :
      Looper(),
      fArray(other.fArray),
      fGrid(other.fGrid) { }

    // Create a symmetric grid for "nvars" variables
    IterGrid( const size_t &nvars, const size_t &npoints,
	      const type &min, const type &max ) :
      Looper(),
      fArray(nvars, 0, npoints) {
      
      for ( size_t i = 0; i < nvars; ++i )
	this->add(npoints, min, max);
    }
    
    // Destructor
    virtual ~IterGrid() { };

    // Add a new variable to the grid. The maximum value will also be
    // reached during iterations.
    void add( const size_t &n, const type &min, const type &max ) {

      fArray.addIndex(0, n);

      std::pair<size_t, Bounds<type> > item(n, Bounds<type>{min, max});
    
      fGrid.emplace_back(std::move(item));
    }

    // This method is set to enable the pythonization of the class
    inline void next() override {

      this->operator++();
    }

    // Return the number of points in the grid
    size_t nloops() const override {

      return fArray.nloops();
    }

    // Get the vector of values for the current iteration
    std::vector<type> values() const {

      std::vector<type> vec(fGrid.size());
      
      auto it = fArray.cbegin();
      auto iv = fGrid.cbegin();

      std::generate(vec.begin(), vec.end(),
		    [&it, &iv] () {
		    
		      const auto &i   = (it++)->Index;
		      const auto &n   = iv->first;
		      const auto &bds = iv->second;

		      ++iv;
		    
		      return bds.iterStep(n, i, true);
		    });

      return vec;
    }
    
    // Start the iteration
    inline void start() override {

      fArray.start();
    }

    // The status is marked by the "LoopArray" instance
    inline bool status() const override {

      return fArray.status();
    }

    // Return the values for the grid item at position "idx"
    inline std::vector<type> operator[]( size_t idx ) const {
      
      auto it = fArray.getIndices().crbegin();
      auto iv = fGrid.crbegin();
      
      std::vector<type> result(fGrid.size());
      
      std::generate(result.rbegin(), result.rend(),
		    [&it, &idx, &iv] () {

		      const auto& i   = (it++)->parse(idx);
		      const auto& n   = iv->first;
		      const auto& bds = iv->second;
		      
		      ++iv;

		      return bds.iterStep(n, i, true);
		    });

      return result;
    }
    
    // Incremental operator
    inline void operator++() {
      
      fArray++;
    }

    // Incremental operator returning a copy of the previous state
    inline IterGrid operator++( int ) {
      
      IterGrid tmp(*this);
      
      this->operator++();
      
      return tmp;
    }

  protected:

    // Store the indices for each dimension
    LoopArray fArray;
    
    // Store the values to iterate over for each variable
    std::vector<std::pair<size_t, Bounds<type> > > fGrid;

  };

}

#endif
