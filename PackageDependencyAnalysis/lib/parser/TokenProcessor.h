#ifndef TOKENPROCESSOR_H
#define TOKENPROCESSOR_H
//////////////////////////////////////////////////////////////////////////
//	TokenProcessor.h - Help actions extracting types					//
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
This is a helper class that used to help actions and rules extracting type,
variables more conveniently

Public Interface:
=================
TokenProcessor::getKnownParentElem(p_Repo);
TokenProcessor::readTokCollection(pTc,pos,p_Repo,nameScope);
std::string name = TokenProcessor::getTypenameTemplate(pTc,pos);
TokenProcessor::appendTemplate(name,pos);
std::string name = TokenProcessor::getTypename(pTc,pos);

Build Process:
==============
Required files
- ActionsAndRules.h, itokcollection.h, StringHelper.h

Maintenance History:
====================
ver 0.1 : 19 Mar 13
- first version

*/
#include <sstream>
#include <string>
#include "ScopeStack.h"
#include "StringHelper.h"
#include "itokcollection.h"
#include "SymbolTable.h"
#include "Tokenizer.h"

///////////////////////////////////////////////////////////////
// ScopeStack element is application specific

struct element
{
	std::string type;
	std::string name;
	size_t lineCount;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << lineCount;
		temp << ")"; 
		return temp.str();
	}
};

///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.

class Repository  // application specific
{
	ScopeStack<element> stack;
	Toker* p_Toker;
	/////////////////////////////////////
	// Add one symbol table here - Kevin Wang
	SymbolTable* p_Symbol;
	// End of Add
	////////////////////////////////////
public:
	/////////////////////////////////////
	// Change constructor here - Kevin Wang
	Repository(Toker* pToker) : p_Toker(pToker) {}
	// End of Change
	////////////////////////////////////
	ScopeStack<element>& scopeStack()
	{
		return stack;
	}
	Toker* Toker()
	{
		return p_Toker;
	}
	size_t lineCount()
	{
		return (size_t)(p_Toker->lines());
	}
	/////////////////////////////////////
	// Add some function to link symbol table  - Kevin Wang
	SymbolTable*& Symbols() {
		return p_Symbol;
	}
	// End of Add
	////////////////////////////////////
};

///////////////////////////////////////////////////////////////
// Token Processor class

class TokenProcessor {

	// delete all white space in token collection
	static inline void filterWhitespace(ITokCollection*& pTc, size_t& pos) {
		std::locale loc;
		while (pos<pTc->length() && Helper::isSpace((*pTc)[pos])) pos++;
	}

	// whether current token starts a new statement
	static inline bool isNewStatement(ITokCollection*& pTc, size_t pos) {
		if (pos>=pTc->length() || pos<1)
			return false;
		while (pos>1 && Helper::isSpace((*pTc)[pos])) pos--;
		return (*pTc)[pos]=="(" || (*pTc)[pos]==")" || (*pTc)[pos]==";" 
			|| (*pTc)[pos].find('[')!=std::string::npos || (*pTc)[pos].find(']')!=std::string::npos;
	}

	// delete all methods, such var.method / var->method
	static inline void filterMethod(std::string & name) {
		size_t _pos1 = name.find('.'),_pos2=name.find("->");
		_pos1=(_pos1==std::string::npos)?name.length():_pos1;
		_pos2=(_pos2==std::string::npos)?name.length():_pos2;
		if (_pos1!=_pos2)	// means at least one is found
			name.erase(_pos1 < _pos2?_pos1:_pos2);
	}

	// count the number of bracket in one string
	static int countBracket(const std::string& str, const size_t mode=1) {
		if (str=="<<") return 0;
		int bracket=0;
		for (size_t i=0;i<str.length();i++) {
			switch (str[i]) {
			case '\'':
			case '"':
				bracket=0;break;
			case '<':
				if (mode<=1)
					bracket++; break;
			case '>':
				if (mode>=1 && !(i>0 && str[i-1]=='-'))	// ->
					bracket--;
				break;
			}
		}
		return bracket;
	}

	// read one variable from token collection
	static std::string readVariable(ITokCollection*& pTc, size_t& pos, int& bracket) {
		std::string name;
		ITokCollection& tc = *pTc;
		for (size_t startPos=pos; pos<tc.length();pos++) {
			if (Helper::isSpace(tc[pos]) || Helper::isModifierKeyword(tc[pos])) { startPos++; continue; }	// delete all whitespace again
			bracket+=countBracket(tc[pos]);
			if (tc[pos]==";" || tc[pos]=="(" || tc[pos]==")" || tc[pos].find("[")!=std::string::npos || tc[pos].find("]")!=std::string::npos || bracket<0)
				bracket=0;	// reset bracket count , as the < may really mean less than
			else if (tc[pos]==">>" && bracket<0) bracket+=2;
			else if (tc[pos].find("::")!=std::string::npos || tc[pos].find('.')!=std::string::npos || tc[pos].find("->")!=std::string::npos)
				startPos=pos+1;	// namespace or nested class
			else if (!bracket && tc[pos].find('>')!=std::string::npos) {
				name += tc[pos]; continue;
			}	// NOTICE: else if, in case >::
			if ((int)(pos-startPos)>0 && bracket==0) break;
			name += tc[pos];	// pos should be at the next available position
		}
		while (pos<tc.length() && tc[pos]==")") pos++;	// does not filter ( here
		filterMethod(name);
		return name;
	}

