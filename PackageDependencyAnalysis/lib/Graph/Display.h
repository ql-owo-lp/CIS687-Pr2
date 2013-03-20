#ifndef DISPLAY_H
#define DISPLAY_H

//////////////////////////////////////////////////////////////////////////
//	Display.h - A display package used to display file dependency usage	//
//  And topological sorted sequence of one Graph						//
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
This module is used to sort graph instance by topological order, then display
them in a neat style.

Public Interface:
=================
SCCSorter sorter(pGraph);
sorter.sort();
sorter.show();
sorter(vertex);

std::vector<Vertex<std::string, std::string>> vList = Display<std::string, std::string>::vertsWithNoParents(graph);
Display<std::string, std::string>::show(graph);

Build Process:
==============
Required files
- Graph., SymbolTable.h, FileManager.h

Maintenance History:
====================
ver 0.1 : 19 Mar 13
- first version

*/
#include <iostream>
#include <algorithm>    // std::reverse
#include "Graph.h"

namespace GraphLib {

	///////////////////////////////////////////////////////////////
	// Topological sorter

	template <typename V, typename E>
	class SCCSorter {
		Graph<V,E>* pGraph;
		std::vector<size_t>* sortedList;
	public:
		SCCSorter(Graph<V,E>* _pGraph) : pGraph(_pGraph), sortedList(new std::vector<size_t>) {}

		// do topological sort
		void sort() {
			if (pGraph->size()<1) return;
			// just dfs the d_Graph
			pGraph->dfs<SCCSorter>(*this);
			// reverse sortedList
			std::reverse(sortedList->begin(), sortedList->end());
		}

		// show current sorted sequence
		void show() {
			std::cout<<"\n Topological Sorted Graph sequence -----";
			for (auto it=sortedList->begin();it!=sortedList->end();it++)
				std::cout<<"\n "<<(*pGraph)[pGraph->findVertexIndexById(*it)].value();
		}

		// a simple functor that push strong component index into list
		void operator() (Vertex<V,E> & v) {
			sortedList->push_back(v.id());
		}
	};

	///////////////////////////////////////////////////////////////
	// Display class

	template<typename V, typename E>
	class Display
	{
	public:
		typedef Graph<V,E> graph;
		typedef Vertex<V,E> vertex;
		typedef std::pair<int,E> edge;

		///////////////////////////////////////////////////////////////
		// Return a list of vertors who do not have parents
		static std::vector< Vertex<V,E> > vertsWithNoParents(graph& g)
		{
			std::vector<size_t> parentCount;
			graph::iterator iter = g.begin();
			while(iter != g.end())
			{
				parentCount.push_back(0);
				++iter;
			}
			iter = g.begin();
			while(iter != g.end())
			{
				vertex v = *iter;
				for(size_t i=0; i<v.size(); ++i)
				{
					edge e = v[i];
					parentCount[e.first]++;
				}
				++iter;
			}
			std::vector<vertex> noParents;
			for(size_t j=0; j<g.size(); ++j)
				if(parentCount[j] == 0)
					noParents.push_back(g[j]);
			return noParents;
		}

		///////////////////////////////////////////////////////////////
		// Display current graph in a neat style
		static void show(graph& g) {
			graph::iterator iter = g.begin();
			std::cout<<"\n\n Outputing Package usage --------";
			while(iter != g.end()) {
				vertex v = *iter;
				std::cout << "\n  PACKAGE [" << (v.value()).c_str()<<"] LINKS TO";
				for(size_t i=0; i<v.size(); ++i) {
					vertex::Edge edge = v[i];
					std::cout << "\n    PACKAGE [" << (edge.second).c_str()<<"]";
				}
				++iter;
			}
			std::cout<<"\n";
		}
	};
}
#endif