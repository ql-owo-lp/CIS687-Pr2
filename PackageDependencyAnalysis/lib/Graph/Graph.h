#ifndef GRAPH_H
#define GRAPH_H
///////////////////////////////////////////////////////////////
// Graph.h - Graph Library                                   //
// Ver 1.5                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Help for CSE687 - OOD, Pr#1, Spring 2013     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
A graph is a collection of vertices, connected by edges.  Each
vertex holds some kind of information held in an instance of
a type V and an integer id, which should be unique with a given
graph.  Each edge connects a parent vertex with a child
vertex and it too holds information in an instance of a type E.

This package provides two classes that support creation of
instances of a graph: Vertex<V,E> and Graph<V,E>.  These are
template-based with parameters V and E, as described above.

The graph holds an aggregated collection of vertices.  Each 
vertex may hold one or more edges.  The edge is implemented
as an instance of std::pair<int,E> where the first parameter
is the id of the child vertex and the second parameter is
an instance of the edge information type E.

Note:
It is important that vertex ids be unique.  If you set any of
them with the second argument of the Vertex constructor or with
the id() function it is up to you to ensure uniqueness.  If you
don't explicitly set any ids then the constructor insures that
vertices get sequential ids in the order constructed.

Public Interface:
=================
Graph<std::string, std::string> g;
Vertex<std::string, std::string> v("value");
g.addVertex(v);
g.addEdge("edge value", v1, v2);
size_t i = g.findVertexIndexById(v.id());
size_t size = g.size();
g.dfs<someFunctor>(v, someFunctorInstance);
g.dfs<someFunctor>(someFunctorInstance);
size_t i = g.addOrFindVertexByVal("some value");
v = g[0];	// get vertex by index
Vertex<std::string, std::string>::Edge e = v[0];	// get edge
std::string val = v.value();
GraphFromXmlString<std::string, std::string>(g, xml);
std::string xml = GraphToXmlString<std::string, std::string>(g);

SCCSearcher<std::string, std::string> searcher(&g);
searcher.search();

Maintanence Information:
========================
Required files:
---------------
Graph.h, Graph.cpp, 
XmlReader.h, XmlReader.cpp, 
XmlWriter.h, XmlWriter.cpp

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv Graph.sln /rebuild debug

Revision History:
-----------------
ver 1.6 : 18 Mar 13
- Now support strong component analyzing and topological sorting
ver 1.5 : 16 Feb 13
- now supports dfs with both function pointers and functors
ver 1.4 : 11 Feb 13
- removed indent function, now part of XmlWriter, 
replaced ctor with default param with two ctors,
one supplies sequential ids, the other accepts user defined id
ver 1.3 : 28 Jan 13 
- added GraphToXmlString and GraphFromXmlString functions
ver 1.2 : 25 Jan 13
- changed id() return to reference so can be changed
ver 1.1 : 21 Jan 13
- added prologue comments, compile test directive
ver 1.0 : 14 Jan 13
- first release

*/
/*
* To Do:
* - move GraphToXml and GraphFromXml to separate package
* - Strong Components, Topological Sorting
* - Find disconnected graph partitions
*/

#include <vector>
#include <unordered_map>
#include <string>
#include "XmlWriter.h"
#include "XmlReader.h"
#include <iostream>
#include <sstream>

namespace GraphLib 
{
	///////////////////////////////////////////////////////////////
	// Vertex class

	template<typename V, typename E> 
	class Vertex
	{
	public:
		typedef std::pair<int, E> Edge;  // graph index of target vertex, edge type
		typename typedef std::vector<Edge>::iterator iterator;
		iterator begin();
		iterator end();
		Vertex(V v, size_t id);
		Vertex(V v);
		void add(Edge& edge);
		// Vertex(const Vertex<V,E>& v);                  compiler generated is correct
		// Vertex<V,E>& operator=(const Vertex<V,E>& v);  compiler generated is correct
		Edge& operator[](size_t i);
		Edge operator[](size_t i) const;
		V& value();
		size_t& id(); 
		size_t size();
		bool& mark();
	private:
		std::vector<Edge> _edges;
		V _v;
		size_t _id;
		static size_t count;
		bool _mark;
	};
	//----< reserve memory for, and initialize, static count >-----
	template<typename V, typename E>
	size_t Vertex<V,E>::count = 0;

	//----< set and return boolean mark, used for traversal >------
	template<typename V, typename E>
	bool& Vertex<V,E>::mark() { return _mark; }

	//----< return iterator pointing to first edge >---------------
	template<typename V, typename E>
	typename Vertex<V,E>::iterator Vertex<V,E>::begin() { return _edges.begin(); }

