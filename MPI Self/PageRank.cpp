// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Custom Includes
#include "include/Job.hpp"
#include "../Graph.h"
#include "../Utility.cpp"
#include "Calculator.cpp"

using namespace std;
using namespace chrono;

// MPI Constants
int RANK,SIZE,HOME=1;

class PageRank{
    public:
        
        // Find PageRank      
        static Column calculatePageRank(const Graph& graph){
            Column pageRanks = Utility::getInitPageRank(graph.numVertices()); // This will contain the PageRanks at the end of the function
            Column pageRanks_(pageRanks); // This is a copy of the PageRank Column
            const Column hyperlinks = Utility::calculateHyperLinkColumn(graph.toList); 
            Value factor; // This will be added to help converge PageRanks Column
            Calculator::PageRankData pageRankData(hyperlinks,graph.toList,pageRanks);
            Value norm;
            unsigned int iteration=1;
            // Main Iteration
            do{
                // if(RANK==0) cout<< "Performing "<< iteration++ <<" Iteration.";
                norm=Calculator::performIteration(pageRanks,pageRanks_,pageRankData);
                // if(RANK==0) cout<<" Current norm is "<< norm << endl;
                pageRanks = pageRanks_;
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
