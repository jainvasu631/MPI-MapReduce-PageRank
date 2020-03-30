#pragma once

// #include<vector>
#include<mpi.h>
// #include<algorithm>
// #include<unordered_map>

#include "MapReduce.hpp"

using namespace std;

template <typename MapTask, typename ReduceTask, typename Generator, typename Combiner, typename Distributor, typename Result>
class Job {
    
    public:
        // Explicit Constructor
        Job(Generator& generator_) : generator(generator_) {MPI_Comm_rank(MPI_COMM_WORLD,&Rank);MPI_Comm_size(MPI_COMM_WORLD,&Size);}

        // Purely Serial Run Function -- To check if library is working
        Result run(){
            Workload workload = getProcessLoad(generator.totalKeys);
            generator.run(workload.first,workload.second);
            // cout<<"Ran Generator"<<endl;
            MapTask maptask(generator);
            maptask.run();
            // cout<<"Ran MapTask"<<endl;
            Combiner combiner(maptask);
            combiner.run();
            // cout<<"Ran Combiner"<<endl;
            Distributor distributor(combiner);
            distributor.run();
            // cout<<"Ran Distributor"<<endl;
            ReduceTask reducetask(distributor);
            reducetask.run();
            // cout<<"Ran ReduceTask"<<endl;
            Result result(reducetask);
            // cout<<"Ran Calculation"<<endl;
            return result;     
        }

    private:
        int Rank,Size;
        
        using Workload = pair<unsigned int, unsigned int>; // Will store information about the Offset and Individual Process Keys

        // Used by each thread/process to find its range of work
        Workload getProcessLoad(int totalKeys){
            int numkeys = (totalKeys/Size);
            int offset = Rank*numkeys;
            int process_numkeys = (Rank==Size-1)? totalKeys-(Rank)*numkeys : numkeys;
            return Workload(offset, process_numkeys);
        }

    protected:
        Generator& generator;        
};