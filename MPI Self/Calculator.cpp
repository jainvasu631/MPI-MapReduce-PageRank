#include<vector>
#include<algorithm>
// This is the Calculator class through which calls to the MapReduce will be used.
// THis is partially Inspired by the C++ cdmh Library and its use of templates

#include "Include/Job.hpp"
#include "../Graph.h"
#include "../Utility.cpp"

class Calculator {    
    using FactorJob = MapReduce<Graph::Vertex,Value,Graph::Size,Value,Graph::Size,Value>;
    
    private:
        class MapFactor: public FactorJob::MapTask{
            // Identity Map
            public: void operator()(const InputKey& key, const InputValue& value) {emit(COMMON,value);}
            private: static constexpr Graph::Size COMMON = 0;              
        };

        class ReduceFactor: public FactorJob::ReduceTask{
            // Calculate Factor by Accumulation
            public: void operator()(const InputKey& key, const InputValue& value) {emit(key,accumulate(value.begin(),value.end(),0.0));}
        };

        class OutputFactor: public FactorJob::Output{
            // return the Factor after dividing by N
            public: Value getFactor(Graph::Size N) {return results[0].second/N;}
        };

    public:
        class FactorData: public FactorJob::Generator{
            public:
                const Column& Hyperlinks;
                Column& PageRanks; 
                Graph::Vertex sequence;
                const Graph::Size N;

                // Main Constructor
                FactorData(const Column& h, Column& pageRanks) :Hyperlinks(h), N(h.size()), PageRanks(pageRanks),sequence(0) {}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {PageRanks = pageRanks; sequence=0;}

                // Inline function to get Value from the key
                inline Value getValue(const MapFactor::InputKey& key) 
                    {return((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key];}
                
                // Function to setup the (Key,Value) data i.e pageRank's factor Component
                bool const getData(int keyId, MapFactor::InputKey& key, MapFactor::InputValue& value) 
                    {key=keyId; value = getValue(key); return true;}      
        };

        using Calculation = Job<MapFactor,ReduceFactor,FactorData,OutputFactor>;
    
        static Value calculateFactor(FactorData& factorData){
            Calculation calculation(factorData);
            OutputFactor outputFactor = calculation.run();
            return outputFactor.getFactor(factorData.N); // The factor
        }
        
        
};