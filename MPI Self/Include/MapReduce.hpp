#pragma once

#include<vector>
// #include<mpi.h>
#include<algorithm>
#include<unordered_map>

#include "Runnable.cpp"
#include "DataSource.cpp"

using namespace std;

// Temporary typenames because using templates is annoying
// using MapKey = int;
// using MapValue = int;
// using ReduceKey = int;
// using ReduceValue = int;
// using ResultKey = int;
// using ResultValue = int;

template<typename MapKey, typename MapValue, typename ReduceKey, typename ReduceValue, typename ResultKey, typename ResultValue>
class MapReduce{
    public:
        // Tuple typenames
        using MapTuple = pair<MapKey,MapValue>;
        using CombinerTuple = pair<ReduceKey, ReduceValue>;
        using ReduceTuple = pair<ReduceKey, vector<ReduceValue>>;
        using ResultTuple = pair<ResultKey, ResultValue>;

        // Map Class
        // This Map operator() is Implemented by User 
        // User is passed Key and Value by reference.
        // It does (k1,v1)->list(k2,v2) where (k2,v2) are created as pairs and added to results 
        using MapTask = Task<MapKey,MapValue,ReduceKey,ReduceValue>;

        // Reduce Class
        // This Reduce operator() is Implemented by User 
        // User is passed Key and vector of Values by reference.
        // It does (k2,list(v2))->list(k3,v3) where (k3,v3) are created as pairs and added to results
        using ReduceTask = Task<ReduceKey,vector<ReduceValue>,ResultKey,ResultValue>;

        // Source class
        using Source = DataSource<MapKey,MapValue>;

        // Combiner Class to combine results from Map Class
        class Combiner: Runnable<vector<CombinerTuple>,unordered_map<ReduceKey,vector<ReduceValue>>>{
            // Iterates over all Key-Value tuples and Combines those Tuples which have Same Key into a List
            protected: void run(){for(auto& tuple : this->input) this->results[tuple.first].push_back(tuple.second);}
        };

        // Distributor Class to Distribute results to different Reduce Class
        class Distributor: Runnable<unordered_map<ReduceKey,vector<ReduceValue>>, vector<ReduceTuple>>{
            // Converts the Map into a Vector to Save Memory Probably extremely inefficient
            // Will also do other || processing stuff later
            protected: void run() {this->results = Results(this->input.begin(),this->input.end());}
        };

        // Class to Aggregate Results From ReduceTask
        class Results{
            private:

            public:
        };


    protected:
        MapTask map;
        ReduceTask reduce;
        Source source;
        Combiner combiner;
        Distributor distributor;
}; 


