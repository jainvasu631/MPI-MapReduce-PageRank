// STL Includes
#include<algorithm>
#include<chrono>
#include<numeric>

// Class Includes
#include "../Graph.h"

using namespace std;

// Type aliases
using Value = double;
using Column = vector<Value>;

class Constant{
    public:
        // Global Constants
        static constexpr Value ALPHA = 0.85;
        static constexpr Value GAMMA = 1/ALPHA;
        static constexpr Value BETA = GAMMA-1;
        static constexpr Value TOL = 1.0e-9;
};

class Utility{
    public:
        // Calculate the Hyperlink Column
        static Column calculateHyperLinkColumn(const Graph::ToList& toList){
            const Graph::Size N = toList.size();
            Column hyperlink(N);
            for(Graph::Size i=0;i<N;i++)
                hyperlink[i] = (toList[i].size()>0)? 1.0/toList[i].size() : 0;
            return hyperlink;
        }

        // Print PageRanks in format
        static void printPageRank(const Column& pageRanks){
            Graph::Size N = pageRanks.size();
            for(Graph::Size i=0; i<N;i++)
                cout << i << " = " << pageRanks[i] << endl;
            cout << "s = " << accumulate(pageRanks.begin(),pageRanks.end(),0.0) << endl;
        }
        
};