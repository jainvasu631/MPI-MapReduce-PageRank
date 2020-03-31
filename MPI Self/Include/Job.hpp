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
            MapTask maptask = processPipe<MapTask,Generator>(generator,workload);
            Combiner combiner = processPipe<Combiner,MapTask>(maptask);
            Distributor distributor = processPipe<Distributor,Combiner>(combiner);
            ReduceTask reducetask = processPipe<ReduceTask,Distributor>(distributor);
            Result result = processPipe<Result,ReduceTask>(reducetask);
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

        // Pipe Function to process the Input and Create Output from processed Input
        template <typename InProcess, typename Runnable> 
        inline InProcess processPipe(Runnable& runnable) {runnable.run(); return InProcess(runnable);} 

        // Pipe Function to process the Input and Create Output from processed Input with Workload
        template <typename InProcess, typename Runnable> 
        inline InProcess processPipe(Runnable& runnable,Workload& workload) {runnable.run(workload.first,workload.second); return InProcess(runnable);} 



    protected:
        Generator& generator;        
};