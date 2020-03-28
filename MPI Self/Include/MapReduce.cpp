#include"MapReduce.hpp"
using namespace std;

// Map Class
// This Map operator() is Implemented by User 
// User is passed Key and Value by reference.
// It does (k1,v1)->list(k2,v2) where (k2,v2) are created as pairs and added to results 
class MapTask: Task<MapKey,MapValue,ReduceKey,ReduceValue> {
    public: using Task::Task;
};

// Reduce Class
// This Reduce operator() is Implemented by User 
// User is passed Key and vector of Values by reference.
// It does (k2,list(v2))->list(k3,v3) where (k3,v3) are created as pairs and added to results
class ReduceTask : Task<ReduceKey,vector<ReduceValue>,ResultKey,ResultValue>{
    public: using Task::Task;
};

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
