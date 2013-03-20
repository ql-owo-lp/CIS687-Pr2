
//////////////////////////////////////////////////////////////////////////
//	FileManager.cpp - to test FileManager package						//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis, CIS687 Pr#1, 2013 Spring	//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Package Operations:
===================
Test function of FileManager

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
FileManager.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 : initial version.

*/

#ifdef TEST_FILEMANAGER

#include "FileManager.h"

//----< test analyzer >-----------------------------------
int main()
{
	std::cout << "\n\n Now test FileManager";
	std::vector<std::string> patterns;
	patterns.push_back("*.h");
	patterns.push_back("*.cpp");
	std::vector<std::string> files = FileManager::getFileList("../", true, patterns);
	if (FileManager::parse(files)) {
		std::cout << "\n All file parsed! \n";
	}
	FileManager::save("./test.txt", "SAVE SUCCEED!");
	std::cout << "\n\n Test Finished! \n";
}
#endif