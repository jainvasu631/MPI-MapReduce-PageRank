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
#include "mapreduce.h"
#include "keyvalue.h"

using namespace std;
using namespace MAPREDUCE_NS;

#define getCharAddress(value) ((char *)&value) // Converting to char* as MapReduce Library accepts stream of chars
#define castTo(Class, instance) ((Class*) instance) // Casting the char* back to Class* 
#define dereferenceTo(Class, instance) (*castTo(Class, instance)) //Dereferencing the * to classes.

// MPI Constants
int RANK,SIZE,HOME=1,ROOT=0;

// Calculator Class for Doing Calculation
class Calculator{
    private:
        const Graph::Size N;
        const Graph::ToList& toList;
        const Column& Hyperlinks;
        Column& PageRanks;
        Value factor;
        Value norm;

        // Function to get the Factor (Key,Value) data
        inline Value const getFactorValue(const Graph::Vertex& key) {return ((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key];}
        
        // Function to get the PageRank (Key,Value) data
        inline Value const getPageRankValue(const Graph::Vertex& key) {return Hyperlinks[key]* PageRanks[key];}
        
    public:
        // Main Constructor
        Calculator(const Column& hyperlinks, const Graph::ToList& tList, Column& pageRanks) 
            : N(tList.size()), toList(tList), Hyperlinks(hyperlinks), PageRanks(pageRanks){}
        // Function to refresh the Class for another recalculation.
        inline void refresh(Column& pageRanks) {PageRanks = pageRanks;norm=0;}
        
        // SIZEOF Constants
        static constexpr Graph::Vertex COMMON = 0;
        static constexpr int VERTEX_SIZE = sizeof(Graph::Vertex);
        static constexpr int VALUE_SIZE = sizeof(Value);
        static constexpr int INT_SORT = 1;
        
        // Map Function for Factor Calculation
        static void MapFactor(Graph::Vertex key, KeyValue* keyvalue, void* calculator){
            Value value = castTo(Calculator,calculator)->getFactorValue(key);
            auto Common = COMMON;
            keyvalue->add(getCharAddress(Common),VERTEX_SIZE,getCharAddress(value),VALUE_SIZE);
        }
        
        // Reduce Function for Factor Calculation
        static void ReduceFactor(char* key, int keybytes, char* multivalue, int nvalues, int* valuebytes, KeyValue* keyvalue, void* calculator){ 
            // cout << ((nvalues==0)? "Trouble" : "Phew") <<endl;
            Value* values = castTo(Value,multivalue);    
            Value factor = accumulate(values,values+nvalues,0.0)/nvalues;
            keyvalue->add(key, VERTEX_SIZE,getCharAddress(factor),VALUE_SIZE);
        }
        
        // Gather Function for putting Factor Back
        static void GatherFactor(uint64_t index, char* key, int keybytes, char* value, int valuebytes, KeyValue* keyvalue, void* calculator)
        { castTo(Calculator,calculator)->factor = dereferenceTo(Value,value); } // Replace Factor
        
        inline Value& getFactor() {return factor;}
        inline Value& getNorm() {return norm;}

        // Map Function for Factor Calculation
        static void MapPageRank(Graph::Vertex key, KeyValue* keyvalue, void* calculator){
            Calculator* calc = castTo(Calculator,calculator);
            Value value = calc->factor; 
            keyvalue->add(getCharAddress(key), VERTEX_SIZE,getCharAddress(value),VALUE_SIZE);
            for (const Graph::Vertex& to : calc->toList[key]) {
                value = calc->getPageRankValue(key);    
                keyvalue->add(getCharAddress(to), VERTEX_SIZE,getCharAddress(value),VALUE_SIZE);
            }
        }
        
        // Reduce Function for Factor Calculation
        static void ReducePageRank(char* key, int keybytes, char* multivalue, int nvalues, int* valuebytes, KeyValue* keyvalue, void* calculator){ 
            // cout << ((nvalues==0)? "Trouble" : "Phew") <<endl;
            Value* values = castTo(Value,multivalue);
            Value new_pageRank = Constant::ALPHA *accumulate(values,values+nvalues,0.0);
            keyvalue->add(key, VERTEX_SIZE,getCharAddress(new_pageRank),VALUE_SIZE);
            // cout<<new_pageRank<<endl;
        }

        // Gather Function for putting PageRanks Back
        static void GatherPageRank(uint64_t index,char* key, int keybytes, char* value, int valuebytes, KeyValue* keyvalue, void* calculator){ 
            Calculator* calc = castTo(Calculator,calculator);
            Value new_pageRank = dereferenceTo(Value,value);
            Value& old_pageRank = calc->PageRanks[dereferenceTo(Graph::Vertex,key)]; // The old value's place
            calc->norm+=abs(old_pageRank-new_pageRank); // Change norm
            old_pageRank=new_pageRank; // Replace
        }

        static Value& calculateFactor(Calculator& calculator, MapReduce& mapreduce){
            Graph::Size N = calculator.PageRanks.size();            
            MPI_Barrier(MPI_COMM_WORLD);

            // Map-Reduce-Gather of Factor
            auto kvPairs = mapreduce.map(N,Calculator::MapFactor,((void*)&calculator));// Map Part
            mapreduce.collate(NULL);
            auto kvmPairs = mapreduce.reduce(Calculator::ReduceFactor,((void*)&calculator));// Reduce Part
            mapreduce.gather(HOME);
            mapreduce.broadcast(ROOT);
            kvPairs = mapreduce.map(&mapreduce,Calculator::GatherFactor,((void*)&calculator));// Gather Part
            return calculator.getFactor();
        }

        static Value performIteration(Column& pageRanks, Calculator& calculator, MapReduce& mapreduce){
            calculator.refresh(pageRanks);
            Value& value = calculateFactor(calculator,mapreduce);
            const Graph::Size N = pageRanks.size();
            
            // Map-Reduce-Gather of PageRank
            auto kvPairs = mapreduce.map(N,Calculator::MapPageRank,((void*)&calculator));// Map Part
            mapreduce.collate(NULL);
            auto kvmPairs = mapreduce.reduce(Calculator::ReducePageRank,((void*)&calculator));// Reduce Part
            mapreduce.gather(HOME);
            mapreduce.sort_keys(Calculator::INT_SORT);
            mapreduce.broadcast(ROOT);
            kvPairs = mapreduce.map(&mapreduce,Calculator::GatherPageRank,((void*)&calculator));// Gather Part
            return calculator.getNorm();
        }
    friend class PageRank;
};