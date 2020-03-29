#include"MapReduce.hpp"
using namespace std;


// Workflow of MapReduce Class
// MapDistributor (calls) -> MapTask -> Combiner -> Reduce Distributor -> ReduceTask -> Output 
// Implementation of MapReduce Class Functions