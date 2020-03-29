#pragma once

// #include<vector>
#include<mpi.h>
// #include<algorithm>
// #include<unordered_map>

#include "MapReduce.cpp"

using namespace std;

template <typename MapTask, typename ReduceTask, typename Generator, typename Output>
class Job {
    
    public:
        // Explicit Constructor
        Job(Generator& generator_) : generator(generator_) {MPI_Comm_rank(MPI_COMM_WORLD,&Rank);MPI_Comm_size(MPI_COMM_WORLD,&Size);}

        Output run(){
            int totalKeys;
            Workload workload = getProcessLoad(totalKeys);
            generator.run(Workload.first,Workload.second);
            MapTask maptask(generator.getResults());
            maptask.run();
            
        }

    private:
        int Rank,Size;
        
        using Workload = pair<unsigned int, unsigned int>; // Will store information about the Offset and Individual Process Keys

        // Used by each thread/process to find its range of work
        Workload getProcessLoad(int totalKeys){
            int numkeys = (totalKeys/Size);
            int offset = Rank*numkeys;
            int process_numKeys = (Rank==Size-1)? totalKeys-(Rank-1)*numkeys : numkeys;
            return Workload(offset, process_numkeys);
        }

    protected:
        Generator& generator;        
};