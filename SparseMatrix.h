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

class SparseMatrix{
    using Size = unsigned int;
    using Value = double;
    class Element{
        private:
            Size Row, Col;
            Value value;
        public:
            Element(Size row,Size col, Value val): value(val),Row(row),Col(col) {}
        
            // Compare two Elements
            inline bool operator<(const Element& other) {return (Row==other.Row)? Col<other.Col : Row<other.Row;}
            inline bool operator<(const Element& other) const {return (Row==other.Row)? Col<other.Col : Row<other.Row;}
            inline bool operator<(Element& other) {return (Row==other.Row)? Col<other.Col : Row<other.Row;}
            inline bool operator<(Element& other) const {return (Row==other.Row)? Col<other.Col : Row<other.Row;}
            
            // Check if two Elements are equal
            inline bool operator==(const Element& other) {return abs(value-other.value)<TOL;}
            inline bool operator==(const Element& other) const {return abs(value-other.value)<TOL;}
            inline bool operator==(Element& other) const {return abs(value-other.value)<TOL;}
            inline bool operator==(Element& other) {return abs(value-other.value)<TOL;}
            
            static inline bool comp(const Element& e1, const Element& e2) {return (e1.Row==e2.Row)? e1.Col<e2.Col : e1.Row<e2.Row;}
            
        friend class SparseMatrix;
    };
    using Data = vector<Element>;
    private:
        Data data;
        // Comparison Operaotr
    public:
        // Simple Constructor
        SparseMatrix() {}
        
        // Add an Element in the Matrix
        void inline addElement(Size row, Size col, Value value) {data.push_back(Element(row,col,value));}
        
        // Size and structure
        inline Size NumElements() {return data.size(); }
        const inline Size NumElements() const {return data.size(); }

        // Find if Index exists in Matrix
        inline bool indexExists(Size row, Size col) {return binary_search(data.begin(),data.end(),Element(row,col,0));}

        // Direct vector access and indexing
        inline Value& operator()(Size row, Size col) {return (*lower_bound(data.begin(),data.end(),Element(row,col,0))).value;}
        inline const Value& operator()(Size row, Size col) const {return (*lower_bound(data.begin(),data.end(),Element(row,col,0))).value;}

        // Return address of first element
        // inline Element* data(Size row=0) {return data.data();}

        // Check if two matrices are Compatible (i.e Have Same Indexes)
        inline bool isComparable(const SparseMatrix& other) {return (NumElements()==other.NumElements()) && equal(data.begin(),data.end(),other.data.begin(),Element::comp);}

        // Check if two matrices are equal or not
        inline bool operator==(const SparseMatrix& other) {return isComparable(other)? data==other.data: false;}

        //Function to print the Matrix
        void print(string preface="Printing Sparse Matrix as (Row,Col,Value) : ", bool TRANSPOSE=false){	
            Size elements = NumElements();
            cout << preface <<std::fixed<< endl;
            cout.precision(5);
            for(const Element& e : data)
                cout << "(" << e.Row << ',' << e.Col << ',' << e.value << ')' << endl;     
        }

        
};
