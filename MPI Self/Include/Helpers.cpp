#include"MapReduce.hpp"
using namespace std;

// Class to Combine results from Map Class
class Combiner: Runnable<vector<CombinerTuple>,unordered_map<ReduceKey,vector<ReduceValue>>>{
    protected:
        // Iterates over all Key-Value tuples and Combines those Tuples which have Same Key into a List
        void run(){for(auto& tuple : input) results[tuple.first].push_back(tuple.second);}

};

// Class to Distribute results to different Reduce Class
class Distributor: Runnable<unordered_map<ReduceKey,vector<ReduceValue>>, vector<ReduceTuple>>{
    protected:
        // Converts the Map into a Vector to Save Memory Probably extremely inefficient
        // Will also do other || processing stuff later
        void run() {results = Results(input.begin(),input.end());}
};

// Class to Aggregate Results From ReduceTask
class Results{
    private:

    public:
};

