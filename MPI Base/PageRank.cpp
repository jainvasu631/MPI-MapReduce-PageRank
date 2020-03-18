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
    private:
        static Value calculateFactor(Column& pageRanks, Calculator& calculator){
            MPI_Init(NULL,NULL);
            int RANK,SIZE,HOME=1;
            MPI_Comm_rank(MPI_COMM_WORLD,&RANK);
            MPI_Comm_size(MPI_COMM_WORLD,&SIZE);
            cout<<"Proc " << RANK<< " is responding"<<endl; 
            MapReduce mapreduce(MPI_COMM_WORLD);
            cout<<"MapReduce created"<<endl;
            Graph::Size N = pageRanks.size();            
            MPI_Barrier(MPI_COMM_WORLD);

            // Map Part
            auto kvPairs = mapreduce.map(N,Calculator::MapFactor,((void*)&calculator));
            mapreduce.collate(NULL);
            
            // Reduce Part
            auto kvmPairs = mapreduce.reduce(Calculator::ReduceFactor,((void*)&calculator));
            MPI_Barrier(MPI_COMM_WORLD);
            
            mapreduce.gather(HOME);
            MPI_Finalize();
            return calculator.factor/N;
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

            const Column hyperlinks = Utility::calculateHyperLinkColumn(graph.toList);
            Column pageRanks = Utility::getInitPageRank(hyperlinks.size());
            Calculator calculator(hyperlinks,graph.toList,pageRanks);
            Value factor = PageRank::calculateFactor(pageRanks,calculator);
            
            cout<<"The factor is " << factor << endl;
            // PageRank Algorithm
            // auto start_pageRank_algorithm = high_resolution_clock::now();
            // const Column pageRanks = calculatePageRank(graph);
            // auto end_pageRank_algorithm = high_resolution_clock::now();
            // auto pageRank_algorithm_duration = duration_cast<milliseconds>(end_pageRank_algorithm- start_pageRank_algorithm);
            // cout << "PageRank Algorithm took " << pageRank_algorithm_duration.count()<<"ms"<<endl;
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