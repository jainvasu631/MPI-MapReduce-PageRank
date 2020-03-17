// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Custom Includes
#include "include/mapreduce.hpp"
#include "../Utility.cpp"
#include "../Graph.h"

// Computing Factor involved in PageRank using MapReduce
namespace Factor {

    typedef mapreduce::job<Factor::Map,Factor::Reduce, mapreduce::null_combiner,Factor::Data<Factor::Map>> Calculation;

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
            void operator()(Runtime& runtime, const key_type& key, Iterator it, Iterator end) const{runtime.emit(std::accumulate(it,end,0.0));}
    };

    template<typename MapTask>
    class Data : mapreduce::detail::noncopyable{
        private:
            Column& Hyperlinks;
            Column& pageRanks; 
            Graph::Vertex sequence;
            Graph::Size N;
        public:
            Data(const Column& h, Column& pg) :Hyperlinks(h), pageRanks(pg),sequence(0),N(h.size()) {}
            
            // Function to Setup Key for each Vertex, will return False when all Keys/Vertices are emitted.
            bool const setup_key(typename MapTask::key_type& key) {key=sequence++; return key<N;}
            // Function to setup the (Key,Value) data i.e pageRank's factor Component
            bool const get_data(typename MapTask::key_type const &key, typename MapTask::value_type& value) 
            {value = ((Hyperlinks[key]>0)? Constants::BETA : Constants::GAMMA) * pageRanks[key]; return true;}      
    };
}

int main(int argc, char **argv){
    
    
    cout << argv[argc-1] << endl;
    string filename = argv[argc-1];
    Graph graph(filename);
    Graph::Size N = graph.numVertices();
    Column hyperlinks = Utility::calculateHyperLinkColumn(graph);
    Column pageRanks(N,1.0/N);
    

    Factor::Calculation::datasource_type data(hyperlinks,pageRanks);
    
    mapreduce::specification spec;
    mapreduce::results result;
    Factor::Calculation calculation(data,spec);
    
    calculation.run<mapreduce::schedule_policy::cpu_parallel<Factor::Calculation>>(result);
    return 0;
}
