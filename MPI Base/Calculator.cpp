// STL Includes
#include<algorithm>
#include<chrono>
#include<iostream>
#include<numeric>
#include<string>

// Custom Includes
#include "../Graph.h"
#include "../Utility.cpp"

// MPI-MapReduce Library Includes
#include "keyvalue.h"

using namespace std;
using namespace MAPREDUCE_NS;

// Calculator Class for Doing Calculation
class Calculator{
    private:
        const Graph::Size N;
        const Graph::ToList& toList;
        const Column& Hyperlinks;
        Column& PageRanks;
        
        // Function to get the (Key,Value) data
        inline Value const getFactorValue(const Graph::Vertex& key) {return ((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key];}
        
    public:
        // Main Constructor
        Calculator(const Column& hyperlinks, const Graph::ToList& tList, Column& pageRanks) 
            : N(tList.size()), toList(tList), Hyperlinks(hyperlinks), PageRanks(pageRanks){}
        // Function to refresh the Class for another recalculation.
        inline void refresh(Column& pageRanks) {PageRanks = pageRanks;}
        
        // SIZEOF Constants
        static constexpr Graph::Vertex COMMON = 0;
        static constexpr int VERTEX_SIZE = sizeof(Graph::Vertex);
        static constexpr int VALUE_SIZE = sizeof(Value);
        
        // Map Function for Factor Calculation
        static void MapFactor(Graph::Vertex key, KeyValue* keyvalue, void* calculator){
            Value value = ((Calculator*) calculator)->getFactorValue(key);
            auto Common = COMMON;
            keyvalue->add((char *)&Common, VERTEX_SIZE,(char *)&value,VALUE_SIZE);
        }
        
        // Reduce Function for Factor Calculation
        static void ReduceFactor(char* key, int keybytes, char* multivalue, int nvalues, int* valuebytes, KeyValue* keyvalue, void* calculator){ 
            cout << ((nvalues==0)? "Trouble" : "Phew") <<endl;
            Value* values = (Value*) multivalue;
            ((Calculator*) calculator)->factor = accumulate(values,values+nvalues,0.0);
        }

        Value factor; 
};