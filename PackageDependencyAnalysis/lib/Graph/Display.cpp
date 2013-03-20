
//////////////////////////////////////////////////////////////////////////
//	Display.cpp - to test Display package								//
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
This is for testing Display.

Public Interface:
=================
none

Maintanence Information:
========================
Required files:
---------------
Graph.h, Display.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv PackageDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 first version

*/

#ifdef TEST_DISPLAY
#include <string>
#include <iostream>
#include <fstream>
#include "Graph.h"
#include "Display.h"
using namespace GraphLib;

typedef Graph<std::string, std::string> graph;
typedef Vertex<std::string, std::string> vertex;
typedef Display<std::string, std::string> display;
typedef SCCSorter<std::string, std::string> sccsorter;

int main()
{
	std::cout << "\n  Testing Display Library";
	std::cout << "\n =======================\n";
	try
	{
		std::cout << "\n  Constructing Graph instance";
		std::cout << "\n -----------------------------";
		graph g;
		vertex v1("v1");
		vertex v2("v2");
		vertex v3("v3");
		vertex v4("v4");
		vertex v5("v5", 50);
		g.addVertex(v2);
		g.addVertex(v1);
		g.addVertex(v3);
		g.addVertex(v4);
		g.addVertex(v5);
		g.addEdge("e1",v1,v2);
		g.addEdge("e2",v1,v3);
		g.addEdge("e3",v2,v3);
		g.addEdge("e4",v4,v3);
		g.addEdge("e5",v5,v2);

		display::show(g);
		std::cout << "\n";
		std::cout << "\n  Vertices with no Parents:";
		std::cout << "\n ---------------------------";

		std::vector< vertex > verts = display::vertsWithNoParents(g);
		std::cout << "\n  ";
		for(size_t i=0; i<verts.size(); ++i)
			std::cout << verts[i].value().c_str() << " ";
		std::cout << "\n";

		std::cout << "\n  Testing Topological sorter";
		std::cout << "\n --------------------------------------------------";

		sccsorter sorter(&g);
		sorter.sort();
		sorter.show();
		std::cout << "\n";
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
	}
	std::cout << "\n\n";
	return 0;
}

#endif