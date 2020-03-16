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

class HyperLinkMatrix{
    using Size = unsigned int;
    using Value = double;
    class Element{
        using Columns = vector<Size>;
        private:
            Size Row;
            Columns Cols;
            Value value;
        public:
            Element(): value(val),Row(row),Cols(cols), numElements(0) {}
        
            // Compare two Elements
            
            // Check if two Elements are equal
            
            
        friend class HyperLinkMatrix;
    };
    using Elements = vector<Element>;
    private:
        Elements elements;
        Size numElements;
        // Comparison Operaotr
    public:
        // Simple Constructor
        HyperLinkMatrix() {}
        
        // Add an Element in the Matrix
        void inline addElement(Size row, Element::Columns cols, Value value) {elements.push_back(Element(row,cols,value)); numElements+=cols.size();}
        
        // Size and structure
        inline Size NumElements() {return elements.size(); }
        const inline Size NumElements() const {return elements.size(); }

        // Find if Index exists in Matrix
        inline bool indexExists(Size row, Size col) {return binary_search(elements.begin(),elements.end(),Element(row,col,0));}

        // Direct vector access and indexing
        inline Value& operator()(Size row, Size col) {return (*lower_bound(elements.begin(),elements.end(),Element(row,col,0))).value;}
        inline const Value& operator()(Size row, Size col) const {return (*lower_bound(elements.begin(),elements.end(),Element(row,col,0))).value;}

        // Return address of first element
        // inline Element* elements(Size row=0) {return elements.data();}

        // Check if two matrices are Compatible (i.e Have Same Indexes)
        inline bool isComparable(const SparseMatrix& other) {return (NumElements()==other.NumElements()) && equal(elements.begin(),elements.end(),other.elements.begin(),Element::comp);}

        // Check if two matrices are equal or not
        inline bool operator==(const SparseMatrix& other) {return isComparable(other)? elements==other.elements: false;}

        //Function to print the Matrix
        void print(string preface="Printing Hyperlink Matrix as (Row,Col,Value) : ", bool TRANSPOSE=false){	
            Size elements = NumElements();
            cout << preface <<std::fixed<< endl;
            cout.precision(5);
            for(const Element& e : elements)
                cout << "(" << e.Row << ',' << e.Col << ',' << e.value << ')' << endl;     
        }

        
};

