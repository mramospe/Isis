///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "LoopArray.h"
#include "Utils.h"


//_______________________________________________________________________________
//
General::LoopArray::LoopArrayIndex::LoopArrayIndex( const size_t &min,
						    const size_t &max ) :
  Index( min ), Min( min ), Max( max ) { }

//_______________________________________________________________________________
//
General::LoopArray::LoopArrayIndex::~LoopArrayIndex() { }

//_______________________________________________________________________________
//
General::LoopArray::LoopArray() : fNloops( 1 ), fPos( 0 ) { }

//_______________________________________________________________________________
//
General::LoopArray::LoopArray( const LoopArray &other ) :
  fNloops( other.fNloops ), fPos( 0 ), fVector( other.fVector ) { }

//_______________________________________________________________________________
//
General::LoopArray::LoopArray( const size_t &size,
			       const size_t &min,
			       const size_t &max ) :
  fNloops( General::IPow( max - min, size ) ),
  fPos( 0 ),
  fVector( std::vector<LoopArrayIndex> ( size, LoopArrayIndex( min, max ) ) ) { }

//_______________________________________________________________________________
//
General::LoopArray::~LoopArray() { }

//_______________________________________________________________________________
//
General::LoopArray& General::LoopArray::operator ++ () {
  fVector.back().Index++;
  auto it = fVector.rbegin();
  while ( it -> Index == it -> Max ) {
    it -> Index = it -> Min;
    if ( it + 1 != fVector.rend() )
      ( ++it ) -> Index++;
  }
  ++fPos;
  return *this;
}

//_______________________________________________________________________________
//
General::LoopArray General::LoopArray::operator ++ ( int ) {
  General::LoopArray tmp( *this );
  this->operator ++ ();
  return tmp;
}

//_______________________________________________________________________________
//
const size_t General::LoopArray::operator [] ( size_t index ) const {
  return fVector[ index ].Index;
}
