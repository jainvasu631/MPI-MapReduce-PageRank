// STL Includes
#include<algorithm>
#include<chrono>
#include <iostream>
#include<numeric>
#include <string>

// Custom Includes
#include "../Graph.h"
#include "../Utility.cpp"

// MPI-MapReduce Library Includes
#include "mpi.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace std;
using namespace chrono;
using namespace MAPREDUCE_NS;

class PageRank{
    private:
        static Value calculateFactor(Column& pageRanks){
            MPI_Init(&narg,&args);
            int RANK,SIZE;
            MPI_Comm_rank(MPI_COMM_WORLD,&RANK);
            MPI_Comm_size(MPI_COMM_WORLD,&SIZE);
            MapReduce mapreduce(MPI_COMM_WORLD);
            mapreduce.verbosity=2;
            mapreduce.timer=1;

            MPI_Barrier(MPI_COMM_WORLD);
            double tstart = MPI_Wtime();
            
            MPI_Finalize();
        }

        static Value performIteration(Column& pageRanks);
    public:
        
        // Find PageRank      
        static Column calculatePageRank(const Graph& graph){
            Column pageRanks = Utility::getInitPageRank(graph.numVertices()); // This will contain the PageRanks at the end of the function
            Value factor; // This will be added to help converge PageRanks Column
            const Column hyperlinks = Utility::calculateHyperLinkColumn(graph.toList); 
            Value norm;
            unsigned int iteration=1;
            // Main Iteration
            do{
                cout<< "Performing "<< iteration++ <<" Iteration.";
                cout<<" Current norm is "<< norm << endl;
            } while(norm>Constant::TOL);
            return pageRanks;
        }
        // Testing Function
        static void test(const string filename){
            Graph graph = Utility::timedGraphCreation(filename);
            // graph.print();
            auto N = graph.numVertices(); 
            
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
    cout << argv[argc-1] << endl;
    string filename = argv[argc-1];
    PageRank::test(filename);
    return 0;
}