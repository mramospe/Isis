////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// ---------------------------------------------------------------------------------
//
//  Description:
//
//  This class defines a multivariable bin. The bin has information about the
//  variables, the number of entries and an associated weight. This is a friend
//  class of < VarWeighter >.
//
// ---------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////


#ifndef VAR_BIN
#define VAR_BIN

#include <map>
#include <string>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Isis {

  class Reweighter;

  class VarBin {

    friend class VarWeighter;

  public:

    // Main constructor
    VarBin();

    // Destructor
    ~VarBin();

    // Increases the number of entries in one unit for this bin if the values given
    // are inside its limits
    void IfInsideAdd( const std::map<std::string, double> &values );

    // Checks if the value for the variable given is inside this bin
    bool IsOutside( const std::map<std::string, double> &values );

    // Sets the weight for this bin. The input parameters are the number of entries
    // for the reference bin, the ratio between the entries for each tree and its
    // error. Both the weight and the error are computed in this method.
    void SetWeight( const size_t &rentries,
		    const double &ratio,
		    const double &sratio,
		    const double &maxrelerr );

    // Displays the information of this bin, where the width of the columns is
    // specified as an input
    void Print( const size_t &width );

    // Returns the number of entries in the bin
    inline const size_t GetEntries() const;
    
    // Returns the error associated to the bin
    inline const double GetError() const;
    
    // Returns the weight associated to the bin
    inline const double GetWeight() const;

  protected:

    // Divides this bin using the information given
    std::vector<VarBin> Split( const std::string &name,
			       const size_t      &nbins,
			       const double      &min,
			       const double      &max );

  protected:
    
    // Error associated with this bin
    double fError;

    // Number of entries in the bin
    size_t fNentries;

    // Ranges for each of the variables
    std::map<std::string,
	     std::pair<double, double> > fVarRanges;

    // Weight of the bin
    double fWeight;

  private:

    // Checks if the values given are inside this bin
    bool IsOutside( const std::string &vname, const double &value );

  };

  //_______________________________________________________________________________
  //
  inline const size_t VarBin::GetEntries() const { return fNentries; }

  //_______________________________________________________________________________
  //
  inline const double VarBin::GetError() const { return fError; }

  //_______________________________________________________________________________
  //
  inline const double VarBin::GetWeight() const { return fWeight; }

}

#endif
