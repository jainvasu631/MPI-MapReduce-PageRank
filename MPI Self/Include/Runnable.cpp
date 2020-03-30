#pragma once
#include<vector>
#include "mpi.h"

using namespace std;

template<typename T1>
class InProcess{
    public:
        // Forced Aliasing so that Child Classes can inherit this
        using Input = T1;

        // Normal Constructor
        InProcess(Input& input_) {input=move(input_);}
        inline Input& getInput() {return input;}
        
    protected: Input input; // Stores Input of Runnable Function
};

template<typename T2>
class OutProcess{
    public:
        // Forced Aliasing so that Child Classes can inherit this
        using Results = T2;

        // Explicit Constructor
        OutProcess(): results(0){}
        inline Results& getResults() {return results;}

    protected: Results results; // Stores results of Runner
};

template<typename T1, typename T2>
class Runnable: public InProcess<T1>, public OutProcess<T2>{
    public:
        // Forced Aliasing so that Child Classes can inherit this
        using Input = T1;
        using Results = T2;

        // Explicit Constructor
        explicit Runnable(Input& input_): InProcess<Input>(input_), OutProcess<Results>() {}
        
        // Runner Function
        virtual void run()=0;
        
};

template<typename T1, typename T2, typename T3, typename T4>
class Task: public Runnable<vector<pair<T1,T2>>,vector<pair<T3,T4>>> {
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
        using Runnable<Input,Results>::Runnable;

        // User Implemented Operator    
        virtual void operator()(const InputKey& key, const InputValue& value)=0;

        // Iterates over input and runs the Operator() Function    
        virtual void run(){for(auto& tuple: this->input) operator()(tuple.first,tuple.second);}
     
        // Add the Key and Value to the Results
    protected: inline void emit(const ResultKey& key, const ResultValue& value) {this->results.push_back(ResultTuple(key,value));}
};