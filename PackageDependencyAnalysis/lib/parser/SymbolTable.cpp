
//////////////////////////////////////////////////////////////////////////
//	SymbolTable.cpp - test Symbol Table									//
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
To test the Symbol Table Model

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
SymbolTable.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 : initial version.

*/
#include "SymbolTable.h"

// static member must be defined!!
SymbolTable::Table SymbolTable::_t;
SymbolTable::strSet SymbolTable::_nonexistFileList;

#ifdef TEST_SYMBOLTABLE

#include <string>
#include <iostream>

int main()
{
	SymbolTable* pSymbol = SymbolTable::create("Graph.h");

	symbol.defineType("GraphLib::Graph<V,E>");
	symbol.declareType("GraphLib::Graph<V,E>");
	symbol.declareType("Vertex<V,E>", GraphLib);
	symbol.includeFile("XmlReader.h");
	symbol.useNamespace("::GraphLib");
	symbol.analyze();
	symbol.cleanFileInc();

	std::vector<std::string> list1 = symbol.fileInc();
	for (auto & str : list1)
		std::cout<<"\n include file "<<str;
	std::unordered_set<std::string> list3 = symbol.types();
	for (auto it=list3.begin();it!=list3.end();it++)
		std::cout<<"\n type defined "<<*it;

	std::string name = SymbolTable::formatTypeName("Graph<V,E>");
	std::cout<<"\n formated name "<<*it;
	std::string name = SymbolTable::getPackageName("./SymbolTable.h");
	std::cout<<"\n package name "<<*it;

	SymbolTable::analyzeAll();

	std::unordered_set<std::string> list2 = symbol.files();
	for (auto it=list2.begin();it!=list2.end();it++)
		std::cout<<"\n file used "<<*it;
	std::cout << "\n\n";
}
#endif