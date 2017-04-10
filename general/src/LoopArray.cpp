///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "LoopArray.h"
#include "Utils.h"


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  LoopArray::LoopArrayIndex::LoopArrayIndex( const size_t &min,
					     const size_t &max ) :
    Index( min ), Min( min ), Max( max ) { }

  //_______________________________________________________________________________
  //
  LoopArray::LoopArrayIndex::~LoopArrayIndex() { }

  //_______________________________________________________________________________
  //
  LoopArray::LoopArray() : fNloops( 1 ), fPos( 0 ) { }

  //_______________________________________________________________________________
  //
  LoopArray::LoopArray( const LoopArray &other ) :
    fNloops( other.fNloops ), fPos( 0 ), fVector( other.fVector ) { }

  //_______________________________________________________________________________
  //
  LoopArray::LoopArray( const size_t &size,
			const size_t &min,
			const size_t &max ) :
    fNloops( ipow( max - min, size ) ),
    fPos( 0 ),
    fVector( std::vector<LoopArrayIndex> ( size, LoopArrayIndex( min, max ) ) ) { }

  //_______________________________________________________________________________
  //
  LoopArray::~LoopArray() { }

  //_______________________________________________________________________________
  //
  LoopArray& LoopArray::operator ++ () {
    
    fVector.back().Index++;
    auto it = fVector.rbegin();
    while ( it->Index == it->Max ) {
      it->Index = it->Min;
      if ( it + 1 != fVector.rend() )
	( ++it )->Index++;
    }
    ++fPos;
    
    return *this;
  }

  //_______________________________________________________________________________
  //
  LoopArray LoopArray::operator ++ ( int ) {
    LoopArray tmp( *this );
    this->operator ++ ();
    return tmp;
  }

  //_______________________________________________________________________________
  //
  const size_t LoopArray::operator [] ( size_t index ) const {
    return fVector[ index ].Index;
  }

}
