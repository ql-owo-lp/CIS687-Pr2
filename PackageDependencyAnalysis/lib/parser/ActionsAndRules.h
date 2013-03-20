#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
Module Operations: 
==================
This module defines several action classes.  Its classes provide 
specialized services needed for specific applications.  The modules
Parser, SemiExpression, and Tokenizer, are intended to be reusable
without change.  This module provides a place to put extensions of
these facilities and is not expected to be reusable. 

Public Interface:
=================
Toker t(someFile);              // create tokenizer instance
SemiExp se(&t);                 // create a SemiExp attached to tokenizer
Parser parser(se);              // now we have a parser
Rule1 r1;                       // create instance of a derived Rule class
Action1 a1;                     // create a derived action
r1.addAction(&a1);              // register action with the rule
parser.addRule(&r1);            // register rule with parser
while(se.getSemiExp())          // get semi-expression
parser.parse();               //   and parse it

Build Process:
==============
Required files
- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp, TokenProcessor.h
ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp

Build commands (either one)
- devenv CodeAnalysis.sln
- cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.1 : 17 Mar 13
- added reading more type of codes
ver 2.0 : 01 Jun 11
- added processing on way to building strong code analyzer
ver 1.1 : 17 Jan 09
- changed to accept a pointer to interfaced ITokCollection instead
of a SemiExpression
ver 1.0 : 12 Jan 06
- first release

