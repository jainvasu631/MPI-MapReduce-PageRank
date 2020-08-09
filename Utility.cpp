#pragma once

// STL Includes
#include<algorithm>
#include<chrono>
#include<iostream>
#include<numeric>
#include <fstream>

// Class Includes
#include "Graph.h"

using namespace std;
using namespace std::chrono;

// Type aliases
using Value = double;
using Column = vector<Value>;
using VertexInfo = pair<Value,Graph::VertexList>;

class Constant{
    public:
        // Global Constants
        static constexpr Value ALPHA = 0.85;
        static constexpr Value GAMMA = 1/ALPHA;
        static constexpr Value BETA = GAMMA-1;
        static constexpr Value TOL = 1.0e-9;
};

class Utility{
    public:
        // Using This Function to create the Column
        static inline Column getInitPageRank(const Graph::Size N) {return Column(N,1.0/N);}

        // Calculate the Hyperlink Column
        static Column calculateHyperLinkColumn(const Graph::ToList& toList){
            const Graph::Size N = toList.size();
            Column hyperlink(N);
            for(Graph::Size i=0;i<N;i++)
                hyperlink[i] = (toList[i].size()>0)? 1.0/toList[i].size() : 0;
            return hyperlink;
        }

        // Print PageRanks in format
        static void printPageRank(const Column& pageRanks, const string filename){
            ofstream file; 
            file.open(filename); 
            
            Graph::Size N = pageRanks.size();
            for(Graph::Size i=0; i<N;i++)
                file << i << " = " << pageRanks[i] << endl;
            file << "s = " << accumulate(pageRanks.begin(),pageRanks.end(),0.0) << endl;
            file.close()
        }

        static Graph timedGraphCreation(const string filename){
            // Graph Creation
            auto start_graph_creation = high_resolution_clock::now();
            Graph graph(filename);
            auto end_graph_creation = high_resolution_clock::now();
            auto graph_creation_duration = duration_cast<milliseconds>(end_graph_creation- start_graph_creation);
            cout << "Graph Creation took " << graph_creation_duration.count()<<"ms"<<endl;
            return graph;
        }

        // Find the new l1 norm    
        static inline Value calculateNorm(const Column& pageRanks, const Column& pageRanks_){
            auto abs_diff = [](Value a, Value b) {return abs(a-b);};
            return inner_product(pageRanks.begin(),pageRanks.end(),pageRanks_.begin(),0.0,plus<Value>(),abs_diff);
        }
        
};