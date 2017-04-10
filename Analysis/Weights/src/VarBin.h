////////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// ---------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
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

namespace isis {

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
    void ifInsideAdd( const std::map<std::string, double> &values );

    // Checks if the value for the variable given is inside this bin
    bool isOutside( const std::map<std::string, double> &values );

    // Sets the weight for this bin. The input parameters are the number of entries
    // for the reference bin, the ratio between the entries for each tree and its
    // error. Both the weight and the error are computed in this method.
    void setWeight( const size_t &rentries,
		    const double &ratio,
		    const double &sratio,
		    const double &maxrelerr );

    // Displays the information of this bin, where the width of the columns is
    // specified as an input
    void display( const size_t &width );

    // Returns the number of entries in the bin
    inline const size_t getEntries() const;
    
    // Returns the error associated to the bin
    inline const double getError() const;
    
    // Returns the weight associated to the bin
    inline const double getWeight() const;

  protected:

    // Divides this bin using the information given
    std::vector<VarBin> split( const std::string &name,
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
    bool isOutside( const std::string &vname, const double &value );

  };

  //_______________________________________________________________________________
  //
  inline const size_t VarBin::getEntries() const { return fNentries; }

  //_______________________________________________________________________________
  //
  inline const double VarBin::getError() const { return fError; }

  //_______________________________________________________________________________
  //
  inline const double VarBin::getWeight() const { return fWeight; }

}

#endif
