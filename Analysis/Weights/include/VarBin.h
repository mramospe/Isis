////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//  Analysis package                                                              //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  AUTHOR: Miguel Ramos Pernas                                                   //
//  e-mail: miguel.ramos.pernas@cern.ch                                           //
//                                                                                //
//  Last update: 16/02/2016                                                       //
//                                                                                //
// ------------------------------------------------------------------------------ //
//                                                                                //
//  Description:                                                                  //
//                                                                                //
//  This class defines a multivariable bin. The bin has information about the     //
//  variables, the number of entries and an associated weight. This is a friend   //
//  class of < VarWeighter >.                                                     //
//                                                                                //
// ------------------------------------------------------------------------------ //
////////////////////////////////////////////////////////////////////////////////////


#ifndef VAR_BIN
#define VAR_BIN

#include <map>
#include <string>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class Reweighter;

  class VarBin {

    friend class VarWeighter;

  public:

    // Constructor and destructor
    VarBin();
    ~VarBin();

    // Methods
    void                IfInsideAdd( const std::map<std::string, double> &values );
    bool                IsOutside( const std::map<std::string, double> &values );
    void                SetWeight( const size_t &rentries,
				   const double &ratio,
				   const double &sratio,
				   const double &maxrelerr );
    void                Print( const size_t &width );

    // Inline methods
    inline const size_t GetEntries() const;
    inline const double GetError() const;
    inline const double GetWeight() const;

  protected:

    // Method
    std::vector<VarBin> Split( const std::string &name,
			       const size_t      &nbins,
			       const double      &min,
			       const double      &max );

    // Attributes
    double                               fError;
    size_t                               fNentries;
    std::map<std::string,
	     std::pair<double, double> > fVarRanges;
    double                               fWeight;

  private:

    // Method
    bool IsOutside( const std::string &vname, const double &value );

  };

  //__________________
  // -- INLINE METHODS

  // Returns the number of entries in the bin
  inline const size_t VarBin::GetEntries() const { return fNentries; }
  // Returns the error associated to the bin
  inline const double VarBin::GetError() const { return fError; }
  // Returns the weight associated to the bin
  inline const double VarBin::GetWeight() const { return fWeight; }

}

#endif
