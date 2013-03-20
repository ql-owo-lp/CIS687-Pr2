#ifndef STRINGHELPER_H
#define STRINGHELPER_H
//////////////////////////////////////////////////////////////////////////
//	StringHelper.h - This is a helper class for some string processing	//
//	work.																//
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
This is basically a helper for manipulating string type, including converting
uppercase char into lowercase char, telling if one string contains white space
telling if one string is a legal variable style etc., which are not provided 
by the standard library

Public Interface:
=================
bool res = Helper::isSpace('a');
bool res = Helper::isSpace("ass  sd");
char chr = Helper::tolower('A');
bool res = Helper::isModifierKeyword("const");
bool res = Helper::isLegalVariable('$');
bool res = Helper::isLegalVariable("Vertex");
bool res = Helper::isLegalTemplatedVariable("Vertex<V,E>");

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

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

///////////////////////////////////////////////////////////////
// Helper namespace
namespace Helper {

	// whether current char is a white space
	static inline bool isSpace(const char & chr) {
		std::locale loc;
		return isspace(chr, loc);
	}

	// whether current string contains white space
	static inline bool isSpace(const std::string & str) {
		bool space = str.empty();
		for (size_t i=0;!space && i<str.length();i++) space =  isSpace(str[i]);
		return space;
	}

	// to lower case, as ::tolower() won't always work
	static inline char tolower(const char& in){
		const static int distance = 'Z'-'z';
		return (in<='Z' && in>='A') ? in-distance : in;
	}

	// is modifier like const, final, etc.
	static inline bool isModifierKeyword(const std::string& s)
	{
		const static std::string keys="|return|new|delete|operator|signed|unsigned|const|volatile|struct|enum|friend|class|union|typename|inline|virtual|static|mutable|thread_local|register|extern|public|private|protected|case|&|*|*&|&*|for|while|else|continue|true|false|switch|if|catch|try|dynamic_cast|static_cast|";
		return keys.find("|"+s+"|")!=std::string::npos;
	}

	// is one char an acceptable variable character
	static inline bool isLegalVariable(const char& chr) {
		std::locale loc;
		return isalnum(chr,loc) || chr=='_' || chr=='$';
	}

	// is this string a legal variable style
	static inline bool isLegalVariable(const std::string& str) {
		bool res=!str.empty();
		size_t pos=0;
		if (str[0]=='~') pos++;
		res = res && isLegalVariable(str[pos]) && !isdigit(str[pos]);
		while (res && ++pos<str.length()) res = isLegalVariable(str[pos]);
		return res;
	}

	// is this string a legal variable style, support template
	static bool isLegalTemplatedVariable(const std::string& str) {
		bool res=!str.empty();
		size_t pos=0;
		if (str[0]=='~') pos++;
		res = res && isLegalVariable(str[pos]) && !isdigit(str[pos]);
		pos++;
		for (size_t bracket=0;res && pos<str.length();pos++) {
			if (str[pos]=='<') { // count bracket
				bracket++; continue;
			}
			else if (str[pos]=='>') {
				bracket--; continue;
			}
			else if (str[pos]==':' || str[pos]=='*' || str[pos]=='&' || str[pos]=='~' || (bracket>0 && str[pos]==','))
				continue;
			res = isLegalVariable(str[pos]);
		}
		return res;
	}
}

#endif