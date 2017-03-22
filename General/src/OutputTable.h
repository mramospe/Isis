////////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 22/03/2017
//
// ---------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to generate tables in c++ output. It allows both to
//  construct the table and then print it, or print the lines manually. Do not
//  forget to rewind the class before making another print. The precision of
//  the numbers displayed, as well as the length of the strings can be modified.
//  The characters decorating the table can be modified too when calling the
//  methods < Print > or < Start >. The inputs can be performed wih the < << >
//  operator or with the < AppendStream > method. Calling the operator < >> >
//  one can get the elements one by one from the buffer. In this case the user
//  has to control the number of times that has to call this operator.
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#ifndef OUTPUT_TABLE
#define OUTPUT_TABLE

#include "Definitions.h"

#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________

namespace Isis {

  class OutputTable {

  public:

    // Main constructor
    OutputTable();

    // Constructor given the precision number and string length
    OutputTable( const unsigned short int &prec, const unsigned short int len = 0 );

    // Destructor
    ~OutputTable();

    // Returns a new line from the buffer
    std::string GetLine();

    // Displays the whole table, including the title and the elements. If this method
    // is used no < Start > has to be called before.
    void Print( const char &chdeco = '*' );

    // Sets the format for the current table. To know which imput parameters are
    // allowed to be introduced see the < SetFormat( const char *format ... ) > method.
    void SetFormat( const char *format, const Strings &titles );

    // Sets the format of the elements displayed in the table. There can be displayed
    // values of any kind: boolean (b), short integers (i), integers (I), floats (f),
    // doubles (d) and strings (s). As second, third, ... arguments the title of the
    // variables have to be specified.
    void SetFormat( const char *format ... );

    // Prints the title and initializes the table. If the pointer of the buffer is
    // not located at the start it is moved to it. Returns the string used to
    // separate the title from the elements.
    std::string Start( const char &chdeco = '*' );

    // Finishes the display of the table showing the separator
    inline void End();

    // Returns the number of lines in the table
    inline size_t GetNlines();

    // Displays another line from the buffer
    inline void PrintLine();

    // Rewinds the buffer
    inline void Rewind();

    // Sets the number of numbers that will be displayed for float and double types
    inline void SetPrecision( const unsigned short int &prec );

    // Sets the length of the strings displayed in the table
    inline void SetStrLength( const unsigned short int &len );

    // Appends a new element to the current buffer, separating it by a tabulation
    // when it corresponds to the same row and by a newline symbol when moving to
    // another column
    template<class type>
    inline void AppendStream( type &str );

    // Returns an element from the buffer
    template<class type>
    friend inline OutputTable& operator << ( const type &str, OutputTable &table );

    // Appends to the current table a new value of any kind. This operator is
    // useful when using it concatenated: "table << 1 << 2 << ..."
    template<class type>
    friend inline OutputTable& operator >> ( OutputTable &table, type &str );

  protected:

    // Buffer to store the information
    std::stringstream fBuffer;

    // Character to decorate the table
    char fDecoChar;

    // Iterator used when reading from streams
    unsigned short int fIvar;

    // Number of lines in the table
    size_t fNlines;

    // Vector to store the number of variables to be printed and the precision for each
    Sizes fPrecisions;

    // Internal variable to separate blocks
    std::string fSeparator;

    // Vector storing the size of the elements to be printed in each column
    Sizes fSizes;

    // Length of the variables which are strings
    unsigned short int fStrLength;

    // Title for each of the variables
    Strings fTitles;

  };
  
  //_______________________________________________________________________________
  //
  inline void OutputTable::End() {
    std::cout << fSeparator << std::endl;
  }

  //_______________________________________________________________________________
  //
  inline size_t OutputTable::GetNlines() { return fNlines; }

  //_______________________________________________________________________________
  //
  void OutputTable::PrintLine() { std::cout << this->GetLine() << std::endl; }

  //_______________________________________________________________________________
  //
  inline void OutputTable::Rewind() { fBuffer.seekg( 0 ); }

  //_______________________________________________________________________________
  //
  inline void OutputTable::SetPrecision( const unsigned short int &prec ) {
    fBuffer.precision( prec );
  }

  //_______________________________________________________________________________
  //
  inline void OutputTable::SetStrLength( const unsigned short int &len ) {
    fStrLength = len;
  }

  //_______________________________________________________________________________
  //
  template<class type>
  inline void OutputTable::AppendStream( type &str ) {
    if ( fPrecisions[ fIvar ] )
      fBuffer.precision( fPrecisions[ fIvar ] );
    fBuffer << str;
    if ( ++fIvar != fSizes.size() )
      fBuffer << '\t';
    else {
      fIvar = 0;
      fNlines++;
      fBuffer << '\n';
    }
  }

  //_______________________________________________________________________________
  //
  template<class type>
  inline OutputTable& operator >> ( OutputTable &table, type &str ) {
    table.fBuffer >> str;
    return table;
  }

  //_______________________________________________________________________________
  //
  template<class type>
  inline OutputTable& operator << ( OutputTable &table, const type &str ) {
    table.AppendStream( str );
    return table;
  }
}

#endif