	// fix variable, delete special words
	static void fixVariable(ITokCollection*& pTc, size_t& pos, int& bracket,std::string& name,size_t initPos) {
		ITokCollection& tc = *pTc;
		if (bracket || (countBracket(name,0)+countBracket(name,2)!=0)) {
			// if this is a less than or greater than
			size_t p1=name.find('<'), p2=name.find('>'), p3=p1<p2?p1:p2;
			if (p3!=std::string::npos) {
				if (name.substr(0, p3)!="operator") {
					name = name.substr(0, p3);
					if (p3>0)
						while (pos>0 && tc[pos].find('<')==std::string::npos && tc[pos].find('>')==std::string::npos) pos--;
					else
						pos=initPos+1;
				}
			}
			else name="";	// error
		}
	}

public:

	///////////////////////////////////////////////////////////////
	// this func is used to get the parent element, extremely useful when trying figuring out in which scope we are now

	static element getKnownParentElem(Repository* pRepos, const std::string& type="type|namespace|function|exception|loop|case") {
		element elem, res;
		if (pRepos->scopeStack().size()<1)	// return an empty element
			return res;
		elem = pRepos->scopeStack().pop();
		res = 
			(("|"+type+"|").find("|"+elem.type+"|") == std::string::npos && pRepos->scopeStack().size()>0)
			? getKnownParentElem(pRepos,type)	// call recursively
			: elem;
		// push back
		pRepos->scopeStack().push(elem);
		return res;
	}

	///////////////////////////////////////////////////////////////
	// get the template perfix of one member outside the class scope

	static std::string getTypenameTemplate(ITokCollection*& pTc, size_t& pos) {
		ITokCollection& tc = *pTc;
		size_t p=tc.find("template")+1;
		filterWhitespace(pTc, p);
		if (p>tc.length()-1) return "";
		std::string templ;
		size_t bracket=0;
		for (bracket = countBracket(tc[p]);bracket>0 && p<tc.length()-1;bracket+=countBracket(tc[++p])) {
			if (tc[p]!="class" && tc[p]!="typename" && !Helper::isSpace(tc[p]))
				templ += tc[p];
		}
		// normally bracket will be zero at this stage
		!bracket ? templ += tc[p++] : templ = "";
		// the pos should be at the next available position
		if (pos<p) pos = p;
		return templ;
	}

	///////////////////////////////////////////////////////////////
	// append template style to current variable

	static void appendTemplate(std::string & name, const std::string & templ) {
		if (!templ.empty() && name.find('>')==std::string::npos) {
			size_t p1 = name.find_last_not_of('&'), p2= name.find_last_not_of('*'), p = (p1<p2?p1:p2)+1;
			p<name.length() ? name.insert(p, templ) : name.append(templ);
		}
	}

	///////////////////////////////////////////////////////////////
	// get type name from the token collection
	// "pos" is starting with which position

	static std::string getTypename(ITokCollection*& pTc, size_t& pos, bool readTemplate=false) {
		if (pos>=pTc->length()) return "";
		ITokCollection& tc = *pTc;
		std::string name, templ = readTemplate ? getTypenameTemplate(pTc,pos) : "";
		while (pos<pTc->length() && (Helper::isSpace(tc[pos]) || tc[pos]=="(" || tc[pos]==")")) pos++;	// ignore modifier and whitespace
		size_t initPos=pos;
		int bracket=0;
		if (pos<pTc->length() && tc[pos]=="~") {
			name+="~";
			pos++;
		}
		name+=readVariable(pTc, pos, bracket);
		fixVariable(pTc, pos, bracket, name, initPos);
		if (!Helper::isLegalTemplatedVariable(name)) name = "";
		else if (readTemplate) appendTemplate(name, templ); // append
		return name;
	}
	
	///////////////////////////////////////////////////////////////
	// read token until end, it will distinguish definition and declaration, push them into SymbolTable directly

	static void readTokCollection(ITokCollection*& pTc, size_t& pos, Repository* p_Repos, const std::string& scopeNS) {
		bool isLastValDefine=false;
		//std::cout<<"\n "<<pTc->show().c_str();
		while (pos<pTc->length()) {
			if (isNewStatement(pTc,pos)) isLastValDefine=false;
			std::string name = getTypename(pTc, pos);
			//std::cout<<"\n "<<name<<" pos "<<pos;
			if (!name.empty())
				isLastValDefine ? p_Repos->Symbols()->defineType(scopeNS+"::"+name)
				: p_Repos->Symbols()->declareType(name, scopeNS), isLastValDefine=true;
			else
				isLastValDefine=false;	// reset
		}
	}
};

#endif