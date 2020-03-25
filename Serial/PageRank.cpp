// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Class Includes
#include "../Graph.h"
#include "../Utility.cpp"

using namespace std;
using namespace chrono;

class PageRank{
    private:
        // Recalculate The Factor.
        static Value calculateFactor(const Column& pageRanks, const Column& hyperlink){
            Value factor=0;
            const Graph::Size N = pageRanks.size();
            for(Graph::Size i=0;i<N;i++)
                factor += ((hyperlink[i]>0)? Constant::BETA : Constant::GAMMA) * pageRanks[i];
            factor/=N;
            return factor;
        }
    public:
        
        // Find PageRank      
        static Column calculatePageRank(const Graph& graph){
            const Graph::Size N = graph.numVertices();
            Column pageRanks = Utility::getInitPageRank(N); // This will contain the PageRanks at the end of the function
            Value factor; // This will be added to help converge PageRanks Column
            const Column hyperlink = Utility::calculateHyperLinkColumn(graph.toList); 
            const Graph::FromList& fromList = graph.fromList;
            Value norm;
            Value new_rank;
            unsigned int iteration=1;
            // Main Iteration
            do{
                cout<< "Performing "<< iteration++ <<" Iteration.";
                norm=0;
                factor = calculateFactor(pageRanks,hyperlink);
                auto weightOp = [hyperlink, pageRanks](Value sum, Graph::Size from){return move(sum)+hyperlink[from]*pageRanks[from];}; // Weight Function
                for(Graph::Size i=0;i<N;i++) { // Recalculate All the PageRanks.
                    new_rank = Constant::ALPHA*accumulate(begin(fromList[i]),end(fromList[i]),factor,weightOp);
                    norm+=abs(new_rank-pageRanks[i]);
                    pageRanks[i]=new_rank;
                }
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
