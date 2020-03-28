#include"MapReduce.hpp"
using namespace std;

// Map Class
class MapTask{
    
    public:
        using Results = vector<CombinerTuple>;
        using Input = vector<MapTuple>;    
    
        // This Map Function is Implemented by User 
        // User is passed Key and Value by reference.
        // It does (k1,v1)->list(k2,v2) where (k2,v2) are created as pairs and added to results
        void operator()(const MapKey& key, const MapValue& value, Results& results);
    private:
        Results results; // Stores results of Map Function
        Input input; // Stores Input of Map Function
    protected:
        // Iterates over input and runs the MapFunction
        void run(){for(auto& tuple: input) operator()(tuple.first,tuple.second,results);}
};

// Class to Combine results from Map Class
class Combiner{
    public:
        using Input = vector<CombinerTuple>;
        using Results = unordered_map<ReduceKey,vector<ReduceValue>>;

    private:
        Results results; // Stores results of Combine Function
        Input input; // Stores Input of Combine Function
    protected:
        // Iterates over all Key-Value tuples and Combines those Tuples which have Same Key into a List
        void run(){for(auto& tuple : input) results[tuple.first].push_back(tuple.second);}

};

// Class to Distribute results to different Reduce Class
class Distributor{
        public:
        using Input = unordered_map<ReduceKey,vector<ReduceValue>>;
        using Results = vector<ReduceTuple>;

    private:
        Results results; // Stores results of Distribute Function
        Input input; // Stores Input of Distribute Function
    protected:
        // Converts the Map into a Vector to Save Memory Probably extremely inefficient
        // Will also do other || processing stuff later
        void run() {results = Results(input.begin(),input.end());}
};

// Reduce Class
class ReduceTask{
    public:
        using Results = vector<ResultTuple>;
        using Input = vector<ReduceTuple>;
    
        // This Reduce Function is Implemented by User 
        // User is passed Key and vector of Values by reference.
        // It does (k2,list(v2))->list(k3,v3) where (k3,v3) are created as pairs and added to results
        void operator()(const ReduceKey& key, const vector<ReduceValue>& values, Results& results);
    private:
        Results results; // Stores results of Reduce Function
        Input input; // Stores Input of Reduce Function
    protected:
        // Iterates over input and runs the Reduce Function
        void run(){for(auto& tuple: input) operator()(tuple.first,tuple.second,results);}

};

// Class to Generate Data
class DataSource{
    public:
        using Results = vector<MapTuple>;
    
        // This Function is Implemented by User 
        // User is passed Key and a Value by reference.
        // If User fills them with Data. They are inserted into the Results.
        // User shall returns false if they have inserted a tuple and true if haven't and are done.
        bool const getData(MapKey& key, MapValue& value);
    private:
        Results results; // Stores output of Result Function
    protected:
        // When number of keys is not known in advance.
        void run(){
            bool done;
            while(!done){
                MapTuple tuple;// Fresh Declaration as Tuple is passed by Reference
                results.push_back(tuple);
                done = getData(tuple.first,tuple.second); 
            }
            results.pop_back();
        }
        // When number of keys is known in advance .. More Efficient
        void run(unsigned int numKeys) {results.resize(numKeys); for(auto& tuple : results) getData(tuple.first,tuple.second);}
};

// Class to Aggregate Results From ReduceTask
class Results{
    private:

    public:
};