*/
//
#include <queue>
#include <string>
#include "Parser.h"
#include "ITokCollection.h"
/////////////////////////////////////
// Add additional include here - Kevin Wang
#include "StringHelper.h"
#include "TokenProcessor.h"
////////////////////////////////////

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule {
public:
	bool doTest(ITokCollection*& pTc) {
		//std::cout << "\n--BeginningOfScope rule";
		if(pTc->find("{") < pTc->length()) doActions(pTc);
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction {
	Repository* p_Repos;
public:
	HandlePush(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		element elem;
		elem.type = "unknown";
		elem.name = "anonymous";
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule {
public:
	bool doTest(ITokCollection*& pTc) {
		//std::cout << "\n--EndOfScope rule";
		if(pTc->find("}") < pTc->length()) doActions(pTc);
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction {
	Repository* p_Repos;
public:
	HandlePop(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		if(p_Repos->scopeStack().size() == 0)
			return;
		p_Repos->scopeStack().pop();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule {
public:
	bool doTest(ITokCollection*& pTc) {
		//std::cout << "\n--PreprocStatement rule";
		if(pTc->find("#") < pTc->length()) {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule {
public:
	bool isSpecialKeyWord(const std::string& tok) {
		const static std::string keys[]
		= { "for", "while", "switch", "if", "catch" };
		for(int i=0; i<5; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc) {
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{") {
			size_t len = tc.find("(");
			if(len < tc.length() && !isSpecialKeyWord(tc[len-1])) {
				//std::cout << "\n--FunctionDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction {
	Repository* p_Repos;
	// get return type
	void readReturnType(ITokCollection*& pTc, size_t _pos, const std::string& name, const std::string& ns) {
		// analyze the return value type
		if (_pos>0) {	// if this is not a constructor / deconstructor
			for (_pos--;Helper::isSpace((*pTc)[_pos]);_pos--);
			std::string returnType = TokenProcessor::getTypename(pTc, _pos);
			if (!returnType.empty() && returnType!=name) p_Repos->Symbols()->declareType(returnType, ns);
		}
	}
public:
	PushFunction(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		size_t pos = pTc->find("(") - 1, _pos;
		while (pos>0 && (*pTc)[pos]=="\n") pos--;
		while (pos>2 && (*pTc)[pos-1].find("::")!=std::string::npos && Helper::isLegalTemplatedVariable((*pTc)[pos-2]))
			pos-=2;
		if (pos>1 && (*pTc)[pos-1]=="~") pos--;	// destructor
		_pos = pos;
		element parent_element = TokenProcessor::getKnownParentElem(p_Repos);
		// push function scope
		std::string name = parent_element.name +"::"+ TokenProcessor::getTypename(pTc,pos);
		TokenProcessor::appendTemplate(name, TokenProcessor::getTypenameTemplate(pTc, pos=0));	// regardless of the pos
		if ((*pTc)[pTc->find("(")-1]=="operator")	// operator() ()
			name+="()";
		element elem;
		elem.type = "function";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->Symbols()->defineType(name);
		readReturnType(pTc, _pos, name, parent_element.name);
		// pop anonymous scope
		p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);
	}
};


///////////////////////////////////////////////////////////////
// rule to detect statement, including variable declaration

class SimicolonDetector : public IRule
{
	// ignore some special phrases (see http://goo.gl/VC7Xn)
	static bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
		= { "typedef", "typename", "return", "for", "while", "catch", "if" };
		for(int i=0; i<7; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc.length()>1 && tc[tc.length()-1] == ";") {  // when .length()==1, means it is an empty phrase, or a {..};
			for (size_t i=0;i<tc.length()-1;i++)
				if (isSpecialKeyWord(tc[i]))
					return false;
			// possible captured: statement (var=value), increment (var++), variable declaration (Type var;)
			// function call (func()), function definition (void func()), anything (var;)
			//std::cout<<"\n Simicolon detected!";
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// rule to detect using namespace statements

class UsingNamespaceDefinition : public IRule {
public:
	bool doTest(ITokCollection*& pTc) {
		if ((*pTc)[0] == "using" && pTc->find("namespace")<pTc->length()) {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push field "using namespace" onto ScopeStack

class PushUsingNamespace : public IAction {
	Repository* p_Repos;
public:
	PushUsingNamespace(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		//std::cout<<"\n using namespace "<< pTc->show().c_str();
		// delete public / private / protected
		size_t pos=pTc->find("namespace")+1;
		while (pos<pTc->length() && Helper::isSpace((*pTc)[pos])) pos++;
		std::string name=TokenProcessor::getTypename(pTc, pos);
		if (!name.empty())
			p_Repos->Symbols()->useNamespace(name);
	}
};

///////////////////////////////////////////////////////////////
// action to push type name onto ScopeStack

class PushType : public IAction {
	Repository* p_Repos;
public:
	PushType(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		// push type scope
		size_t pos=0;
		std::string name = TokenProcessor::getTypename(pTc, pos);
		if (name.empty())	// anonymous type
			name="(anonymous type)";
		name = TokenProcessor::getKnownParentElem(p_Repos).name+"::"+name;
		element elem;
		elem.type = "type";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		// pop anonymous scope
		p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);	// replace anonymous scope with "type" scope
		//std::cout<<"\n type "<<name;
		p_Repos->Symbols()->defineType(name);
	}
};

///////////////////////////////////////////////////////////////
// action to push type inheritance relationship onto ScopeStack

class PushTypeInheritance : public IAction {
	Repository* p_Repos;
public:
	PushTypeInheritance(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		element parent_elem = TokenProcessor::getKnownParentElem(p_Repos);
		size_t pos = pTc->find(":")+1;
		std::string name = "(initial value)";	// to start the following progress
		while (pos<pTc->length() && Helper::isLegalTemplatedVariable((*pTc)[pos]) && !name.empty()) {
			name  = TokenProcessor::getTypename(pTc, pos);
			//std::cout<<"\n inheritance "<<name;
			p_Repos->Symbols()->declareType(name, parent_elem.name);
			if (pos<pTc->length()-1 && (*pTc)[pos]==",") pos++;
		}
	}
};

///////////////////////////////////////////////////////////////
// rule to detect special statement, like "for", "while", "catch"

class BracketStatementDefinition : public IRule {
	std::string keyword;
public:
	BracketStatementDefinition(const std::string & key): keyword(key) {}
	bool doTest(ITokCollection*& pTc) {
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{" || tc[tc.length()-1] == ";") {
			size_t len = tc.find("(");	// check len>0 in case it's (statement);
			if(len>0 && len < tc.length()-1 && tc[len-1]==keyword) {
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// push exception capture onto stack

class PushBracketStatement : public IAction {
	Repository* p_Repos;
	std::string eType;
	std::string eName;
public:
	PushBracketStatement(Repository* pRepos, std::string etype, std::string ename) 
		: p_Repos(pRepos), eType(etype), eName(ename) {}
	void doAction(ITokCollection*& pTc) {
		size_t pos = pTc->find("(")+1;
		std::string scopeNS = TokenProcessor::getKnownParentElem(p_Repos).name +"::"+ eName;
		TokenProcessor::readTokCollection(pTc, pos, p_Repos, scopeNS);
		element elem;
		elem.type = eType;
		elem.name = scopeNS;
		elem.lineCount = p_Repos->lineCount();
		if((*pTc)[pTc->length()-1] == "{") {	// sometimes there is no scope follows
			p_Repos->scopeStack().pop();
			p_Repos->scopeStack().push(elem);
		}
	}
};

///////////////////////////////////////////////////////////////
// detect scope

class ScopeDetector : public IRule {
	std::string keywords;
public:
	ScopeDetector(std::string key) : keywords("|"+key+"|") {}
	bool doTest(ITokCollection*& pTc) {
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{") {
			// after template
			size_t pos=0;
			TokenProcessor::getTypenameTemplate(pTc, pos);	// don't care the value, just move the pos
			for (;pos<pTc->length() && Helper::isSpace(tc[pos]);pos++);
			if (keywords.find(tc[pos])!=std::string::npos) {
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// push scope onto stack

class PushScope : public IAction {
	Repository* p_Repos;
	std::string eType;
	std::string eName;
	bool insertNS;
public:
	PushScope(Repository* pRepos, std::string etype, std::string ename="(anonymous)", bool insertNs=true) 
		: p_Repos(pRepos), eType(etype), eName(ename), insertNS(insertNs) {}
	void doAction(ITokCollection*& pTc) {
		//std::cout<<"\n push scope "<<pTc->show().c_str();
		size_t pos = 1;
		if (pos>=pTc->length()) return;	// no name is found
		std::string name = TokenProcessor::getTypename(pTc, pos);
		if (name.empty()) name = eName;
		if (insertNS)
			name = TokenProcessor::getKnownParentElem(p_Repos).name+"::"+name;
		element elem;
		elem.type = eType;
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		if (!name.empty())
			p_Repos->Symbols()->defineType(name);
		// pop anonymous scope
		p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// action to push field statement onto ScopeStack

class PushField : public IAction {
	Repository* p_Repos;
	bool isSpecialWord(std::string const & str) {
		static const std::string keys="|public|private|protected|case|";
		return Helper::isSpace(str) || keys.find("|"+str+"|")!=std::string::npos;
	}
public:
	PushField(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		//std::cout<<"\n field "<< pTc->show().c_str();
		// delete public / private / protected
		size_t pos=0;
		while (pos<pTc->length() && (isSpecialWord((*pTc)[pos]) || (*pTc)[pos].find(':')!=std::string::npos)) pos++;
		// find the first word
		TokenProcessor::readTokCollection(pTc, pos, p_Repos, TokenProcessor::getKnownParentElem(p_Repos).name);
	}
};

///////////////////////////////////////////////////////////////
// push include files

class PushInclude : public IAction {
	Repository* p_Repos;
public:
	PushInclude(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		ITokCollection& tc = *pTc;
		if (tc[1]!="include") return;
		// read the < or "
		size_t pos=2;
		std::string inc;
		while (pos<pTc->length() && Helper::isSpace((*pTc)[pos])) pos++;
		if ((*pTc)[pos]=="<") {
			while (++pos<pTc->length() && (*pTc)[pos].find('>')==std::string::npos)
				inc+=(*pTc)[pos];
			if (pos<pTc->length() && (*pTc)[pos]!=">")
				inc+=(*pTc)[pos].substr(0,(*pTc)[pos].length()-1);	// delete the last >
		}
		else
			inc = (*pTc)[pos].substr(1,(*pTc)[pos].length()-2);	// delete the quote mark
		p_Repos->Symbols()->includeFile(inc);
	}
};

///////////////////////////////////////////////////////////////
// action to push function argument to repository

class PushArgument : public IAction
{
	Repository* p_Repos;
public:
	PushArgument(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc) {
		element parent_elem = TokenProcessor::getKnownParentElem(p_Repos);
		for (size_t pos=pTc->find("(")+1;pos<pTc->length()-1;) {
			// read one token, which is argument type
			std::string type = TokenProcessor::getTypename(pTc, pos);
			// read one token, which is argument variable
			std::string argv = TokenProcessor::getTypename(pTc, pos);
			if (type.empty() || argv.empty()) break;
			// declare type, define argv
			argv = parent_elem.name+"::"+argv;
			p_Repos->Symbols()->declareType(type, parent_elem.name);
			p_Repos->Symbols()->defineType(argv);
			pos<pTc->length() && (*pTc)[pos].find(',')!=std::string::npos ? pos++ : 0;
		}
	}
};

///////////////////////////////////////////////////////////////
// rule to detect typedef alias

class TypeDefDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == ";" && tc.find("typedef")<tc.length()) {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push typedef name onto ScopeStack

class PushTypeDef : public IAction
{
	Repository* p_Repos;
public:
	PushTypeDef(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		std::string alias;
		size_t pos = pTc->find("(");
		element parent_elem = TokenProcessor::getKnownParentElem(p_Repos);
		if (pos>=pTc->length()) {	// doesn't find
			pos = pTc->find("typedef")+1;
			std::string name = TokenProcessor::getTypename(pTc,pos);
			std::locale loc;
			for (pos=pTc->length()-2;pos>0 && isspace((*pTc)[pos][0], loc);pos--) ;
			alias = (*pTc)[pos];
			// declare name, define alias
			p_Repos->Symbols()->declareType(name, parent_elem.name);
		}
		else  // function pointer
			alias = TokenProcessor::getTypename(pTc,pos);
		alias = parent_elem.name +"::"+ alias;
		p_Repos->Symbols()->defineType(alias);
	}
};

#endif
