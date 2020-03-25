// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>
#include<iostream>

// Custom Includes
#include "include/mapreduce.hpp"
#include "../Utility.cpp"
#include "../Graph.h"

// Computing Factor involved in PageRank using MapReduce
class FactorCalculator {
    private:
        class Map : public mapreduce::map_task<Graph::Vertex, Value>{
            public:
                // Identity Map
                template<typename Runtime>
                void operator()(Runtime& runtime, const key_type& key, const value_type& value) const{runtime.emit_intermediate(COMMON,value);}
            private:
                static constexpr Graph::Size COMMON = 0;              
        };

        class Reduce : public mapreduce::reduce_task<Graph::Size, Value>{
            public:
                // Calculate Factor by Accumulation
                template<typename Runtime, typename Iterator>
                void operator()(Runtime& runtime, const key_type& key, Iterator it, Iterator end) const{runtime.emit(key,std::accumulate(it,end,0.0));}
        };
    public:
        class Data : mapreduce::detail::noncopyable{
            private:
                const Column& Hyperlinks;
                Column& PageRanks; 
                Graph::Vertex sequence;
                const Graph::Size N;
            public:
                // Main Constructor
                Data(const Column& h, Column& pageRanks) :Hyperlinks(h), N(h.size()), PageRanks(pageRanks),sequence(0) {}

                // Function to refresh the Class for another recalculation.
                void refresh(Column& pageRanks) {PageRanks = pageRanks; sequence=0;}
                // Function to Setup Key for each Vertex, will return False when all Keys/Vertices are emitted.
                bool const setup_key(Map::key_type& key) {key=sequence++; return key<N;}
                // Function to setup the (Key,Value) data i.e pageRank's factor Component
                bool const get_data(const Map::key_type& key, Map::value_type& value) 
                {value = ((Hyperlinks[key]>0)? Constant::BETA : Constant::GAMMA) * PageRanks[key]; return true;}      
        };

        using Calculation = mapreduce::job<Map,Reduce, mapreduce::null_combiner,Data>;

        static Value calculate(Column& pageRanks, Data& data){
            data.refresh(pageRanks);
            mapreduce::specification spec;
            Calculation calculation(data,spec);
            mapreduce::results result;
            calculation.run<mapreduce::schedule_policy::cpu_parallel<Calculation>>(result);
            return (calculation.begin_results()->second)/pageRanks.size(); // The factor
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