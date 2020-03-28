#include<vector>
// #include<mpi.h>
#include<algorithm>
#include<unordered_map>

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
using CombinerTuple = pair<ReduceKey, ReduceValue>;
using ReduceTuple = pair<ReduceKey, vector<ReduceValue>>;
using ResultTuple = pair<ResultKey, ResultValue>;

template<typename T1, typename T2>
class Runnable{
    public:
        // Forced Aliasing so that Child Classes can inherit this
        using Input = T1; 
        using Results = T2;

        // Explicit Constructor
        explicit Runnable(const Input& input_): input(input_), results(0){}
    protected:
        Results results; // Stores results of Runnable Function
        Input& input; // Stores Input of Runnable Function
        // Runner Function
        void run();
};

template<typename T1, typename T2, typename T3, typename T4>
class Task {
    public:
        // Alias Tuple Definitions
        using InputKey = T1;
        using InputValue = T2;
        using ResultKey = T3;
        using ResultValue = T4;
        
        using InputTuple = pair<T1,T2>;
        using ResultTuple = pair<T3,T4>;       
        
        using Input = vector<InputTuple>; 
        using Results = vector<ResultTuple>;
        
        // Constructor
        explicit Task(const Input& input_): input(input_), results(0){}

        // User Implemented Operator    
        void operator()(const InputKey& key, const InputValue& value, Task::Results& results);
    protected:
        Results results; // Stores results of Runnable Function
        Input& input; // Stores Input of Runnable Function
        
        // Iterates over input and runs the MapFunction
        void run(){for(auto& tuple: input) operator()(tuple.first,tuple.second,results);}
};