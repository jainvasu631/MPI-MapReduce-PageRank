#pragma once
#include<vector>

using namespace std;

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

        Results& getResults() {return results;}
};

template<typename T1, typename T2, typename T3, typename T4>
class Task: Runnable<vector<pair<T1,T2>>,vector<pair<T3,T4>>> {
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

        // User Implemented Operator    
        void operator()(const InputKey& key, const InputValue& value, Task::Results& results);
    
    // Iterates over input and runs the Operator() Function
    protected: void run(){for(auto& tuple: this->input) operator()(tuple.first,tuple.second,this->results);}
};