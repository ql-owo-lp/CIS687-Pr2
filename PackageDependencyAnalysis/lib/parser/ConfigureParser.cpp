/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <string>
#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "SymbolTable.h"

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated
	delete pHandlePush;
	delete pBeginningOfScope;
	delete pHandlePop;
	delete pEndOfScope;
	delete pPushFunction;
	delete pFunctionDefinition;
	delete pFR;
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	delete pPushType;
	delete pTypeDefDefinition;
	delete pPushTypeDef;
	delete pPushArgument;
	delete pPushField;
	delete pSimicolonDetector;
	delete pPreprocStatement;
	delete pPushInclude;
	delete pUsingNamespaceDefinition;
	delete pPushUsingNamespace;
	for (size_t i=0;i<4;i++) delete pSDetector[i];
	for (size_t i=0;i<3;i++) delete pPushS[i];
	for (size_t i=0;i<7;i++) delete pPushBS[i];
	for (size_t i=0;i<6;i++) delete pBSDefinition[i];
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if(pToker == 0)
		return false;
	pRepo->Symbols() = isFile ? SymbolTable::create(name) : SymbolTable::create("anonymous");	// anonymous file
	return pToker->attach(name, isFile);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build()
{
	try
	{
		// add Parser's main parts
		pToker = new Toker;
		pToker->returnComments(false);	// set as do not return comment
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker);
		// add code folding rules

		pFR = new codeFoldingRules;
		pParser->addFoldingRules(pFR);
		initRules();
		initActions();
		loadActions();
		loadRules();
		// push a empty file scope here, so that we need not to worry out of stack index
		element fileScope;
		fileScope.type="file";
		//fileScope.name="";
		fileScope.lineCount = 0;
		pRepo->scopeStack().push(fileScope);

		return pParser;
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

//----< init rules >------------

void ConfigParseToConsole::initRules() {
	pBeginningOfScope = new BeginningOfScope;
	pEndOfScope = new EndOfScope;
	pPreprocStatement = new PreprocStatement;
	pUsingNamespaceDefinition = new UsingNamespaceDefinition;
	pSDetector[0] = new ScopeDetector("try");
	pSDetector[1] = new ScopeDetector("namespace");
	pSDetector[2] = new ScopeDetector("class|struct|union|enum");
	pSDetector[3] = new ScopeDetector("do");
	pFunctionDefinition = new FunctionDefinition;
	pTypeDefDefinition = new TypeDefDefinition;
	pBSDefinition[0] = new BracketStatementDefinition("catch");
	pBSDefinition[1] = new BracketStatementDefinition("for");
	pBSDefinition[2] = new BracketStatementDefinition("while");
	pBSDefinition[3] = new BracketStatementDefinition("if");
	pBSDefinition[4] = new BracketStatementDefinition("else");
	pBSDefinition[5] = new BracketStatementDefinition("switch");
	pSimicolonDetector = new SimicolonDetector;
}
//----< load rules into configure parser >------------

void ConfigParseToConsole::loadRules() {
	// configure to manage scope
	// these must come first - they return true on match
	// so rule checking continues
	pParser->addRule(pBeginningOfScope);
	pParser->addRule(pEndOfScope);
	pParser->addRule(pPreprocStatement);
	pParser->addRule(pUsingNamespaceDefinition);
	for (size_t i=0;i<4;i++)
		pParser->addRule(pSDetector[i]);
	pParser->addRule(pFunctionDefinition);
	pParser->addRule(pTypeDefDefinition);
	for (size_t i=0;i<6;i++)
		pParser->addRule(pBSDefinition[i]);
	pParser->addRule(pSimicolonDetector);

}
//----< init actions >------------

void ConfigParseToConsole::initActions() {
	pHandlePush = new HandlePush(pRepo);
	pHandlePop = new HandlePop(pRepo);
	pPushInclude = new PushInclude(pRepo);
	pPushUsingNamespace = new PushUsingNamespace(pRepo);
	pPushS[0] = new PushScope(pRepo, "exception", "try");
	pPushS[1] = new PushScope(pRepo, "namespace", "(anonymous namespace)");
	pPushS[2] = new PushScope(pRepo, "loop", "while");
	pPushType = new PushType(pRepo);
	pPushFunction = new PushFunction(pRepo);
	pPushArgument = new PushArgument(pRepo);
	pPushTypeInheritance = new PushTypeInheritance(pRepo);
	pPushTypeDef = new PushTypeDef(pRepo);
	pPushBS[0] = new PushBracketStatement(pRepo, "exception", "catch");
	pPushBS[1] = new PushBracketStatement(pRepo, "loop", "for");
	pPushBS[2] = new PushBracketStatement(pRepo, "loop", "while");
	pPushBS[3] = new PushBracketStatement(pRepo, "loop", "while");
	pPushBS[4] = new PushBracketStatement(pRepo, "case", "if");
	pPushBS[5] = new PushBracketStatement(pRepo, "case", "else-if");
	pPushBS[6] = new PushBracketStatement(pRepo, "case", "switch");
	pPushField = new PushField(pRepo);
}
//----< load actions into rules >------------

void ConfigParseToConsole::loadActions() {
	pFunctionDefinition->addAction(pPushFunction);
	// must push function first then push function argument
	pFunctionDefinition->addAction(pPushArgument);
	pTypeDefDefinition->addAction(pPushTypeDef);
	pBSDefinition[0]->addAction(pPushBS[0]);
	pBSDefinition[1]->addAction(pPushBS[1]);
	pBSDefinition[2]->addAction(pPushBS[2]);
	pBSDefinition[2]->addAction(pPushBS[3]);
	pBSDefinition[3]->addAction(pPushBS[4]);
	pBSDefinition[4]->addAction(pPushBS[5]);
	pBSDefinition[5]->addAction(pPushBS[6]);
	pBeginningOfScope->addAction(pHandlePush);
	pEndOfScope->addAction(pHandlePop);
	pPreprocStatement->addAction(pPushInclude);
	pUsingNamespaceDefinition->addAction(pPushUsingNamespace);
	pSDetector[0]->addAction(pPushS[0]);
	pSDetector[1]->addAction(pPushS[1]);
	pSDetector[2]->addAction(pPushType);
	// must push type first then push type inheritance
	pSDetector[2]->addAction(pPushTypeInheritance);
	pSDetector[3]->addAction(pPushS[2]);
	pSimicolonDetector->addAction(pPushField);
}


#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32,'=') << std::endl;

	// collecting tokens from files, named on the command line

	if(argc < 2)
	{
		std::cout 
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for(int i=1; i<argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if(pParser)
			{
				if(!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while(pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch(std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif
