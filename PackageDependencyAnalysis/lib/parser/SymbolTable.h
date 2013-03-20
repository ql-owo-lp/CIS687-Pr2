#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

//////////////////////////////////////////////////////////////////////////
//	SymbolTable.h - Mapping classes to their namespaces and				//
//	corresponding files													//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis							//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Module Operations: 
==================
This class is used to determine the dependency relationship between files,
when we detected a type (namely "class", "struct", "union"), we need to add
it to this symbol table, so that we know where the type is defined.

When analyzing the file dependency, you should notice that this class will only
analyze the included file instead of all file set.

should ALWAYS use SymbolTable::create() to declare a new intance of SymbolTable,
so that the SymbolTable instance will be added into SymbolTable set

Public Interface:
=================
SymbolTable symbol(packageName);
or
SymbolTable* pSymbol = SymbolTable::create(packageName);

symbol.defineType(typeName);
symbol.declareType(typeName);
symbol.declareType(typeName, nameScope);
symbol.includeFile(packageName);
symbol.useNamespace(nameSpace);
symbol.analyze();
symbol.cleanFileInc();

std::vector<std::string> list = symbol.fileInc();
std::unordered_set<std::string> list = symbol.files();
std::unordered_set<std::string> list = symbol.types();

std::string name = SymbolTable::formatTypeName(typeName);
std::string name = SymbolTable::getPackageName(filePath);
SymbolTable::analyzeAll();

Maintanence Information:
========================
Required files:
---------------
StringHelper.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 first version

*/

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <locale>
#include <iostream>
#include "StringHelper.h"

///////////////////////////////////////////////////////////////
// An extension to std namespace
namespace std {
	///////////////////////////////////////////////////////////////
	// A hash function for std::pair<std::string, std::string> 
	template <>
	struct hash<std::pair<std::string, std::string>> {
		size_t operator()(const std::pair<std::string, std::string> & t) const {
			return std::hash<std::string>()(t.second+"::"+t.first);
		}
	};
}

///////////////////////////////////////////////////////////////
// SymbolTable class
class SymbolTable {
public:
	typedef std::unordered_map<std::string, SymbolTable*> Table;
	typedef std::vector<std::string> strArray;
	typedef std::unordered_set<std::string> strSet;
	typedef std::pair<std::string, std::string> declaredType;
	typedef Table::iterator iterator;

	// each symbol table should be binded to one file
	SymbolTable(std::string file) : _file(file) {
		// the following two will not be included in the graph, just for convenient that their index are zero
		_fileIncluded.push_back(file);	// always put itself as "included"
	}

	// add the type as it is defined in current file
	void defineType(std::string t) {
		t = formatTypeName(t);
		//std::cout<<"\n type defined "<< t;
		if (t.empty() || _typeDefined.find(t) != _typeDefined.end())
			return;
		_typeDefined.insert(t);
	}

	// add the type as its instance is declared in current file
	void declareType(declaredType t) {
		t.first = formatTypeName(t.first);
		//std::cout<<"\n type declared "<<t.first<<" "<<t.second;
		if (t.first.empty() || _typeDeclared.find(t) != _typeDeclared.end())
			return;
		_typeDeclared.insert(t);
	}

	// declare a type (new an instance etc.)
	void declareType(std::string t, std::string scope) {
		declareType(declaredType(t, scope));
	}

	// add "using namespace"
	void useNamespace(std::string ns) {
		ns = formatTypeName(ns);
		if (ns.empty() || _namespaceUsing.find(ns) != _namespaceUsing.end())
			return;
		//std::cout<<"\n using namespace [ "<<ns<<" ]";
		_namespaceUsing.insert(ns);
	}

	// add "#include"
	void includeFile(std::string f) {
		// we need to extract the file name, eliminate the file path
		// there is a BUG that I don't distinguish packages which have same name but different path
		// but I don't intend to fix it at all, as normally people don't do that, right?
		f = getPackageName(f);
		//std::cout<<"\n include file [ "<<f<<" ]";
		if (f.empty() || f==_file || std::find(_fileIncluded.begin(), _fileIncluded.end(),f) != _fileIncluded.end())
			return;
		_fileIncluded.push_back(f);
	}

	// extract package name, delete file extension and folder path
	static std::string getPackageName(std::string f) {
		int pos1 = f.find_last_of('/'), pos2 = f.find_last_of('\\');
		pos1=(pos1==std::string::npos) ? -1 : pos1;
		pos2=(pos2==std::string::npos) ? -1 : pos2;
		size_t pos = ((pos1 > pos2) ? pos1 : pos2)+1;	// start from the next character from '/' or '\'
		// record the last ., delete file extension
		if ((pos1=f.find_last_of('.'))!=std::string::npos)
			pos2=pos1-pos;
		// to lower case
		std::transform(f.begin(), f.end(), f.begin(), Helper::tolower);
		return f.substr(pos, pos2);
	}

	// analyze current file's dependency
	void analyze() {
		cleanFileInc();
		//std::cout<<"\n file "<<_file;
		// traverse all type declared
		for (const auto & type : _typeDeclared)
			searchFile(type);
	}

