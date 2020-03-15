#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

class FlatMatrix{
    private:
        int ROWS;
        int COLS;

        std::vector<double> matrixData;
    
    public:
        // Simple Constructor
        FlatMatrix(int rows, int cols) : matrixData(rows*cols,0) , ROWS(rows), COLS(cols) {}
        
        // Size and structure
        inline int NumRows() const                          { return ROWS; }
        inline int NumColumns() const                       { return COLS; }
        inline int NumElements() const                      { return matrixData.size(); }

        // Direct vector access and indexing
        inline double& operator()(int row, int col)              { return matrixData[row * COLS + col]; }
        inline const double& operator()(int row, int col) const  { return matrixData[row * COLS + col]; }

        // Return address of first element
        inline double* data(int row=0)                      { return matrixData.data() + row*COLS; }

        // Check if two matrices are equal or not
        static bool isEqual(const FlatMatrix& c,const FlatMatrix& c_, double TOL =1e-8){
            int rows = c.NumRows(), columns = c_.NumColumns();
            for(int i=0; i<rows; i++){
                for(int j=0; j<columns; j++){
                    if (abs(c(i,j)-c_(i,j))>TOL)
                        return false;
                }
            }
            return true;
        }

        //Function to print the Matrix
        static void printMatrix(const FlatMatrix& a,string preface="Printing Matrix a : ", bool TRANSPOSE=false){	
            int rows = a.NumRows(), columns = a.NumColumns();
            cout << preface <<std::fixed<< endl;
            cout.precision(5);
            if (TRANSPOSE){
                for(int i=0; i<columns; i++){
                    for(int j=0; j<rows; j++)
                        cout<< a(j,i) <<"  ";
                    cout<<endl;
                }
            }
            else{
                for(int i=0; i<rows; i++){
                    for(int j=0; j<columns; j++)
                        cout<< a(i,j) <<"  ";
                    cout<<endl;
                }
            }   
        }
};
