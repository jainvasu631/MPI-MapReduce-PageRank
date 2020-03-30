#pragma once

#include<vector>
#include<mpi.h>
#include<algorithm>
#include<unordered_map>

#include "Runnable.cpp"

using namespace std;

// Templated Class/Namespace. From Here The User Can Inherit MapTask, ReduceTask, Generator and Output Classes
// MapKey and MapValue may have a variable size
// All other Key and Values must be atomic or have a fixed size.
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

        // Class to Generate Data
        class Generator: public OutProcess<vector<MapTuple>>{    
            public: 
                unsigned int totalKeys;
                // User defined function. Where user is passed reference to Key,Value and a KeyId
                virtual bool const getData(int keyId, MapKey& key, MapValue& value)=0;
                // The numkeys is the number of keys to be created from the Generator Object
                // The offset is to allow a way to divide key generation across multiple processors.
                virtual void run(int offset, int numKeys) {
                this->results.resize(numKeys); 
                for(auto& tuple : this->results) 
                    this->getData(offset++,tuple.first,tuple.second);
                }
        };
                
        // Combiner Class to combine results from Map Class
        class Combiner: public Runnable<vector<CombinerTuple>,unordered_map<ReduceKey,vector<ReduceValue>>>{
            using Runnable<vector<CombinerTuple>,unordered_map<ReduceKey,vector<ReduceValue>>>::Runnable;
            // Iterates over all Key-Value tuples and Combines those Tuples which have Same Key into a List
            public: void run(){for(auto& tuple : this->input) this->results[tuple.first].push_back(tuple.second);}
        };

        // Distributor Class to Distribute results to different Reduce Class
        class Distributor: public Runnable<unordered_map<ReduceKey,vector<ReduceValue>>, vector<ReduceTuple>>{
            using Runnable<unordered_map<ReduceKey,vector<ReduceValue>>, vector<ReduceTuple>>::Runnable;
            // Converts the Map into a Vector to Save Memory Probably extremely inefficient
            // Will also do other || processing stuff later
            public: void run() {this->results.insert(this->results.end(),this->input.begin(),this->input.end());}
        };

        // Result Class
        // Class to Aggregate Results From ReduceTask and do final processing
        // Will be subclasses by User to do what the user wants.
        using Result = InProcess<vector<ResultTuple>>;

}; 


