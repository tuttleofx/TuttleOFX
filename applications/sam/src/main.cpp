/*
 * main.cpp
 *
 *  Created on: 07 oct. 2009
 *      Author: Nicolas Rondaud
 */

#include <tuttle/graph/Graph.hpp>
#include <tuttle/graph/Edge.hpp>
#include <tuttle/graph/Vertex.hpp>
#include <iostream>

using namespace std;


int main(int argc, char * argv[]) {
	try{
		cout << "Sam command line tool" << endl;

		tuttle::Graph<Vertex, Edge> graph;

		Vertex A( "nodeA" );
		Vertex B( "nodeB" );
		Vertex C( "nodeC" );
		Vertex D( "nodeD" );

		Edge AtoB( "AtoB" );
		Edge BtoA( "BtoA" );
		Edge BtoC( "BtoC" );
		Edge BtoD( "BtoD" );
		Edge CtoB( "CtoB" );
		Edge DtoB( "DtoB" );

		tuttle::Graph<Vertex, Edge>::Vertex ADesc = graph.AddVertex(A);
		tuttle::Graph<Vertex, Edge>::Vertex BDesc = graph.AddVertex(B);
		tuttle::Graph<Vertex, Edge>::Vertex CDesc = graph.AddVertex(C);
		tuttle::Graph<Vertex, Edge>::Vertex DDesc = graph.AddVertex(D);

		graph.AddEdge(ADesc, BDesc, AtoB);
		graph.AddEdge(BDesc, CDesc, BtoC);

		graph.RemoveVertex(CDesc);
		graph.AddEdge(BDesc, DDesc, BtoD);

		//graph.has_cycle();
		graph.exportAsDOT("testgraph.dot");

		std::cout << "edges: " << graph.getEdgeCount() << std::endl;
		std::cout << "vertices: " << graph.getVertexCount() << std::endl;

	}
	catch(std::exception & e) {
		cerr << "main: " << e.what() << endl;
		return 1;
	}
	catch(...) {
		cerr << "main: Unknown error." << endl;
		return 1;
	}
	return 0;
}
