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
            using FactorJob::MapTask::MapTask;
            // Identity Map
            public: void operator()(const InputKey& key, const InputValue& value) {emit(COMMON,value);}
            private: static constexpr Graph::Size COMMON = 0;              
        };

        class ReduceFactor: public FactorJob::ReduceTask{
            using FactorJob::ReduceTask::ReduceTask;
            // Calculate Factor by Accumulation
            public: void operator()(const InputKey& key, const InputValue& value) {emit(key,accumulate(value.begin(),value.end(),0.0));}
        };

        class OutputFactor: public FactorJob::Result{
            using FactorJob::Result::Result;
            // return the Factor after dividing by N
            public: Value getFactor(Graph::Size N) {return input[0].second/N;}
        };

        using VertexInfo = pair<Value,Graph::VertexList>;
        using PageRankJob = MapReduce<Graph::Vertex,VertexInfo,Graph::Vertex,Value,Graph::Vertex,Value>;
        
        class MapPageRank : public PageRankJob::MapTask{
            using PageRankJob::MapTask::MapTask;
            // Function to generate Intermediate Values
            // Will map for each to Vertex a probability contribution that is summed by reduce  
            // Emitting Zero Insures that each Node has atleast Tuple    
            public: void operator()(const InputKey& key, const InputValue& value)
                {emit(key, ZERO); for (const Graph::Vertex& to : value.second) emit(to, value.first);}
            private: static constexpr Value ZERO = 0.0;              
        
        };

        class ReducePageRank : public PageRankJob::ReduceTask{
            using PageRankJob::ReduceTask::ReduceTask;
            // Function to add all probabilities in Hyperlink Matrix        
            public: void operator()(const InputKey& key, const InputValue& value) {emit(key,accumulate(value.begin(),value.end(),0.0));}
        };

        class OutputPageRank: public PageRankJob::Result{
            using PageRankJob::Result::Result;
            // Function to calc the PageRanks from the Output of Reduce Function returns the new PageRanks
            public: void getPageRanks(Column& pageRanks_,Value factor) 
                {for(const PageRankJob::ResultTuple& tuple:input) pageRanks_[tuple.first]=Constant::ALPHA*(tuple.second+factor);}
        };

    public:
        class FactorData: public FactorJob::Generator{
            public:
                const Column& Hyperlinks;
                Column& PageRanks; 
                const Graph::Size N;

                // Main Constructor
                FactorData(const Column& h, Column& pageRanks) :Hyperlinks(h), N(h.size()), PageRanks(pageRanks) {this->totalKeys=N;}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {PageRanks = pageRanks;}

                // Inline function to get Value from the key
                inline MapFactor::InputValue getValue(const MapFactor::InputKey& key) 
                    {return ((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA)*PageRanks[key];}
                
                // Function to setup the (Key,Value) data i.e pageRank's factor Component
                bool const getData(int keyId, MapFactor::InputKey& key, MapFactor::InputValue& value) 
                    {key=keyId; value = getValue(key); return true;}      
        };

        class PageRankData: public PageRankJob::Generator{
            private:
                const Graph::ToList& toList;
                FactorData factorData;
            public:
                // Main Constructor
                PageRankData(const Column& hyperlinks, const Graph::ToList& tList, Column& pageRanks) 
                    : factorData(hyperlinks,pageRanks), toList(tList) {this->totalKeys=pageRanks.size();}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {factorData.refresh(pageRanks);}
                
                // Inline function to get Value from the key
                inline MapPageRank::InputValue getValue(const MapFactor::InputKey& key) 
                    {return VertexInfo(factorData.Hyperlinks[key]*factorData.PageRanks[key],toList[key]);}
                
                // Function to setup the (Key,Value) data i.e Value is VertexInfo
                bool const getData(int keyId, MapPageRank::InputKey& key, MapPageRank::InputValue& value)
                    {key=keyId; value = getValue(key); return true;} 

            friend class Calculator;
        };

        using Calculation = Job<MapFactor,ReduceFactor,FactorData,FactorJob::Combiner,FactorJob::Distributor,OutputFactor>;
        using Iteration = Job<MapPageRank,ReducePageRank,PageRankData,PageRankJob::Combiner,PageRankJob::Distributor,OutputPageRank>;
    
        static Value calculateFactor(FactorData& factorData){
            Calculation calculation(factorData);
            OutputFactor outputFactor = calculation.run();
            return outputFactor.getFactor(factorData.N); // The factor
        }

        static Value performIteration(Column& pageRanks,Column& pageRanks_,Calculator::PageRankData& pageRankData){
            pageRankData.refresh(pageRanks);
            const Value factor = calculateFactor(pageRankData.factorData);
            Iteration iteration(pageRankData);
            OutputPageRank outputPageRank = iteration.run();
            outputPageRank.getPageRanks(pageRanks_,factor); // Put New PageRanks back into pageRanks and return the Norm
            return Utility::calculateNorm(pageRanks,pageRanks_);
        }
        
        
};