#include<vector>
#include<algorithm>
#include "mpi.h"
#include<string>
#include<iostream>

using namespace std;

// MapDistributor Class (may be subclassed from a common Distributor Class)
class MapDistributor{
    public:
        // Alias Tuple Definitions
        using Key = int;
        using Value = int;
        using Tuple = pair<Key,Value>;
        using Results = vector<Tuple>;
        using Input = MapReduce::Generator;

        // Constructor
        MapDistributor(int numKeys_, Input input_ ): numKeys(numKeys_), input(input_), result(0) 
        {MPI_Comm_rank(MPI_COMM_WORLD,&Rank);MPI_Comm_size(MPI_COMM_WORLD,&Size);}

    private:
        int numKeys;
        int Rank;
        int Size;
        Input& input;
        Results result;
        
    protected:
        void run();
};
