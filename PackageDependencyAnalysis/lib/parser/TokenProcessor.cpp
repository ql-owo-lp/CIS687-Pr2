
//////////////////////////////////////////////////////////////////////////
//	TokenProcessor.cpp - test Token Processor							//
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
To test the Token Processor

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
TokenProcessor.h, SemiExpression.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 : initial version.

*/

#ifdef TEST_TOKENPROCESSOR

#include <string>
#include <iostream>
#include "SemiExpression.h"
#include "TokenProcessor.h"

void _test1() {
	// simulate a tokcollection:
	// Graph<std::string, std::string> g=new Graph<std::string, std::string>;
	SemiExp test(0);
	test.push_back("Graph");
	test.push_back("<");
	test.push_back("std::string");
	test.push_back(",");
	test.push_back("std::string");
	test.push_back(">");
	test.push_back("g");
	test.push_back("=");
	test.push_back("new");
	test.push_back("Graph");
	test.push_back("<");
	test.push_back("std::string");
	test.push_back(",");
	test.push_back("std::string");
	test.push_back(">");
	test.push_back(";");
	std::cout << "\n  " << test.show();
	std::string type = TokenProcessor::getTypename(&test, 0);
	std::cout<<"\n type read "<< type;

}

void _test2() {
	// function : template<> Graph<std::string, std::string> create() {
	SemiExp test1(0);
	test1.push_back("template");
	test1.push_back("<>");
	test1.push_back("Graph");
	test1.push_back("<");
	test1.push_back("std::string");
	test1.push_back(",");
	test1.push_back("std::string");
	test1.push_back(">::");
	test1.push_back("create");
	test1.push_back("(");
	test1.push_back(")");
	test1.push_back("{");
	std::string templ = TokenProcessor::getTypenameTemplate(&test1, 0);
	std::cout<<"\n template read "<< type;

	TokenProcessor::appendTemplate(type,templ);
	std::cout<<"\n new name "<< type;
}

int main()
{
	std::cout << "\n  testing Token Processor(...)";
	std::cout << "\n -----------------------------";
	test1();
	test2();
}
#endif