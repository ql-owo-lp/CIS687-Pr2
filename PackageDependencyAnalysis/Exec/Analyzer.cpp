
//////////////////////////////////////////////////////////////////////////
//	Analyzer.cpp - to test Analyzer class								//
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
This is for testing Analyzer.

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

#ifdef TEST_ANALYZER

#include <string>
#include <iostream>
#include "Analyzer.h"

//----< test analyzer >-----------------------------------
int main()
{
	std::cout << "\n\n Now test Analyzer";
	std::vector<std::string> patterns;
	patterns.push_back("*.h");
	patterns.push_back("*.cpp");
	Analyzer anal("../", patterns, true);
	anal.doWork();
	std::cout << "\n\n Test Finished! \n";
}
#endif
