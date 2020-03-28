#include"MapReduce.hpp"
using namespace std;

// Map Class
// This Map operator() is Implemented by User 
// User is passed Key and Value by reference.
// It does (k1,v1)->list(k2,v2) where (k2,v2) are created as pairs and added to results
     
class MapTask: Task<MapKey,MapValue,ReduceKey,ReduceValue> {
    public: using Task::Task;
};

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

// Reduce Class
// This Reduce operator() is Implemented by User 
// User is passed Key and vector of Values by reference.
// It does (k2,list(v2))->list(k3,v3) where (k3,v3) are created as pairs and added to results
class ReduceTask : Task<ReduceKey,vector<ReduceValue>,ResultKey,ResultValue>{
    public: using Task::Task;
};


// Class to Aggregate Results From ReduceTask
// class Results{
//     private:

//     public:
// };

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
