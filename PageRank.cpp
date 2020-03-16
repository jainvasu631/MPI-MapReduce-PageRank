// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Class Includes
#include "Graph.h"

using namespace std;

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
            // Main Iteration
            for (Size i=0; i<20; i++){
                factor = calculateFactor(pageRanks,hyperlink);
                // Recalculate All the PageRanks.
                auto weightOp = [hyperlink, pageRanks](Value sum, Size from){return move(sum)+hyperlink[from]*pageRanks[from];}; // Weight Function
                for(Size i=0;i<N;i++)
                    pageRanks[i] = ALPHA*accumulate(begin(fromList[i]),end(fromList[i]),factor,weightOp);
            }
            return pageRanks;
        }        
};

// The main method
int main(int argc, char const *argv[]){   
    Graph graph("Tests/diamond.txt");
    graph.print();

    Column pageRanks = PageRank::calculatePageRank(graph);
    PageRank::printPageRank(pageRanks);
    return 0;
}
