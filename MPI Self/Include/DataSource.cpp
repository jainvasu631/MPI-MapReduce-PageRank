#include<vector>

using namespace std;

// Class to Generate Data
template<typename T1,typename T2>
class DataSource{
    public:
        using MapKey = T1;
        using MapValue = T2;
        using MapTuple = pair<T1,T2>;
        using Results = vector<MapTuple>;
    
        // This Function is Implemented by User 
        // User is passed Key and a Value by reference.
        // If User fills them with Data. They are inserted into the Results.
        // User shall returns false if they have inserted a tuple and true if haven't and are done.
        bool const getData(MapKey& key, MapValue& value);
    private:
        Results results; // Stores output of Result Function
    protected:
        // When number of keys is not known in advance.
        void run(){
            bool done;
            while(!done){
                MapTuple tuple;// Fresh Declaration as Tuple is passed by Reference
                results.push_back(tuple);
                done = getData(tuple.first,tuple.second); 
            }
            results.pop_back();
        }
        // When number of keys is known in advance .. More Efficient
        void run(unsigned int numKeys) {results.resize(numKeys); for(auto& tuple : results) getData(tuple.first,tuple.second);}
};
