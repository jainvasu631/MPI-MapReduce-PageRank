// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Custom Includes
#include "include/mapreduce.hpp"
#include "../Graph.h"
#include "../Utility.cpp"
#include "FactorCalculator.cpp"

// Computing PageRanks using MapReduce
// PageRank essentially involves multiple iterations of Matrix Multiplication
// p->Gp where p is the pageRanks Column and G is the Google Matrix derived from the Graph

// (Key,Value) is (Vertex vertex, VertexList toListofVertex)
// (Intermediate Key, Intermediate Value) is (Vertex to, Value 1.0/toListofVertex.size())

// Map Function : (vertex, toListofVertex) -> for_each to in toListofVertex (to, 1.0/toListofVertex.size())
// Reduce Function : (to, Iterator<Value> containing all probabilities) -> (to, Value pageRank)

using VertexInfo = pair<Value,Graph::VertexList>;

class PageRankCalculator {
    private:
        class Map : public mapreduce::map_task<Graph::Vertex, VertexInfo >{
            public:
                // Function to generate Intermediate Values
                // Will map for each to Vertex a probability contribution that is summed by reduce  
                template<typename Runtime>
                void operator()(Runtime& runtime, const key_type& key, const value_type& value) const
                {for (const Graph::Vertex& to : value.second) runtime.emit_intermediate(to, value.first);}
        };

        class Reduce : public mapreduce::reduce_task<Graph::Vertex, Value>{
            public:
                // Function to add all probabilities in Hyperlink Matrix
                template<typename Runtime, typename Iterator>
                void operator()(Runtime& runtime, const key_type& key, Iterator it, Iterator end) const{runtime.emit(key,std::accumulate(it,end,0.0));}
        };

    public:
        class Data : mapreduce::detail::noncopyable{
            private:
                const Graph::Size N;
                const Graph::ToList& toList;
                const Column& Hyperlinks;
                Column& PageRanks; 
                Graph::Vertex sequence;
            public:
                // Main Constructor
                Data(const Column& hyperlinks, const Graph::ToList& tList, Column& pageRanks) 
                    : N(tList.size()), toList(tList), Hyperlinks(hyperlinks), PageRanks(pageRanks) ,sequence(0) {}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {PageRanks = pageRanks; sequence=0;}
                // Function to Setup Key for each Vertex, will return False when all Keys/Vertices are emitted.
                bool const setup_key(Map::key_type& key) {key=sequence++; return key<N;}
                // Function to setup the (Key,Value) data i.e Value is VertexInfo
                bool const get_data(const Map::key_type& key, Map::value_type& value) 
                {value = VertexInfo(Hyperlinks[key]*PageRanks[key],toList[key]); return true;}
        };

        using Iteration = mapreduce::job<Map,Reduce, mapreduce::null_combiner,Data>;

        static Value performIteration(Column& pageRanks, PageRankCalculator::Data& pageRankData, FactorCalculator::Data& factorData){
            const Value factor = FactorCalculator::calculate(pageRanks,factorData);
            pageRankData.refresh(pageRanks);
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