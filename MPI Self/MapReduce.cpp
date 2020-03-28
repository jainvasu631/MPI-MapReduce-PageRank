#include<vector>
#include<mpi.h>
#include<algorithm>

using namespace std;

// Temporary typenames because using templates is annoying
using MapKey = int;
using MapValue = int;
using ReduceKey = int;
using ReduceValue = int;
using ResultKey = int;
using ResultValue = int;

class MapTask{
    
    public:
        using Results = vector<pair<ReduceKey,ReduceValue>>;
        using Input = vector<pair<MapKey, MapValue>>;    
    
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

class Combiner{

};

class ReduceTask{
    private:
        using Results = vector<pair<ResultKey,ResultValue>>;
        using Input = vector<pair<ResultKey, ResultValue>>;
    public:
};

class DataSource{

};

class Results{
    private:

    public:
};