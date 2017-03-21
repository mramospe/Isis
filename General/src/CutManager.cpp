///////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------
///////////////////////////////////////////////////////////


#include "CutManager.h"
#include "Definitions.h"
#include "Messenger.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  CutManager::CutManager( const std::string &file_path ) :
    fFile( new std::ifstream ), fOptions( { {"and", "&&"}, {"or", "||"} } ) {
    if ( file_path.size() )
      this->Open( file_path );
  }

  //_______________________________________________________________________________
  //
  CutManager::CutManager( const CutManager &other ) {
    fCuts    = other.fCuts;
    fFile    = other.fFile;
    fOptions = other.fOptions;
  }

  //_______________________________________________________________________________
  //
  CutManager::~CutManager() {
    if ( fFile.use_count() == 1 && (*fFile).is_open() )
      (*fFile).close();
  }

  //_______________________________________________________________________________
  //
  std::string CutManager::BookCut( const std::string &key, const bool &print ) {
    if ( fCuts.find( key ) != fCuts.end() ) {
      IWarning << "Cut with name < " << key << " > already booked" << IEndMsg;
      return "";
    }
    std::string cut = this->GetCut( key );
    if ( cut.size() ) {
      fCuts.insert( std::make_pair( key, cut ) );
      if ( print )
	IBegMsg << "Booked new cut < " << key << " >: " << fCuts[ key ] << IEndMsg;
    }
    return cut;
  }

  //_______________________________________________________________________________       
  //
  std::string CutManager::GetCut( const std::string &key ) {

    // Each time a cut is obtained the pointer to the file is set to its start
    (*fFile).clear();
    (*fFile).seekg( 0 );

    std::string cuts, newcut, sstr;
    std::string::size_type pos, ifirst, ilast, fpos;
    bool cond( true ), mismatch;

    while ( cond && std::getline( *fFile, cuts ) ) {

      // Checks whether the line is empty
      if ( !cuts.empty() ) {

	// If there are whitespaces before the keyword, they are omitted
	while ( cuts[ 0 ] == ' ' ) cuts.erase( 0, 1 );

	if ( !cuts.compare( 0, key.size(), key ) ) {

	  cuts = cuts.substr( cuts.find( '=' ) + 1 );

	  while ( cuts[ 0 ] == ' ' ) cuts.erase( 0, 1 );

	  // If another cut is found it is replaced by its content. The position at the file
	  // is saved and retrieved, in order to properly scan the file.
	  while ( ( ifirst = cuts.find( '$' ) ) != std::string::npos ) {

	    mismatch = false;
	    ilast    = cuts.find( '$', ifirst + 1 );

	    if ( ilast == std::string::npos ) {
	      mismatch = true;
	      sstr.clear();
	    }
	    else {
	      sstr = cuts.substr( ifirst + 1, ilast - ifirst - 1 );
	      if ( sstr.find( ' ' ) != std::string::npos )
		mismatch = true;
	    }
	  
	    if ( mismatch ) {
	      IError <<
		"Mismatched < $ > symbol when scanning cut < "
		     << key << " >" << IEndMsg;
	      return std::string();
	    }

	    // Replaces the cut name by its value
	    fpos  = (*fFile).tellg();
	    newcut = this->GetCut( sstr );	  
	    cuts.replace( ifirst, ilast + 1 - ifirst, newcut );
	    (*fFile).seekg( fpos );
	  }

	  cond = false;
	}
      }

      if ( (*fFile).eof() && cond ) {
	IError << "Cut with name < " << key << " > does not exist." << IEndMsg;
	return 0;
      }
    }
    // Changes the name of the conditionals in the cuts set
    for ( StrMap::iterator it = fOptions.begin();
	  it != fOptions.end(); ++it )
      while ( ( pos = cuts.find( it->first ) ) != std::string::npos )
	cuts.replace( pos, ( it->first ).size(), it->second );

    return '(' + cuts + ')';
  }

  //_______________________________________________________________________________
  //
  std::string CutManager::MakeMergedCut( std::string joinop ) {
    joinop = ' ' + joinop + ' ';
    auto it = fCuts.begin();
    std::string gcut( it->second );
    for ( ++it; it != fCuts.end(); ++it )
      gcut.append( joinop + it->second );
    return gcut;
  }

  //_______________________________________________________________________________
  //
  void CutManager::Open( const std::string &file_path ) {
    if ( fFile.use_count() == 1 && (*fFile).is_open() )
      (*fFile).close();
    (*fFile).open( file_path.c_str() );
    if ( !(*fFile) ) {
      IError << "File < " << file_path << " > does not exist" << IEndMsg;
      return;
    }
    std::string line, str;
    size_t nl = 0, pos, newpos;
    while ( std::getline( (*fFile), line ) ) {
      ++nl;
      if ( line.size() )
	if ( line.front() != '#' ) {
	  if ( ( pos = line.find( '=' ) ) != std::string::npos ) {

	    // Checks whether the cut name is free of whitespaces
	    str = line.substr( 0, pos );
	    while ( str.front() == ' ' )
	      str.erase( str.begin() );
	    while ( str.back() == ' ' )
	      str.erase( str.end() - 1 );
	    if ( str.find( ' ' ) != std::string::npos ) {
	      IError << "The cut defined in line < " << nl <<
		" > has whitespaces on its name" << IEndMsg;
	      return;
	    }

	    // Checks the matching of the < $ > symbols in the expression
	    while ( ( pos = line.find( '$', ++pos ) ) != std::string::npos ) {
	      newpos = line.find( '$', ++pos );
	      str    = line.substr( pos, newpos - pos );
	      if ( newpos == std::string::npos || str.find( ' ' ) != std::string::npos ) {
		IError << "Mismatched < $ > symbol in line < " << nl << " >" << IEndMsg;
		return;
	      }
	      pos = newpos + 1;
	    }
	  }
	  else
	    IWarning<< "Line number < " << nl <<
	      " > not a cut line; must be commented (starting by #)" << IEndMsg;
	}
    }
    (*fFile).clear();
    (*fFile).seekg( 0 );
  }

  //_______________________________________________________________________________       
  //
  void CutManager::Print() const {
    size_t maxsize = std::max_element( fCuts.begin(),
				       fCuts.end(),
				       [] ( const std::pair<std::string, std::string> &p1,
					    const std::pair<std::string, std::string> &p2 ) {
					 return p1.first.size() < p2.first.size(); } )->first.size();
    for ( auto it = fCuts.begin(); it != fCuts.end(); ++it )
      IBegMsg << std::left << std::setw( maxsize ) <<
	it->first  << " => " << it->second << IEndMsg;
  }

}