	//----< return iterator pointing to one past last edge >-------
	template<typename V, typename E>
	typename Vertex<V,E>::iterator Vertex<V,E>::end() { return _edges.end(); }

	//----< construct instance, specifying id - must be unique >---
	template<typename V, typename E>
	Vertex<V,E>::Vertex(V v, size_t id) : _v(v), _id(id), _mark(false) {}

	//----< construct instance - creates id sequentially >-------
	template<typename V, typename E>
	Vertex<V,E>::Vertex(V v) : _v(v), _id(count++), _mark(false) {}

	//----< add edge to vertex edge collection >-------------------
	template<typename V, typename E>
	void Vertex<V,E>::add(Edge& edge) { _edges.push_back(edge); }

	//----< index non-const vertex's edges >-----------------------
	template<typename V, typename E>
	typename Vertex<V,E>::Edge& Vertex<V,E>::operator[](size_t i) { return _edges[i]; }

	//----< index const vertex's edges >---------------------------
	template<typename V, typename E>
	typename Vertex<V,E>::Edge Vertex<V,E>::operator[](size_t i) const { return _edges[i]; }

	//----< set and read value of vertex's held type, V >----------
	template<typename V, typename E>
	V& Vertex<V,E>::value() { return _v; }

	//----< return vertex's id >-----------------------------------
	template<typename V, typename E>
	size_t& Vertex<V,E>::id() { return _id; }

	//----< return number of edges >-------------------------------
	template<typename V, typename E>
	size_t Vertex<V,E>::size() { return _edges.size(); }

	///////////////////////////////////////////////////////////////
	// Graph class

	template<typename V, typename E>
	class Graph
	{
	public:
		typename typedef std::vector< Vertex<V,E> >::iterator iterator;
		iterator begin();
		iterator end();
		// Graph(const Graph<V,E>& g);                  compiler generated is correct
		// Graph<V,E>& operator=(const Graph<V,E>& g);  compiler generated is correct
		Vertex<V,E>& operator[](size_t i);
		Vertex<V,E> operator[](size_t i) const;
		void addVertex(Vertex<V,E> v);
		void addEdge(E eval, Vertex<V,E>& parent, Vertex<V,E>& child);
		size_t findVertexIndexById(size_t id);
		size_t size();
		template<typename F>
		void dfs(Vertex<V,E>& v, F f);
		template<typename F>
		void dfs(F f);
		size_t addOrFindVertexByVal(const V& val);
	private:
		std::vector< Vertex<V,E> > adj;
		std::unordered_map<size_t, size_t> idMap; // id maps to graph index
		template<typename F>
		void dfsCore(Vertex<V,E>& v, F f);
		template<typename F>
		void dfsChildren(Vertex<V,E>& v, F f);
	};

	//----< return iterator pointing to first vertex >-------------
	template<typename V, typename E>
	typename Graph<V,E>::iterator Graph<V,E>::begin() { return adj.begin(); }

	//----< return iterator pointing one past last vertex >--------
	template<typename V, typename E>
	typename Graph<V,E>::iterator Graph<V,E>::end() { return adj.end(); }

	//----< index non-const graph's vertex collection >------------
	template<typename V, typename E>
	typename Vertex<V,E>& Graph<V,E>::operator[](size_t i) { return adj[i]; }

	//----< index const graph's vertex collection >----------------
	template<typename V, typename E>
	typename Vertex<V,E> Graph<V,E>::operator[](size_t i) const { return adj[i]; }

	//----< add vertex to graph's vertex collection >--------------
	template<typename V, typename E>
	void Graph<V,E>::addVertex(Vertex<V,E> v) 
	{ 
		adj.push_back(v);
		idMap[v.id()] = adj.size() - 1;
	}
	//----< return number of vertices in graph's collection >------
	template<typename V, typename E>
	size_t Graph<V,E>::size() { return adj.size(); }

	//----< return index of vertex with specified id >-------------
	template<typename V, typename E>
	size_t Graph<V,E>::findVertexIndexById(size_t id)
	{
		return idMap[id];
	}
	//----< add edge from specified parent to child vertices >-----
	template<typename V, typename E>
	void Graph<V,E>::addEdge(E eVal, Vertex<V,E>& parent, Vertex<V,E>& child)
	{
		size_t childIndex = findVertexIndexById(child.id());
		if(childIndex == adj.size())
			throw std::exception("no edge child");
		size_t parentIndex = findVertexIndexById(parent.id());
		if(parentIndex == adj.size())
			throw std::exception("no edge parent");
		Vertex<V,E>::Edge e;
		e.first = childIndex;
		e.second = eVal;
		for (auto it=adj[parentIndex].begin();it!=adj[parentIndex].end();it++)
			if (it->first==e.first && it->second==e.second) return;
		adj[parentIndex].add(e);
	}
	//----< recursive depth first search with action f >-----------
	template<typename V, typename E>
	template<typename F>
	void Graph<V,E>::dfsCore(Vertex<V,E>& v, F f)
	{
		f(v);
		v.mark() = true;
		for(auto edge : v)
		{
			if(adj[edge.first].mark() == false)
				dfsCore(adj[edge.first], f);
		}
		for(auto& vert : adj)
		{
			if(vert.mark() == false)
				dfsCore(vert, f);
		}
	}
	//----< depth first search, clears marks for next search >-----
	template<typename V, typename E>
	template<typename F>
	void Graph<V,E>::dfs(Vertex<V,E>& v, F f)
	{
		dfsCore(v, f);
		for(auto& vert : adj)
			vert.mark() = false;
	}

