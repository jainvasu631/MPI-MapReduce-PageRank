#include<vector>
#include<algorithm>
// This is the Calculator class through which calls to the MapReduce will be used.
// THis is partially Inspired by the C++ cdmh Library and its use of templates

#include "Include/Job.hpp"
#include "../Graph.h"
#include "../Utility.cpp"

class Calculator {    
    private:
        using FactorJob = MapReduce<Graph::Vertex,Value,Graph::Size,Value,Graph::Size,Value>;
    
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

        using VertexInfo = pair<Value,Graph::VertexList>;
        using PageRankJob = MapReduce<Graph::Vertex,VertexInfo,Graph::Vertex,Value,Graph::Vertex,Value>;
        
        class MapPageRank : public PageRankJob::MapTask{
            // Function to generate Intermediate Values
            // Will map for each to Vertex a probability contribution that is summed by reduce      
            public: void operator()(const InputKey& key, const InputValue& value)
                {for (const Graph::Vertex& to : value.second) emit(to, value.first);}
        };

        class ReducePageRank : public PageRankJob::ReduceTask{
            // Function to add all probabilities in Hyperlink Matrix        
            public: void operator()(const InputKey& key, const InputValue& value) {emit(key,accumulate(value.begin(),value.end(),0.0));}
        };

        class OutputPageRank: public PageRankJob::Output{
            // return the PageRanks
            public: Value getPageRanks(Column& pageRanks, Value factor) {
                Value norm;
                // for(Graph::Size i=0;i<N;i++) { // Recalculate All the PageRanks.
                //     new_rank = Constant::ALPHA*((i==it->first)? (it++)->second + factor : factor);
                //     norm+=abs(new_rank-pageRanks[i]);
                //     pageRanks[i]=new_rank;
                // }
                return norm;
            }
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
                inline MapFactor::InputValue getValue(const MapFactor::InputKey& key) 
                    {return((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key];}
                
                // Function to setup the (Key,Value) data i.e pageRank's factor Component
                bool const getData(int keyId, MapFactor::InputKey& key, MapFactor::InputValue& value) 
                    {key=keyId; value = getValue(key); return true;}      
        };

        class PageRankData: public PageRankJob::Generator{
            private:
                const Graph::ToList& toList;
                Graph::Vertex sequence;
                FactorData factorData;
            public:
                // Main Constructor
                PageRankData(const Column& hyperlinks, const Graph::ToList& tList, Column& pageRanks) 
                    : factorData(hyperlinks,pageRanks), toList(tList), sequence(0) {}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {factorData.refresh(pageRanks); sequence=0;}
                
                // Inline function to get Value from the key
                inline MapPageRank::InputValue getValue(const MapFactor::InputKey& key) 
                    {return VertexInfo(factorData.Hyperlinks[key]*factorData.PageRanks[key],toList[key]);}
                
                // Function to setup the (Key,Value) data i.e Value is VertexInfo
                bool const getData(int keyId, MapPageRank::InputKey& key, MapPageRank::InputValue& value)
                    {key=keyId; value = getValue(key); return true;} 

            friend class Calculator;
        };

        using Calculation = Job<MapFactor,ReduceFactor,FactorData,OutputFactor>;
        using Iteration = Job<MapPageRank,ReducePageRank,PageRankData,OutputPageRank>;
    
        static Value calculateFactor(FactorData& factorData){
            Calculation calculation(factorData);
            OutputFactor outputFactor = calculation.run();
            return outputFactor.getFactor(factorData.N); // The factor
        }

        static Value performIteration(Column& pageRanks, Calculator::PageRankData& pageRankData){
            pageRankData.refresh(pageRanks);
            const Value factor = calculateFactor(pageRankData.factorData);
            Iteration iteration(pageRankData);
            OutputPageRank outputPageRank = iteration.run();
            return outputPageRank.getPageRanks(pageRanks,factor); // Put New PageRanks back into pageRanks and return the Norm
        }
        
        
};