
//////////////////////////////////////////////////////////////////////////
//	StringHelper.cpp - A helper class for some string processing		//
//	work.																//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis							//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Package Operations:
===================
To test the String Helper

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
none

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 : initial version.

*/

#ifdef TEST_STRINGHELPER

#include <string>
#include "StringHelper.h"

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing string helper module\n ";
	
	std::cout<<"\n "<<Helper::isSpace('a');
	std::cout<<"\n "<<Helper::isSpace("ass  sd");
	std::cout<<"\n "<<Helper::tolower('A');
	std::cout<<"\n "<<Helper::isModifierKeyword("const");
	std::cout<<"\n "<<Helper::isLegalVariable('$');
	std::cout<<"\n "<<Helper::isLegalVariable("Vertex");
	std::cout<<"\n "<<Helper::isLegalTemplatedVariable("Vertex<V,E>");

	std::cout<<"\n\n";
}
#endif