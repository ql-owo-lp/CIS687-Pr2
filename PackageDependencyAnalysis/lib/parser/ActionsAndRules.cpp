/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - implements new parsing rules and actions //
//  ver 2.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ActionsAndRules class\n "
		<< std::string(30,'=') << std::endl;

	try
	{
		//std::queue<std::string> resultsQ;
		PrintPreproc pp;
		PreprocStatement pps;
		pps.addAction(&pp);

		FunctionDefinition fnd;
		PrintFunction prt;
		fnd.addAction(&prt);

		Toker toker("../ActionsAndRules.h");
		SemiExp se(&toker);
		Parser parser(&se);
		parser.addRule(&pps);
		parser.addRule(&fnd);
		while(se.get())
			parser.parse();
		std::cout << "\n\n";
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
	}
}
#endif