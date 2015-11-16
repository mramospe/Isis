/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //       
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 13/11/2015                            //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  Description:                                       //
//                                                     //
//  Implements two classes perform analysis with the   //
//  TMVA package. BDTClassifier is the class that      //
//  allows to storage and manage all possible methods  //
//  provided by TMVA and to get the weights related    //
//  to one of them from a signal and a background      //
//  files. BDTApplier allows to apply some or all of   //
//  this methods in various data files.                //
//                                                     //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"

#include "BDTAnalysis.h"


//_______________________________________________________________________________
//////////////////////////////////
//     BDT CLASSIFIER CLASS     //
//////////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::BDTClassifier::BDTClassifier() :
  fBackgroundFile( 0 ),
  fFactory( 0 ),
  fSignalFile( 0 ),
  fStoreFile( 0 ) { 

  TMVA::Tools::Instance();

  fBackgroundCuts         = "";
  fSignalCuts             = "";

  fUse["BDT"]             = 0;
  fUse["BDTG"]            = 0;
  fUse["BDTB"]            = 0;
  fUse["BDTD"]            = 0;
  fUse["BDTF"]            = 0;

  // --- Cut optimisation
  fUse["Cuts"]            = 0;
  fUse["CutsD"]           = 0;
  fUse["CutsPCA"]         = 0;
  fUse["CutsGA"]          = 0;
  fUse["CutsSA"]          = 0;

  // --- 1-dimensional likelihood ("naive Bayes estimator") 
  fUse["Likelihood"]      = 0;
  fUse["LikelihoodD"]     = 0; // Decorrelated input variables (see option strings)
  fUse["LikelihoodPCA"]   = 0; // PCA-transformed input variables (see option strings)
  fUse["LikelihoodKDE"]   = 0;
  fUse["LikelihoodMIX"]   = 0;
                     
  // --- Mutidimensional likelihood and Nearest-Neighbour methods
  fUse["PDERS"]           = 0;
  fUse["PDERSD"]          = 0;
  fUse["PDERSPCA"]        = 0;
  fUse["PDEFoam"]         = 0;
  fUse["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
  fUse["KNN"]             = 0; // k-nearest neighbour method

  // --- Linear Discriminant Analysis
  fUse["LD"]              = 0; // Linear Discriminant identical to Fisher
  fUse["Fisher"]          = 0;
  fUse["FisherG"]         = 0;
  fUse["BoostedFisher"]   = 0; // uses generalised MVA method boosting
  fUse["HMatrix"]         = 0;                                                                                                             
  // --- Function Discriminant analysis
  fUse["FDA_GA"]          = 0;
  fUse["FDA_SA"]          = 0;
  fUse["FDA_MC"]          = 0;
  fUse["FDA_MT"]          = 0;
  fUse["FDA_GAMT"]        = 0;
  fUse["FDA_MCMT"]        = 0;

  // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
  fUse["MLP"]             = 0; // Recommended ANN
  fUse["MLPBFGS"]         = 0; // MLP and adds optional training method
  fUse["MLPBNN"]          = 0; // MLP and adds BFGS training method and bayesian regulator
  fUse["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH  
  fUse["TMlpANN"]         = 0; // ROOT's own ANN

  // --- Support Vector Machine                
  fUse["SVM"]             = 0;
}

//_______________________________________________________________________________
// Constructor giving BDT type
Analysis::BDTClassifier::BDTClassifier( const char *name,
					const char *file_name,
					const char *opts ) :
  BDTClassifier() {

  this -> SetStoreFile( file_name, "RECREATE" );
  this -> SetFactory( name, opts );
}

//_______________________________________________________________________________
// Destructor
Analysis::BDTClassifier::~BDTClassifier() {

  if ( fBackgroundFile ) { fBackgroundFile -> Close(); delete fBackgroundFile; }
  if ( fSignalFile )     { fSignalFile -> Close();     delete fSignalFile;     }
  if ( fStoreFile )      { fStoreFile -> Close();      delete fStoreFile;      }
  if ( fFactory )        { delete fFactory; }
}

//_______________________________________________________________________________


// -- PUBLIC METHODS  

//_______________________________________________________________________________
// Adds the background file given its name
void Analysis::BDTClassifier::AddBackgroundFile( const char *name,
						 const char *tree_path,
						 Double_t weight ) {

  if ( fBackgroundFile )
    fBackgroundFile -> Close();

  fBackgroundFile       = new TFile( name, "READ" );
  fBackgroundWeight     = weight;

  fFactory -> AddBackgroundTree( (TTree*) fBackgroundFile -> Get( tree_path ),
				 fBackgroundWeight );
}

//_______________________________________________________________________________
// Adds the background file given its pointer
void Analysis::BDTClassifier::AddBackgroundFile( TFile *file,
						 const char *tree_path,
						 Double_t weight ) {

  if ( fBackgroundFile )
    fBackgroundFile -> Close();

  fBackgroundFile       = file;
  fBackgroundWeight     = weight;

  fFactory -> AddBackgroundTree( (TTree*) fBackgroundFile -> Get( tree_path ),
				 fBackgroundWeight );
}

//_______________________________________________________________________________
// Adds the signal file given its name
void Analysis::BDTClassifier::AddSignalFile( const char *name,
					     const char *tree_path,
					     Double_t weight ) {

  if ( fSignalFile )
    fSignalFile -> Close();

  fSignalFile       = new TFile( name, "READ" );
  fSignalWeight     = weight;

  fFactory -> AddSignalTree( (TTree*) fSignalFile -> Get( tree_path ),
			     fSignalWeight );
}

//_______________________________________________________________________________
// Adds the signal file given its pointer
void Analysis::BDTClassifier::AddSignalFile( TFile *file,
					     const char *tree_path,
					     Double_t weight ) {

  if ( fSignalFile )
    fSignalFile -> Close();

  fSignalFile       = file;
  fSignalWeight     = weight;

  fFactory -> AddSignalTree( (TTree*) fSignalFile -> Get( tree_path ),
			     fSignalWeight );
}

//_______________________________________________________________________________
// Performs the classification
void Analysis::BDTClassifier::Classificate( const char *opts ) {

  if ( !opts )
    opts = "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V";

  fFactory -> PrepareTrainingAndTestTree( fSignalCuts, fBackgroundCuts, opts );

  fFactory -> TrainAllMethods();
  fFactory -> TestAllMethods();
  fFactory -> EvaluateAllMethods();

  fStoreFile -> Close();

  delete fStoreFile;   fStoreFile = 0;
  delete fFactory;     fFactory   = 0;
}

//_______________________________________________________________________________
// Adds an option to the book method
void Analysis::BDTClassifier::BookMethod( const char *method, const char *opts ) {

  fUse[method] = 1;

  // Default options
  if ( !opts )
    {
      if ( !strcmp( method, "BDT" ) )
	opts = "!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning";
    
      else if ( !strcmp( method, "BDTG" ) )
	opts = "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5";

      else if ( !strcmp( method, "BDTB" ) )
	opts = "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning";

      else if ( !strcmp( method, "BDTD" ) )
	opts = "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate";
    
      else if ( !strcmp( method, "BDTF" ) )
	opts = "!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning";
    
      else if ( !strcmp( method, "Cuts" ) )
	opts = "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart";

      else if ( !strcmp( method, "CutsD" ) )
	opts = "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate";

      else if ( !strcmp( method, "CutsPCA" ) )
	opts = "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA";
    
      else if ( !strcmp( method, "CutsGA" ) )
	opts = "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95";
    
      else if ( !strcmp( method, "CutsSA" ) )
	opts = "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale";
    
      else if ( !strcmp( method, "Likelihood" ) )
	opts = "H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50";
    
      else if ( !strcmp( method, "LikelihoodD" ) )
	opts = "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate";
    
      else if ( !strcmp( method, "LikelihoodPCA" ) )
	opts = "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA"; 

      else if ( !strcmp( method, "LikelihoodKDE" ) )
	opts = "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50"; 

      else if ( !strcmp( method, "LikelihoodMIX" ) )
	opts = "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50"; 
    
      else if ( !strcmp( method, "PDERS" ) )
	opts = "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600";

      else if ( !strcmp( method, "PDERSD" ) )
	opts = "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate";

      else if ( !strcmp( method, "PDERSPCA" ) )
	opts = "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA";

      else if ( !strcmp( method, "PDEFoam" ) )
	opts = "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T";
    
      else if ( !strcmp( method, "PDEFoamBoost" ) )
	opts = "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T";

      else if ( !strcmp( method, "KNN" ) )
	opts = "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim";

      else if ( !strcmp( method, "HMatrix" ) )
	opts = "!H:!V";

      else if ( !strcmp( method, "LD" ) )
	opts = "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10";
    
      else if ( !strcmp( method, "Fisher" ) )
	opts = "H:!V:Fisher:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10";
    
      else if ( !strcmp( method, "FisherG" ) )
	opts = "H:!V:VarTransform=Gauss";

      else if ( !strcmp( method, "BoostedFisher" ) )
	opts = "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2";

      else if ( !strcmp( method, "FDA_MC" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1";

      else if ( !strcmp( method, "FDA_GA" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1";

      else if ( !strcmp( method, "FDA_SA" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale";

      else if ( !strcmp( method, "FDA_MT" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch";

      else if ( !strcmp( method, "FDA_GAMT" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim";

      else if ( !strcmp( method, "FDA_MCMT" ) )
	opts = "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20";

      else if ( !strcmp( method, "MLP" ) )
	opts = "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator";

      else if ( !strcmp( method, "MLPBFGS" ) )
	opts = "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator";

      else if ( !strcmp( method, "MLPBNN" ) )
	opts = "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator";

      else if ( !strcmp( method, "CFMlpANN" ) )
	opts = "!H:!V:NCycles=2000:HiddenLayers=N+1,N" ;

      else if ( !strcmp( method, "TMlpANN" ) )
	opts = "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3" ;

      else if ( !strcmp( method, "SVM" ) )
	opts = "Gamma=0.25:Tol=0.001:VarTransform=Norm";

      else if ( !strcmp( method, "RuleFit" ) )
	opts = "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02";

    }

  if ( ( (TString) method ).Contains( "BDT" ) )
    fFactory -> BookMethod( TMVA::Types::kBDT, method, opts );

  else if ( ( (TString) method ).Contains( "Cuts" ) )
    fFactory -> BookMethod( TMVA::Types::kCuts, method, opts );

  else if ( ( (TString) method ).Contains( "Likelihood" ) )
    fFactory -> BookMethod( TMVA::Types::kLikelihood, method, opts );

  else if ( ( (TString) method ).Contains( "PDERS" ) )
    fFactory -> BookMethod( TMVA::Types::kPDERS, method, opts );

  else if ( ( (TString) method ).Contains( "PDERS" ) )
    fFactory -> BookMethod( TMVA::Types::kPDERS, method, opts );

  else if ( ( (TString) method ).Contains( "PDEFoam" ) )
    fFactory -> BookMethod( TMVA::Types::kPDEFoam, method, opts );

  else if ( ( (TString) method ).Contains( "KNN" ) )
    fFactory -> BookMethod( TMVA::Types::kKNN, method, opts );

  else if ( ( (TString) method ).Contains( "HMatrix" ) )
    fFactory -> BookMethod( TMVA::Types::kHMatrix, method, opts );

  else if ( ( (TString) method ).Contains( "LD" ) )
    fFactory -> BookMethod( TMVA::Types::kLD, method, opts );

  else if ( ( (TString) method ).Contains( "Fisher" ) )
    fFactory -> BookMethod( TMVA::Types::kFisher, method, opts );

  else if ( ( (TString) method ).Contains( "FDA" ) )
    fFactory -> BookMethod( TMVA::Types::kFDA, method, opts );

  else if ( ( (TString) method ).Contains( "MLP" ) )
    fFactory -> BookMethod( TMVA::Types::kMLP, method, opts );

  else if ( ( (TString) method ).Contains( "CFMlpANN" ) )
    fFactory -> BookMethod( TMVA::Types::kCFMlpANN, method, opts );

  else if ( ( (TString) method ).Contains( "TMlpANN" ) )
    fFactory -> BookMethod( TMVA::Types::kTMlpANN, method, opts );

  else if ( ( (TString) method ).Contains( "SVM" ) )
    fFactory -> BookMethod( TMVA::Types::kSVM, method, opts );
}

//_______________________________________________________________________________
// Creates the factory
void Analysis::BDTClassifier::SetFactory( const char *name, const char *opts ) {

  if ( !fStoreFile )
    fStoreFile = new TFile( "TMVA.root", "RECREATE" );
  if ( !opts )
    opts = "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification";

  fFactory = new TMVA::Factory( name, fStoreFile, opts );
}

//_______________________________________________________________________________
// Sets the file where the TMVA output is going to be written
void Analysis::BDTClassifier::SetStoreFile( const char *file_name,
					    const char *opt) {

  if ( fStoreFile ) {
    fStoreFile -> Close();
    delete fStoreFile;
  }

  fStoreFile = new TFile( file_name, opt );

  if ( fFactory ) {
    delete fFactory;
    fFactory = 0;
  }
}

//_______________________________________________________________________________



//_______________________________________________________________________________
///////////////////////////////
//     BDT APPLIER CLASS     //
///////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::BDTApplier::BDTApplier() : fReader( 0 ) { }

//_______________________________________________________________________________
// Constructor given a name
Analysis::BDTApplier::BDTApplier( const char *name,
				  const char *factory_name,
				  const char *opts) : BDTApplier() {

  fName = name;
  fFactoryName = factory_name;
  this -> SetReader( opts );
}

//_______________________________________________________________________________
// Destructor
Analysis::BDTApplier::~BDTApplier() { if ( fReader ) delete fReader; }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Applies the methods to the data
void Analysis::BDTApplier::Apply() {

  TTree *input_tree;
  TTree *output_tree;

  const Int_t n_methods    = fMethods.size();
  const Int_t n_variables  = fVariables.size();
  const Int_t n_spectators = fSpectators.size();
  Long64_t nentries;

  Double_t methods_val[n_methods];
  Float_t variables[n_variables];
  Float_t spectators[n_spectators];
  TLeaf *leafs[n_variables];


  for ( Int_t i = 0; i < n_variables; i++ )
    fReader -> AddVariable( fVariables[i], &variables[i] );

  for ( Int_t i = 0; i < n_spectators; i++ )
    fReader -> AddSpectator( fSpectators[i], &spectators[i] );

  for ( Int_t i = 0; i < n_methods; i++)
    fReader -> BookMVA( fMethods[i],
			TString( "weights/" )     + (TString) fFactoryName +
			TString( "_" )            + (TString) fMethods[i]  + 
			TString( ".weights.xml" ) );

  for ( Int_t ifile = 0; ifile < this -> GetNReadFiles(); ifile++ ) {

    std::cout << "\n --- Working with file: " <<
      this -> GetReadFile( ifile ) -> GetName() << std::endl;


    input_tree = (TTree*) this -> GetReadFile( ifile ) -> Get( fTrees[ifile] );
    GetWriteFile( ifile ) -> cd();
    output_tree = input_tree -> CloneTree( 0 );


    for ( Int_t i = 0; i < n_methods; i++)
      output_tree -> Branch( fMethods[i],
			     &methods_val[i],
			     ( (TString) fMethods[i] ) + "/D" );


    for ( Int_t i = 0; i < n_variables; i++)
      leafs[i] = input_tree -> GetLeaf( fVariables[i] );


    nentries = input_tree -> GetEntries();
    for ( Long64_t ievt = 0; ievt < nentries; ievt++) {

      input_tree -> GetEntry( ievt );

      for ( Int_t i = 0; i < n_variables; i++)
	variables[i] = leafs[i] -> GetValue();
      for ( Int_t i = 0; i < n_methods; i++)
	methods_val[i] = fReader -> EvaluateMVA( fMethods[i] );

      
      output_tree -> Fill();

      std::cout << " Progress: " << 100*( ievt + 1 )/nentries << " %\r";
      std::cout.flush();
    }

    std::cout << std::endl << " Saving output tree...\n" << std::endl;
    output_tree -> AutoSave();

    this -> CloseReadFile( ifile );
  }
}

//_______________________________________________________________________________
// Sets the path to the input tree of file in <ifile>
void Analysis::BDTApplier::SetInputTreePath( Int_t ifile, const char *path ) {

  if ( (Int_t) fTrees.size() < ( ifile + 1 ) ) {
    while ( (Int_t) fTrees.size() != ( ifile + 1 ) )
      fTrees.push_back( "" );
  }
  
  fTrees[ifile] = path;
}

//_______________________________________________________________________________
// Sets the reader
void Analysis::BDTApplier::SetReader( const char *opts ) {

  if ( fReader )
    fReader -> SetOptions( opts );
  else
    fReader = new TMVA::Reader( opts );
}
