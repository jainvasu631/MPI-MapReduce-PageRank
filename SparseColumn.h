#pragma once

// STL Includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

double TOL = 1.0e-8;

class SparseColumn{
    using Size = unsigned int;
    using Value = double;
    class Element{
        private:
            Size Index;
            Value value;
        public:
            Element(Size index, Value val): value(val), Index(index) {}
        
            // Compare two Elements
            inline bool operator<(const Element& other) {return Index>other.Index;}
            inline bool operator<(const Element& other) const {return Index>other.Index;}
            inline bool operator<(Element& other) {return Index>other.Index;}
            inline bool operator<(Element& other) const {return Index>other.Index;}
            
            // Check if two Elements are equal
            inline bool operator==(const Element& other) {return abs(value-other.value)<TOL;}
            inline bool operator==(const Element& other) const {return abs(value-other.value)<TOL;}
            inline bool operator==(Element& other) const {return abs(value-other.value)<TOL;}
            inline bool operator==(Element& other) {return abs(value-other.value)<TOL;}
            
            static inline bool comp(const Element& e1, const Element& e2) {return e1<e2;}
            
        friend class SparseColumn;
    };
    using Data = vector<Element>;
    private:
        Data data;
        // Comparison Operaotr
    public:
        // Simple Constructor
        SparseColumn() {}
        
        // Add an Element in the Matrix
        void inline addElement(Size index, Value value) {data.push_back(Element(index,value));}
        
        // Size and structure
        inline Size NumElements() {return data.size(); }
        const inline Size NumElements() const {return data.size(); }

        // Find if Index exists in Matrix
        inline bool indexExists(Size index) {return binary_search(data.begin(),data.end(),Element(index,0));}

        // Direct vector access and indexing
        inline Value& operator()(Size index) {return (*lower_bound(data.begin(),data.end(),Element(index,0))).value;}
        inline const Value& operator()(Size index) const {return (*lower_bound(data.begin(),data.end(),Element(index,0))).value;}
        
        // Return address of first element
        // inline Element* data(Size row=0) {return data.data();}

        // Check if two matrices are Compatible (i.e Have Same Indexes)
        inline bool isComparable(const SparseColumn& other) {return (NumElements()==other.NumElements()) && equal(data.begin(),data.end(),other.data.begin(),Element::comp);}

        // Check if two matrices are equal or not
        inline bool operator==(const SparseColumn& other) {return isComparable(other)? data==other.data: false;}

        //Function to print the Matrix
        void print(string preface="Printing Sparse Matrix as (Index,Value) : ", bool TRANSPOSE=false){	
            Size elements = NumElements();
            cout << preface <<std::fixed<< endl;
            cout.precision(5);
            for(const Element& e : data)
                cout << "(" << e.Index << ',' << e.value << ')' << endl;     
        }

        
};

