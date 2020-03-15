// STL Includes
#include<chrono>

// Class Includes
#include "Graph.h"
#include "FlatMatrix.h"
#include "SparseMatrix.h"

// Type aliases
using Size = unsigned int;
using Column = vector<double>;
using Matrix = vector<Column>;

// Global Constants

// Assign the global constants
inline void assignConstants();

// Method to create the Hyperlink Matrix
SparseMatrix createHyperlinkMatrix(Graph& graph){
    const Size N = graph.numVertices();
    SparseMatrix result;
    for(Graph::Vertex from=0; from<N; from++){
        const Graph::VertexList& toList = graph.adjacencyList[from];
        double importance = 1.0/toList.size();
        for(const Graph::Vertex& to : toList)
            result.addElement(from,to,importance);
    }
    return result;
};
