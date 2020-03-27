// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Custom Includes
#include "include/mapreduce.hpp"
#include "../Graph.h"
#include "../Utility.cpp"

class Calculator {
    private:
        class MapFactor : public mapreduce::map_task<Graph::Vertex, Value>{
            // Identity Map
            public: template<typename Runtime>
                void operator()(Runtime& runtime, const key_type& key, const value_type& value) const{runtime.emit_intermediate(COMMON,value);}
            private: static constexpr Graph::Size COMMON = 0;              
        };

        class ReduceFactor : public mapreduce::reduce_task<Graph::Size, Value>{
            // Calculate Factor by Accumulation
            public: template<typename Runtime, typename Iterator>
                void operator()(Runtime& runtime, const key_type& key, Iterator it, Iterator end) const{runtime.emit(key,std::accumulate(it,end,0.0));}
        };

        using VertexInfo = pair<Value,Graph::VertexList>;
        class MapPageRank : public mapreduce::map_task<Graph::Vertex, VertexInfo >{
            // Function to generate Intermediate Values
            // Will map for each to Vertex a probability contribution that is summed by reduce      
            public: template<typename Runtime>
                void operator()(Runtime& runtime, const key_type& key, const value_type& value) const
                {for (const Graph::Vertex& to : value.second) runtime.emit_intermediate(to, value.first);}
        };

        class ReducePageRank : public mapreduce::reduce_task<Graph::Vertex, Value>{
            // Function to add all probabilities in Hyperlink Matrix        
            public: template<typename Runtime, typename Iterator>
                void operator()(Runtime& runtime, const key_type& key, Iterator it, Iterator end) const{runtime.emit(key,std::accumulate(it,end,0.0));}
        };

        class FactorData : mapreduce::detail::noncopyable{
            public:
                const Column& Hyperlinks;
                Column& PageRanks; 
                Graph::Vertex sequence;
                const Graph::Size N;

                // Main Constructor
                FactorData(const Column& h, Column& pageRanks) :Hyperlinks(h), N(h.size()), PageRanks(pageRanks),sequence(0) {}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {PageRanks = pageRanks; sequence=0;}
                // Function to Setup Key for each Vertex, will return False when all Keys/Vertices are emitted.
                bool const setup_key(MapFactor::key_type& key) {key=sequence++; return key<N;}
                // Function to setup the (Key,Value) data i.e pageRank's factor Component
                bool const get_data(const MapFactor::key_type& key, MapFactor::value_type& value) 
                {value = ((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key]; return true;}      
        };

    public:
        class PageRankData : mapreduce::detail::noncopyable{
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
                // Function to Setup Key for each Vertex, will return False when all Keys/Vertices are emitted.
                bool const setup_key(MapPageRank::key_type& key) {key=sequence++; return key<factorData.N;}
                // Function to setup the (Key,Value) data i.e Value is VertexInfo
                bool const get_data(const MapPageRank::key_type& key, MapPageRank::value_type& value) 
                {value = VertexInfo(factorData.Hyperlinks[key]*factorData.PageRanks[key],toList[key]); return true;}
            
            friend class Calculator;
        };

        using Calculation = mapreduce::job<MapFactor,ReduceFactor, mapreduce::null_combiner,FactorData>;
        using Iteration = mapreduce::job<MapPageRank, ReducePageRank, mapreduce::null_combiner,PageRankData>;

        static Value calculateFactor(FactorData& factorData){
            mapreduce::specification spec;
            Calculation calculation(factorData,spec);
            mapreduce::results result;
            calculation.run<mapreduce::schedule_policy::cpu_parallel<Calculation>>(result);
            return (calculation.begin_results()->second)/factorData.N; // The factor
        }
        
        static Value performIteration(Column& pageRanks, Calculator::PageRankData& pageRankData){
            pageRankData.refresh(pageRanks);
            const Value factor = calculateFactor(pageRankData.factorData);
            mapreduce::specification spec;
            Iteration iteration(pageRankData,spec);
            mapreduce::results result;
            iteration.run<mapreduce::schedule_policy::cpu_parallel<Iteration>>(result);
            
            // Put New PageRanks back into pageRanks
            const Graph::Size N = pageRanks.size();
            auto it=iteration.begin_results();
            Value new_rank,norm=0;
            for(Graph::Size i=0;i<N;i++) { // Recalculate All the PageRanks.
                new_rank = Constant::ALPHA*((i==it->first)? (it++)->second + factor : factor);
                norm+=abs(new_rank-pageRanks[i]);
                pageRanks[i]=new_rank;
            }
            return norm;
        }

        static void check(mapreduce::results result){
            // output the results
            std::cout << "\n\n  " << "Map:";
            std::cout << "\n    " << "Total Map keys                           : " << result.counters.map_keys_executed;
            std::cout << "\n    " << "Map keys processed                       : " << result.counters.map_keys_completed;
            std::cout << "\n    " << "Map key processing errors                : " << result.counters.map_key_errors;
            std::cout << "\n    " << "Number of Map Tasks run (in parallel)    : " << result.counters.actual_map_tasks;
            std::cout << "\n\n  " << "Reduce:";
            std::cout << "\n    " << "Number of Reduce Tasks run (in parallel) : " << result.counters.actual_reduce_tasks;
            std::cout << "\n    " << "Number of Result Files                   : " << result.counters.num_result_files;
            std::cout << "\n    " << "Total Reduce keys                        : " << result.counters.reduce_keys_executed;
            std::cout << "\n    " << "Reduce keys processed                    : " << result.counters.reduce_keys_completed;
            std::cout << "\n    " << "Reduce key processing errors             : " << result.counters.reduce_key_errors << std::endl;
        }
        
};