	//----< depth first search, with topological order >-----
	template<typename V, typename E>
	template<typename F>
	void Graph<V,E>::dfs(F f) {
		// start with a vertex with no parent
		std::vector<Vertex<V,E>> vList = Display<V,E>::vertsWithNoParents(*this);
		for(auto& vert : vList)
			dfsChildren(vert, f);
		for(auto& vert : adj)
			vert.mark() = false;
	}
	//----< recursive depth first search with action f, search children only >-----------
	template<typename V, typename E>
	template<typename F>
	void Graph<V,E>::dfsChildren(Vertex<V,E>& v, F f)
	{
		v.mark() = true;
		for(auto edge : v) { // visit children first
			if(!adj[edge.first].mark())
				dfsChildren(adj[edge.first], f);
		}
		f(v);
	}

	//----< return a reference to one vertex with specific value, will create one when it does not exist >-----
	template<typename V, typename E>
	size_t Graph<V,E>::addOrFindVertexByVal(const V& val) {
		iterator it = std::find_if(begin(), end(), [&val] (Vertex<V,E>& _v) {
			if (_v.value() == val) return true; else return false;
		});
		if (it!=end()) return findVertexIndexById(it->id());
		addVertex(Vertex<V,E>(val));
		return size()-1;	// last added vertex
	}

