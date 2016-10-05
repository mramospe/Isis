////////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2016
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

#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class OutputTable {

  public:

    // Constructor and destructor
    OutputTable();
    OutputTable( const unsigned short int &prec, const unsigned short int len = 0 );
    ~OutputTable();

    // Methods
    std::string GetLine();
    void        Print( const char &chdeco = '*' );
    void        SetFormat( const char *format, const std::vector<std::string> &titles );
    void        SetFormat( const char *format ... );
    std::string Start( const char &chdeco = '*' );

    // Inline methods
    inline void   End();
    inline size_t GetNlines();
    inline void   PrintLine();
    inline void   Rewind();
    inline void   SetPrecision( const unsigned short int &prec );
    inline void   SetStrLength( const unsigned short int &len );

    // Template method
    template<class type>
    inline void AppendStream( type &str );

    // Operators
    template<class type>
    friend inline OutputTable& operator << ( const type &str, OutputTable &table );
    template<class type>
    friend inline OutputTable& operator >> ( OutputTable &table, type &str );

  protected:

    // Attributes
    std::stringstream        fBuffer;
    char                     fDecoChar;
    unsigned short int       fIvar;
    size_t                   fNlines;
    std::vector<size_t>      fPrecisions;
    std::string              fSeparator;
    std::vector<size_t>      fSizes;
    unsigned short int       fStrLength;
    std::vector<std::string> fTitles;

  };
  
  //_______________
  // INLINE METHODS

  // Finishes the display of the table showing the separator
  inline void OutputTable::End() {
    std::cout << fSeparator << std::endl;
  }
  // Returns the number of lines in the table
  inline size_t OutputTable::GetNlines() { return fNlines; }
  // Displays another line from the buffer
  void General::OutputTable::PrintLine() {
    std::cout << this -> GetLine() << std::endl;
  }
  // Rewinds the buffer
  inline void OutputTable::Rewind() {
    fBuffer.seekg( 0 );
  }
  // Sets the number of numbers that will be displayed for float and double types
  inline void OutputTable::SetPrecision( const unsigned short int &prec ) {
    fBuffer.precision( prec );
  }
  // Sets the length of the strings displayed in the table
  inline void OutputTable::SetStrLength( const unsigned short int &len ) { fStrLength = len; }

  //___________________
  // TEMPLATE FUNCTIONS

  // Appends a new element to the current buffer, separating it by a tabulation when it corresponds
  // to the same row and by a newline symbol when moving to another column
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
  // Returns an element from the buffer
  template<class type>
  inline OutputTable& operator >> ( OutputTable &table, type &str ) {
    table.fBuffer >> str;
    return table;
  }
  // Appends to the current table a new value of any kind. This operator is useful when using it
  // concatenated: "table << 1 << 2 << ..."
  template<class type>
  inline OutputTable& operator << ( OutputTable &table, const type &str ) {
    table.AppendStream( str );
    return table;
  }
}

#endif
