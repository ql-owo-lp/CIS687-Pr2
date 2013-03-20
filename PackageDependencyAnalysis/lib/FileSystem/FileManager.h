#ifndef FILEMANAGER_H
#define FILEMANAGER_H

//////////////////////////////////////////////////////////////////////////
//	FileManager.h - Manipulate files, including parsing files, getting	//
//	file list, etc.														//
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
This is basically a helper class which is an extension to the FileSystem.
It includes a couple of static functions which will read/write file, also,
the parse function will directly initialize the ConfigureParser instance and
parse the files with it.

Public Interface:
=================
FileManager::parse(arrayOfFile);
FileManager::parse(filePath);
string content = FileManager::read(filePath);
FileManager::save(filePath, content);
FileManager::getFileList(path, isRecursive, searchPattern);

Maintanence Information:
========================
Required files:
---------------
- ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp, FileSystem.h, FileSystem.cpp

Maintenance History:
====================
ver 0.1 : 01 Jun 11
- first version

*/
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include "Parser.h"
#include "ConfigureParser.h"
#include "FileSystem.h"

///////////////////////////////////////////////////////////////
// FileManager class
class FileManager {
public:

	///////////////////////////////////////////////////////////////
	// parse all files listed in the vector, return whether the parsing succeed

	static bool parse(std::vector<std::string>& files) {
		bool res=true;
		if (files.size()<1) {
			std::cout<<"\n No file is found under the specific directory!!";
			return false;
		}
		// start parsing file
		for (size_t i=0;res && i<files.size();i++)
			res = parse(files[i]);
		return res;
	}

	///////////////////////////////////////////////////////////////
	// parse file use parser

	static bool parse(const std::string& path) {
		std::cout << "\n  Processing file " << path;
		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try {
			if(pParser) {
				if(!configure.Attach(path)) {
					std::cout << "\n  could not open file " << path << std::endl;
					return false;
				}
			}
			else {
				std::cout << "\n\n  Parser not built\n\n";
				return false;
			}
			// now that parser is built, use it
			while(pParser->next()) pParser->parse();
		}
		catch(std::exception& ex) {
			std::cout << "\n\n    " << ex.what() << "\n\n";
			return false;
		}
		return true;
	}

	///////////////////////////////////////////////////////////////
	// save a string to a file

	static bool save(const std::string& outpath, const std::string& str) {
		bool res=false;
		std::ofstream outf(outpath);
		if (outf.good()) { 
			outf<<str;
			res=true;
		}
		outf.close();
		return res;
	}

	///////////////////////////////////////////////////////////////
	// get string from a file

	static std::string read(const std::string& inpath) {
		std::ifstream inf(inpath);
		//std::string str;
		std::ostringstream ss;
		if (inf.good()) ss << inf.rdbuf() << std::endl;
		inf.close();
		return ss.str();
	}

	///////////////////////////////////////////////////////////////
	// get a list of file path under specific path

	static std::vector<std::string> getFileList(std::string path, bool isRecursive, std::vector<std::string> patterns) {
		std::vector<std::string> files;
		std::for_each(patterns.begin(), patterns.end(), [&path, &isRecursive, &files] (const std::string &pattern) {
			std::vector<std::string> _files = getFileList(path, isRecursive, pattern);
			files.insert(files.end(), _files.begin(), _files.end());
		});
		return files;
	}

	///////////////////////////////////////////////////////////////
	// get a list of file path under one specific directory

	static std::vector<std::string> getFileList(std::string path, bool isRecursive, std::string pattern) {
		if (path[path.length()-1]!='/' && path[path.length()-1]!='\\')
			path += "/";
		std::vector<std::string> files = FileSystem::Directory::getFiles(path, pattern);
		for (size_t i=0;i<files.size();i++) files[i]=path+files[i];
		if (isRecursive) {
			// get files from sub-directories
			std::vector<std::string> subDirectories = FileSystem::Directory::getDirectories(path);
			for (size_t i=0;i<subDirectories.size();i++) {
				if (subDirectories[i]=="." || subDirectories[i]=="..")
					continue;
				else {
					std::vector<std::string> _files = getFileList(path+subDirectories[i], isRecursive, pattern);
					files.insert(files.end(), _files.begin(), _files.end());	// copy the files from sub-directories to current file list
				}
			}
		}
		return files;
	}
};

#endif