	// clean included files for current SymbolTable instance
	void cleanFileInc() {
		// skip the first file (i=0), which is itself
		for (size_t i=1;i<_fileIncluded.size();) {
			if (_t.find(_fileIncluded[i])==_t.end()) {
				_nonexistFileList.insert(_fileIncluded[i]);
				_fileIncluded.erase(_fileIncluded.begin()+i);	// not registered, remove it from file include list
			}
			else {
				SymbolTable* s = _t[_fileIncluded[i]];
				strArray fList = s->fileInc();
				// expanding the list, include the files which header files included
				for (std::string & f : fList) {
					if (_nonexistFileList.find(f)==_nonexistFileList.end())
						includeFile(f);
				}
				i++;
			}
		}
	}

	// return included files
	strArray fileInc() {
		return _fileIncluded;
	}

	// return file list which depend on current file
	strSet files() {
		return _fileUsed;
	}

	// return type list which is defined in current file
	strSet types() {
		return _typeDefined;
	}

	// analyze the file dependency relationship based on SymbolTable set.
	static void analyzeAll() {
		// analyze each file
		for (iterator it=_t.begin();it!=_t.end();it++)
			it->second->analyze();
	}

	// mainly this function will format a template class name into <> or <,>, and delete the whitespace
	static std::string formatTypeName(std::string t) {
		size_t pos = t.find('<'), bracket=0;
		if (pos==std::string::npos) return t;
		std::locale loc;
		while (pos<t.length()) {
			(t[pos]=='<') ? bracket++ : (t[pos]=='>') ? bracket-- : 0;
			if (!(bracket==1 && (t[pos]==',' || t[pos]=='<') || bracket==0) || std::isspace(t[pos],loc))
				t.erase(pos,1);
			else
				pos++;
		}
		if (bracket>0 || t.empty())
			return "";
		// delete * and & in the beginning and end
		for (size_t i=0;i<t.length();)
			if (t[i]=='*' || t[i]=='&' || t[i]=='~')
				t.erase(i,1);
			else
				i++;
		return t;
	}

	// return all SymbolTable instances' first iterator
	static iterator begin() {
		return _t.begin();
	}
	
	// return all SymbolTable instances' last iterator
	static iterator end() {
		return _t.end();
	}
	
	// return number of SymbolTable instances
	static size_t size() {
		return _t.size();
	}
	
	// create a SymbolTable instance and add it to global static set
	static SymbolTable* create(std::string f) {
		f = getPackageName(f);
		return (_t.find(f) != _t.end()) ? _t[f] : _t[f] = new SymbolTable(f);
	}

private:
	static Table _t;	// the collection of all files
	strSet _typeDefined;	// the types which are defined in current file
	std::unordered_set<declaredType> _typeDeclared;	// the types which are declard in current file, (typename, declare scope)
	strSet _namespaceUsing;	// the namespaces current file are using
	std::string _file;	// current file
	strArray _fileIncluded;	// the files included in current file
	strSet _fileUsed;	// record which files are actually used by current file
	static strSet _nonexistFileList;	// record file which doesn't exist, avoiding system lib header file being added again and again

	// add files(packages) which are used
	void addFileUsed(std::string f) {
		f = getPackageName(f);
		if (f.empty() || f==_file || _fileUsed.find(f) != _fileUsed.end())
			return;
		//std::cout<<"\n PACKAGE ["<<_file<<"] USES ["<<f<<"]";
		_fileUsed.insert(f);
	}

	// return true when a type is found
	bool isTypeFound(const std::string & f, strSet& types, const std::string & ns, const std::string name) {
		// reminder : if ns is ended with "::", means this type is declared in a temporary/private scope
		std::string t = ns+"::"+name;
		if (types.find(t)!=types.end()) {
			addFileUsed(f);	// cheer! found!
			//std::cout<<"\n f "<<_file<<" "<<f<<" "<<t;
			return true;
		}
		return false;
	}

	// search file from "included file" for a specific type
	void searchFile(const declaredType& type) {
		// we shall start from scaning the included files instead of scaning the types declared
		for (std::string & f : _fileIncluded) {
			//std::cout<<"\n search "<<f;
			// speed up here! no need to check this file when it is already used
			if (f!=_file && _fileUsed.find(f)!=_fileUsed.end()) continue;
			std::string name = type.first, scope = type.second;
			//std::cout<<"\n matching type "<< name<<" "<<scope;
			strSet types = _t[f]->types();
			if (isTypeFound(f,types,scope,name)) return;
			while (!scope.empty()) {
				// reduce scope by one namespace
				size_t pos = scope.find_last_of("::");
				if (pos != std::string::npos) scope.erase(pos); else scope="";
				if (isTypeFound(f,types,scope,name)) return;
			}
			// well, no scope left, now try "using namespace"
			for (auto ns=_namespaceUsing.begin();ns!=_namespaceUsing.end();ns++) {
				if (isTypeFound(f,types,*ns,name)) return;
			}
		}
	}
};

#endif