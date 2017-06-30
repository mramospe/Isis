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
//  Abstract class to define an object to iterate over. It ensures that
//  the class can also be pythonized.
//
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////


#ifndef LOOPER_H
#define LOOPER_H


//_________________________________________________________________________

namespace isis {

  class Looper {

  public:

    // Constructor
    Looper() { };

    // Destructor
    virtual ~Looper() { };

    // For each inheriting class should call "operator++()"
    virtual void next() = 0;

    // Return the number of loops to be performed
    virtual size_t nloops() const = 0;

    // Start the iteration
    virtual void start() = 0;

    // Check the status. Return "false" if at the end of the iteration
    virtual bool status() const = 0;

  };

}

#endif
