#include<vector>
#include<mpi.h>
#include<algorithm>
#include<unordered_map>

using namespace std;

// Temporary typenames because using templates is annoying
using MapKey = int;
using MapValue = int;
using ReduceKey = int;
using ReduceValue = int;
using ResultKey = int;
using ResultValue = int;

// Tuple typenames
using MapTuple = pair<MapKey,MapValue>;
using CombinerTuple = pair<ReduceKey, ReduceValue>;
using ReduceTuple = pair<ReduceKey, vector<ReduceValue>>;
using ResultTuple = pair<ResultKey, ResultValue>;

template<typename Input, typename Results>
class Runnable{
    public:
        explicit Runnable(const Input& input_): input(input_){}
    private:
        Results results; // Stores results of Runnable Function
        Input& input; // Stores Input of Runnable Function
    protected:
        // Runner Function
        void run();
};