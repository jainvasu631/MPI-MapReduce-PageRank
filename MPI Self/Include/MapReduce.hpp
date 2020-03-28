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
