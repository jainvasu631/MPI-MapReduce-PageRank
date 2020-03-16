// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Class Includes
#include "Graph.h"

using namespace std;
using namespace chrono;

// Type aliases
using Size = unsigned int;
using Value = double;
using Column = vector<Value>;

// Assign the global constants
inline void assignConstants();

class PageRank{
    private:
        // Global Constants
        static constexpr Value ALPHA = 0.85;
        static constexpr Value BETA = 1/ALPHA-1;
        static constexpr Value GAMMA = 1/ALPHA;
        static constexpr Value TOL = 1.0e-4;
        
        // Recalculate The Factor.
        static Value calculateFactor(const Column& pageRanks, const Column& hyperlink){
            Value factor=0;
            const Size N = pageRanks.size();
            for(Size i=0;i<N;i++)
                factor += ((hyperlink[i]>0)? BETA : GAMMA) * pageRanks[i];
            factor/=N;
            return factor;
        }
        // Calculate the Hyperlink Column
        static Column calculateHyperLinkColumn(const Graph::ToList& toList){
            const Size N = toList.size();
            Column hyperlink(N);
            for(Size i=0;i<N;i++)
                hyperlink[i] = (toList[i].size()>0)? 1.0/toList[i].size() : 0;
            return hyperlink;
        }

    public:
        // Print PageRanks in format
        static void printPageRank(const Column& pageRanks){
            Size N = pageRanks.size();
            for(Size i=0; i<N;i++)
                cout << i << " = " << pageRanks[i] << endl;
            cout << "s = " << accumulate(pageRanks.begin(),pageRanks.end(),0.0) << endl;
        }
        
        // Find PageRank      
        static Column calculatePageRank(const Graph& graph){
            const Size N = graph.numVertices();
            Column pageRanks(N,0); // This will contain the PageRanks at the end of the function
            Value factor; // This will be added to help converge PageRanks Column
            pageRanks[0]=1;
            const Column hyperlink = calculateHyperLinkColumn(graph.toList); 
            const Graph::FromList& fromList = graph.fromList;
            Value norm;
            Value new_rank;
            Size iteration=1;
            // Main Iteration
            do{
                cout<< "Performing "<< iteration++ <<" Iteration.";
                norm=0;
                factor = calculateFactor(pageRanks,hyperlink);
                auto weightOp = [hyperlink, pageRanks](Value sum, Size from){return move(sum)+hyperlink[from]*pageRanks[from];}; // Weight Function
                for(Size i=0;i<N;i++) { // Recalculate All the PageRanks.
                    new_rank = ALPHA*accumulate(begin(fromList[i]),end(fromList[i]),factor,weightOp);
                    norm+=abs(new_rank-pageRanks[i]);
                    pageRanks[i]=new_rank;
                }
                cout<<" Current norm is "<< norm << endl;
            } while(norm>TOL);
            return pageRanks;
        }
        // Testing Function
        static void test(const string filename){
            // Graph Creation
            auto start_graph_creation = high_resolution_clock::now();
            Graph graph(filename);
            auto end_graph_creation = high_resolution_clock::now();
            auto graph_creation_duration = duration_cast<milliseconds>(end_graph_creation- start_graph_creation);
            cout << "Graph Creation took " << graph_creation_duration.count()<<"ms"<<endl;
            graph.print();

            // PageRank Algorithm
            auto start_pageRank_algorithm = high_resolution_clock::now();
            const Column pageRanks = calculatePageRank(graph);
            auto end_pageRank_algorithm = high_resolution_clock::now();
            auto pageRank_algorithm_duration = duration_cast<milliseconds>(end_pageRank_algorithm- start_pageRank_algorithm);
            cout << "PageRank Algorithm took " << pageRank_algorithm_duration.count()<<"ms"<<endl;
            
            // printPageRank(pageRanks);
        }        
};

// The main method
int main(int argc, char const *argv[]){   
    cout << argv[argc-1] << endl;
    string filename = argv[argc-1];
    PageRank::test(filename);
    return 0;
}
