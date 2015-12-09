/////////////////////////////////////////////////////////
// --------------------------------------------------- //
//                                                     //
// Program to get the luminosity of one or more data   //
// files in standard LHCb notation or given a tree     //
// path and a branch name.                             //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR : Miguel Ramos Pernas                       //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last Update: 09/12/2015                            //
//                                                     //
// --------------------------------------------------- //
//                                                     //
// Usage:                                              //
//                                                     //
// >> ./GetLuminosity path_to_file                     //
// >> ./GetLuminosity path_to_file1 path_to_file2      //
// >> ./GetLuminosity path_to_file1 path_to_file2      //
//    tree_path branch_path                            // 
//                                                     //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////


#include <cstdlib>
#include <iostream>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"


bool ExistsFile(const char*, bool print_opt = true);
void PrintLuminosities(const char**, int, const char*, const char*);


// ===================================== MAIN FUNCTION

int main(int argc, const char *argv[]) {

  int ifile, nfiles = 0;
  bool dec, tkfile = true;

  // Conditionals to define the three posible work options
  if (argc == 1) {
    std::cout << "\n You have to specify at least one target file\n" << std::endl;
    exit(0);
  }
  else if (argc == 2) {
    if (ExistsFile(argv[1]))
      nfiles = 1;
    else
      exit(0);
  }
  else if (argc == 3) {
    if (ExistsFile(argv[1]) && ExistsFile(argv[2]))
      nfiles = 2;
    else
      exit(0);
  }
  else {
    for (ifile = 1; ifile < argc; ifile++) {
      dec = ExistsFile(argv[ifile], false);
      if (!dec && ifile < (argc - 2)) {
	std::cout << "\n The file << " << argv[ifile] << " >> does not exist\n" << std::endl;
	exit(0);
      }
      if (dec && tkfile) 
	nfiles++;
      if (!dec)
	tkfile = false;
    }
  }

  std::cout << "\n Working with "<< nfiles << " file(s)" << std::endl;
   
  // According to the number of files detected and the input parameters selects the work method
  if (nfiles == argc - 2) {
    std::cout << "\n You have to specify both tree name and branch name at the end of the declaration\n" << std::endl;
    exit(0);
  }
  else if (nfiles < argc - 2)
    PrintLuminosities(argv, nfiles, argv[argc - 2], argv[argc -1]);
  else
    PrintLuminosities(argv, nfiles, "GetIntegratedLuminosity/LumiTuple", "IntegratedLuminosity");

  return 0;
}

//_________________________________________
// Function to test if a file exists
bool ExistsFile(const char *file_name, bool print_opt) {

  if (!gSystem->AccessPathName(file_name))
    return true;
  else {
    if (print_opt)
      std::cout << "\n The file << " << file_name << " >> does not exist\n" << std::endl;
    return false;
  }
}

//__________________________________________________________________________________
// Function to get the luminosity from a set of files, trees and branch names
void PrintLuminosities(const char *file_names[], int nfiles, const char tree_path[], const char branch_name[]) {

  TFile *file;
  TTree *tree;
  TLeaf *leaf;
  bool is_able;
  double SumLum;

  // Loop over the input files to get their luminosities
  std::cout << "\n";
  for (int ifile = 1; ifile < nfiles + 1; ifile++) {

    file = new TFile(file_names[ifile], "READ");
    tree = (TTree*) file->Get(tree_path);
    is_able = true;

    // Verifies if the tree path exists and if the object pointed is of class <TTree>
    if (file->Get(tree_path) == NULL || TString(file->Get(tree_path)->ClassName()).CompareTo(TString("TTree"))) {
      std::cout << " The path << " << tree_path << " >> doesn't match a valid path to the luminosity tree\n"
		<< " File: << " << file_names[ifile] << " >>" << std::endl;
      is_able = false;
    }

    // Verifies if the branch exists
    else if (!tree->GetListOfBranches()->Contains(branch_name)) {
      std::cout << " The branch << " << branch_name << " >> doesn't match a valid path to the luminosity branch\n"
		<< " File: << " << file_names[ifile] << " >>" << " Tree: << " << tree_path << " >>\n" << std::endl;
      is_able = false;
    }

    // If it can not work with this file it moves to the next one
    if (is_able) {
      leaf = tree->GetLeaf(branch_name);

      // Loop over the data to get the luminosity
      SumLum = 0;
      for (Long64_t ievt = 0; ievt < tree->GetEntries(); ievt++) {
	tree->GetEntry(ievt);
	if (leaf->GetValue() > 0)
	  SumLum += leaf->GetValue();
      }

      // Prints the file name and its luminosity
      std::cout << " --- " << file_names[ifile] << " ==> " << SumLum <<  std::endl;
      file->Close();
      delete file;
    }
  }

  std::cout << "\n";
}
