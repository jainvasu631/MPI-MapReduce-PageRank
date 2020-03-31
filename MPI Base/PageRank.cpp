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

class PageRank{
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
                if(RANK==0) cout<< "Performing "<< iteration++ <<" Iteration.";
                norm = Calculator::performIteration(pageRanks,calculator,mapreduce);
                if(RANK==0) cout<<" Current norm is "<< norm << endl;
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
            if(RANK==0) cout << "PageRank Algorithm took " << pageRank_algorithm_duration.count()<<"ms"<<endl;
            // Utility::printPageRank(pageRanks);
        }        
};


// The main method
int main(int argc, char const *argv[]){   
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&RANK);
    MPI_Comm_size(MPI_COMM_WORLD,&SIZE);
    string filename = argv[argc-1];
    if(RANK==0) cout << filename << endl;
    PageRank::test(filename);
    MPI_Finalize();
    return 0;
}