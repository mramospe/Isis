////////////////////////////////////////////////////
//  Example to generate a BDT using TMVA package  //
////////////////////////////////////////////////////

#include "BDTAnalysis.h"

#include <cstring>

Int_t main( Int_t argc, Char_t *argv[] ) {


  if ( argc == 1 )
    std::cout <<
      " Specificate one of the two avaliable options: classificate/apply"
	      << std::endl;
  else {

    // Methods to be used in the training
    const Int_t   nmeth          = 5;
    const Char_t *methods[nmeth] = { "BDT",
				     "BDTG",
				     "BDTB",
				     "BDTD",
				     "BDTF" };

    // Variables for the BDT training
    const Int_t   nvars              = 6;
    const Char_t *variables[nvars]   = { "Lambda_b0_ENDVERTEX_CHI2",
					 "Lambda_b0_IPCHI2_OWNPV",
					 "Lambda_b0_FDCHI2_OWNPV",
					 "Lambda_b0_LnPT",
					 "Min_IPCHI2_GD",
					 "Min_LnPT_GD" };

    const Char_t *var_titles[nvars]  = {"EndV_{#Chi^{2}} (#Lambda_{b}^{0})",
					"IP_{#Chi^{2}} (#Lambda_{b}^{0})",
					"FD_{#Chi^{2}} (#Lambda_{b}^{0})",
					"LnP_{T} (#Lambda_{b}^{0})",
					"Min. IP_{#Chi^{2}} (gd)",
					"Min. LnP_{t} (gd)" };

    // Spectator variables
    const Int_t   nspec              = 10;
    const Char_t *spectators[nspec]  = { "Lambda_b0_Phi",
					 "Lambda_b0_P",
					 "Kaon_P",
					 "Pion1_P",
					 "Kaon_PIDKp",
					 "Pion1_PIDK",
					 "Proton_P",
					 "Pion2_P",
					 "Proton_PIDpK",
					 "Pion2_PIDK" };

    const Char_t *spec_titles[nspec] = { "#Lambda_{b}^{0}_{#Phi}",
					 "#Lambda_{b}^{0}_{P}",
					 "Kaon_{P}",
					 "Pion^{1}_{P}",
					 "Kaon_{PID_{p-K}}",
					 "Pion^{1}_{PID_{K-#pi}}",
					 "Proton_{P}",
					 "Pion^{2}_{P}",
					 "Proton_{PID_{p-K}}",
					 "Pion^{2}_{PID_{K-#pi}}" };

    //_____________________________________________________________________

    //---------------------
    // Classificate option 
    //---------------------

    if ( !strcmp( argv[1], "classificate" )
	 or !strcmp( argv[1], "Classificate" ) ) {

      Analysis::BDTClassifier BDTClass( "Factory", "TMVA.root" );

      // Background file
      BDTClass.AddBackgroundFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/TMVA_Samples_2011.root", "Background" );

      // Signal file
      BDTClass.AddSignalFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/TMVA_Samples_2011.root" , "Signal" );
  
      // Adds the variables, spectators and methods to book
      for (Int_t i = 0; i < nvars; i++)
	BDTClass.AddVariable( variables[i], var_titles[i], 'F');

      for (Int_t i = 0; i < nspec; i++)
	BDTClass.AddSpectator( spectators[i], spec_titles[i] );

      for (Int_t i = 0; i < nmeth; i++)
	BDTClass.BookMethod( methods[i] );

      // Performs the classification
      BDTClass.Classificate();

    }

    //--------------
    // Apply option 
    //--------------

    else if ( !strcmp( argv[1], "apply" ) or !strcmp( argv[1], "Apply" ) ) {

      Analysis::BDTApplier BDTClass( "Applier" , "Factory" );
      
      // Input files
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2Kpippi_B2FourBodyLine_2011_magnetUp_MoreVariables.root"    );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2Kpippi_B2FourBodyLine_2011_magnetDown_MoreVariables.root"  );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2D0ppi_B2FourBodyLine_MC2011_magnetUp_MoreVariables.root"   );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2D0ppi_B2FourBodyLine_MC2011_magnetDown_MoreVariables.root" );
      
      // Output files
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2Kpippi_B2FourBodyLine_2011_magnetUp_MoreVariables_BDT.root"  , "RECREATE"  );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2Kpippi_B2FourBodyLine_2011_magnetDown_MoreVariables_BDT.root", "RECREATE"  );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2D0ppi_B2FourBodyLine_MC2011_magnetUp_MoreVariables_BDT.root" , "RECREATE"  );
      BDTClass.AddFile( "/scratch07/miguel.ramos/Lb2Kpippi_B2FourBodyLine/Lb2D0ppi_B2FourBodyLine_MC2011_magnetDown_MoreVariables_BDT.root", "RECREATE" );

      // Input paths
      for (Int_t i = 0; i < 4; i++)
	BDTClass.SetInputTreePath( i, "DecayTree" );

      // Output tree name
      BDTClass.SetOutputTreeName( "DecayTree" );

      for (Int_t i = 0; i < nvars; i++)
	BDTClass.AddVariable( variables[i] );

      for (Int_t i = 0; i < nspec; i++)
	BDTClass.AddSpectator( spectators[i] );

      for (Int_t i = 0; i < nmeth; i++)
	BDTClass.UseMethod( methods[i] );

      BDTClass.Apply();

    }
    else
      std::cout <<
	" Option not valid. Avaliable options are: classificate/apply"
		<< std::endl;
  }
  
  return 0;
}