	//----< convert E to string >--------------------------
	template<typename V>
	std::string ToString(V i)
	{
		std::ostringstream in;
		in << i;
		return in.str();
	}
	//----< convert string to E >--------------------------
	template<typename V>
	V ToValue(const std::string& str)
	{
		std::istringstream in(str);
		V retn;
		in >> retn;
		return retn;
	}
	//----< construct graph from XML string >--------------------
	template<typename V, typename E>
	void GraphFromXmlString(Graph<V,E>& gr, const std::string& Xml)
	{
		// add vertices
		XmlReader vrdr(Xml);
		while(vrdr.next())
		{
			if(vrdr.tag() == "vertex")
			{
				XmlReader::attribElems elems = vrdr.attributes();
				if(elems.size() > 1)
				{
					Vertex<V,E> vert(ToVal<V>(elems[1].second));
					vert.id() = ToVal<size_t>(elems[0].second);
					gr.addVertex(vert);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
		}
		// add edges
		XmlReader erdr(Xml);
		int parentId;
		while(erdr.next())
		{
			if(erdr.tag() == "vertex")
			{
				XmlReader::attribElems elems = erdr.attributes();
				if(elems.size() > 0)
				{
					parentId = ToVal<size_t>(elems[0].second);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
			if(erdr.tag() == "edge")
			{
				XmlReader::attribElems eelems = erdr.attributes();
				if(eelems.size() > 1)
				{
					int childId = ToVal<size_t>(eelems[0].second);
					E eval = ToVal<E>(eelems[1].second);
					int parentIndex = gr.findVertexIndexById(parentId);
					Vertex<V,E> parent = gr[parentIndex];
					int childIndex = gr.findVertexIndexById(childId);
					Vertex<V,E> child = gr[childIndex];
					gr.addEdge(eval, parent, child);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
		}
	}
	//----< serialize graph to XML string >----------------------
	template<typename V, typename E>
	std::string GraphToXmlString(Graph<V,E>& gr)
	{
		XmlWriter wrg;
		wrg.indent();
		wrg.start("graph");
		for(auto vert : gr)
		{
			wrg.start("vertex");
			wrg.addAttribute("id", ToString<size_t>(vert.id()));
			wrg.addAttribute("value", ToString<V>(vert.value()));
			for(auto edge : vert)
			{
				wrg.start("edge");
				wrg.addAttribute("targetId", ToString<size_t>(gr[edge.first].id()));
				wrg.addAttribute("value", ToString<E>(edge.second));
				wrg.end();
			}
			wrg.end();
		}
		wrg.end();
		return wrg.xml();
	}

	///////////////////////////////////////////////////////////////
	// strong connected components searcher
	template <typename V, typename E>
	class SCCSearcher {
		typedef std::pair<size_t, size_t> vertexInfo;	// this is a pair of lowlink and index for vertex
		typedef std::vector<size_t> strongComponent;	// collection of vertex index
		typedef std::vector<strongComponent> strongComponents;
		typedef std::vector<size_t> strongComponentList;	// an array of strong component index.  NOTICE, it is only index
		typedef Graph<std::string, size_t> dGraph;	// a condensed graph used for topological sorting, vertex value is strong component's value set, edge value doesn't really matter
		typedef Vertex<std::string, size_t> dVertex;	// dependency vertex, again, edge doesn't matter
		typedef Graph<V,E> graph;
		typedef Vertex<V,E> vertex;
		graph* pGraph;
		dGraph d_Graph;
		size_t index;
		strongComponent vStack;
		std::unordered_map<size_t, vertexInfo> infoMap;	// key=vertex.index, value={lowlink, index}
		strongComponents* SCCs;	// strong components list

		//----< create condensed map vertex from strong component vertex set >--------------------------
		void createCondensedVertex(std::unordered_map<size_t, size_t> & idSCCMap) {
			// mapping vertex with strong component value
			for (size_t i=0;i<SCCs->size();i++) {
				std::ostringstream val;
				val <<"{ ";
				for (size_t vIndex : (*SCCs)[i]) // for each vertex in one strong component
					val << (*pGraph)[vIndex].value() << " ";
				val <<"}";
				d_Graph.addVertex(dVertex(val.str()));
				for (size_t vIndex : (*SCCs)[i])
					idSCCMap[vIndex]=i;
			}
		}

		//----< create consdensed map edge >--------------------------
		void createCondensedEdge(std::unordered_map<size_t, size_t> & idSCCMap) {
			for (size_t i=0;i<SCCs->size();i++) {
				for (size_t vIndex : (*SCCs)[i]) { // for each vertex in one strong component
					Vertex<V,E>& v=(*pGraph)[vIndex];
					for (Vertex<V,E>::iterator it=v.begin();it!=v.end();it++) { // for each edge in this vertex
						size_t sccIndex = idSCCMap[it->first];	// where this edge is linked to and find the strong component index
						d_Graph.addEdge(0,d_Graph[i],d_Graph[sccIndex]);
					}
				}
			}
		}

		//----< create condensed graph >--------------------------
		void createCondensedGraph() {
			std::unordered_map<size_t, size_t> idSCCMap;	// key=v.index, value=strongComponent.index where it is in
			if (SCCs->size()<1) return;
			createCondensedVertex(idSCCMap);
			createCondensedEdge(idSCCMap);
		}
	public:
		//----< constructor >--------------------------
		SCCSearcher(graph* g) : index(0), pGraph(g), SCCs(new strongComponents) {}

		// search strong components
		void search() {
			if (pGraph->size()<1)
				return;
			// pick up the first vertex
			pGraph->dfs<SCCSearcher<V,E>>((*pGraph)[0],*this);
			createCondensedGraph();
		}

		// Tarjan's algorithm
		void operator() (vertex& v) {
			v.mark()=true;	// manually mark this vertex as visited, as we call this function ourself
			size_t vIndex = pGraph->findVertexIndexById(v.id());
			vertexInfo& info = infoMap[vIndex] = vertexInfo(index,index);	// init both lowlink and index to index
			index++;
			vStack.push_back(vIndex);
			// now traverse v's edge
			for (vertex::iterator it=v.begin();it!=v.end();it++) {
				vertex& child = (*pGraph)[it->first];
				if (!child.mark()) {
					(*this)(child);	// recursive call
					if (info.first > infoMap[it->first].first) info.first=infoMap[it->first].first;	// update lowlink
				}
				else if (std::find(vStack.begin(), vStack.end(), it->first)!=vStack.end())
					if (info.first > infoMap[it->first].second) info.first=infoMap[it->first].second;
			}
			// if current vertex is root
			if (info.first == info.second) {
				// strong component found
				strongComponent c;
				do {
					c.push_back(vStack.back());
					vStack.pop_back();
				} while (c.back()!=vIndex);
				SCCs->push_back(c);
			}
		}

		// return sorted strong componets
		dGraph& Graph() {
			return d_Graph;
		}

		// out put strong components
		void show() {
			std::cout<<"\n\n Strong Components -----------";
			for (size_t i=0;i<SCCs->size();i++) {
				std::cout<<"\n { ";
				for (size_t vIndex : (*SCCs)[i])
					std::cout<<(*pGraph)[vIndex].value()<<" ";
				std::cout<<"}";
			}
		}
	};

}
#endif
