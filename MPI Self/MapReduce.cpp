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

// Tuple typenames
using MapTuple = pair<MapKey,MapValue>;
using IntermediateTuple = pair<ReduceKey, ReduceValue>;
using ReduceTuple = pair<ReduceKey, vector<ReduceValue>>;
using ResultTuple = pair<ResultKey, ResultValue>;

class MapTask{
    
    public:
        using Results = vector<IntermediateTuple>;
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

class Combiner{

};

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

class DataSource{
    public:
        using Result = vector<MapTuple>;
    
        // This Function is Implemented by User 
        // User is passed Key and a Value by reference.
        // If User fills them with Data. They are inserted into the Results and User shall return True
        // If User returns false, this implies he is done with sending Data.
        bool const getData(MapKey& key, MapValue& value);
    private:
        Results result; // Stores output of Result Function
    protected:
        
        void run(){
            do{
                
            }while(true);
        }


};

class Results{
    private:

    public:
};