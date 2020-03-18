// STL Includes
#include<algorithm>
#include<chrono>
#include<iostream>
#include<numeric>
#include<string>

// Custom Includes
#include "../Graph.h"
#include "../Utility.cpp"
#include "Calculator.cpp"

// MPI-MapReduce Library Includes
#include "mpi.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace std;
using namespace chrono;
using namespace MAPREDUCE_NS;

// MPI Constants
int RANK,SIZE,HOME=1;

class PageRank{
    private:
        static Value& calculateFactor(Calculator& calculator, MapReduce& mapreduce){
            Graph::Size N = calculator.PageRanks.size();            
            MPI_Barrier(MPI_COMM_WORLD);

            auto kvPairs = mapreduce.map(N,Calculator::MapFactor,((void*)&calculator));// Map Part
            mapreduce.collate(NULL);
            auto kvmPairs = mapreduce.reduce(Calculator::ReduceFactor,((void*)&calculator));// Reduce Part
            mapreduce.gather(HOME);
            kvPairs = mapreduce.map(&mapreduce,Calculator::GatherFactor,((void*)&calculator));// Gather Part
            MPI_Barrier(MPI_COMM_WORLD);
            return calculator.getFactor();
        }

        static Value performIteration(Column& pageRanks, Calculator& calculator, MapReduce& mapreduce){
            calculator.refresh(pageRanks);
            Value& value = calculateFactor(calculator,mapreduce);
            const Graph::Size N = pageRanks.size();
            
            MPI_Bcast((void*) &value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            
            auto kvPairs = mapreduce.map(N,Calculator::MapPageRank,((void*)&calculator));// Map Part
            mapreduce.collate(NULL);
            
            auto kvmPairs = mapreduce.reduce(Calculator::ReducePageRank,((void*)&calculator));// Reduce Part
            MPI_Barrier(MPI_COMM_WORLD);
            mapreduce.gather(HOME);
            mapreduce.sort_keys(Calculator::INT_SORT);
            
            
            kvPairs = mapreduce.map(&mapreduce,Calculator::GatherPageRank,((void*)&calculator));// Gather Part
            
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Bcast(pageRanks.data(), N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            // if(RANK==0) Utility::printPageRank(pageRanks);
            MPI_Barrier(MPI_COMM_WORLD);
            return calculator.getNorm();
        }

    public:
        // Find PageRank      
        static Column calculatePageRank(const Graph& graph){
            Column pageRanks = Utility::getInitPageRank(graph.numVertices()); // This will contain the PageRanks at the end of the function
            const Column hyperlinks = Utility::calculateHyperLinkColumn(graph.toList); 
            Calculator calculator(hyperlinks,graph.toList,pageRanks);
            MapReduce mapreduce(MPI_COMM_WORLD);
            Value norm;
            unsigned int iteration=1;
            // Main Iteration
            do{
                cout<< "Performing "<< iteration++ <<" Iteration.";
                norm = performIteration(pageRanks,calculator,mapreduce);
                MPI_Bcast((void*) &norm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                cout<<" Current norm is "<< norm << endl;
            } while(norm>Constant::TOL);
            return pageRanks;
        }
        // Testing Function
        static void test(const string filename){
            Graph graph = Utility::timedGraphCreation(filename);
            // graph.print();

            // PageRank Algorithm
            auto start_pageRank_algorithm = high_resolution_clock::now();
            const Column pageRanks = calculatePageRank(graph);
            auto end_pageRank_algorithm = high_resolution_clock::now();
            auto pageRank_algorithm_duration = duration_cast<milliseconds>(end_pageRank_algorithm- start_pageRank_algorithm);
            cout << "PageRank Algorithm took " << pageRank_algorithm_duration.count()<<"ms"<<endl;
            // Utility::printPageRank(pageRanks);
        }        
};


// The main method
int main(int argc, char const *argv[]){   
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&RANK);
    MPI_Comm_size(MPI_COMM_WORLD,&SIZE);
    
    
    string filename = argv[argc-1];
    cout << filename << endl;
    PageRank::test(filename);
    
    
    MPI_Finalize();
    return 0;
}