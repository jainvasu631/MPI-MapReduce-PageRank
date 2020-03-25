#pragma once

#include<algorithm> 
#include<cmath>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>

using namespace std;


class Graph{
    private:
        // Function to Sort Edges
        void inline sortEdges() {sort(Edges.begin(),Edges.end());}
        // Function to Sort Vertices and Remove Duplicates. Implementation may change later.
        void inline sortVertices() {sort(Vertices.begin(),Vertices.end());Vertices.erase(unique(Vertices.begin(), Vertices.end()), Vertices.end());}
        // Function to Sort Graph
        void inline sortGraph(){sortEdges();sortVertices();}
        // Form the AdjacencyList
        void inline formAdjacencyLists(){
            Size N = Vertices[Vertices.size()-1]+1; 
            toList.resize(N);
            fromList.resize(N);
            for (auto const& Edge : Edges){
                toList[Edge.first].push_back(Edge.second);
                fromList[Edge.second].push_back(Edge.first);
            }
                 
        } 

    public:
        // Type Aliases
        using Size = unsigned int;
        using Vertex = int;
        using Edge = pair<Vertex,Vertex>;
        using EdgeList = vector<Edge>;
        using VertexList = vector<Vertex>;
        using ToList = vector<VertexList>;
        using FromList = vector<VertexList>;

        ToList toList;
        FromList fromList;

        // Default Constructor Function
        Graph() {}
        // File Constructor Function
        Graph(string filename){
            ifstream inputFileStream(filename);
            Vertex from,to;
            // This parses each line and pushes the result to from and to which are then converted to the edgelist.
            while(inputFileStream >> from >> to) 
                addToGraph(from,to);
            sortGraph();
            formAdjacencyLists();
        }

        // Find the number of Vertices
        Size inline numVertices() const {return toList.size();}
        // Find the number of Edges
        Size inline numEdges() {return Edges.size();}

        // Find number of Edges Away From a Graph
        Size inline numEdgesFromVertex(Vertex vertex) {return toList[vertex].size();}

        // Find if Graph contains Vertex.
        bool inline isVertexInGraph(Vertex vertex) {return vertex < numVertices();}
        // Find if Graph contains Edge.
        bool inline isEdgeInGraph(Vertex from, Vertex to) {return binary_search(toList[from].begin(), toList[from].end(), to);}

        void print(){
            int i=0;
            for(const VertexList& vertexList : toList){
                cout<<"From "<<i++<<" To:"; 
                for(Vertex vertex : vertexList)
                    cout<< vertex << "->";
                cout <<"|"<< endl;
            }
        }
    protected:
        VertexList Vertices;
        EdgeList Edges;
        
        
        void inline addEdge(Vertex from, Vertex to) {Edges.push_back(Edge(from,to));}
        void inline addVertex(Vertex vertex){Vertices.push_back(vertex);}
        void inline addToGraph(Vertex from, Vertex to) {addVertex(from);addVertex(to);addEdge(from,to);}
};
