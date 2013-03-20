
//////////////////////////////////////////////////////////////////////////
//	Main.cpp - the entry point of this program.							//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis, CIS687 Pr#1, 2013 Spring	//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Module Operations: 
==================
This is entry point of this program.  It only extracts command line parameters 
from user input and passes them to Analyzer, does no other thing.

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
Analyzer.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 first version

*/

#include <iostream>
#include <string>
#include "Analyzer.h"

///////////////////////////////////////////////////////////////
// main entry point of the program

int main(int argc, char* argv[])
{
	try {
		std::cout << "\nPackage Dependency Analysis starts! - by Kevin Wang"<< std::string(10,' ')<<"\nfor Spring 2013, Pr#2"<< std::endl;
		bool isRecursive = false;
		if (argc>1) isRecursive=(std::string(argv[1])=="\\R");
		// see if command arguments are valid
		if(argc < 2 || (isRecursive && argc<3)) {
			std::cout<< "\n  please enter name of file to process on command line\n\n";
			return 1;
		}
		std::vector<std::string> patterns;
		std::string path = isRecursive ? argv[2] : argv[1];
		// get all patterns
		for (int i=isRecursive ? 3 : 2; i<argc; ++i) patterns.push_back(argv[i]);
		if (patterns.size()<1) {	// no value is specific
			patterns.push_back("*.h");
			patterns.push_back("*.cpp");
		}
		// start analyze
		Analyzer anal(path, patterns, isRecursive);
		anal.doWork();
	}
	catch (std::exception& ex) {
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
	return 0;
}