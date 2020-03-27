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
            Column pageRanks_(pageRanks); // This is a copy of the PageRank Column
            Value factor; // This will be added to help converge PageRanks Column
            const Column hyperlink = Utility::calculateHyperLinkColumn(graph.toList); 
            const Graph::FromList& fromList = graph.fromList;
            Value norm;
            unsigned int iteration=1;
            // Main Iteration
            do{
                cout<< "Performing "<< iteration++ <<" Iteration.";
                factor = calculateFactor(pageRanks,hyperlink);
                auto weightOp = [&](Value sum, Graph::Size from){return move(sum)+hyperlink[from]*pageRanks[from];}; // Weight Function
                auto iteration = [&](const Graph::VertexList& froms){ return Constant::ALPHA*accumulate(begin(froms),end(froms),factor,weightOp);}; // Iteration Function
                transform(fromList.begin(), fromList.end(),pageRanks_.begin(),iteration); // Perform Iteration and put old pageRanks back
                
                // Find the new norm
                auto abs_diff = [](Value a, Value b) {return abs(a-b);};
                norm = inner_product(pageRanks.begin(),pageRanks.end(),pageRanks_.begin(),0.0,plus<Value>(),abs_diff);
                cout<<" Current norm is "<< norm << endl;
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
