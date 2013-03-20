#ifndef ANALYZER_H
#define ANALYZER_H

//////////////////////////////////////////////////////////////////////////
//	Analyzer.h - Analyzing SymbolTable, extract the file dependencies 	//
//	Find strong components, sort them topologically.  Also convert		//
//  SymbolTable into Graph												//
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
This module is used to organize FileManager, SymbolTable and Graph.  Its merely
interface doWork() will first use FileManager to parse all files under specific path,
then generates Graph use SymbolTable which is used to file dependency when parsing.
At last, it will extract strong component from the Graph, sort it topologically
and save Graph as XML to disk.

Public Interface:
=================
Analyzer(path, pattern, isRecursive);
Analyzer::doWork();

Build Process:
==============
Required files
- Graph.h, Display.h, SymbolTable.h, FileManager.h

Maintenance History:
====================
ver 0.1 : 19 Mar 13
- first version

*/

#include <iostream>
#include "Graph.h"
#include "Display.h"
#include "SymbolTable.h"
#include "FileManager.h"

///////////////////////////////////////////////////////////////
// Analyzer class
class Analyzer {
	typedef GraphLib::Graph<std::string, std::string> graph;	// file graph
	typedef GraphLib::Vertex<std::string, std::string> vertex;	// file vertex

	//----< convert SymbolTable into Graph >-----------------------------------
	void convertToGraph() {
		for (SymbolTable::iterator it=SymbolTable::begin(); it!=SymbolTable::end(); it++) {
			size_t pIndex = pGraph->addOrFindVertexByVal(it->first);
			for (const auto & package : it->second->files()) {
				size_t cIndex = pGraph->addOrFindVertexByVal(package);
				pGraph->addEdge((*pGraph)[cIndex].value(), (*pGraph)[pIndex], (*pGraph)[cIndex]);
			}
		}
	}

	//----< save Graph to disk >-----------------------------------
	void saveGraph(const std::string & path="./graph.xml") {
		std::string xml = GraphLib::GraphToXmlString<std::string, std::string>(*pGraph);
		FileManager::save(path, xml);
	}
	// graph instance
	graph* pGraph;
	std::string path;
	std::vector<std::string> patterns;
	bool isRecursive;
public:

	//----< promotional constructor >-----------------------------------
	Analyzer(std::string _path, std::vector<std::string> _patterns, bool _isRecursive)
		: path(_path), patterns(_patterns), isRecursive(_isRecursive) {
			pGraph = new graph;
	}
	//----< destroctor, release resource >-----------------------------------
	~Analyzer() {
		delete pGraph;
	}

	//----< the merely interface, do all work >-----------------------------------
	void doWork() {
		// the command line should be :  [option] path [file pattern] [file_pattern]
		std::vector<std::string> files = FileManager::getFileList(path, isRecursive, patterns);
		std::cout<<"\n Now start analyzing.. Please be patient..\n";
		if (FileManager::parse(files)) {
			std::cout<<"\n\n Searching for package dependency...";
			SymbolTable::analyzeAll();
			convertToGraph();
			saveGraph();
			std::cout<<"\n Graph has been save to ./graph.xml";
			GraphLib::Display<std::string, std::string>::show(*pGraph);
			std::cout<<"\n Now searching for strong components...";
			GraphLib::SCCSearcher<std::string, std::string> searcher(pGraph);
			searcher.search();
			std::cout<<"\n Sorting strong components...";
			GraphLib::SCCSorter<std::string, size_t> sorter(&searcher.Graph());
			sorter.sort();
			// output sorted strong components
			sorter.show();
			std::cout<<"\n\n Done!! All relationship has been analyzed.\n";
		}
	}
};

#endif