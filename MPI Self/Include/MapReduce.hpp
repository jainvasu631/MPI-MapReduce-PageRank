#pragma once

#include<vector>
// #include<mpi.h>
#include<algorithm>
#include<unordered_map>

#include "Runnable.cpp"

using namespace std;

// Temporary typenames because using templates is annoying

// MapKey and MapValue may have a variable size
// All other Key and Values must be atomic or have a fixed size.
template<typename MapKey, typename MapValue, typename ReduceKey, typename ReduceValue, typename ResultKey, typename ResultValue, typename DataSource>
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
        class Generator: Runnable<DataSource,vector<MapTuple>>{    
            
            // User defined function. Where user is passed reference to Key,Value and a KeyId
            public: bool const getData(int keyId, MapKey& key, MapValue& value);
            
            // The numkeys is the number of keys to be created from the Generator Object
            // The offset is to allow a way to divide key generation across multiple processors.
            protected: void run(int offset, int numKeys) {
                this->results.resize(numKeys); 
                for(auto& tuple : this->results) 
                    this->getData(offset++,tuple.first,tuple.second);
            }
        };
                
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
        class Output{
            private:
            
            public:
        };

        // Explicit Constructor
        MapReduce(DataSource& data) : generator(data) {MPI_Comm_rank(MPI_COMM_WORLD,&Rank);MPI_Comm_size(MPI_COMM_WORLD,&Size);}

    private:
        int Rank,Size;
        
        using Workload = pair<unsigned int, unsigned int>; // Will store information about the Offset and Individual Process Keys

        // Used by each thread/process to find its range of work
        Workload getProcessLoad(int totalKeys){
            int numkeys = (totalKeys/Size);
            int offset = Rank*numkeys;
            int process_numKeys = (Rank==Size-1)? totalKeys-(Rank-1)*numkeys : numkeys;
            return Workload(offset, process_numkeys);
        }

    protected:
        Combiner combiner;
        Distributor distributor;
        Generator generator;

        Output run(){
            int totalKeys;
            Workload workload = getProcessLoad(totalKeys);
            generator.run(Workload.first,Workload.second);
            MapTask maptask(generator.getResults());
            maptask.run();
            
        }
}; 


