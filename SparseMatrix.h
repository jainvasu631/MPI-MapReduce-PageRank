#pragma once

// STL Includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

class SparseMatrix{
    using Size = unsigned int;
    using Value = double;
    using Element = tuple<Size,Size,Value>;
    using Data = vector<Element>;
    private:
        Data data;
        // Comparison Operaotr
        inline bool compIndex(const Element& e1, const Element& e2) {return (get<0>(e1)==get<0>(e2))? get<1>(e1)<get<1>(e2) : get<0>(e1)<get<0>(e2);}
        
    public:
        // Simple Constructor
        SparseMatrix() {}
        
        // Add an Element in the Matrix
        void inline addElement(Size row, Size col, Value value) {data.push_back(Element(row,col,value));}
        
        // Size and structure
        inline Size NumElements() {return data.size(); }
        const inline Size NumElements() const {return data.size(); }

        // Find if Index exists in Matrix
        inline bool indexExists(Size row, Size col) {return binary_search(data.begin(),data.end(),Element(row,col,0), compIndex);}

        // Direct vector access and indexing
        inline Value& operator()(Size row, Size col) {return get<2>(*lower_bound(data.begin(),data.end(),Element(row,col,0), compIndex));}
        inline const Value& operator()(Size row, Size col) const {return get<2>(*lower_bound(data.begin(),data.end(),Element(row,col,0), compIndex));}

        // Return address of first element
        inline Element* data(Size row=0) {return data.data();}

        //Function to print the Matrix
        static void print(const SparseMatrix& a, string preface="Printing Sparse Matrix a : ", bool TRANSPOSE=false){	
            Size elements = a.NumElements();
            cout << preface <<std::fixed<< endl;
            cout.precision(5);
            if (TRANSPOSE)
                return;
            else
                return;  
        }

        // Check if two matrices are Compatible (i.e Have Same Indexes)
        static bool isComparable(const SparseMatrix& c,const SparseMatrix& c_) {
            return (c.NumElements()==c_.NumElements()) ? equal(c.data.begin(),c.data.end(),c_.data.begin(), compIndex) : false; 
        }

    // Check if two matrices are equal or not
    friend bool operator==(const SparseMatrix& c,const SparseMatrix& c_){
        const Value TOL = 1e-8;
        if (isComparable(c,c_)) {
            for(int i = c.NumElements()-1; i>=0; i--)
                if(abs(c.data[i]-c_.data[i])>TOL)
                    return false;
            return true;
        }
        else
            return false;
    }
};

